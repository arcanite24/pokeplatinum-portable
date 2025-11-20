#include "main.h"

#include <dwc.h>

#include "constants/graphics.h"
#include "constants/heap.h"

#include "assert.h"
#include "brightness_controller.h"
#include "communication_system.h"
#include "font.h"
#include "game_overlay.h"
#include "game_start.h"
#include "main.h"
#include "math_util.h"
#include "overlay_manager.h"
#include "play_time_manager.h"
#include "rtc.h"
#include "save_player.h"
#include "savedata.h"
#include "screen_fade.h"
#include "sound_system.h"
#include "sys_task_manager.h"
#include "system.h"
#include "timer.h"
#include "touch_pad.h"
#include "unk_0202419C.h"
#include "unk_0202CC64.h"
#include "unk_020366A0.h"
#include "unk_02038FFC.h"

#ifdef PLATFORM_SDL
#include "platform/pal_graphics.h"
#include "platform/pal_background.h"
#include "platform/pal_sprite.h"
#endif
#include "unk_02039814.h"
#include "unk_02039A64.h"
#include "unk_0209A74C.h"

#define RESET_COMBO (PAD_BUTTON_START | PAD_BUTTON_SELECT | PAD_BUTTON_L | PAD_BUTTON_R)

#ifdef PLATFORM_DS
FS_EXTERN_OVERLAY(game_start);
#else
// TODO: Port FS_EXTERN_OVERLAY to PAL
#endif
#ifdef PLATFORM_DS
FS_EXTERN_OVERLAY(game_opening);
#else
// TODO: Port FS_EXTERN_OVERLAY to PAL
#endif

typedef struct Application {
    FSOverlayID currOverlayID;
    ApplicationManager *currApplication;
    FSOverlayID nextOverlayID;
    const ApplicationManagerTemplate *nextApplication;
    ApplicationArgs args;
} Application;

static void InitApplication(void);
static void RunApplication(void);
static void WaitFrame(void);
static void TrySystemReset(enum OSResetParameter resetParam);
static void SoftReset(enum OSResetParameter resetParam);
static void HeapCanaryFailed(int resetParam, int param1);
static void CheckHeapCanary(void);

static Application sApplication;
// This variable doesn't really makes sense. If it's set to off, the game will
// repeatedly try to restore the backlight to its saved state.
static PMBackLightSwitch sSavedBacklightState;
BOOL gIgnoreCartridgeForWake;
extern const ApplicationManagerTemplate gOpeningCutsceneAppTemplate;

