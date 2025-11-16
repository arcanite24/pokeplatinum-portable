/**
 * @file main_sdl.c
 * @brief SDL3 platform entry point
 * 
 * This is the SDL3 equivalent of NitroMain() from main.c.
 * It initializes all game systems using PAL abstractions and runs the main game loop.
 */

#include "platform/platform_config.h"
#include "platform/pal_graphics.h"
#include "platform/pal_input.h"
#include "platform/pal_audio.h"
#include "platform/pal_timer.h"
#include "platform/pal_background.h"
#include "platform/pal_sprite.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Game system includes (same as main.c)
#include "constants/heap.h"
#include "brightness_controller.h"
#include "font.h"
#include "game_start.h"
#include "main.h"
#include "overlay_manager.h"
#include "play_time_manager.h"
#include "rtc.h"

// Global game language (stub for SDL - defaults to English)
// Real DS version loaded from save data
const u8 gGameLanguage = 2; // LANGUAGE_ENGLISH
const u8 gGameVersion = 12; // VERSION_PLATINUM
#include "savedata.h"
#include "save_player.h"
#include "unk_0202CC64.h"
#include "screen_fade.h"
#include "sound_system.h"
#include "sys_task_manager.h"
#include "system.h"
#include "timer.h"
#include "unk_0202419C.h"
#include "unk_020366A0.h"

#ifdef PLATFORM_SDL

// Forward declarations for game functions (from main.c)
extern void InitRNG(void);
extern void EnqueueApplication(FSOverlayID overlayID, const ApplicationManagerTemplate *template);
extern const ApplicationManagerTemplate gOpeningCutsceneAppTemplate;
extern const ApplicationManagerTemplate gGameStartLoadSaveAppTemplate;

// External overlay declarations
FS_EXTERN_OVERLAY(game_start);
FS_EXTERN_OVERLAY(game_opening);

