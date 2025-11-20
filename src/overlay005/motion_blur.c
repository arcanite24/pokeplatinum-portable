#include "overlay005/motion_blur.h"

#include <nitro.h>
#include <string.h>

#include "sys_task.h"
#include "sys_task_manager.h"
#include "unk_0200679C.h"

#define MOTION_BLUR_INIT_BLEND_COEFF_A 16
#define MOTION_BLUR_INIT_BLEND_COEFF_B 0

static void MotionBlur_VBlankInit(SysTask *task, void *param);
static void MotionBlur_SetCapture(SysTask *task, void *param);
static void MotionBlur_Init(MotionBlurParams *params);

MotionBlur *MotionBlur_New(MotionBlurParams *params)
{
    SysTask *task = SysTask_StartAndAllocateParam(MotionBlur_SetCapture, sizeof(MotionBlur), 5, params->heapID);
    MotionBlur *motionBlur = SysTask_GetParam(task);

    motionBlur->params = *params;
    motionBlur->task = task;
    motionBlur->initialized = FALSE;
    #ifdef PLATFORM_DS
    motionBlur->lcdcVRamBank = GX_GetBankForLCDC();
    #else
    // TODO: Port GX_GetBankForLCDC to PAL
    #endif

    MotionBlur_Init(&motionBlur->params);
    SysTask_ExecuteAfterVBlank(MotionBlur_VBlankInit, motionBlur, 0);

    return motionBlur;
}

void MotionBlur_Delete(MotionBlur **motionBlur, GXDispMode displayMode, GXBGMode bgMode, GXBG0As bg0Mode)
{
    #ifdef PLATFORM_DS
    GX_SetGraphicsMode(displayMode, bgMode, bg0Mode);
    #else
    // TODO: Port GX_SetGraphicsMode to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_SetBankForLCDC((*motionBlur)->lcdcVRamBank);
    #else
    // TODO: Port GX_SetBankForLCDC to PAL
    #endif

    switch ((*motionBlur)->params.displayMode) {
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_A:
    #else
    // TODO: Port GX_DISPMODE_VRAM_A to PAL
    #endif
        MI_CpuClearFast((void *)HW_LCDC_VRAM_A, HW_VRAM_A_SIZE);
        break;
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_B:
    #else
    // TODO: Port GX_DISPMODE_VRAM_B to PAL
    #endif
        MI_CpuClearFast((void *)HW_LCDC_VRAM_B, HW_VRAM_B_SIZE);
        break;
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_C:
    #else
    // TODO: Port GX_DISPMODE_VRAM_C to PAL
    #endif
        MI_CpuClearFast((void *)HW_LCDC_VRAM_C, HW_VRAM_C_SIZE);
        break;
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_D:
    #else
    // TODO: Port GX_DISPMODE_VRAM_D to PAL
    #endif
        MI_CpuClearFast((void *)HW_LCDC_VRAM_D, HW_VRAM_D_SIZE);
        break;
    default:
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port GX_SetBankForLCDC to PAL
        #endif
        GX_SetBankForLCDC(GX_VRAM_LCDC_NONE);
        #else
        // TODO: Port GX_VRAM_LCDC_NONE to PAL
        #endif
        break;
    }

    SysTask_FinishAndFreeParam((*motionBlur)->task);
    *motionBlur = NULL;
}

static void MotionBlur_SetCapture(SysTask *task, void *param)
{
    MotionBlur *motionBlur = param;

    if (motionBlur->initialized) {
        #ifdef PLATFORM_DS
        GX_SetCapture(
        #else
        // TODO: Port GX_SetCapture to PAL
        #endif
            motionBlur->params.captureSize,
            motionBlur->params.captureMode,
            motionBlur->params.captureSourceA,
            motionBlur->params.captureSourceB,
            motionBlur->params.captureDestination,
            motionBlur->params.blendCoeffA,
            motionBlur->params.blendCoeffB);
    }
}

static void MotionBlur_VBlankInit(SysTask *task, void *param)
{
    MotionBlur *motionBlur = (MotionBlur *)param;

    switch (motionBlur->params.displayMode) {
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_A:
    #else
    // TODO: Port GX_DISPMODE_VRAM_A to PAL
    #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port GX_SetBankForLCDC to PAL
        #endif
        GX_SetBankForLCDC(GX_VRAM_LCDC_A);
        #else
        // TODO: Port GX_VRAM_LCDC_A to PAL
        #endif
        break;
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_B:
    #else
    // TODO: Port GX_DISPMODE_VRAM_B to PAL
    #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port GX_SetBankForLCDC to PAL
        #endif
        GX_SetBankForLCDC(GX_VRAM_LCDC_B);
        #else
        // TODO: Port GX_VRAM_LCDC_B to PAL
        #endif
        break;
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_C:
    #else
    // TODO: Port GX_DISPMODE_VRAM_C to PAL
    #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port GX_SetBankForLCDC to PAL
        #endif
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        #else
        // TODO: Port GX_VRAM_LCDC_C to PAL
        #endif
        break;
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_D:
    #else
    // TODO: Port GX_DISPMODE_VRAM_D to PAL
    #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port GX_SetBankForLCDC to PAL
        #endif
        GX_SetBankForLCDC(GX_VRAM_LCDC_D);
        #else
        // TODO: Port GX_VRAM_LCDC_D to PAL
        #endif
        break;
    default:
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port GX_SetBankForLCDC to PAL
        #endif
        GX_SetBankForLCDC(GX_VRAM_LCDC_NONE);
        #else
        // TODO: Port GX_VRAM_LCDC_NONE to PAL
        #endif
        break;
    }

    #ifdef PLATFORM_DS
    GX_SetGraphicsMode(motionBlur->params.displayMode, motionBlur->params.bgMode, motionBlur->params.bg0Mode);
    #else
    // TODO: Port GX_SetGraphicsMode to PAL
    #endif
    motionBlur->initialized = TRUE;
    SysTask_Done(task);
}

static void MotionBlur_Init(MotionBlurParams *params)
{
    switch (params->displayMode) {
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_A:
    #else
    // TODO: Port GX_DISPMODE_VRAM_A to PAL
    #endif
        MI_CpuClearFast((void *)HW_LCDC_VRAM_A, HW_VRAM_A_SIZE);
        break;
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_B:
    #else
    // TODO: Port GX_DISPMODE_VRAM_B to PAL
    #endif
        MI_CpuClearFast((void *)HW_LCDC_VRAM_B, HW_VRAM_B_SIZE);
        break;
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_C:
    #else
    // TODO: Port GX_DISPMODE_VRAM_C to PAL
    #endif
        MI_CpuClearFast((void *)HW_LCDC_VRAM_C, HW_VRAM_C_SIZE);
        break;
    #ifdef PLATFORM_DS
    case GX_DISPMODE_VRAM_D:
    #else
    // TODO: Port GX_DISPMODE_VRAM_D to PAL
    #endif
        MI_CpuClearFast((void *)HW_LCDC_VRAM_D, HW_VRAM_D_SIZE);
        break;
    default:
        break;
    }

    #ifdef PLATFORM_DS
    GX_SetCapture(
    #else
    // TODO: Port GX_SetCapture to PAL
    #endif
        params->captureSize,
        params->captureMode,
        params->captureSourceA,
        params->captureSourceB,
        params->captureDestination,
        MOTION_BLUR_INIT_BLEND_COEFF_A,
        MOTION_BLUR_INIT_BLEND_COEFF_B);
}