void NitroMain(void)
{
    InitSystem();
    InitVRAM();
    InitKeypadAndTouchpad();

    SetGBACartridgeVersion(VERSION_NONE);
    PM_GetBackLight(&sSavedBacklightState, NULL);
    sub_0202419C();
    InitRTC();
    InitApplication();

    Fonts_Init();
    Font_InitManager(FONT_SYSTEM, HEAP_ID_APPLICATION);
    Font_InitManager(FONT_MESSAGE, HEAP_ID_APPLICATION);
    Font_InitManager(FONT_UNOWN, HEAP_ID_APPLICATION);

    sApplication.args.unk_00 = -1;
    sApplication.args.saveData = SaveData_Init();

    SoundSystem_Init(SaveData_GetChatotCry(sApplication.args.saveData), SaveData_GetOptions(sApplication.args.saveData));
    Timer_Start();

    if (sub_02038FFC(HEAP_ID_APPLICATION) == DWC_INIT_RESULT_DESTROY_OTHER_SETTING) {
        sub_02039A64(HEAP_ID_APPLICATION, 0);
    }

    if (SaveData_BackupExists(sApplication.args.saveData) == FALSE) {
        sub_0209A74C(HEAP_ID_SYSTEM);
    } else {
        #ifdef PLATFORM_DS
        switch (OS_GetResetParameter()) {
        #else
        // TODO: Port OS_GetResetParameter to PAL
        #endif
        case RESET_CLEAN:
            sApplication.args.error = FALSE;
            #ifdef PLATFORM_DS
            EnqueueApplication(FS_OVERLAY_ID(game_opening), &gOpeningCutsceneAppTemplate);
            #else
            // TODO: Port FS_OVERLAY_ID to PAL
            #endif
            break;

        case RESET_ERROR:
            SetScreenColorBrightness(DS_SCREEN_MAIN, COLOR_BLACK);
            SetScreenColorBrightness(DS_SCREEN_SUB, COLOR_BLACK);
            sApplication.args.error = TRUE;
            #ifdef PLATFORM_DS
            EnqueueApplication(FS_OVERLAY_ID(game_start), &gGameStartLoadSaveAppTemplate);
            #else
            // TODO: Port FS_OVERLAY_ID to PAL
            #endif
            break;

        default:
            GF_ASSERT(FALSE);
        }
    }

    gSystem.showTitleScreenIntro = TRUE;
    gSystem.frameCounter = 0;

    InitRNG();
    BrightnessController_ResetAllControllers();
    PlayTime_FlagNotStarted();

    gIgnoreCartridgeForWake = FALSE;

    while (TRUE) {
        CheckHeapCanary();
        HandleConsoleFold();
        ReadKeypadAndTouchpad();

        if ((gSystem.heldKeysRaw & RESET_COMBO) == RESET_COMBO && !gSystem.inhibitReset) {
            SoftReset(RESET_CLEAN);
        }

        if (CommSys_Update()) {
            CheckHeapCanary();
            
#ifdef PLATFORM_SDL
            // Begin frame rendering
            PAL_Graphics_BeginFrame();
#endif
            
            RunApplication();
            SysTaskManager_ExecuteTasks(gSystem.mainTaskMgr);
            SysTaskManager_ExecuteTasks(gSystem.printTaskMgr);

#ifdef PLATFORM_SDL
            // Render all PAL subsystems
            // Note: Background and sprite managers need to be stored in gSystem for access here
            // For now, we'll just end the frame - actual rendering will be added when
            // background and sprite managers are properly initialized
            PAL_Graphics_EndFrame();
#endif

            if (!gSystem.frameCounter) {
#ifdef PLATFORM_DS
                #ifdef PLATFORM_DS
                #ifdef PLATFORM_DS
                #else
                // TODO: Port OS_WaitIrq to PAL
                #endif
                OS_WaitIrq(TRUE, OS_IE_V_BLANK);
                #else
                // TODO: Port OS_IE_V_BLANK to PAL
                #endif
#endif
                gSystem.vblankCounter++;
            }
        }

        UpdateRTC();
        PlayTime_IncrementTimer();
        sub_020241CC();
        SysTaskManager_ExecuteTasks(gSystem.printTaskMgr);

        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port OS_WaitIrq to PAL
        #endif
        OS_WaitIrq(TRUE, OS_IE_V_BLANK);
        #else
        // TODO: Port OS_IE_V_BLANK to PAL
        #endif

        gSystem.vblankCounter++;
        gSystem.frameCounter = 0;

        BrightnessController_Update();
        ExecScreenFade();

        if (gSystem.vblankCallback != NULL) {
            gSystem.vblankCallback(gSystem.vblankCallbackData);
        }

        SoundSystem_Tick();
        SysTaskManager_ExecuteTasks(gSystem.postVBlankTaskMgr);
    }
}

static void InitApplication()
{
    #ifdef PLATFORM_DS
    sApplication.currOverlayID = FS_OVERLAY_ID_NONE;
    #else
    // TODO: Port FS_OVERLAY_ID_NONE to PAL
    #endif
    sApplication.currApplication = NULL;
    #ifdef PLATFORM_DS
    sApplication.nextOverlayID = FS_OVERLAY_ID_NONE;
    #else
    // TODO: Port FS_OVERLAY_ID_NONE to PAL
    #endif
    sApplication.nextApplication = NULL;
}

static void RunApplication(void)
{
    if (sApplication.currApplication == NULL) {
        if (sApplication.nextApplication == NULL) {
            return;
        }

        #ifdef PLATFORM_DS
        if (sApplication.nextOverlayID != FS_OVERLAY_ID_NONE) {
        #else
        // TODO: Port FS_OVERLAY_ID_NONE to PAL
        #endif
            Overlay_LoadByID(sApplication.nextOverlayID, OVERLAY_LOAD_NORMAL);
        }

        sApplication.currOverlayID = sApplication.nextOverlayID;
        sApplication.currApplication = ApplicationManager_New(sApplication.nextApplication, &sApplication.args, HEAP_ID_SYSTEM);
        #ifdef PLATFORM_DS
        sApplication.nextOverlayID = FS_OVERLAY_ID_NONE;
        #else
        // TODO: Port FS_OVERLAY_ID_NONE to PAL
        #endif
        sApplication.nextApplication = NULL;
    }

    if (ApplicationManager_Exec(sApplication.currApplication)) {
        ApplicationManager_Free(sApplication.currApplication);
        sApplication.currApplication = NULL;

        #ifdef PLATFORM_DS
        if (sApplication.currOverlayID != FS_OVERLAY_ID_NONE) {
        #else
        // TODO: Port FS_OVERLAY_ID_NONE to PAL
        #endif
            Overlay_UnloadByID(sApplication.currOverlayID);
        }
    }
}

void EnqueueApplication(FSOverlayID overlayID, const ApplicationManagerTemplate *template)
{
    GF_ASSERT(sApplication.nextApplication == NULL);

    sApplication.nextOverlayID = overlayID;
    sApplication.nextApplication = template;
}

