#include "overlay099/ov99_021D3C18.h"

#include <nitro.h>
#include <string.h>

#include "overlay099/ov99_021D4134.h"
#include "overlay099/struct_ov99_021D2CB0.h"
#include "overlay099/struct_ov99_021D3A40.h"
#include "overlay099/struct_ov99_021D3C18.h"

#include "bg_window.h"
#include "brightness_controller.h"

static void ov99_021D3CF8(UnkStruct_ov99_021D3C18 *param0, u32 param1);
static BOOL ov99_021D3D50(UnkStruct_ov99_021D3C18 *param0);

BOOL ov99_021D3C18(UnkStruct_ov99_021D2CB0 *param0, UnkStruct_ov99_021D3A40 *param1)
{
    UnkStruct_ov99_021D3C18 *v0 = &param1->unk_08_val5;

    switch (param1->unk_00) {
    case 0:
        #ifdef PLATFORM_DS
        BrightnessController_StartTransition(24, 0, -16, (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD), BRIGHTNESS_BOTH_SCREENS);
        #else
        // TODO: Port GX_BLEND_PLANEMASK_BD to PAL
        #endif
        param1->unk_00++;
        break;
    case 1:
        v0->unk_00++;

        if (v0->unk_00 > ((8280 - 7980 - (4 * 16) * (4 - 1)) / 4)) {
            v0->unk_00 = 0;
            param1->unk_00++;
        }
        break;
    case 2:
        if (v0->unk_04 >= 4 - 1) {
            #ifdef PLATFORM_DS
            BrightnessController_StartTransition(32, -16, 0, (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD), BRIGHTNESS_BOTH_SCREENS);
            #else
            // TODO: Port GX_BLEND_PLANEMASK_BD to PAL
            #endif
            param1->unk_00 = 4;
        } else {
            if (v0->unk_04 & 1) {
                ov99_021D439C(
                    param0, param0->unk_1105, 2, 1);
                #ifdef PLATFORM_DS
                ov99_021D3CF8(v0, (GX_BLEND_PLANEMASK_BG3));
                #else
                // TODO: Port GX_BLEND_PLANEMASK_BG3 to PAL
                #endif
            } else {
                ov99_021D439C(
                    param0, param0->unk_1105, 3, 1);
                #ifdef PLATFORM_DS
                ov99_021D3CF8(v0, (GX_BLEND_PLANEMASK_BG2));
                #else
                // TODO: Port GX_BLEND_PLANEMASK_BG2 to PAL
                #endif
            }

            param0->unk_1105++;
            v0->unk_04++;
            param1->unk_00 = 3;
        }
        break;
    case 3:
        if (ov99_021D3D50(v0) == 1) {
            param1->unk_00 = 1;
        }
        break;
    case 4:
        if (BrightnessController_IsTransitionComplete(BRIGHTNESS_BOTH_SCREENS) == TRUE) {
            return 1;
        }
        break;
    }

    return 0;
}

static void ov99_021D3CF8(UnkStruct_ov99_021D3C18 *param0, u32 param1)
{
    param0->unk_08 = param1;
    param0->unk_05 = 16;
    param0->unk_06 = 0;

    #ifdef PLATFORM_DS
    if (param1 == (GX_BLEND_PLANEMASK_BG2)) {
    #else
    // TODO: Port GX_BLEND_PLANEMASK_BG2 to PAL
    #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port G2_SetBlendAlpha to PAL
        #endif
        G2_SetBlendAlpha((GX_BLEND_PLANEMASK_BG2), (GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD), param0->unk_05, param0->unk_06);
        #else
        // TODO: Port GX_BLEND_PLANEMASK_BD to PAL
        #endif
        Bg_ToggleLayer(BG_LAYER_MAIN_3, 1);
    } else {
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port G2_SetBlendAlpha to PAL
        #endif
        G2_SetBlendAlpha((GX_BLEND_PLANEMASK_BG3), (GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BD), param0->unk_05, param0->unk_06);
        #else
        // TODO: Port GX_BLEND_PLANEMASK_BD to PAL
        #endif
        Bg_ToggleLayer(BG_LAYER_MAIN_2, 1);
    }
}

static BOOL ov99_021D3D50(UnkStruct_ov99_021D3C18 *param0)
{
    param0->unk_05 -= 1;

    if (param0->unk_05 < 16) {
        param0->unk_06 += 1;
    }

    if (param0->unk_05 < 0) {
        param0->unk_05 = 0;
    }

    if (param0->unk_06 > 16) {
        param0->unk_06 = 16;
    }

    #ifdef PLATFORM_DS
    G2_ChangeBlendAlpha(param0->unk_05, param0->unk_06);
    #else
    // TODO: Port G2_ChangeBlendAlpha to PAL
    #endif

    if ((param0->unk_05 == 0) && (param0->unk_06 == 16)) {
        #ifdef PLATFORM_DS
        if (param0->unk_08 == (GX_BLEND_PLANEMASK_BG2)) {
        #else
        // TODO: Port GX_BLEND_PLANEMASK_BG2 to PAL
        #endif
            Bg_ToggleLayer(BG_LAYER_MAIN_2, 0);
            Bg_SetPriority(BG_LAYER_MAIN_2, 3);
            Bg_SetPriority(BG_LAYER_MAIN_3, 2);
        } else {
            Bg_ToggleLayer(BG_LAYER_MAIN_3, 0);
            Bg_SetPriority(BG_LAYER_MAIN_2, 2);
            Bg_SetPriority(BG_LAYER_MAIN_3, 3);
        }

        return 1;
    }

    return 0;
}
