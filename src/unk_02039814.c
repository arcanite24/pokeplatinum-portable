#include "unk_02039814.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "constants/graphics.h"

#include "struct_defs/struct_02099F80.h"

#include "bg_window.h"
#include "brightness_controller.h"
#include "font.h"
#include "gx_layers.h"
#include "heap.h"
#include "message.h"
#include "render_window.h"
#include "screen_fade.h"
#include "strbuf.h"
#include "string_template.h"
#include "system.h"
#include "text.h"

#include "res/text/bank/network_errors.h"

static const UnkStruct_02099F80 Unk_020E5EFC = {
    #ifdef PLATFORM_DS
    GX_VRAM_BG_256_AB,
    #else
    // TODO: Port GX_VRAM_BG_256_AB to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_VRAM_BGEXTPLTT_NONE,
    #else
    // TODO: Port GX_VRAM_BGEXTPLTT_NONE to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_VRAM_SUB_BG_NONE,
    #else
    // TODO: Port GX_VRAM_SUB_BG_NONE to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_VRAM_SUB_BGEXTPLTT_NONE,
    #else
    // TODO: Port GX_VRAM_SUB_BGEXTPLTT_NONE to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_VRAM_OBJ_NONE,
    #else
    // TODO: Port GX_VRAM_OBJ_NONE to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_VRAM_OBJEXTPLTT_NONE,
    #else
    // TODO: Port GX_VRAM_OBJEXTPLTT_NONE to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_VRAM_SUB_OBJ_NONE,
    #else
    // TODO: Port GX_VRAM_SUB_OBJ_NONE to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_VRAM_SUB_OBJEXTPLTT_NONE,
    #else
    // TODO: Port GX_VRAM_SUB_OBJEXTPLTT_NONE to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_VRAM_TEX_NONE,
    #else
    // TODO: Port GX_VRAM_TEX_NONE to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_VRAM_TEXPLTT_NONE
    #else
    // TODO: Port GX_VRAM_TEXPLTT_NONE to PAL
    #endif
};

