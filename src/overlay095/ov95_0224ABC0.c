#include "overlay095/ov95_0224ABC0.h"

#include <nitro.h>
#include <string.h>

#include "struct_defs/struct_02099F80.h"

#include "overlay095/ov95_02246C20.h"
#include "overlay095/ov95_022476F0.h"
#include "overlay095/struct_ov95_02247004_decl.h"
#include "overlay095/struct_ov95_022472C4_decl.h"
#include "overlay095/struct_ov95_0224773C_decl.h"
#include "overlay095/struct_ov95_02247958_decl.h"

#include "bg_window.h"
#include "camera.h"
#include "enums.h"
#include "graphics.h"
#include "gx_layers.h"
#include "heap.h"
#include "screen_fade.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "unk_0202419C.h"

enum {
    UnkEnum_ov95_0224AE1C_00 = -384
};

enum {
    UnkEnum_ov95_0224B1A8_00 = 0,
    UnkEnum_ov95_0224B1A8_01 = 40
};

enum {
    UnkEnum_ov95_0224AE1C_01 = -192
};

enum {
    UnkEnum_ov95_0224B1F8_00 = 192
};

enum {
    UnkEnum_ov95_0224B084_00 = 140 << FX32_SHIFT
};

enum {
    UnkEnum_ov95_0224B274_00 = 0x32000,
    UnkEnum_ov95_0224B274_01 = 16
};

enum {
    UnkEnum_ov95_0224AD98_00 = 0x400,
    UnkEnum_ov95_0224AD98_01 = 1,
    UnkEnum_ov95_0224AD98_02 = 0x128
};

typedef struct {
    TradeSequenceData *unk_00;
    int unk_04;
    int unk_08;
    BgConfig *unk_0C;
    UnkStruct_ov95_02247004 *unk_10;
    BOOL unk_14;
    UnkStruct_ov95_022472C4 *unk_18;
    volatile BOOL unk_1C;
    UnkStruct_ov95_0224773C *unk_20;
    UnkStruct_ov95_02247958 *unk_24;
    VecFx16 unk_28;
    SysTask *unk_30;
    SysTask *unk_34;
    SysTask *unk_38;
} UnkStruct_ov95_0224AC64;

typedef struct {
    UnkStruct_ov95_0224AC64 *unk_00;
    BgConfig *unk_04;
    int unk_08;
    fx32 unk_0C;
    fx32 unk_10;
    fx32 unk_14;
    fx32 unk_18;
    SysTask **unk_1C;
    SysTask *unk_20;
} UnkStruct_ov95_0224B1A8;

typedef struct {
    SysTask **unk_00;
    int unk_04;
    UnkStruct_ov95_02247958 *unk_08;
    VecFx32 unk_0C;
    fx32 unk_18;
} UnkStruct_ov95_0224B274;

typedef struct {
    SysTask **unk_00;
    fx32 unk_04;
    fx32 unk_08;
    fx32 unk_0C;
    int unk_10;
    UnkStruct_ov95_02247958 *unk_14;
} UnkStruct_ov95_0224B324;

static void ov95_0224AC64(UnkStruct_ov95_0224AC64 *param0);
static int ov95_0224AC98(UnkStruct_ov95_0224AC64 *param0, int *param1);
static int ov95_0224ACC8(UnkStruct_ov95_0224AC64 *param0, int *param1);
static int ov95_0224AD98(UnkStruct_ov95_0224AC64 *param0, int *param1);
static void ov95_0224AE1C(UnkStruct_ov95_0224AC64 *param0);
static void ov95_0224B050(UnkStruct_ov95_0224AC64 *param0);
static void ov95_0224B084(UnkStruct_ov95_0224AC64 *param0);
static void ov95_0224B194(UnkStruct_ov95_0224AC64 *param0);
static void ov95_0224B1A8(UnkStruct_ov95_0224AC64 *param0, SysTask **param1);
static void ov95_0224B1F8(SysTask *param0, void *param1);
static void ov95_0224B258(SysTask *param0);
static void ov95_0224B274(UnkStruct_ov95_0224AC64 *param0, SysTask **param1);
static void ov95_0224B2C8(SysTask *param0, void *param1);
static void ov95_0224B308(SysTask *param0);
static void ov95_0224B324(UnkStruct_ov95_02247958 *param0, int param1, int param2, int param3, SysTask **param4);
static void ov95_0224B388(SysTask *param0, void *param1);
static void ov95_0224B3BC(SysTask *param0);

