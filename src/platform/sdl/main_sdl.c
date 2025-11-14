/**
 * @file main_sdl.c
 * @brief SDL3 platform entry point
 * 
 * This is a minimal SDL3 entry point for testing the platform abstraction layer.
 * Eventually, this will integrate with the main game code.
 */

#include "platform/platform_config.h"
#include "platform/pal_graphics.h"
#include "platform/pal_input.h"
#include "platform/pal_audio.h"
#include "platform/pal_timer.h"

#include <SDL3/SDL.h>
#include <stdio.h>

#ifdef PLATFORM_SDL

int main(int argc, char* argv[]) {
    (void)argc;  // Unused
    (void)argv;  // Unused
    
    printf("Pokemon Platinum SDL3 Port - Build System Test\n");
    printf("Platform: %s\n", PAL_PLATFORM_NAME);
    
    // Initialize SDL3
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
        fprintf(stderr, "Failed to initialize SDL3: %s\n", SDL_GetError());
        return 1;
    }
    
    // Initialize PAL subsystems
    if (!PAL_Graphics_Init(512, 192)) {
        fprintf(stderr, "Failed to initialize graphics\n");
        SDL_Quit();
        return 1;
    }
    
    if (!PAL_Input_Init()) {
        fprintf(stderr, "Failed to initialize input\n");
        PAL_Graphics_Shutdown();
        SDL_Quit();
        return 1;
    }
    
    if (!PAL_Audio_Init(44100, 2)) {
        fprintf(stderr, "Failed to initialize audio\n");
        PAL_Input_Shutdown();
        PAL_Graphics_Shutdown();
        SDL_Quit();
        return 1;
    }
    
    if (!PAL_Timer_Init()) {
        fprintf(stderr, "Failed to initialize timer\n");
        PAL_Audio_Shutdown();
        PAL_Input_Shutdown();
        PAL_Graphics_Shutdown();
        SDL_Quit();
        return 1;
    }
    
    printf("All PAL subsystems initialized successfully!\n");
    printf("Window: 512x192 (dual screens side-by-side)\n");
    printf("Controls:\n");
    printf("  Arrow Keys - D-Pad\n");
    printf("  Z/Enter    - A Button\n");
    printf("  X/Backspace- B Button\n");
    printf("  A          - X Button\n");
    printf("  S          - Y Button\n");
    printf("  Q          - L Button\n");
    printf("  W          - R Button\n");
    printf("  Space      - Start\n");
    printf("  Shift      - Select\n");
    printf("  Mouse      - Touch (right screen)\n");
    printf("  ESC        - Quit\n\n");
    
    // Main loop
    BOOL running = TRUE;
    u64 frame_count = 0;
    u64 start_time = PAL_Timer_GetTicks();
    
    while (running) {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = FALSE;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                running = FALSE;
            }
        }
        
        // Update input
        PAL_Input_Update();
        
        // Get input state
        u32 held = PAL_Input_GetHeld();
        u32 pressed = PAL_Input_GetPressed();
        PAL_TouchState touch = PAL_Input_GetTouch();
        
        // Print input state every 60 frames
        if (frame_count % 60 == 0) {
            printf("Frame %llu - Held: 0x%04X, Pressed: 0x%04X", 
                   (unsigned long long)frame_count, held, pressed);
            if (touch.held) {
                printf(", Touch: (%d, %d)", touch.x, touch.y);
            }
            printf("\n");
        }
        
        // Begin rendering
        PAL_Graphics_BeginFrame();
        
        // TODO: Render game content here
        
        // End rendering and present
        PAL_Graphics_EndFrame();
        
        frame_count++;
        
        // Simple frame rate limiting (aim for 60 FPS)
        PAL_Timer_Delay(16);
    }
    
    u64 end_time = PAL_Timer_GetTicks();
    u64 elapsed_ms = end_time - start_time;
    float fps = (float)frame_count / ((float)elapsed_ms / 1000.0f);
    
    printf("\nShutting down...\n");
    printf("Total frames: %llu\n", (unsigned long long)frame_count);
    printf("Elapsed time: %llu ms\n", (unsigned long long)elapsed_ms);
    printf("Average FPS: %.2f\n", fps);
    
    // Cleanup
    PAL_Timer_Shutdown();
    PAL_Audio_Shutdown();
    PAL_Input_Shutdown();
    PAL_Graphics_Shutdown();
    SDL_Quit();
    
    printf("Clean shutdown complete!\n");
    return 0;
}

#endif // PLATFORM_SDL
