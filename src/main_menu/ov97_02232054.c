#include "main_menu/ov97_02232054.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "struct_defs/struct_02099F80.h"

#include "main_menu/mystery_gift_app.h"
#include "savedata/save_table.h"

#include "bg_window.h"
#include "gx_layers.h"
#include "mystery_gift.h"
#include "savedata.h"

void ov97_02232054(void)
{
    UnkStruct_02099F80 v0 = {
        #ifdef PLATFORM_DS
        GX_VRAM_BG_128_C,
        #else
        // TODO: Port GX_VRAM_BG_128_C to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_BGEXTPLTT_NONE,
        #else
        // TODO: Port GX_VRAM_BGEXTPLTT_NONE to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_SUB_BG_32_H,
        #else
        // TODO: Port GX_VRAM_SUB_BG_32_H to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_SUB_BGEXTPLTT_NONE,
        #else
        // TODO: Port GX_VRAM_SUB_BGEXTPLTT_NONE to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_OBJ_64_E,
        #else
        // TODO: Port GX_VRAM_OBJ_64_E to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_OBJEXTPLTT_NONE,
        #else
        // TODO: Port GX_VRAM_OBJEXTPLTT_NONE to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_SUB_OBJ_16_I,
        #else
        // TODO: Port GX_VRAM_SUB_OBJ_16_I to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_SUB_OBJEXTPLTT_NONE,
        #else
        // TODO: Port GX_VRAM_SUB_OBJEXTPLTT_NONE to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_TEX_0_B,
        #else
        // TODO: Port GX_VRAM_TEX_0_B to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_TEXPLTT_01_FG
        #else
        // TODO: Port GX_VRAM_TEXPLTT_01_FG to PAL
        #endif
    };

    GXLayers_SetBanks(&v0);
}

void ov97_02232074(BgConfig *param0)
{
    {
        GraphicsModes v0 = {
            #ifdef PLATFORM_DS
            GX_DISPMODE_GRAPHICS,
            #else
            // TODO: Port GX_DISPMODE_GRAPHICS to PAL
            #endif
            #ifdef PLATFORM_DS
            GX_BGMODE_0,
            #else
            // TODO: Port GX_BGMODE_0 to PAL
            #endif
            #ifdef PLATFORM_DS
            GX_BGMODE_0,
            #else
            // TODO: Port GX_BGMODE_0 to PAL
            #endif
            #ifdef PLATFORM_DS
            GX_BG0_AS_2D
            #else
            // TODO: Port GX_BG0_AS_2D to PAL
            #endif
        };

        SetAllGraphicsModes(&v0);
    }

    {
        BgTemplate v1 = {
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
            .screenBase = GX_BG_SCRBASE_0xe000,
            .charBase = GX_BG_CHARBASE_0x00000,
            #ifdef PLATFORM_DS
            .bgExtPltt = GX_BG_EXTPLTT_01,
            #else
            // TODO: Port GX_BG_EXTPLTT_01 to PAL
            #endif
            .priority = 1,
            .areaOver = 0,
            .mosaic = FALSE,
        };

        Bg_InitFromTemplate(param0, BG_LAYER_MAIN_0, &v1, 0);
        Bg_ClearTilemap(param0, BG_LAYER_MAIN_0);
    }

    {
        BgTemplate v2 = {
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
            .screenBase = GX_BG_SCRBASE_0xe800,
            .charBase = GX_BG_CHARBASE_0x08000,
            #ifdef PLATFORM_DS
            .bgExtPltt = GX_BG_EXTPLTT_01,
            #else
            // TODO: Port GX_BG_EXTPLTT_01 to PAL
            #endif
            .priority = 2,
            .areaOver = 0,
            .mosaic = FALSE,
        };

        Bg_InitFromTemplate(param0, BG_LAYER_MAIN_1, &v2, 0);
        Bg_ClearTilemap(param0, BG_LAYER_MAIN_1);
    }

    {
        BgTemplate v3 = {
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
            .screenBase = GX_BG_SCRBASE_0xf000,
            .charBase = GX_BG_CHARBASE_0x00000,
            #ifdef PLATFORM_DS
            .bgExtPltt = GX_BG_EXTPLTT_23,
            #else
            // TODO: Port GX_BG_EXTPLTT_23 to PAL
            #endif
            .priority = 0,
            .areaOver = 0,
            .mosaic = FALSE,
        };

        Bg_InitFromTemplate(param0, BG_LAYER_MAIN_2, &v3, 0);
        Bg_ClearTilemap(param0, BG_LAYER_MAIN_2);
    }

    {
        BgTemplate v4 = {
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
            .screenBase = GX_BG_SCRBASE_0xf800,
            .charBase = GX_BG_CHARBASE_0x0c000,
            #ifdef PLATFORM_DS
            .bgExtPltt = GX_BG_EXTPLTT_23,
            #else
            // TODO: Port GX_BG_EXTPLTT_23 to PAL
            #endif
            .priority = 3,
            .areaOver = 0,
            .mosaic = FALSE,
        };

        Bg_InitFromTemplate(param0, BG_LAYER_MAIN_3, &v4, 0);
        Bg_ClearTilemap(param0, BG_LAYER_MAIN_3);
    }
}

int ov97_02232148(SaveData *saveData, MysteryGiftEventData *param1)
{
    u32 v0;
    MysteryGift *v1 = SaveData_GetMysteryGift(saveData);

    v0 = param1->header.validGames;

    if ((v0 == 0xFFFFFFFF) && (param1->header.id == 0xFFFF)) {
        MysteryGift_Init(v1);
        SaveData_SaveBlock(saveData, 0);

        #ifdef PLATFORM_DS
        OS_ResetSystem(0);
        #else
        // TODO: Port OS_ResetSystem to PAL
        #endif
    }

    if ((param1->header.id >= 100) && (param1->header.id <= 152)) {
        v0 |= (1 << 12) + (1 << 7) + (1 << 8);
    }

    if (v0 == 0) {
        v0 = ~0;
    }

    if ((v0 & (1 << GAME_VERSION)) == 0) {
        return 1;
    }

    if ((param1->header.unique == 1) && (MysteryGift_GetWcIDReceived(v1, param1->header.id) == 1)) {
        return 2;
    }

    if ((param1->header.hasWonderCard == 1) && (MysteryGift_CheckFreeWcSlotExists(v1) == 0)) {
        return 4;
    }

    if (MysteryGift_CheckFreePgtSlotExists(v1) == 0) {
        return 3;
    }

    if (param1->header.fromSharing == 1) {
        return 5;
    }

    return 0;
}