void *ov95_0224ABC0(TradeSequenceData *param0)
{
    UnkStruct_ov95_0224AC64 *v0 = Heap_Alloc(HEAP_ID_58, sizeof(UnkStruct_ov95_0224AC64));

    if (v0) {
        int v1;

        v0->unk_00 = param0;
        v0->unk_04 = 0;
        v0->unk_0C = ov95_02247628(param0);
        v0->unk_20 = NULL;
        v0->unk_10 = NULL;
        v0->unk_14 = 0;
        v0->unk_18 = NULL;
        v0->unk_30 = NULL;
        v0->unk_34 = NULL;
        v0->unk_38 = NULL;
    }

    return v0;
}

void ov95_0224ABF4(void *param0)
{
    UnkStruct_ov95_0224AC64 *v0 = param0;

    if (v0) {
        int v1;

        ov95_0224B050(v0);
        ov95_0224B194(v0);

        if (v0->unk_30) {
            ov95_0224B308(v0->unk_30);
        }

        if (v0->unk_34) {
            ov95_0224B258(v0->unk_34);
        }

        if (v0->unk_38) {
            ov95_0224B3BC(v0->unk_38);
        }

        Heap_Free(v0);
    }
}

BOOL ov95_0224AC2C(void *param0, int *param1)
{
    static int (*const v0[])(UnkStruct_ov95_0224AC64 *, int *) = {
        ov95_0224AC98,
        ov95_0224ACC8,
        ov95_0224AD98
    };

    UnkStruct_ov95_0224AC64 *v1 = param0;

    if ((*param1) < NELEMS(v0)) {
        if (v0[*param1](v1, &(v1->unk_04))) {
            (*param1)++;
            v1->unk_04 = 0;
        }

        ov95_0224AC64(v1);

        return 0;
    }

    return 1;
}

static void ov95_0224AC64(UnkStruct_ov95_0224AC64 *param0)
{
    if (param0->unk_20) {
        param0->unk_28.y += 0x1000;

        ov95_02247990(param0->unk_24, &(param0->unk_28));
        G3X_Reset();
        ov95_02247770(param0->unk_20);
        #ifdef PLATFORM_DS
        G3_RequestSwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);
        #else
        // TODO: Port GX_BUFFERMODE_Z to PAL
        #endif
    }
}

static int ov95_0224AC98(UnkStruct_ov95_0224AC64 *param0, int *param1)
{
    ov95_0224AE1C(param0);
    ov95_0224B084(param0);
    StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_IN, FADE_TYPE_BRIGHTNESS_IN, COLOR_WHITE, 8, 1, HEAP_ID_58);

    return 1;
}

static int ov95_0224ACC8(UnkStruct_ov95_0224AC64 *param0, int *param1)
{
    switch (*param1) {
    case 0:
        if (IsScreenFadeDone()) {
            param0->unk_08 = 0;
            ov95_0224B274(param0, &(param0->unk_30));
            (*param1)++;
        }
        break;
    case 1:
        if (++(param0->unk_08) > 8) {
            ov95_0224B1A8(param0, &(param0->unk_34));
            (*param1)++;
        }
        break;
    case 2:
        if ((param0->unk_34 == NULL) && (param0->unk_30 == NULL)) {
            (*param1)++;
        }
        break;
    case 3:
        param0->unk_10 = ov95_02246F30(&(param0->unk_14), -1);
        param0->unk_08 = 0;
        (*param1)++;
        break;
    case 4:
        if (++(param0->unk_08) > 30) {
            ov95_0224B324(param0->unk_24, 31, 0, 40, &(param0->unk_38));
            (*param1)++;
        }
        break;
    case 5:
        if (param0->unk_38 == NULL) {
            ov95_02247004(param0->unk_10);
            (*param1)++;
        }
        break;
    case 6:
        if (param0->unk_14 == 0) {
            return 1;
        }
        break;
    }

    return 0;
}

