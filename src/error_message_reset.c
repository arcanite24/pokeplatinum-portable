#include "error_message_reset.h"

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
#include "communication_system.h"
#include "font.h"
#include "graphics.h"
#include "gx_layers.h"
#include "heap.h"
#include "main.h"
#include "message.h"
#include "render_window.h"
#include "screen_fade.h"
#include "strbuf.h"
#include "system.h"
#include "text.h"
#include "unk_020366A0.h"

static const UnkStruct_02099F80 sErrorMessageBanksConfig = {
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

static const GraphicsModes sErrorMessageBgModeSet = {
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

static const BgTemplate sErrorMessageBgTemplate = {
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

static const WindowTemplate sErrorMessageWindowTemplate = {
    0x0,
    0x3,
    0x3,
    0x1A,
    0x12,
    0x1,
    0x23
};

static const HeapParam sErrorMessageHeapParams[] = {
    { .size = 0x20000,
        #ifdef PLATFORM_DS
        .arena = OS_ARENA_MAIN }
        #else
        // TODO: Port OS_ARENA_MAIN to PAL
        #endif
};

static BOOL sErrorMessagePrinterLock;

static void VBlankIntr(void)
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

void ErrorMessageReset_PrintErrorAndReset(void)
{
    BgConfig *bgConfig;
    Window window;
    MessageLoader *errorMsgData;
    Strbuf *errorString;
    int v4;
    int heapID = HEAP_ID_SYSTEM;

    if (sErrorMessagePrinterLock == TRUE) {
        return;
    }

    sErrorMessagePrinterLock = TRUE;

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_InitArenaHiAndLo to PAL
    #endif
    OS_InitArenaHiAndLo(OS_ARENA_MAIN);
    #else
    // TODO: Port OS_ARENA_MAIN to PAL
    #endif
    Heap_InitSystem(sErrorMessageHeapParams, NELEMS(sErrorMessageHeapParams), NELEMS(sErrorMessageHeapParams), 0);

    v4 = 3;

    SetScreenColorBrightness(DS_SCREEN_MAIN, COLOR_BLACK);
    SetScreenColorBrightness(DS_SCREEN_SUB, COLOR_BLACK);

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_DisableIrqMask to PAL
    #endif
    OS_DisableIrqMask(OS_IE_V_BLANK);
    #else
    // TODO: Port OS_IE_V_BLANK to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_SetIrqFunction to PAL
    #endif
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    #else
    // TODO: Port OS_IE_V_BLANK to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_EnableIrqMask to PAL
    #endif
    OS_EnableIrqMask(OS_IE_V_BLANK);
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

    GXLayers_SetBanks(&sErrorMessageBanksConfig);
    bgConfig = BgConfig_New(heapID);

    SetAllGraphicsModes(&sErrorMessageBgModeSet);
    Bg_InitFromTemplate(bgConfig, BG_LAYER_MAIN_0, &sErrorMessageBgTemplate, 0);
    Bg_ClearTilemap(bgConfig, BG_LAYER_MAIN_0);
    LoadStandardWindowGraphics(bgConfig, BG_LAYER_MAIN_0, 512 - 9, 2, 0, heapID);
    Font_LoadTextPalette(PAL_LOAD_MAIN_BG, 1 * (2 * 16), heapID);
    Bg_ClearTilesRange(BG_LAYER_MAIN_0, 32, 0, heapID);
    Bg_MaskPalette(BG_LAYER_MAIN_0, 0x6c21);
    Bg_MaskPalette(BG_LAYER_SUB_0, 0x6c21);

    errorMsgData = MessageLoader_Init(MESSAGE_LOADER_NARC_HANDLE, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_NETWORK_ERRORS, heapID);
    errorString = Strbuf_Init(0x180, heapID);

    Text_ResetAllPrinters();

    Window_AddFromTemplate(bgConfig, &window, &sErrorMessageWindowTemplate);
    Window_FillRectWithColor(&window, 15, 0, 0, 26 * 8, 18 * 8);
    Window_DrawStandardFrame(&window, 0, 512 - 9, 2);
    MessageLoader_GetStrbuf(errorMsgData, v4, errorString);
    Text_AddPrinterWithParams(&window, FONT_SYSTEM, errorString, 0, 0, TEXT_SPEED_INSTANT, NULL);
    Strbuf_Free(errorString);

    GXLayers_TurnBothDispOn();
    ResetScreenMasterBrightness(DS_SCREEN_MAIN);
    ResetScreenMasterBrightness(DS_SCREEN_SUB);
    #ifdef PLATFORM_DS
    BrightnessController_SetScreenBrightness(0, GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, BRIGHTNESS_BOTH_SCREENS);
    #else
    // TODO: Port GX_BLEND_PLANEMASK_BD to PAL
    #endif
    sub_02037DB0();

    while (TRUE) {
        HandleConsoleFold();
        CommSys_Update();

        if (sub_02038AB8()) {
            break;
        }

        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port OS_WaitIrq to PAL
        #endif
        OS_WaitIrq(1, OS_IE_V_BLANK);
        #else
        // TODO: Port OS_IE_V_BLANK to PAL
        #endif
    }

    while (TRUE) {
        HandleConsoleFold();

        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
                #ifdef PLATFORM_DS
        #endif
        if (PAD_Read() & PAD_BUTTON_A) {
        #else
        // TODO: Port PAD_BUTTON_A to PAL
        #endif
            break;
        }

        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port OS_WaitIrq to PAL
        #endif
        OS_WaitIrq(1, OS_IE_V_BLANK);
        #else
        // TODO: Port OS_IE_V_BLANK to PAL
        #endif
    }

    SetScreenColorBrightness(DS_SCREEN_MAIN, COLOR_WHITE);
    SetScreenColorBrightness(DS_SCREEN_SUB, COLOR_WHITE);

    Window_Remove(&window);
    MessageLoader_Free(errorMsgData);
    Heap_Free(bgConfig);

    #ifdef PLATFORM_DS
    OS_ResetSystem(0);
    #else
    // TODO: Port OS_ResetSystem to PAL
    #endif
}