int main(int argc, char* argv[]) {
    (void)argc;  // Unused
    (void)argv;  // Unused
    
    printf("========================================\n");
    printf("Pokemon Platinum SDL3 Port\n");
    printf("Platform: %s\n", PAL_PLATFORM_NAME);
    printf("========================================\n\n");
    
    // Step 1: Initialize SDL3 subsystems
    printf("Initializing SDL3...\n");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD)) {
        fprintf(stderr, "Failed to initialize SDL3: %s\n", SDL_GetError());
        return 1;
    }
    
    // Step 2: Initialize game systems (mirrors NitroMain)
    printf("Initializing game systems...\n");
    
    // InitSystem() - will call PAL_Graphics_Init internally for SDL
    InitSystem();
    printf("  - System initialized\n");
    
    // InitVRAM() - SDL stub, initializes PAL graphics memory
    InitVRAM();
    printf("  - VRAM initialized\n");
    
    // InitKeypadAndTouchpad() - will use PAL_Input internally
    InitKeypadAndTouchpad();
    printf("  - Input initialized\n");
    
    // Initialize PAL subsystems explicitly (in case InitSystem doesn't do it all)
    if (!PAL_Input_Init()) {
        fprintf(stderr, "Failed to initialize PAL input\n");
        SDL_Quit();
        return 1;
    }
    
    if (!PAL_Audio_Init(44100, 2)) {
        fprintf(stderr, "Failed to initialize PAL audio\n");
        PAL_Input_Shutdown();
        SDL_Quit();
        return 1;
    }
    
    if (!PAL_Timer_Init()) {
        fprintf(stderr, "Failed to initialize PAL timer\n");
        PAL_Audio_Shutdown();
        PAL_Input_Shutdown();
        SDL_Quit();
        return 1;
    }
    
    printf("  - PAL subsystems ready\n");
    
    // Step 3: Continue with game initialization (from NitroMain)
    printf("\nInitializing game data...\n");
    
    sub_0202419C();
    InitRTC();
    printf("  - RTC initialized\n");
    
    // Initialize fonts
    Fonts_Init();
    Font_InitManager(FONT_SYSTEM, HEAP_ID_APPLICATION);
    Font_InitManager(FONT_MESSAGE, HEAP_ID_APPLICATION);
    Font_InitManager(FONT_UNOWN, HEAP_ID_APPLICATION);
    printf("  - Fonts initialized\n");
    
    // Initialize application args
    ApplicationArgs args;
    args.unk_00 = -1;
    args.saveData = SaveData_Init();
    printf("  - Save data initialized\n");
    
    // Initialize sound system
    SoundSystem_Init(SaveData_GetChatotCry(args.saveData), SaveData_GetOptions(args.saveData));
    Timer_Start();
    printf("  - Sound system initialized\n");
    
    // Step 4: Determine which application to start (from NitroMain)
    printf("\nStarting game...\n");
    
    if (SaveData_BackupExists(args.saveData) == FALSE) {
        printf("  - No save data found, creating new save\n");
        // Would call sub_0209A74C here, but for now skip to game start
        args.error = FALSE;
        EnqueueApplication(FS_OVERLAY_ID(game_start), &gGameStartLoadSaveAppTemplate);
    } else {
        printf("  - Save data found, starting opening cutscene\n");
        args.error = FALSE;
        EnqueueApplication(FS_OVERLAY_ID(game_opening), &gOpeningCutsceneAppTemplate);
    }
    
    // Initialize remaining game systems
    gSystem.showTitleScreenIntro = TRUE;
    gSystem.frameCounter = 0;
    InitRNG();
    BrightnessController_ResetAllControllers();
    PlayTime_FlagNotStarted();
    
    printf("\n=== Game Ready ===\n");
    printf("Controls:\n");
    printf("  Arrow Keys - D-Pad\n");
    printf("  Z/Enter    - A Button\n");
    printf("  X/Backspace- B Button\n");
    printf("  ESC        - Quit\n\n");
    
    // Step 5: Main game loop (from NitroMain)
    BOOL running = TRUE;
    u64 frame_count = 0;
    
    printf("Entering main game loop...\n");
    
    while (running) {
        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = FALSE;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                running = FALSE;
            }
            
            // Handle gamepad connection/disconnection
            if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
                SDL_Gamepad* gamepad = SDL_OpenGamepad(event.gdevice.which);
                if (gamepad) {
                    PAL_Input_AddGamepad(gamepad);
                }
            } else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
                SDL_Gamepad* gamepad = SDL_GetGamepadFromID(event.gdevice.which);
                if (gamepad) {
                    PAL_Input_RemoveGamepad(gamepad);
                    SDL_CloseGamepad(gamepad);
                }
            }
        }
        
        // Read input (replaces ReadKeypadAndTouchpad)
        PAL_Input_Update();
        
        // TODO: Check for soft reset combo
        // if ((gSystem.heldKeysRaw & RESET_COMBO) == RESET_COMBO && !gSystem.inhibitReset) {
        //     SoftReset(RESET_CLEAN);
        // }
        
        // TODO: Check for CommSys_Update()
        // For now, assume it always returns TRUE
        
        // Begin frame rendering
        PAL_Graphics_BeginFrame();
        
        // Run the application (this calls the current game screen)
        // TODO: Call RunApplication() from main.c
        // RunApplication();
        
        // Execute task managers
        SysTaskManager_ExecuteTasks(gSystem.mainTaskMgr);
        SysTaskManager_ExecuteTasks(gSystem.printTaskMgr);
        
        // Render PAL subsystems
        // TODO: Render backgrounds and sprites when they're set up
        // PAL_Bg_RenderAll(bgConfig);
        // PAL_Sprite_RenderAll(spriteManager);
        
        // End frame rendering
        PAL_Graphics_EndFrame();
        
        // VBlank handling
        if (!gSystem.frameCounter) {
            gSystem.vblankCounter++;
        }
        
        // Update RTC and play time
        UpdateRTC();
        PlayTime_IncrementTimer();
        sub_020241CC();
        SysTaskManager_ExecuteTasks(gSystem.printTaskMgr);
        
        // Wait for VBlank (SDL: just delay for frame timing)
        PAL_Timer_Delay(16);  // ~60 FPS
        
        gSystem.vblankCounter++;
        gSystem.frameCounter = 0;
        
        // Update brightness and screen fade
        BrightnessController_Update();
        ExecScreenFade();
        
        // VBlank callback
        if (gSystem.vblankCallback != NULL) {
            gSystem.vblankCallback(gSystem.vblankCallbackData);
        }
        
        // Sound system tick
        SoundSystem_Tick();
        SysTaskManager_ExecuteTasks(gSystem.postVBlankTaskMgr);
        
        frame_count++;
        
        // Debug output every 60 frames
        if (frame_count % 60 == 0) {
            printf("Frame %llu - Running\n", (unsigned long long)frame_count);
        }
    }
    
    printf("\nShutting down...\n");
    printf("Total frames: %llu\n", (unsigned long long)frame_count);
    
    // Cleanup game systems
    // TODO: Proper cleanup of save data, fonts, etc.
    
    // Cleanup PAL subsystems
    PAL_Timer_Shutdown();
    PAL_Audio_Shutdown();
    PAL_Input_Shutdown();
    PAL_Graphics_Shutdown();
    SDL_Quit();
    
    printf("Clean shutdown complete!\n");
    return 0;
}

#endif // PLATFORM_SDL
