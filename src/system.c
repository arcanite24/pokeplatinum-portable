#include "system.h"

#include "nitro/pad/common/pad.h"
#include <string.h>

#include "constants/graphics.h"
#include "constants/heap.h"

#include "boot.h"
#include "heap.h"
#include "math_util.h"
#include "sys_task_manager.h"

#ifdef PLATFORM_SDL
#include "platform/pal_input.h"
#include "platform/pal_graphics.h"
#include "platform/pal_background.h"
#include "platform/pal_sprite.h"
#include <stdio.h>
#include <stdlib.h>
#endif

#define MAIN_TASK_MAX        160
#define VBLANK_TASK_MAX      32
#define POST_VBLANK_TASK_MAX 32
#define PRINT_TASK_MAX       4

#define CACHE_ENTRY_MAX 128

typedef struct CacheEntry {
    void *data;
    u32 hash;
} CacheEntry;

static void VBlankIntr(void);
static void DummyVBlankIntr(void);
static void HBlankIntr(void);
static void SetHBlankEnabled(BOOL param0);
static void InitHeapSystem(void);
static void ApplyButtonModeToInput(void);

System gSystem;

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
    SysTaskManager_ExecuteTasks(gSystem.vBlankTaskMgr);
    gSystem.frameCounter++;
}

static void DummyVBlankIntr(void)
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

void SetDummyVBlankIntr(void)
{
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
    OS_SetIrqFunction(OS_IE_V_BLANK, DummyVBlankIntr);
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
}

void SetVBlankCallback(Callback cb, void *data)
{
    gSystem.vblankCallback = cb;
    gSystem.vblankCallbackData = data;
}

void DisableHBlank(void)
{
    SetHBlankEnabled(FALSE);
    gSystem.hblankCallback = NULL;
    gSystem.hblankCallbackData = NULL;
}

BOOL SetHBlankCallback(Callback cb, void *data)
{
    if (cb == NULL) {
        SetHBlankEnabled(FALSE);
        gSystem.hblankCallback = NULL;
        gSystem.hblankCallbackData = NULL;
        return TRUE;
    } else {
        if (gSystem.hblankCallback == NULL) {
            gSystem.hblankCallbackData = data;
            gSystem.hblankCallback = cb;
            SetHBlankEnabled(TRUE);
            return TRUE;
        } else {
            return FALSE;
        }
    }
}

static void HBlankIntr(void)
{
    if (gSystem.hblankCallback) {
        gSystem.hblankCallback(gSystem.hblankCallbackData);
    }
}

static void SetHBlankEnabled(BOOL enabled)
{
    #ifdef PLATFORM_DS
    OS_DisableIrq();
    #else
    // TODO: Port OS_DisableIrq to PAL
    #endif
    if (!enabled) {
        #ifdef PLATFORM_DS
        OSIrqMask savedMask = OS_GetIrqMask();
        #else
        // TODO: Port OS_GetIrqMask to PAL
        #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port OS_DisableIrqMask to PAL
        #endif
        OS_DisableIrqMask(OS_IE_H_BLANK);
        #else
        // TODO: Port OS_IE_H_BLANK to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_HBlankIntr(FALSE);
        #else
        // TODO: Port GX_HBlankIntr to PAL
        #endif
    } else {
        #ifdef PLATFORM_DS
        OSIrqMask savedMask = OS_GetIrqMask();
        #else
        // TODO: Port OS_GetIrqMask to PAL
        #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port OS_SetIrqFunction to PAL
        #endif
        OS_SetIrqFunction(OS_IE_H_BLANK, HBlankIntr);
        #else
        // TODO: Port OS_IE_H_BLANK to PAL
        #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port OS_EnableIrqMask to PAL
        #endif
        OS_EnableIrqMask(OS_IE_H_BLANK);
        #else
        // TODO: Port OS_IE_H_BLANK to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_HBlankIntr(TRUE);
        #else
        // TODO: Port GX_HBlankIntr to PAL
        #endif
    }
    #ifdef PLATFORM_DS
    OS_EnableIrq();
    #else
    // TODO: Port OS_EnableIrq to PAL
    #endif
}