static int ov95_0224AD98(UnkStruct_ov95_0224AC64 *param0, int *param1)
{
    switch (*param1) {
    case 0:
        param0->unk_18 = ov95_022472C4(param0->unk_0C, 0x1000, UnkEnum_ov95_0224AD98_00, UnkEnum_ov95_0224AD98_01, UnkEnum_ov95_0224AD98_02, 1, &(param0->unk_1C));
        param0->unk_08 = 0;
        (*param1)++;
        break;
    case 1:
        if (++(param0->unk_08) > 13) {
            StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_OUT, FADE_TYPE_BRIGHTNESS_OUT, COLOR_BLACK, 8, 1, HEAP_ID_58);
            (*param1)++;
        }
        break;
    case 2:
        if (IsScreenFadeDone() && (param0->unk_1C)) {
            return 1;
        }
        break;
    }

    return 0;
}

static void ov95_0224AE1C(UnkStruct_ov95_0224AC64 *param0)
{
    static const UnkStruct_02099F80 v0 = {
        #ifdef PLATFORM_DS
        GX_VRAM_BG_128_B,
        #else
        // TODO: Port GX_VRAM_BG_128_B to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_BGEXTPLTT_23_G,
        #else
        // TODO: Port GX_VRAM_BGEXTPLTT_23_G to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_SUB_BG_128_C,
        #else
        // TODO: Port GX_VRAM_SUB_BG_128_C to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_SUB_BGEXTPLTT_32_H,
        #else
        // TODO: Port GX_VRAM_SUB_BGEXTPLTT_32_H to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_OBJ_16_F,
        #else
        // TODO: Port GX_VRAM_OBJ_16_F to PAL
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
        GX_VRAM_TEX_0_A,
        #else
        // TODO: Port GX_VRAM_TEX_0_A to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_TEXPLTT_0123_E
        #else
        // TODO: Port GX_VRAM_TEXPLTT_0123_E to PAL
        #endif
    };
    static const GraphicsModes v1 = {
        #ifdef PLATFORM_DS
        GX_DISPMODE_GRAPHICS,
        #else
        // TODO: Port GX_DISPMODE_GRAPHICS to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_BGMODE_4,
        #else
        // TODO: Port GX_BGMODE_4 to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_BGMODE_4,
        #else
        // TODO: Port GX_BGMODE_4 to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_BG0_AS_3D
        #else
        // TODO: Port GX_BG0_AS_3D to PAL
        #endif
    };
    static const BgTemplate v2 = {
        .x = 0,
        .y = 0,
        .bufferSize = 0,
        .baseTile = 0,
        .screenSize = BG_SCREEN_SIZE_1024x1024,
        #ifdef PLATFORM_DS
        .colorMode = GX_BG_COLORMODE_256,
        #else
        // TODO: Port GX_BG_COLORMODE_256 to PAL
        #endif
        .screenBase = GX_BG_SCRBASE_0xb000,
        .charBase = GX_BG_CHARBASE_0x00000,
        #ifdef PLATFORM_DS
        .bgExtPltt = GX_BG_EXTPLTT_01,
        #else
        // TODO: Port GX_BG_EXTPLTT_01 to PAL
        #endif
        .priority = 2,
        .areaOver = 1,
        .mosaic = FALSE,
    };
    static const BgTemplate v3 = {
        .x = 0,
        .y = 0,
        .bufferSize = 0x800,
        .baseTile = 0,
        .screenSize = BG_SCREEN_SIZE_256x256,
        #ifdef PLATFORM_DS
        .colorMode = GX_BG_COLORMODE_256,
        #else
        // TODO: Port GX_BG_COLORMODE_256 to PAL
        #endif
        .screenBase = GX_BG_SCRBASE_0xf000,
        .charBase = GX_BG_CHARBASE_0x10000,
        #ifdef PLATFORM_DS
        .bgExtPltt = GX_BG_EXTPLTT_01,
        #else
        // TODO: Port GX_BG_EXTPLTT_01 to PAL
        #endif
        .priority = 3,
        .areaOver = 1,
        .mosaic = FALSE,
    };
    static const u16 v4[] = {
        0x20,
        0x60,
        0xa0,
        0xe0,
    };
    u32 v5;

    GXLayers_SetBanks(&v0);
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_SetDispSelect to PAL
    #endif
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    #else
    // TODO: Port GX_DISP_SELECT_MAIN_SUB to PAL
    #endif
    SetAllGraphicsModes(&v1);
    Bg_InitFromTemplate(param0->unk_0C, BG_LAYER_MAIN_2, &v2, 1);
    Bg_InitFromTemplate(param0->unk_0C, BG_LAYER_SUB_2, &v2, 1);

    {
        #ifdef PLATFORM_DS
        OSIntrMode v6 = OS_DisableInterrupts();
        #else
        // TODO: Port OS_DisableInterrupts to PAL
        #endif

        Bg_InitFromTemplate(param0->unk_0C, BG_LAYER_MAIN_3, &v3, 2);
        Bg_InitFromTemplate(param0->unk_0C, BG_LAYER_SUB_3, &v3, 2);

        #ifdef PLATFORM_DS
        OS_RestoreInterrupts(v6);
        #else
        // TODO: Port OS_RestoreInterrupts to PAL
        #endif
    }

    Graphics_LoadTilesToBgLayer(NARC_INDEX_GRAPHIC__DEMO_TRADE, 2, param0->unk_0C, 2, 0, 0, 1, HEAP_ID_58);
    Graphics_LoadTilesToBgLayer(NARC_INDEX_GRAPHIC__DEMO_TRADE, 2, param0->unk_0C, 6, 0, 0, 1, HEAP_ID_58);
    Graphics_LoadTilemapToBgLayer(NARC_INDEX_GRAPHIC__DEMO_TRADE, 1, param0->unk_0C, 2, 0, 0, 1, HEAP_ID_58);
    Graphics_LoadTilemapToBgLayer(NARC_INDEX_GRAPHIC__DEMO_TRADE, 1, param0->unk_0C, 6, 0, 0, 1, HEAP_ID_58);

    v5 = TradeSequence_GetBackground(param0->unk_00);

    Graphics_LoadPalette(NARC_INDEX_GRAPHIC__DEMO_TRADE, 3, 0, 0, 0x20, HEAP_ID_58);
    Graphics_LoadPalette(NARC_INDEX_GRAPHIC__DEMO_TRADE, 3, 4, 0, 0x20, HEAP_ID_58);
    Graphics_LoadPaletteWithSrcOffset(93, 3, 0, v4[v5], 0x20, 0x40, HEAP_ID_58);
    Graphics_LoadPaletteWithSrcOffset(93, 3, 4, v4[v5], 0x20, 0x40, HEAP_ID_58);

    Graphics_LoadTilesToBgLayer(NARC_INDEX_GRAPHIC__DEMO_TRADE, 5, param0->unk_0C, 3, 0, 0, 1, HEAP_ID_58);
    Graphics_LoadTilesToBgLayer(NARC_INDEX_GRAPHIC__DEMO_TRADE, 5, param0->unk_0C, 7, 0, 0, 1, HEAP_ID_58);
    Graphics_LoadTilemapToBgLayer(NARC_INDEX_GRAPHIC__DEMO_TRADE, 4, param0->unk_0C, 3, 0, 0, 1, HEAP_ID_58);
    Graphics_LoadTilemapToBgLayer(NARC_INDEX_GRAPHIC__DEMO_TRADE, 4, param0->unk_0C, 7, 0, 0, 1, HEAP_ID_58);

    {
        u8 *v7 = Heap_Alloc(HEAP_ID_58, 96);

        if (v7) {
            MI_CpuClear32(v7, 96);
            DC_FlushRange(v7, 96);

            #ifdef PLATFORM_DS
            GX_BeginLoadBGExtPltt();
            #else
            // TODO: Port GX_BeginLoadBGExtPltt to PAL
            #endif
            #ifdef PLATFORM_DS
            GXS_BeginLoadBGExtPltt();
            #else
            // TODO: Port GXS_BeginLoadBGExtPltt to PAL
            #endif
            #ifdef PLATFORM_DS
            GX_LoadBGExtPltt(v7, 0x6000, 96);
            #else
            // TODO: Port GX_LoadBGExtPltt to PAL
            #endif
            #ifdef PLATFORM_DS
            GXS_LoadBGExtPltt(v7, 0x6000, 96);
            #else
            // TODO: Port GXS_LoadBGExtPltt to PAL
            #endif
            #ifdef PLATFORM_DS
            GX_EndLoadBGExtPltt();
            #else
            // TODO: Port GX_EndLoadBGExtPltt to PAL
            #endif
            #ifdef PLATFORM_DS
            GXS_EndLoadBGExtPltt();
            #else
            // TODO: Port GXS_EndLoadBGExtPltt to PAL
            #endif

            Heap_Free(v7);
        }
    }

    Bg_SetOffset(param0->unk_0C, BG_LAYER_MAIN_2, 3, UnkEnum_ov95_0224AE1C_00);
    Bg_SetOffset(param0->unk_0C, BG_LAYER_SUB_2, 3, UnkEnum_ov95_0224AE1C_01);
    Bg_SetOffset(param0->unk_0C, BG_LAYER_MAIN_3, 3, 67);

    Bg_ToggleLayer(BG_LAYER_SUB_3, 0);

    #ifdef PLATFORM_DS
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_OBJ, 1);
    #else
    // TODO: Port GX_PLANEMASK_OBJ to PAL
    #endif
    #ifdef PLATFORM_DS
    GXLayers_EngineBToggleLayers(GX_PLANEMASK_OBJ, 1);
    #else
    // TODO: Port GX_PLANEMASK_OBJ to PAL
    #endif
}

