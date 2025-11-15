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
#include "platform/pal_sprite.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include for testing graphics loading
#include "graphics.h"
#include "constants/narc.h"
#include "constants/heap.h"

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
    
    // ===== PHASE 2.4: SPRITE SYSTEM TEST SETUP =====
    
    // Create sprite manager
    PAL_SpriteManager* spriteManager = PAL_Sprite_CreateManager(64, 0);
    if (!spriteManager) {
        fprintf(stderr, "Failed to create sprite manager\n");
        PAL_Bg_DestroyConfig(bgConfig);
        PAL_Timer_Shutdown();
        PAL_Audio_Shutdown();
        PAL_Input_Shutdown();
        PAL_Graphics_Shutdown();
        SDL_Quit();
        return 1;
    }
    
    // Create test sprite graphics data (16x16 sprite, 4bpp)
    u8 sprite_graphics[128]; // 16x16 pixels, 4bpp = 128 bytes
    
    // Fill with a simple pattern (cross/plus shape)
    memset(sprite_graphics, 0, sizeof(sprite_graphics));
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            int byte_idx = (y * 16 + x) / 2;
            int nibble = x & 1;
            u8 color;
            
            // Create a cross pattern
            if (x == 8 || y == 8) {
                color = 3; // Bright color
            } else if (x >= 6 && x <= 10 && y >= 6 && y <= 10) {
                color = 2; // Medium color
            } else if ((x + y) % 3 == 0) {
                color = 1; // Light pattern
            } else {
                color = 0; // Transparent
            }
            
            if (nibble == 0) {
                sprite_graphics[byte_idx] = color;
            } else {
                sprite_graphics[byte_idx] |= (color << 4);
            }
        }
    }
    
    // Create sprite palette
    u16 sprite_palette[16];
    sprite_palette[0] = 0x0000;                       // Transparent
    sprite_palette[1] = ((31 << 10) | (20 << 5) | 20);  // Light pink
    sprite_palette[2] = ((25 << 10) | (10 << 5) | 31);  // Purple
    sprite_palette[3] = ((31 << 10) | (31 << 5) | 0);   // Yellow
    for (int i = 4; i < 16; i++) {
        sprite_palette[i] = 0x7FFF; // White
    }
    
    // Create test sprites with different properties
    PAL_SpriteTemplate sprite_templates[4];
    
    // Sprite 0: Normal sprite on main screen
    sprite_templates[0].graphics_data = sprite_graphics;
    sprite_templates[0].graphics_size = sizeof(sprite_graphics);
    sprite_templates[0].palette_data = sprite_palette;
    sprite_templates[0].palette_size = sizeof(sprite_palette);
    sprite_templates[0].size = PAL_SPRITE_SIZE_16x16;
    sprite_templates[0].color_mode = PAL_SPRITE_COLOR_4BPP;
    sprite_templates[0].x = 100;
    sprite_templates[0].y = 80;
    sprite_templates[0].priority = 2;
    sprite_templates[0].palette_num = 0;
    sprite_templates[0].screen = PAL_SCREEN_MAIN;
    
    // Sprite 1: Flipped sprite
    sprite_templates[1] = sprite_templates[0];
    sprite_templates[1].x = 150;
    sprite_templates[1].priority = 1;
    
    // Sprite 2: Rotating sprite on sub screen
    sprite_templates[2] = sprite_templates[0];
    sprite_templates[2].x = 100;
    sprite_templates[2].y = 80;
    sprite_templates[2].screen = PAL_SCREEN_SUB;
    sprite_templates[2].priority = 3;
    
    // Sprite 3: Scaling sprite
    sprite_templates[3] = sprite_templates[0];
    sprite_templates[3].x = 180;
    sprite_templates[3].y = 120;
    sprite_templates[3].priority = 0;
    
    // Create sprites
    PAL_Sprite* test_sprites[4];
    for (int i = 0; i < 4; i++) {
        test_sprites[i] = PAL_Sprite_Create(spriteManager, &sprite_templates[i]);
        if (!test_sprites[i]) {
            fprintf(stderr, "Warning: Failed to create sprite %d\n", i);
        }
    }
    
    // Configure sprites with different effects
    if (test_sprites[1]) {
        PAL_Sprite_SetFlip(test_sprites[1], PAL_SPRITE_FLIP_H);
    }
    if (test_sprites[2]) {
        PAL_Sprite_EnableAffine(test_sprites[2], TRUE);
    }
    if (test_sprites[3]) {
        PAL_Sprite_EnableAffine(test_sprites[3], TRUE);
    }
    
    printf("Sprite test initialized: 4 test sprites created\n");
    printf("- Sprite 0: Normal (priority 2)\n");
    printf("- Sprite 1: H-flipped (priority 1)\n");
    printf("- Sprite 2: Rotating on sub screen (priority 3)\n");
    printf("- Sprite 3: Scaling (priority 0)\n\n");
    
    // ===== END SPRITE SYSTEM TEST SETUP =====
    
    // ===== PHASE 3.4.2: GRAPHICS LOADING TEST =====
    printf("========================================\n");
    printf("Testing Graphics Loading Functions\n");
    printf("========================================\n");
    
    // Initialize background layer for title screen test
    PAL_BgTemplate titleBgTemplate = {0};
    titleBgTemplate.x = 0;
    titleBgTemplate.y = 0;
    titleBgTemplate.bufferSize = 32 * 24 * sizeof(u16);
    titleBgTemplate.baseTile = 0;
    titleBgTemplate.screenSize = PAL_BG_SCREEN_SIZE_256x256;
    titleBgTemplate.colorMode = PAL_BG_COLOR_MODE_8BPP;  // Title screen uses 8bpp
    titleBgTemplate.priority = 0;  // Highest priority
    
    PAL_Bg_InitFromTemplate(bgConfig, PAL_BG_LAYER_MAIN_1, &titleBgTemplate, PAL_BG_TYPE_STATIC);
    
    printf("\nTest 1: Loading top_border tiles (member 5)\n");
    printf("-------------------------------------------\n");
    u32 tilesLoaded = Graphics_LoadTilesToBgLayer(
        NARC_INDEX_DEMO__TITLE__TITLEDEMO,  // NARC 319
        5,                                    // Member 5 = top_border
        bgConfig,
        PAL_BG_LAYER_MAIN_1,
        0,                                    // offset 0
        0,                                    // load all tiles
        FALSE,                                // not compressed
        HEAP_ID_APPLICATION
    );
    printf("✅ Loaded %u bytes of tiles\n\n", tilesLoaded);
    
    printf("Test 2: Loading palette_1 (member 4)\n");
    printf("-------------------------------------------\n");
    Graphics_LoadPaletteWithSrcOffset(
        NARC_INDEX_DEMO__TITLE__TITLEDEMO,  // NARC 319
        4,                                    // Member 4 = palette_1
        PAL_LOAD_MAIN_BG,                    // Main screen background
        0,                                    // src offset 0
        0,                                    // palette offset 0
        0,                                    // load all colors
        HEAP_ID_APPLICATION
    );
    printf("✅ Palette loaded\n\n");
    
    printf("Test 3: Loading logo_top tiles (member 9)\n");
    printf("-------------------------------------------\n");
    tilesLoaded = Graphics_LoadTilesToBgLayer(
        NARC_INDEX_DEMO__TITLE__TITLEDEMO,  // NARC 319
        9,                                    // Member 9 = logo_top
        bgConfig,
        PAL_BG_LAYER_MAIN_1,
        0,                                    // offset 0
        0,                                    // load all tiles
        FALSE,                                // not compressed
        HEAP_ID_APPLICATION
    );
    printf("✅ Loaded %u bytes of tiles\n\n", tilesLoaded);
    
    printf("========================================\n");
    printf("Graphics Loading Test Complete!\n");
    printf("========================================\n\n");
    
    // ===== END GRAPHICS LOADING TEST =====
    
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
    
    // Sprite animation variables
    float rotation_angle = 0.0f;
    float scale_factor = 1.0f;
    float scale_dir = 0.01f;
    
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
        
        // ===== PHASE 2.4: SPRITE ANIMATION =====
        
        // Animate rotating sprite
        if (test_sprites[2]) {
            rotation_angle += 2.0f;
            if (rotation_angle >= 360.0f) rotation_angle -= 360.0f;
            PAL_Sprite_SetRotation(test_sprites[2], rotation_angle);
        }
        
        // Animate scaling sprite (pulse effect)
        if (test_sprites[3]) {
            scale_factor += scale_dir;
            if (scale_factor >= 2.0f || scale_factor <= 0.5f) {
                scale_dir = -scale_dir;
            }
            PAL_Sprite_SetScale(test_sprites[3], scale_factor, scale_factor);
        }
        
        // Update and render all sprites
        PAL_Sprite_UpdateAll(spriteManager);
        PAL_Sprite_RenderAll(spriteManager);
        
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
    
    // Cleanup sprite system
    PAL_Sprite_DestroyManager(spriteManager);
    
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