static const HeapParam sHeapInitParams[] = {
    #ifdef PLATFORM_DS
    { HEAP_SIZE_SYSTEM, OS_ARENA_MAIN },
    #else
    // TODO: Port OS_ARENA_MAIN to PAL
    #endif
    #ifdef PLATFORM_DS
    { HEAP_SIZE_SAVE, OS_ARENA_MAIN },
    #else
    // TODO: Port OS_ARENA_MAIN to PAL
    #endif
    #ifdef PLATFORM_DS
    { HEAP_SIZE_DEBUG, OS_ARENA_MAIN },
    #else
    // TODO: Port OS_ARENA_MAIN to PAL
    #endif
    #ifdef PLATFORM_DS
    { HEAP_SIZE_APPLICATION, OS_ARENA_MAIN }
    #else
    // TODO: Port OS_ARENA_MAIN to PAL
    #endif
};

static void InitHeapSystem(void)
{
    u32 lowEntropyData[8];
    u8 md5[MATH_MD5_DIGEST_SIZE];

    #ifdef PLATFORM_DS
    OS_GetLowEntropyData(lowEntropyData);
    #else
    // TODO: Port OS_GetLowEntropyData to PAL
    #endif
    MATH_CalcMD5(md5, lowEntropyData, sizeof(lowEntropyData));

    u32 offset = 0;
    for (u32 i = 0; i < MATH_MD5_DIGEST_SIZE; i++) {
        offset += md5[i];
    }

    offset %= 256;
    while (offset % 4) {
        offset++;
    }

    Heap_InitSystem(sHeapInitParams, NELEMS(sHeapInitParams), HEAP_ID_MAX, offset);
}

void InitSystem(void)
{
#ifdef PLATFORM_DS
    OS_Init();
    FX_Init();
    GX_SetPower(GX_POWER_ALL);
    GX_Init();
    OS_InitTick();
    InitHeapSystem();

    gSystem.mainTaskMgr = SysTaskManager_Init(MAIN_TASK_MAX, OS_AllocFromMainArenaLo(SysTaskManager_GetRequiredSize(MAIN_TASK_MAX), 4));
    gSystem.vBlankTaskMgr = SysTaskManager_Init(VBLANK_TASK_MAX, OS_AllocFromMainArenaLo(SysTaskManager_GetRequiredSize(VBLANK_TASK_MAX), 4));
    gSystem.postVBlankTaskMgr = SysTaskManager_Init(POST_VBLANK_TASK_MAX, OS_AllocFromMainArenaLo(SysTaskManager_GetRequiredSize(POST_VBLANK_TASK_MAX), 4));
    gSystem.printTaskMgr = SysTaskManager_Init(PRINT_TASK_MAX, OS_AllocFromMainArenaLo(SysTaskManager_GetRequiredSize(PRINT_TASK_MAX), 4));

    GX_DispOff();
    GXS_DispOff();
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
#else
    // SDL3 implementation - initialize PAL graphics system
    InitHeapSystem();
    
    if (!PAL_Graphics_Init()) {
        fprintf(stderr, "Failed to initialize PAL graphics system\n");
        exit(1);
    }
    
    // Initialize task managers (but with standard malloc for SDL)
    gSystem.mainTaskMgr = SysTaskManager_Init(MAIN_TASK_MAX, malloc(SysTaskManager_GetRequiredSize(MAIN_TASK_MAX)));
    gSystem.vBlankTaskMgr = SysTaskManager_Init(VBLANK_TASK_MAX, malloc(SysTaskManager_GetRequiredSize(VBLANK_TASK_MAX)));
    gSystem.postVBlankTaskMgr = SysTaskManager_Init(POST_VBLANK_TASK_MAX, malloc(SysTaskManager_GetRequiredSize(POST_VBLANK_TASK_MAX)));
    gSystem.printTaskMgr = SysTaskManager_Init(PRINT_TASK_MAX, malloc(SysTaskManager_GetRequiredSize(PRINT_TASK_MAX)));
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
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_EnableIrqMask to PAL
    #endif
    OS_EnableIrqMask(OS_IE_FIFO_RECV);
    #else
    // TODO: Port OS_IE_FIFO_RECV to PAL
    #endif
    #ifdef PLATFORM_DS
    OS_EnableIrq();
    #else
    // TODO: Port OS_EnableIrq to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_VBlankIntr(TRUE);
    #else
    // TODO: Port GX_VBlankIntr to PAL
    #endif
    #ifdef PLATFORM_DS
    FS_Init(1);
    #else
    // TODO: Port FS_Init to PAL
    #endif
    CheckForMemoryTampering();

    #ifdef PLATFORM_DS
    u32 fsTableSize = FS_GetTableSize();
    #else
    // TODO: Port FS_GetTableSize to PAL
    #endif
    #ifdef PLATFORM_DS
    void *fsTable = OS_AllocFromMainArenaLo(fsTableSize, 4);
    #else
    // TODO: Port OS_AllocFromMainArenaLo to PAL
    #endif

    GF_ASSERT(fsTable != NULL);
    #ifdef PLATFORM_DS
    FS_LoadTable(fsTable, fsTableSize);
    #else
    // TODO: Port FS_LoadTable to PAL
    #endif

    gSystem.vblankCallback = NULL;
    gSystem.hblankCallback = NULL;
    gSystem.dummyCallback_10 = NULL;
    gSystem.dummyCallback_14 = NULL;
    gSystem.heapCanary = NULL;
    gSystem.vblankCounter = 0;
    gSystem.whichScreenIs3D = DS_SCREEN_MAIN;

    CARD_SetCacheFlushThreshold(0x500, 0x2400);
    InitCRC16Table(HEAP_ID_SYSTEM);
}