static const GraphicsModes Unk_020E5ED0 = {
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

static const BgTemplate Unk_020E5EE0 = {
    .x = 0x0,
    .y = 0x0,
    .bufferSize = 0x800,
    .baseTile = 0x0,
    .screenSize = BG_SCREEN_SIZE_256x256,
    #ifdef PLATFORM_DS
    .colorMode = GX_BG_COLORMODE_16,
    #else
    // TODO: Port GX_BG_COLORMODE_16 to PAL
    #endif
    .screenBase = GX_BG_SCRBASE_0x0000,
    .charBase = GX_BG_CHARBASE_0x18000,
    #ifdef PLATFORM_DS
    .bgExtPltt = GX_BG_EXTPLTT_01,
    #else
    // TODO: Port GX_BG_EXTPLTT_01 to PAL
    #endif
    .priority = 0x1,
    .areaOver = 0x0,
    .mosaic = FALSE,
};

static const WindowTemplate Unk_020E5EC8 = {
    0x0,
    0x3,
    0x3,
    0x1A,
    0x12,
    0x1,
    0x23
};

static void sub_02039814(void)
{
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_SetIrqCheckFlag to PAL
    #endif
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
    #else
    // TODO: Port OS_IE_V_BLANK to PAL
    #endif
    #ifdef PLATFORM_DS
    MI_WaitDma(GX_DEFAULT_DMAID);
    #else
    // TODO: Port GX_DEFAULT_DMAID to PAL
    #endif
}

void NetworkError_DisplayNetworkError(int heapID, int networkErrorId, int errorCode)
{
    BgConfig *v0;
    Window v1;
    MessageLoader *v2;
    Strbuf *v3;
    Strbuf *v4;
    StringTemplate *v5;
    int networkErrorMessageId;

    switch (networkErrorId) {
    case 0:
    default:
        networkErrorMessageId = NetworkError_Text_Generic;
        break;
    case 1:
        networkErrorMessageId = pl_msg_00000214_00002;
        break;
    case 2:
        networkErrorMessageId = pl_msg_00000214_00003;
        break;
    case 3:
        networkErrorMessageId = NetworkError_Text_GTSUnreachable;
        break;
    case 4:
        networkErrorMessageId = pl_msg_00000214_00005;
        break;
    case 5:
        networkErrorMessageId = pl_msg_00000214_00006;
        break;
    case 6:
        networkErrorMessageId = pl_msg_00000214_00007;
        break;
    }

    SetScreenColorBrightness(DS_SCREEN_MAIN, COLOR_BLACK);
    SetScreenColorBrightness(DS_SCREEN_SUB, COLOR_BLACK);

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_DisableIrqMask to PAL
    #endif
    (void)OS_DisableIrqMask(OS_IE_V_BLANK);
    #else
    // TODO: Port OS_IE_V_BLANK to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_SetIrqFunction to PAL
    #endif
    OS_SetIrqFunction(OS_IE_V_BLANK, sub_02039814);
    #else
    // TODO: Port OS_IE_V_BLANK to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_EnableIrqMask to PAL
    #endif
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    #else
    // TODO: Port OS_IE_V_BLANK to PAL
    #endif

    SetVBlankCallback(NULL, NULL);
    SetHBlankCallback(NULL, NULL);
    GXLayers_DisableEngineALayers();
    GXLayers_DisableEngineBLayers();

    #ifdef PLATFORM_DS
    GX_SetVisiblePlane(0);
    #else
    // TODO: Port GX_SetVisiblePlane to PAL
    #endif
    #ifdef PLATFORM_DS
    GXS_SetVisiblePlane(0);
    #else
    // TODO: Port GXS_SetVisiblePlane to PAL
    #endif

    SetAutorepeat(4, 8);
    gSystem.whichScreenIs3D = DS_SCREEN_MAIN;
    GXLayers_SwapDisplay();

    #ifdef PLATFORM_DS
    G2_BlendNone();
    #else
    // TODO: Port G2_BlendNone to PAL
    #endif
    G2S_BlendNone();
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_SetVisibleWnd to PAL
    #endif
    GX_SetVisibleWnd(GX_WNDMASK_NONE);
    #else
    // TODO: Port GX_WNDMASK_NONE to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GXS_SetVisibleWnd to PAL
    #endif
    GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    #else
    // TODO: Port GX_WNDMASK_NONE to PAL
    #endif

    GXLayers_SetBanks(&Unk_020E5EFC);
    v0 = BgConfig_New(heapID);

    SetAllGraphicsModes(&Unk_020E5ED0);
    Bg_InitFromTemplate(v0, BG_LAYER_MAIN_0, &Unk_020E5EE0, 0);
    Bg_ClearTilemap(v0, BG_LAYER_MAIN_0);
    LoadStandardWindowGraphics(v0, BG_LAYER_MAIN_0, 512 - 9, 2, 0, heapID);
    Font_LoadTextPalette(0, 1 * (2 * 16), heapID);
    Bg_ClearTilesRange(BG_LAYER_MAIN_0, 32, 0, heapID);
    Bg_MaskPalette(BG_LAYER_MAIN_0, 0x6c21);
    Bg_MaskPalette(BG_LAYER_SUB_0, 0x6c21);

    v2 = MessageLoader_Init(MESSAGE_LOADER_NARC_HANDLE, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_NETWORK_ERRORS, heapID);
    v3 = Strbuf_Init(0x180, heapID);
    v4 = Strbuf_Init(0x180, heapID);
    Text_ResetAllPrinters();
    v5 = StringTemplate_Default(heapID);

    Window_AddFromTemplate(v0, &v1, &Unk_020E5EC8);
    Window_FillRectWithColor(&v1, 15, 0, 0, 26 * 8, 18 * 8);
    Window_DrawStandardFrame(&v1, 0, 512 - 9, 2);

    StringTemplate_SetNumber(v5, 0, errorCode, 5, 2, 1);
    MessageLoader_GetStrbuf(v2, networkErrorMessageId, v4);
    StringTemplate_Format(v5, v3, v4);

    Text_AddPrinterWithParams(&v1, FONT_SYSTEM, v3, 0, 0, TEXT_SPEED_INSTANT, NULL);
    Strbuf_Free(v3);

    GXLayers_TurnBothDispOn();
    ResetScreenMasterBrightness(DS_SCREEN_MAIN);
    ResetScreenMasterBrightness(DS_SCREEN_SUB);
    #ifdef PLATFORM_DS
    BrightnessController_SetScreenBrightness(0, GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, BRIGHTNESS_BOTH_SCREENS);
    #else
    // TODO: Port GX_BLEND_PLANEMASK_BD to PAL
    #endif

    Window_Remove(&v1);
    MessageLoader_Free(v2);
    StringTemplate_Free(v5);
    Heap_Free(v0);
}
