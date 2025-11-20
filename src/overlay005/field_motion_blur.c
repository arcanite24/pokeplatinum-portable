#include "overlay005/field_motion_blur.h"

#include <nitro.h>
#include <string.h>

#include "constants/heap.h"

#include "overlay005/motion_blur.h"

#include "gx_layers.h"
#include "heap.h"

FieldMotionBlur *FieldMotionBlur_Start(int coeffA, int coeffB)
{
    FieldMotionBlur *fieldMotionBlur;

    #ifdef PLATFORM_DS
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_BG1, 0);
    #else
    // TODO: Port GX_PLANEMASK_BG1 to PAL
    #endif
    #ifdef PLATFORM_DS
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_BG2, 0);
    #else
    // TODO: Port GX_PLANEMASK_BG2 to PAL
    #endif
    #ifdef PLATFORM_DS
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_BG3, 0);
    #else
    // TODO: Port GX_PLANEMASK_BG3 to PAL
    #endif

    #ifdef PLATFORM_DS
    GX_ResetBankForBG();
    #else
    // TODO: Port GX_ResetBankForBG to PAL
    #endif

    fieldMotionBlur = Heap_Alloc(HEAP_ID_FIELD1, sizeof(FieldMotionBlur));
    MI_CpuClear32(fieldMotionBlur, sizeof(FieldMotionBlur));

    {
        MotionBlurParams motionBlurParams = {
            #ifdef PLATFORM_DS
            GX_DISPMODE_VRAM_C,
            #else
            // TODO: Port GX_DISPMODE_VRAM_C to PAL
            #endif
            #ifdef PLATFORM_DS
            GX_BGMODE_0,
            #else
            // TODO: Port GX_BGMODE_0 to PAL
            #endif
            #ifdef PLATFORM_DS
            GX_BG0_AS_3D,
            #else
            // TODO: Port GX_BG0_AS_3D to PAL
            #endif
            GX_CAPTURE_SIZE_256x192,
            #ifdef PLATFORM_DS
            GX_CAPTURE_MODE_AB,
            #else
            // TODO: Port GX_CAPTURE_MODE_AB to PAL
            #endif
            #ifdef PLATFORM_DS
            GX_CAPTURE_SRCA_2D3D,
            #else
            // TODO: Port GX_CAPTURE_SRCA_2D3D to PAL
            #endif
            GX_CAPTURE_SRCB_VRAM_0x00000,
            GX_CAPTURE_DEST_VRAM_C_0x00000,
            0,
            0,
            4
        };

        motionBlurParams.blendCoeffA = coeffA;
        motionBlurParams.blendCoeffB = coeffB;

        fieldMotionBlur->motionBlur = MotionBlur_New(&motionBlurParams);
    }

    return fieldMotionBlur;
}

void FieldMotionBlur_Stop(FieldMotionBlur **fieldMotionBlur)
{
    #ifdef PLATFORM_DS
    MotionBlur_Delete(&(*fieldMotionBlur)->motionBlur, GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    #else
    // TODO: Port GX_BG0_AS_3D to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_SetBankForBG to PAL
    #endif
    GX_SetBankForBG(GX_VRAM_BG_128_C);
    #else
    // TODO: Port GX_VRAM_BG_128_C to PAL
    #endif
    #ifdef PLATFORM_DS
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, 1);
    #else
    // TODO: Port GX_PLANEMASK_BG3 to PAL
    #endif
    Heap_FreeExplicit(HEAP_ID_FIELD1, *fieldMotionBlur);
}