void InitVRAM(void)
{
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_SetBankForLCDC to PAL
    #endif
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    #else
    // TODO: Port GX_VRAM_LCDC_ALL to PAL
    #endif
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    #ifdef PLATFORM_DS
    GX_DisableBankForLCDC();
    #else
    // TODO: Port GX_DisableBankForLCDC to PAL
    #endif
    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
    MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);
}

void *ReadFileToHeap(int heapID, const char *filename)
{
    FSFile file;
    #ifdef PLATFORM_DS
    FS_InitFile(&file);
    #else
    // TODO: Port FS_InitFile to PAL
    #endif

    void *buf;
    #ifdef PLATFORM_DS
    if (FS_OpenFile(&file, filename)) {
    #else
    // TODO: Port FS_OpenFile to PAL
    #endif
        #ifdef PLATFORM_DS
        u32 length = FS_GetLength(&file);
        #else
        // TODO: Port FS_GetLength to PAL
        #endif
        buf = Heap_Alloc(heapID, length);
        if (buf != NULL) {
            #ifdef PLATFORM_DS
            if (FS_ReadFile(&file, buf, length) != length) {
            #else
            // TODO: Port FS_ReadFile to PAL
            #endif
                Heap_FreeExplicit(heapID, buf);
                buf = NULL;
            }
        }

        #ifdef PLATFORM_DS
        FS_CloseFile(&file);
        #else
        // TODO: Port FS_CloseFile to PAL
        #endif
    } else {
        buf = NULL;
    }

    return buf;
}

void ReadFileToBuffer(const char *filename, void **buf)
{
    FSFile file;
    #ifdef PLATFORM_DS
    FS_InitFile(&file);
    #else
    // TODO: Port FS_InitFile to PAL
    #endif

    #ifdef PLATFORM_DS
    if (FS_OpenFile(&file, filename)) {
    #else
    // TODO: Port FS_OpenFile to PAL
    #endif
        #ifdef PLATFORM_DS
        u32 length = FS_GetLength(&file);
        #else
        // TODO: Port FS_GetLength to PAL
        #endif
        if (*buf != NULL) {
            #ifdef PLATFORM_DS
            if (FS_ReadFile(&file, *buf, length) != length) {
            #else
            // TODO: Port FS_ReadFile to PAL
            #endif
                /* error not handled */
            }
        }

        #ifdef PLATFORM_DS
        FS_CloseFile(&file);
        #else
        // TODO: Port FS_CloseFile to PAL
        #endif
    }
}