static void WaitFrame(void)
{
    CommSys_Update();

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_WaitIrq to PAL
    #endif
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
    #else
    // TODO: Port OS_IE_V_BLANK to PAL
    #endif

    gSystem.vblankCounter++;
    gSystem.frameCounter = 0;

    if (gSystem.vblankCallback != NULL) {
        gSystem.vblankCallback(gSystem.vblankCallbackData);
    }
}

static void TrySystemReset(enum OSResetParameter resetParam)
{
    if (sub_02038AB8() && CARD_TryWaitBackupAsync() == TRUE) {
        #ifdef PLATFORM_DS
        OS_ResetSystem(resetParam);
        #else
        // TODO: Port OS_ResetSystem to PAL
        #endif
    }

    WaitFrame();
}

static void CheckHeapCanary(void)
{
    int v0 = sub_020389D8();

    switch (v0) {
    case 1:
        HeapCanaryFailed(1, v0);
        break;
    case 2:
        HeapCanaryFailed(0, v0);
        break;
    case 3:
        HeapCanaryFailed(1, v0);
        break;
    }
}

static void SoftReset(enum OSResetParameter resetParam)
{
    SetScreenColorBrightness(DS_SCREEN_MAIN, COLOR_WHITE);
    SetScreenColorBrightness(DS_SCREEN_SUB, COLOR_WHITE);

    if (sub_02037DB0()) {
        SaveData_SaveStateCancel(SaveData_Ptr());
    }

    while (TRUE) {
        HandleConsoleFold();
        TrySystemReset(resetParam);
    }
}

static void HeapCanaryFailed(int resetParam, int param1)
{
    int elapsed;

    if (param1 == 3) {
        NetworkError_DisplayNetworkError(HEAP_ID_SYSTEM, 3, 0);
    } else if (resetParam == RESET_CLEAN) {
        if (CommMan_IsConnectedToWifi() == TRUE) {
            NetworkError_DisplayNetworkError(HEAP_ID_SYSTEM, 6, 0);
        } else {
            NetworkError_DisplayNetworkError(HEAP_ID_SYSTEM, 2, 0);
        }
    } else {
        if (CommMan_IsConnectedToWifi() == TRUE) {
            NetworkError_DisplayNetworkError(HEAP_ID_SYSTEM, 5, 0);
        } else {
            NetworkError_DisplayNetworkError(HEAP_ID_SYSTEM, 0, 0);
        }
    }

    sub_02037DB0();
    WaitFrame();
    SoundSystem_Tick();

    elapsed = 0;

    while (TRUE) {
        HandleConsoleFold();
        ReadKeypadAndTouchpad();

        #ifdef PLATFORM_DS
        if (elapsed >= 30 && gSystem.pressedKeys & PAD_BUTTON_A) {
        #else
        // TODO: Port PAD_BUTTON_A to PAL
        #endif
            break;
        }

        WaitFrame();

        if (elapsed < 30) {
            elapsed++;
        }
    }

    SoftReset(resetParam);
}

void InitRNG(void)
{
    RTCDate date;
    RTCTime time;
    GetCurrentDateTime(&date, &time);

    u32 seed = date.year + date.month * 0x100 * date.day * 0x10000 + time.hour * 0x10000 + (time.minute + time.second) * 0x1000000 + gSystem.vblankCounter;

    MTRNG_SetSeed(seed);
    LCRNG_SetSeed(seed);
}

void HandleConsoleFold(void)
{
    PMBackLightSwitch top, bottom;
    PMWakeUpTrigger trigger;

    #ifdef PLATFORM_DS
    if (PAD_DetectFold()) {
    #else
    // TODO: Port PAD_DetectFold to PAL
    #endif
        if (gSystem.inhibitSleep == 0) {
            BeforeSleep();

            if (CTRDG_IsPulledOut() == TRUE) {
                gIgnoreCartridgeForWake = TRUE;
            }

sleep_again:
            trigger = PM_TRIGGER_COVER_OPEN | PM_TRIGGER_CARD;

            if (gSystem.gbaCartridgeVersion && !gIgnoreCartridgeForWake) {
                trigger |= PM_TRIGGER_CARTRIDGE;
            }

            PM_GoSleepMode(trigger, 0, 0);

            if (CARD_IsPulledOut()) {
                PM_ForceToPowerOff();
            #ifdef PLATFORM_DS
            } else if (PAD_DetectFold()) {
            #else
            // TODO: Port PAD_DetectFold to PAL
            #endif
                // Woke up because the cartridge got pulled out
                gIgnoreCartridgeForWake = TRUE;
                goto sleep_again;
            }

            AfterSleep();
        } else {
            PM_GetBackLight(&top, &bottom);

            if (top == PM_BACKLIGHT_ON) {
                PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF);
            }
        }
    } else {
        PM_GetBackLight(&top, &bottom);

        if (top == PM_BACKLIGHT_OFF) {
            PM_SetBackLight(PM_LCD_ALL, sSavedBacklightState);
        }
    }
}
