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
    printf("\n=== Graphics Test Enabled ===\n");
    printf("Demonstrating Phase 2.2: Graphics PAL Implementation\n");
    printf("- Colored rectangles on both screens\n");
    printf("- Moving sprites\n");
    printf("- Line drawing\n\n");
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
    
    // Graphics test variables
    int sprite_x = 50;
    int sprite_y = 50;
    int sprite_dx = 2;
    int sprite_dy = 1;
    
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
        
        // ===== PHASE 2.2 GRAPHICS TEST =====
        
        // Get screen surfaces
        PAL_Surface* main_screen = PAL_Graphics_GetScreen(PAL_SCREEN_MAIN);
        PAL_Surface* sub_screen = PAL_Graphics_GetScreen(PAL_SCREEN_SUB);
        
        // Clear main screen to dark blue
        PAL_Graphics_ClearSurface(main_screen, 20, 30, 60, 255);
        
        // Clear sub screen to dark green
        PAL_Graphics_ClearSurface(sub_screen, 20, 60, 30, 255);
        
        // Draw some colored rectangles on main screen
        PAL_Graphics_FillRect(main_screen, 10, 10, 50, 30, 255, 0, 0, 255);      // Red
        PAL_Graphics_FillRect(main_screen, 70, 10, 50, 30, 0, 255, 0, 255);      // Green
        PAL_Graphics_FillRect(main_screen, 130, 10, 50, 30, 0, 0, 255, 255);     // Blue
        PAL_Graphics_FillRect(main_screen, 190, 10, 50, 30, 255, 255, 0, 255);   // Yellow
        
        // Draw moving "sprite" (simple rectangle)
        PAL_Graphics_FillRect(main_screen, sprite_x, sprite_y, 20, 20, 255, 128, 0, 255);
        
        // Update sprite position (bouncing)
        sprite_x += sprite_dx;
        sprite_y += sprite_dy;
        if (sprite_x <= 0 || sprite_x >= PAL_SCREEN_WIDTH - 20) sprite_dx = -sprite_dx;
        if (sprite_y <= 0 || sprite_y >= PAL_SCREEN_HEIGHT - 20) sprite_dy = -sprite_dy;
        
        // Draw some lines on sub screen
        int center_x = PAL_SCREEN_WIDTH / 2;
        int center_y = PAL_SCREEN_HEIGHT / 2;
        PAL_Graphics_DrawLine(sub_screen, 0, center_y, PAL_SCREEN_WIDTH, center_y, 
                             255, 255, 255, 255);
        PAL_Graphics_DrawLine(sub_screen, center_x, 0, center_x, PAL_SCREEN_HEIGHT,
                             255, 255, 255, 255);
        
        // Draw touch indicator if touch is active
        if (touch.held) {
            PAL_Graphics_FillRect(sub_screen, touch.x - 5, touch.y - 5, 10, 10,
                                 255, 0, 255, 255);
            
            // Draw crosshair
            PAL_Graphics_DrawLine(sub_screen, touch.x - 10, touch.y, touch.x + 10, touch.y,
                                 255, 255, 0, 255);
            PAL_Graphics_DrawLine(sub_screen, touch.x, touch.y - 10, touch.x, touch.y + 10,
                                 255, 255, 0, 255);
        }
        
        // Draw frame counter on main screen
        if (frame_count % 60 < 30) {
            PAL_Graphics_FillRect(main_screen, 10, PAL_SCREEN_HEIGHT - 20, 
                                 60, 15, 0, 0, 0, 200);
        }
        
        // ===== END GRAPHICS TEST =====
        
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
