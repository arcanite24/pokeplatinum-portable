/**
 * Test program for graphics loading functions
 * 
 * Tests Graphics_LoadTilesToBgLayer() and Graphics_LoadPaletteWithSrcOffset()
 * to verify they can load title screen assets correctly.
 */

#include <stdio.h>
#include <stdlib.h>
#include "constants/narc.h"
#include "constants/heap.h"
#include "graphics.h"
#include "heap.h"
#include "platform/pal_background.h"
#include "platform/pal_graphics.h"

int main(void) {
    printf("========================================\n");
    printf("Graphics Loading Test\n");
    printf("========================================\n\n");
    
    // Initialize heap system
    printf("Initializing heap system...\n");
    Heap_Init();
    printf("✅ Heap initialized\n\n");
    
    // Initialize PAL graphics
    printf("Initializing graphics system...\n");
    if (!PAL_Graphics_Init()) {
        fprintf(stderr, "❌ Failed to initialize graphics\n");
        return 1;
    }
    printf("✅ Graphics initialized\n\n");
    
    // Create background config
    printf("Creating background config...\n");
    PAL_BgConfig* bgConfig = PAL_Bg_CreateConfig(HEAP_ID_APPLICATION);
    if (!bgConfig) {
        fprintf(stderr, "❌ Failed to create background config\n");
        return 1;
    }
    printf("✅ Background config created\n\n");
    
    // Test loading tiles (top_border - member 5)
    printf("========================================\n");
    printf("Test 1: Loading top_border tiles (member 5)\n");
    printf("========================================\n");
    Graphics_LoadTilesToBgLayer(
        NARC_INDEX_DEMO__TITLE__TITLEDEMO,  // NARC ID 319
        5,                                   // Member 5 = top_border
        bgConfig,
        0,                                   // BG layer 0
        0,                                   // offset 0
        0,                                   // load all tiles
        FALSE,                               // not compressed
        HEAP_ID_APPLICATION
    );
    printf("Test 1 complete\n\n");
    
    // Test loading palette (palette_1 - member 4)
    printf("========================================\n");
    printf("Test 2: Loading palette_1 (member 4)\n");
    printf("========================================\n");
    Graphics_LoadPaletteWithSrcOffset(
        NARC_INDEX_DEMO__TITLE__TITLEDEMO,  // NARC ID 319
        4,                                   // Member 4 = palette_1
        bgConfig,
        0,                                   // BG layer 0
        0,                                   // offset 0
        0,                                   // load all colors
        0,                                   // src offset 0
        HEAP_ID_APPLICATION
    );
    printf("Test 2 complete\n\n");
    
    // Test loading logo top tiles (member 9)
    printf("========================================\n");
    printf("Test 3: Loading logo_top tiles (member 9)\n");
    printf("========================================\n");
    Graphics_LoadTilesToBgLayer(
        NARC_INDEX_DEMO__TITLE__TITLEDEMO,  // NARC ID 319
        9,                                   // Member 9 = logo_top
        bgConfig,
        1,                                   // BG layer 1
        0,                                   // offset 0
        0,                                   // load all tiles
        FALSE,                               // not compressed
        HEAP_ID_APPLICATION
    );
    printf("Test 3 complete\n\n");
    
    // Test loading unknown member (should use numeric fallback)
    printf("========================================\n");
    printf("Test 4: Loading unknown member (should fail gracefully)\n");
    printf("========================================\n");
    Graphics_LoadTilesToBgLayer(
        NARC_INDEX_DEMO__TITLE__TITLEDEMO,  // NARC ID 319
        999,                                 // Member 999 = doesn't exist
        bgConfig,
        2,                                   // BG layer 2
        0,                                   // offset 0
        0,                                   // load all tiles
        FALSE,                               // not compressed
        HEAP_ID_APPLICATION
    );
    printf("Test 4 complete (expected to fail)\n\n");
    
    // Cleanup
    printf("========================================\n");
    printf("Cleanup\n");
    printf("========================================\n");
    PAL_Bg_DestroyConfig(bgConfig);
    printf("✅ Background config destroyed\n");
    
    PAL_Graphics_Shutdown();
    printf("✅ Graphics shutdown\n");
    
    printf("\n========================================\n");
    printf("✅ All tests complete!\n");
    printf("========================================\n");
    
    return 0;
}
