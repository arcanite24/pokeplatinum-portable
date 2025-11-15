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
#include "platform/pal_background.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    printf("Demonstrating Phase 2.2-2.3: Graphics + Background System\n");
    printf("- Colored rectangles on both screens\n");
    printf("- Moving sprites\n");
    printf("- Line drawing\n");
    printf("- Tilemap rendering with scrolling\n\n");
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
    
    // ===== PHASE 2.3: Background System Test Setup =====
    
    // Create background configuration
    PAL_BgConfig* bgConfig = PAL_Bg_CreateConfig(0);
    if (!bgConfig) {
        fprintf(stderr, "Failed to create background configuration\n");
        PAL_Timer_Shutdown();
        PAL_Audio_Shutdown();
        PAL_Input_Shutdown();
        PAL_Graphics_Shutdown();
        SDL_Quit();
        return 1;
    }
    
    // Create a simple 8x8 tile (checkerboard pattern) - 4bpp format
    u8 test_tiles[32 * 2]; // 2 tiles: one light, one dark
    
    // Tile 0: Light tile (color index 1)
    for (int i = 0; i < 32; i++) {
        test_tiles[i] = 0x11; // Both nibbles = color 1
    }
    
    // Tile 1: Dark tile (color index 2)
    for (int i = 0; i < 32; i++) {
        test_tiles[32 + i] = 0x22; // Both nibbles = color 2
    }
    
    // Initialize main screen background layer 0
    PAL_BgTemplate bgTemplate = {0};
    bgTemplate.x = 0;
    bgTemplate.y = 0;
    bgTemplate.bufferSize = 32 * 24 * sizeof(u16); // 32x24 tiles
    bgTemplate.baseTile = 0;
    bgTemplate.screenSize = PAL_BG_SCREEN_SIZE_256x256;
    bgTemplate.colorMode = PAL_BG_COLOR_MODE_4BPP;
    bgTemplate.priority = 1;
    
    PAL_Bg_InitFromTemplate(bgConfig, PAL_BG_LAYER_MAIN_0, &bgTemplate, PAL_BG_TYPE_STATIC);
    
    // Load tiles
    PAL_Bg_LoadTiles(bgConfig, PAL_BG_LAYER_MAIN_0, test_tiles, sizeof(test_tiles), 0);
    
    // Create a checkerboard tilemap pattern
    u16* tilemap = (u16*)PAL_Bg_GetTilemapBuffer(bgConfig, PAL_BG_LAYER_MAIN_0);
    if (tilemap) {
        for (int y = 0; y < 24; y++) {
            for (int x = 0; x < 32; x++) {
                // Checkerboard: alternate between tile 0 and tile 1
                int tileIdx = ((x + y) & 1);
                // Use palette 0
                tilemap[y * 32 + x] = tileIdx;
            }
        }
        PAL_Bg_CopyTilemapBufferToVRAM(bgConfig, PAL_BG_LAYER_MAIN_0);
    }
    
    // Set up palette for the background (using RGB555 format)
    u16 palette[256];
    memset(palette, 0, sizeof(palette));
    palette[0] = 0x0000;  // Transparent (ignored for color index 0)
    palette[1] = ((12 << 10) | (18 << 5) | 31);  // Light blue (RGB555)
    palette[2] = ((4 << 10) | (7 << 5) | 15);     // Dark blue (RGB555)
    
    // Load palette to background layer
    PAL_Bg_LoadPalette(bgConfig, PAL_BG_LAYER_MAIN_0, palette, sizeof(palette), 0);
    
    // Enable the layer
    PAL_Bg_ToggleLayer(PAL_BG_LAYER_MAIN_0, TRUE);
    
    printf("Background test initialized: 32x24 checkerboard tilemap\n");
    printf("Press arrow keys to scroll the background!\n\n");
    
    // ===== END BACKGROUND SYSTEM TEST SETUP =====
    
    // Main loop
    BOOL running = TRUE;
    u64 frame_count = 0;
    u64 start_time = PAL_Timer_GetTicks();
    
    // Graphics test variables
    int sprite_x = 50;
    int sprite_y = 50;
    int sprite_dx = 2;
    int sprite_dy = 1;
    
    // Background scroll test variables
    int bg_scroll_x = 0;
    int bg_scroll_y = 0;
    
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
        
        // Handle background scrolling with arrow keys
        if (held & PAL_BUTTON_LEFT) {
            bg_scroll_x -= 2;
            PAL_Bg_SetOffset(bgConfig, PAL_BG_LAYER_MAIN_0, PAL_BG_OFFSET_SET_X, bg_scroll_x);
        }
        if (held & PAL_BUTTON_RIGHT) {
            bg_scroll_x += 2;
            PAL_Bg_SetOffset(bgConfig, PAL_BG_LAYER_MAIN_0, PAL_BG_OFFSET_SET_X, bg_scroll_x);
        }
        if (held & PAL_BUTTON_UP) {
            bg_scroll_y -= 2;
            PAL_Bg_SetOffset(bgConfig, PAL_BG_LAYER_MAIN_0, PAL_BG_OFFSET_SET_Y, bg_scroll_y);
        }
        if (held & PAL_BUTTON_DOWN) {
            bg_scroll_y += 2;
            PAL_Bg_SetOffset(bgConfig, PAL_BG_LAYER_MAIN_0, PAL_BG_OFFSET_SET_Y, bg_scroll_y);
        }
        
        // Print input state every 60 frames
        if (frame_count % 60 == 0) {
            printf("Frame %llu - Held: 0x%04X, Pressed: 0x%04X, BG Scroll: (%d, %d)", 
                   (unsigned long long)frame_count, held, pressed, bg_scroll_x, bg_scroll_y);
            if (touch.held) {
                printf(", Touch: (%d, %d)", touch.x, touch.y);
            }
            printf("\n");
        }
        
        // Begin rendering
        PAL_Graphics_BeginFrame();
        
        // ===== PHASE 2.3: BACKGROUND RENDERING =====
        
        // Render background layers first (beneath sprites)
        PAL_Bg_RenderAll(bgConfig);
        
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
    
    // Cleanup background system
    PAL_Bg_DestroyConfig(bgConfig);
    
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