static void ov95_0224B050(UnkStruct_ov95_0224AC64 *param0)
{
    if (param0->unk_14) {
        ov95_02247018(param0->unk_10);
    }

    Bg_FreeTilemapBuffer(param0->unk_0C, BG_LAYER_MAIN_2);
    Bg_FreeTilemapBuffer(param0->unk_0C, BG_LAYER_SUB_2);
    Bg_FreeTilemapBuffer(param0->unk_0C, BG_LAYER_MAIN_3);
    Bg_FreeTilemapBuffer(param0->unk_0C, BG_LAYER_SUB_3);
}

static void ov95_0224B084(UnkStruct_ov95_0224AC64 *param0)
{
    static const GXRgb v0[8] = {
        #ifdef PLATFORM_DS
        GX_RGB(4, 4, 4),
        #else
        // TODO: Port GX_RGB to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_RGB(4, 4, 4),
        #else
        // TODO: Port GX_RGB to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_RGB(4, 4, 4),
        #else
        // TODO: Port GX_RGB to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_RGB(4, 4, 4),
        #else
        // TODO: Port GX_RGB to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_RGB(4, 4, 4),
        #else
        // TODO: Port GX_RGB to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_RGB(4, 4, 4),
        #else
        // TODO: Port GX_RGB to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_RGB(4, 4, 4),
        #else
        // TODO: Port GX_RGB to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_RGB(4, 4, 4)
        #else
        // TODO: Port GX_RGB to PAL
        #endif
    };
    int v1;

    #ifdef PLATFORM_DS
    NNS_G3dInit();
    #else
    // TODO: Port NNS_G3dInit to PAL
    #endif

    G3X_InitMtxStack();
    #ifdef PLATFORM_DS
    G3X_SetShading(GX_SHADING_TOON);
    #else
    // TODO: Port GX_SHADING_TOON to PAL
    #endif
    G3X_AntiAlias(1);
    G3X_AlphaTest(0, 0);
    G3X_AlphaBlend(1);
    G3X_EdgeMarking(1);
    G3X_SetEdgeColorTable(v0);
    #ifdef PLATFORM_DS
    G3X_SetClearColor(GX_RGB(31, 31, 16), 0, 0x7fff, 63, 0);
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_SORTMODE_AUTO to PAL
    #endif
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
    #else
    // TODO: Port GX_BUFFERMODE_W to PAL
    #endif
    G3_ViewPort(0, 0, 255, 191);

    #ifdef PLATFORM_DS
    NNS_GfdInitFrmTexVramManager(1, 1);
    #else
    // TODO: Port NNS_GfdInitFrmTexVramManager to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_GfdInitFrmPlttVramManager(0x4000, 1);
    #else
    // TODO: Port NNS_GfdInitFrmPlttVramManager to PAL
    #endif

    #ifdef PLATFORM_DS
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_BG0, 1);
    #else
    // TODO: Port GX_PLANEMASK_BG0 to PAL
    #endif
    #ifdef PLATFORM_DS
    G2_SetBG0Priority(0);
    #else
    // TODO: Port G2_SetBG0Priority to PAL
    #endif

    param0->unk_20 = ov95_022476F0(1, 0, 0, 0);
    param0->unk_24 = ov95_022478B4(param0->unk_20, 0, NARC_INDEX_GRAPHIC__DEMO_TRADE, 27, 0, UnkEnum_ov95_0224B084_00, 0, 1);

    {
        static CameraAngle v2;

        v2.x = 0;
        v2.y = 0;
        v2.z = 0;

        ov95_02247AB0(param0->unk_20, &v2);
        ov95_02247AD0(param0->unk_20, 1);
    }

    ov95_022479D4(param0->unk_24, 0x2000);
    ov95_02247978(param0->unk_24, &param0->unk_28);
}