CacheEntry sCache[CACHE_ENTRY_MAX];

void ClearUnusedSystemCache(void)
{
    for (int i = CACHE_ENTRY_MAX - 1; i > -1; i--) {
        if (sCache[i].data != NULL) {
            Heap_Free(sCache[i].data);
            sCache[i].data = NULL;
            sCache[i].hash = 0;
        }
    }
}

void InitKeypadAndTouchpad(void)
{
    gSystem.buttonMode = 0;
    gSystem.heldKeysRaw = 0;
    gSystem.pressedKeysRaw = 0;
    gSystem.pressedKeysRepeatableRaw = 0;
    gSystem.heldKeys = 0;
    gSystem.pressedKeys = 0;
    gSystem.pressedKeysRepeatable = 0;
    gSystem.autorepeatTimer = 0;
    gSystem.autorepeatRate = 4;
    gSystem.autorepeatDelay = 8;
    gSystem.touchX = 0;
    gSystem.touchY = 0;
    gSystem.touchPressed = 0;
    gSystem.touchHeld = 0;
    gSystem.touchAutoSampling = FALSE;

    #ifdef PLATFORM_DS
    TP_Init();
    #else
    // TODO: Port TP_Init to PAL
    #endif

    TPCalibrateParam touchCalibration;
    #ifdef PLATFORM_DS
    if (TP_GetUserInfo(&touchCalibration) == TRUE) {
    #else
    // TODO: Port TP_GetUserInfo to PAL
    #endif
        #ifdef PLATFORM_DS
        TP_SetCalibrateParam(&touchCalibration);
        #else
        // TODO: Port TP_SetCalibrateParam to PAL
        #endif
    } else {
        touchCalibration.x0 = 0x02AE;
        touchCalibration.y0 = 0x058C;
        touchCalibration.xDotSize = 0x0E25;
        touchCalibration.yDotSize = 0x1208;
        #ifdef PLATFORM_DS
        TP_SetCalibrateParam(&touchCalibration);
        #else
        // TODO: Port TP_SetCalibrateParam to PAL
        #endif
    }
}

void SetGBACartridgeVersion(int version)
{
    gSystem.gbaCartridgeVersion = version;
}

void SleepLock(u8 mask)
{
    gSystem.inhibitSleep |= mask;
}

void SleepUnlock(u8 mask)
{
    gSystem.inhibitSleep &= ~(mask);
}

