#include "applications/poketch/unavailable/graphics.h"

#include "constants/heap.h"

#include "bg_window.h"
#include "graphics.h"
#include "heap.h"
#include "narc.h"

#define POKETCH_UNAVAILABLE_SCREEN_NARC_TILES_IDX   10
#define POKETCH_UNAVAILABLE_SCREEN_NARC_TILEMAP_IDX 11
#define POKETCH_UNAVAILABLE_SCREEN_NARC_PALETTE_IDX 12

void PoketchUnavailableScreen_Init(BgConfig *bgConfig)
{
    static const BgTemplate template = {
        .x = 0,
        .y = 0,
        .bufferSize = 0x800,
        .baseTile = 0,
        .screenSize = BG_SCREEN_SIZE_256x256,
        #ifdef PLATFORM_DS
        .colorMode = GX_BG_COLORMODE_16,
        #else
        // TODO: Port GX_BG_COLORMODE_16 to PAL
        #endif
        .screenBase = GX_BG_SCRBASE_0x6000,
        .charBase = GX_BG_CHARBASE_0x00000,
        #ifdef PLATFORM_DS
        .bgExtPltt = GX_BG_EXTPLTT_01,
        #else
        // TODO: Port GX_BG_EXTPLTT_01 to PAL
        #endif
        .priority = 0,
        .areaOver = 0,
        .mosaic = FALSE,
    };

    Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_POKETCH_APP, HEAP_SIZE_PRE_POKETCH_SUBSCREEN);

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GXS_SetGraphicsMode to PAL
    #endif
    GXS_SetGraphicsMode(GX_BGMODE_0);
    #else
    // TODO: Port GX_BGMODE_0 to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_SetBankForSubBG to PAL
    #endif
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H);
    #else
    // TODO: Port GX_VRAM_SUB_BG_32_H to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_SetBankForSubOBJ to PAL
    #endif
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);
    #else
    // TODO: Port GX_VRAM_SUB_OBJ_16_I to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GXS_SetOBJVRamModeChar to PAL
    #endif
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);
    #else
    // TODO: Port GX_OBJVRAMMODE_CHAR_1D_32K to PAL
    #endif

    Bg_InitFromTemplate(bgConfig, BG_LAYER_SUB_0, &template, BG_TYPE_STATIC);

    NARC *narc = NARC_ctor(NARC_INDEX_GRAPHIC__POKETCH, HEAP_ID_POKETCH_APP);

    Graphics_LoadTilesToBgLayerFromOpenNARC(narc, POKETCH_UNAVAILABLE_SCREEN_NARC_TILES_IDX, bgConfig, BG_LAYER_SUB_0, 0, 0, TRUE, HEAP_ID_POKETCH_APP);
    Graphics_LoadTilemapToBgLayerFromOpenNARC(narc, POKETCH_UNAVAILABLE_SCREEN_NARC_TILEMAP_IDX, bgConfig, BG_LAYER_SUB_0, 0, 0, TRUE, HEAP_ID_POKETCH_APP);
    Graphics_LoadPaletteFromOpenNARC(narc, POKETCH_UNAVAILABLE_SCREEN_NARC_PALETTE_IDX, PAL_LOAD_SUB_BG, 0, 0x20, HEAP_ID_POKETCH_APP);
    NARC_dtor(narc);

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GXS_SetVisibleWnd to PAL
    #endif
    GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    #else
    // TODO: Port GX_WNDMASK_NONE to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GXS_SetVisiblePlane to PAL
    #endif
    GXS_SetVisiblePlane(GX_PLANEMASK_BG0);
    #else
    // TODO: Port GX_PLANEMASK_BG0 to PAL
    #endif
}

void PoketchUnavailableScreen_Exit(BgConfig *bgConfig)
{
    Bg_FreeTilemapBuffer(bgConfig, BG_LAYER_SUB_0);
    Heap_Destroy(HEAP_ID_POKETCH_APP);
}

BOOL PoketchUnavailableScreen_IsDone(BgConfig *bgConfig)
{
    return TRUE;
}