static void ov95_0224B194(UnkStruct_ov95_0224AC64 *param0)
{
    if (param0->unk_20) {
        ov95_0224773C(param0->unk_20);
        param0->unk_20 = NULL;
    }
}

static void ov95_0224B1A8(UnkStruct_ov95_0224AC64 *param0, SysTask **param1)
{
    UnkStruct_ov95_0224B1A8 *v0 = Heap_Alloc(HEAP_ID_58, sizeof(UnkStruct_ov95_0224B1A8));

    if (v0) {
        v0->unk_00 = param0;
        v0->unk_04 = param0->unk_0C;
        v0->unk_0C = UnkEnum_ov95_0224AE1C_00 << 12;
        v0->unk_10 = UnkEnum_ov95_0224AE1C_01 << 12;
        v0->unk_14 = ((UnkEnum_ov95_0224B1A8_00 - UnkEnum_ov95_0224AE1C_00) << 12) / UnkEnum_ov95_0224B1A8_01;
        v0->unk_08 = UnkEnum_ov95_0224B1A8_01;
        v0->unk_1C = param1;
        *param1 = SysTask_ExecuteOnVBlank(ov95_0224B1F8, v0, 0);

        if (*param1 == NULL) {
            Heap_Free(v0);
        }
    }
}

static void ov95_0224B1F8(SysTask *param0, void *param1)
{
    UnkStruct_ov95_0224B1A8 *v0 = param1;

    if (v0->unk_08) {
        v0->unk_0C += v0->unk_14;
        v0->unk_10 += v0->unk_14;
        Bg_SetOffset(v0->unk_04, BG_LAYER_MAIN_2, 3, v0->unk_0C >> 12);
        Bg_SetOffset(v0->unk_04, BG_LAYER_SUB_2, 3, v0->unk_10 >> 12);
        v0->unk_08--;
    } else {
        Bg_SetOffset(v0->unk_04, BG_LAYER_MAIN_2, 3, UnkEnum_ov95_0224B1A8_00);
        Bg_SetOffset(v0->unk_04, BG_LAYER_SUB_2, 3, UnkEnum_ov95_0224B1F8_00);
        ov95_0224B258(param0);
    }
}