void ReadKeypadAndTouchpad(void)
{
#ifdef PLATFORM_DS
    TPData tpRaw;
    TPData tp;
    u32 padRead;

    if (PAD_DetectFold()) {
        // Can't press any buttons while the lid is closed.
        gSystem.pressedKeys = 0;
        gSystem.heldKeys = 0;
        gSystem.pressedKeysRepeatable = 0;
        gSystem.touchPressed = 0;
        gSystem.touchHeld = 0;
        return;
    }

    padRead = PAD_Read();

    gSystem.pressedKeysRaw = padRead & (padRead ^ gSystem.heldKeysRaw);
    gSystem.pressedKeysRepeatableRaw = gSystem.pressedKeysRaw;

    if (padRead != 0 && gSystem.heldKeysRaw == padRead) {
        if (--gSystem.autorepeatTimer == 0) {
            gSystem.pressedKeysRepeatableRaw = padRead;
            gSystem.autorepeatTimer = gSystem.autorepeatRate;
        }
    } else {
        gSystem.autorepeatTimer = gSystem.autorepeatDelay;
    }

    gSystem.heldKeysRaw = padRead;

    gSystem.pressedKeys = gSystem.pressedKeysRaw;
    gSystem.heldKeys = gSystem.heldKeysRaw;
    gSystem.pressedKeysRepeatable = gSystem.pressedKeysRepeatableRaw;

    ApplyButtonModeToInput();

    if (gSystem.touchAutoSampling == FALSE) {
        #ifdef PLATFORM_DS
        while (TP_RequestRawSampling(&tpRaw) != FALSE)
        #else
        // TODO: Port TP_RequestRawSampling to PAL
        #endif
            ;
    } else {
        #ifdef PLATFORM_DS
        TP_GetLatestRawPointInAuto(&tpRaw);
        #else
        // TODO: Port TP_GetLatestRawPointInAuto to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    TP_GetCalibratedPoint(&tp, &tpRaw);
    #else
    // TODO: Port TP_GetCalibratedPoint to PAL
    #endif
#else
    // SDL3 implementation using PAL
    PAL_Input_Update();
    
    u32 heldKeys = PAL_Input_GetHeld();
    u32 pressedKeys = PAL_Input_GetPressed();
    
    // Update raw key state
    gSystem.heldKeysRaw = heldKeys;
    gSystem.pressedKeysRaw = pressedKeys;
    gSystem.pressedKeysRepeatableRaw = pressedKeys;
    
    // Handle key repeat (autorepeat)
    if (heldKeys != 0 && gSystem.heldKeys == heldKeys) {
        if (--gSystem.autorepeatTimer == 0) {
            gSystem.pressedKeysRepeatableRaw = heldKeys;
            gSystem.autorepeatTimer = gSystem.autorepeatRate;
        }
    } else {
        gSystem.autorepeatTimer = gSystem.autorepeatDelay;
    }
    
    // Update processed key state
    gSystem.pressedKeys = gSystem.pressedKeysRaw;
    gSystem.heldKeys = gSystem.heldKeysRaw;
    gSystem.pressedKeysRepeatable = gSystem.pressedKeysRepeatableRaw;
    
    ApplyButtonModeToInput();
    
    // Update touch state
    PAL_TouchState touch = PAL_Input_GetTouch();
    
    if (touch.pressed || touch.held) {
        gSystem.touchX = touch.x;
        gSystem.touchY = touch.y;
    }
    
    gSystem.touchPressed = touch.pressed ? 1 : 0;
    gSystem.touchHeld = touch.held ? 1 : 0;
    
    return;
#endif
    
#ifdef PLATFORM_DS
    // DS-only touch handling continuation
    if (tp.validity == TP_VALIDITY_VALID) {
        gSystem.touchX = tp.x;
        gSystem.touchY = tp.y;
    } else {
        if (gSystem.touchHeld) {
            switch (tp.validity) {
            case TP_VALIDITY_INVALID_X:
                gSystem.touchY = tp.y;
                break;
            case TP_VALIDITY_INVALID_Y:
                gSystem.touchX = tp.x;
                break;
            case TP_VALIDITY_INVALID_XY:
                break;
            default:
                break;
            }
        } else {
            tp.touch = 0;
        }
    }

    gSystem.touchPressed = (u16)(tp.touch & (tp.touch ^ gSystem.touchHeld));
    gSystem.touchHeld = tp.touch;
#endif
}

#define CONVERT_KEY(member, convertFrom, convertTo) \
    {                                               \
        if (member & convertFrom) {                 \
            member |= convertTo;                    \
        }                                           \
    }
#define SWAP_KEY(member, swapA, swapB)        \
    {                                         \
        u32 tmp = 0;                          \
        if (member & swapA) {                 \
            tmp |= swapB;                     \
        }                                     \
        if (member & swapB) {                 \
            tmp |= swapA;                     \
        }                                     \
        member &= ((swapA | swapB) ^ 0xFFFF); \
        member |= tmp;                        \
    }
#define CLEAR_KEY(member, key)    \
    {                             \
        member &= (key ^ 0xFFFF); \
    }

static void ApplyButtonModeToInput(void)
{
    switch (gSystem.buttonMode) {
    default:
    case BUTTON_MODE_NORMAL:
        break;

    case BUTTON_MODE_START_IS_X:
        #ifdef PLATFORM_DS
        CONVERT_KEY(gSystem.pressedKeys, PAD_BUTTON_START, PAD_BUTTON_X);
        #else
        // TODO: Port PAD_BUTTON_X to PAL
        #endif
        #ifdef PLATFORM_DS
        CONVERT_KEY(gSystem.heldKeys, PAD_BUTTON_START, PAD_BUTTON_X);
        #else
        // TODO: Port PAD_BUTTON_X to PAL
        #endif
        #ifdef PLATFORM_DS
        CONVERT_KEY(gSystem.pressedKeysRepeatable, PAD_BUTTON_START, PAD_BUTTON_X);
        #else
        // TODO: Port PAD_BUTTON_X to PAL
        #endif
        break;

    case BUTTON_MODE_SWAP_XY:
        #ifdef PLATFORM_DS
        SWAP_KEY(gSystem.pressedKeys, PAD_BUTTON_X, PAD_BUTTON_Y);
        #else
        // TODO: Port PAD_BUTTON_Y to PAL
        #endif
        #ifdef PLATFORM_DS
        SWAP_KEY(gSystem.heldKeys, PAD_BUTTON_X, PAD_BUTTON_Y);
        #else
        // TODO: Port PAD_BUTTON_Y to PAL
        #endif
        #ifdef PLATFORM_DS
        SWAP_KEY(gSystem.pressedKeysRepeatable, PAD_BUTTON_X, PAD_BUTTON_Y);
        #else
        // TODO: Port PAD_BUTTON_Y to PAL
        #endif
        break;

    case BUTTON_MODE_L_IS_A:
        #ifdef PLATFORM_DS
        CONVERT_KEY(gSystem.pressedKeys, PAD_BUTTON_L, PAD_BUTTON_A);
        #else
        // TODO: Port PAD_BUTTON_A to PAL
        #endif
        #ifdef PLATFORM_DS
        CONVERT_KEY(gSystem.heldKeys, PAD_BUTTON_L, PAD_BUTTON_A);
        #else
        // TODO: Port PAD_BUTTON_A to PAL
        #endif
        #ifdef PLATFORM_DS
        CONVERT_KEY(gSystem.pressedKeysRepeatable, PAD_BUTTON_L, PAD_BUTTON_A);
        #else
        // TODO: Port PAD_BUTTON_A to PAL
        #endif
        #ifdef PLATFORM_DS
        CLEAR_KEY(gSystem.pressedKeys, (PAD_BUTTON_L | PAD_BUTTON_R));
        #else
        // TODO: Port PAD_BUTTON_R to PAL
        #endif
        #ifdef PLATFORM_DS
        CLEAR_KEY(gSystem.heldKeys, (PAD_BUTTON_L | PAD_BUTTON_R));
        #else
        // TODO: Port PAD_BUTTON_R to PAL
        #endif
        #ifdef PLATFORM_DS
        CLEAR_KEY(gSystem.pressedKeysRepeatable, (PAD_BUTTON_L | PAD_BUTTON_R));
        #else
        // TODO: Port PAD_BUTTON_R to PAL
        #endif
        break;
    }
}

void SetAutorepeat(int rate, int delay)
{
    gSystem.autorepeatRate = rate;
    gSystem.autorepeatDelay = delay;
}

void ResetLock(u8 mask)
{
    gSystem.inhibitReset |= mask;
}

void ResetUnlock(u8 mask)
{
    gSystem.inhibitReset &= ~(mask);
}

#define HEAP_CANARY 0x2F93A1BC

void InitHeapCanary(enum HeapID heapID)
{
    GF_ASSERT(gSystem.heapCanary == NULL);

    gSystem.heapCanary = Heap_AllocAtEnd(heapID, sizeof(u32));
    *(gSystem.heapCanary) = HEAP_CANARY;
}

void FreeHeapCanary(void)
{
    GF_ASSERT(gSystem.heapCanary != NULL);

    *(gSystem.heapCanary) = 0;
    Heap_Free(gSystem.heapCanary);
    gSystem.heapCanary = NULL;
}

BOOL HeapCanaryOK(void)
{
    // Explicit if required to match.
    if (gSystem.heapCanary && *gSystem.heapCanary == HEAP_CANARY) {
        return TRUE;
    }
    return FALSE;
}