static void ov95_0224B258(SysTask *param0)
{
    if (param0) {
        UnkStruct_ov95_0224B1A8 *v0 = SysTask_GetParam(param0);

        *(v0->unk_1C) = NULL;
        ov95_022476C8(v0);
        SysTask_Done(param0);
    }
}

static void ov95_0224B274(UnkStruct_ov95_0224AC64 *param0, SysTask **param1)
{
    UnkStruct_ov95_0224B274 *v0 = Heap_Alloc(HEAP_ID_58, sizeof(UnkStruct_ov95_0224B274));

    if (v0) {
        v0->unk_00 = param1;
        v0->unk_08 = param0->unk_24;

        ov95_02247958(v0->unk_08, &v0->unk_0C);

        v0->unk_18 = (UnkEnum_ov95_0224B274_00 - v0->unk_0C.y) / UnkEnum_ov95_0224B274_01;
        v0->unk_04 = UnkEnum_ov95_0224B274_01;
        *param1 = SysTask_Start(ov95_0224B2C8, v0, 0);

        if (*param1 == NULL) {
            Heap_Free(v0);
        }
    }
}

static void ov95_0224B2C8(SysTask *param0, void *param1)
{
    UnkStruct_ov95_0224B274 *v0 = param1;

    if (v0->unk_04) {
        v0->unk_0C.y += v0->unk_18;
        ov95_02247968(v0->unk_08, &v0->unk_0C);
        v0->unk_04--;
    } else {
        v0->unk_0C.y = UnkEnum_ov95_0224B274_00;
        ov95_02247968(v0->unk_08, &v0->unk_0C);
        ov95_0224B308(param0);
    }
}

static void ov95_0224B308(SysTask *param0)
{
    if (param0) {
        UnkStruct_ov95_0224B274 *v0 = SysTask_GetParam(param0);

        *(v0->unk_00) = NULL;

        Heap_Free(v0);
        SysTask_Done(param0);
    }
}

static void ov95_0224B324(UnkStruct_ov95_02247958 *param0, int param1, int param2, int param3, SysTask **param4)
{
    UnkStruct_ov95_0224B324 *v0 = Heap_Alloc(HEAP_ID_58, sizeof(UnkStruct_ov95_0224B324));

    if (v0) {
        v0->unk_00 = param4;
        v0->unk_04 = param1 << 12;
        v0->unk_0C = param2 << 12;
        v0->unk_08 = (v0->unk_0C - v0->unk_04) / param3;
        v0->unk_10 = param3;
        v0->unk_14 = param0;
        ov95_022479AC(param0, param1);
        *param4 = SysTask_Start(ov95_0224B388, v0, 0);

        if (*param4 == NULL) {
            Heap_Free(v0);
        }
    } else {
        *param4 = NULL;
    }
}

static void ov95_0224B388(SysTask *param0, void *param1)
{
    UnkStruct_ov95_0224B324 *v0 = param1;

    if (--(v0->unk_10) > 0) {
        v0->unk_04 += v0->unk_08;
        ov95_022479AC(v0->unk_14, v0->unk_04 >> 12);
    } else {
        ov95_022479AC(v0->unk_14, v0->unk_0C >> 12);
        ov95_0224B3BC(param0);
    }
}

static void ov95_0224B3BC(SysTask *param0)
{
    if (param0) {
        UnkStruct_ov95_0224B324 *v0 = SysTask_GetParam(param0);

        *(v0->unk_00) = NULL;
        Heap_Free(v0);
        SysTask_Done(param0);
    }
}
