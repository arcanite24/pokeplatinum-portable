#include "gx_layers.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#include <nnsys.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "constants/graphics.h"

#include "struct_defs/struct_02099F80.h"

#include "system.h"

static int sEngineALayers;
static int sEngineBLayers;

void GXLayers_SetBanks(const UnkStruct_02099F80 *banks)
{
    GX_ResetBankForBG();
    #ifdef PLATFORM_DS
    GX_ResetBankForBGExtPltt();
    #else
    // TODO: Port GX_ResetBankForBGExtPltt to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_ResetBankForSubBG();
    #else
    // TODO: Port GX_ResetBankForSubBG to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_ResetBankForSubBGExtPltt();
    #else
    // TODO: Port GX_ResetBankForSubBGExtPltt to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_ResetBankForOBJ();
    #else
    // TODO: Port GX_ResetBankForOBJ to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_ResetBankForOBJExtPltt();
    #else
    // TODO: Port GX_ResetBankForOBJExtPltt to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_ResetBankForSubOBJ();
    #else
    // TODO: Port GX_ResetBankForSubOBJ to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_ResetBankForSubOBJExtPltt();
    #else
    // TODO: Port GX_ResetBankForSubOBJExtPltt to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_ResetBankForTex();
    #else
    // TODO: Port GX_ResetBankForTex to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_ResetBankForTexPltt();
    #else
    // TODO: Port GX_ResetBankForTexPltt to PAL
    #endif

    #ifdef PLATFORM_DS
    GX_SetBankForBG(banks->unk_00);
    #else
    // TODO: Port GX_SetBankForBG to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_SetBankForBGExtPltt(banks->unk_04);
    #else
    // TODO: Port GX_SetBankForBGExtPltt to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_SetBankForSubBG(banks->unk_08);
    #else
    // TODO: Port GX_SetBankForSubBG to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_SetBankForSubBGExtPltt(banks->unk_0C);
    #else
    // TODO: Port GX_SetBankForSubBGExtPltt to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_SetBankForOBJ(banks->unk_10);
    #else
    // TODO: Port GX_SetBankForOBJ to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_SetBankForOBJExtPltt(banks->unk_14);
    #else
    // TODO: Port GX_SetBankForOBJExtPltt to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_SetBankForSubOBJ(banks->unk_18);
    #else
    // TODO: Port GX_SetBankForSubOBJ to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_SetBankForSubOBJExtPltt(banks->unk_1C);
    #else
    // TODO: Port GX_SetBankForSubOBJExtPltt to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_SetBankForTex(banks->unk_20);
    #else
    // TODO: Port GX_SetBankForTex to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_SetBankForTexPltt(banks->unk_24);
    #else
    // TODO: Port GX_SetBankForTexPltt to PAL
    #endif
}

void GXLayers_DisableEngineALayers(void)
{
    sEngineALayers = 0;
}

void GXLayers_EngineAToggleLayers(u8 layerMask, u8 enable)
{
    if (enable == 1) {
        if (sEngineALayers & layerMask) {
            return;
        }
    } else {
        if (!(sEngineALayers & layerMask)) {
            return;
        }
    }

    sEngineALayers ^= layerMask;
    #ifdef PLATFORM_DS
    GX_SetVisiblePlane(sEngineALayers);
    #else
    // TODO: Port GX_SetVisiblePlane to PAL
    #endif
}

void GXLayers_EngineASetLayers(int layers)
{
    sEngineALayers = layers;
    #ifdef PLATFORM_DS
    GX_SetVisiblePlane(sEngineALayers);
    #else
    // TODO: Port GX_SetVisiblePlane to PAL
    #endif
}

void GXLayers_DisableEngineBLayers(void)
{
    sEngineBLayers = 0;
}

void GXLayers_EngineBToggleLayers(u8 layerMask, u8 enable)
{
    if (enable == 1) {
        if (sEngineBLayers & layerMask) {
            return;
        }
    } else {
        if (!(sEngineBLayers & layerMask)) {
            return;
        }
    }

    sEngineBLayers ^= layerMask;
    #ifdef PLATFORM_DS
    GXS_SetVisiblePlane(sEngineBLayers);
    #else
    // TODO: Port GXS_SetVisiblePlane to PAL
    #endif
}

void GXLayers_EngineBSetLayers(int layers)
{
    sEngineBLayers = layers;
    #ifdef PLATFORM_DS
    GXS_SetVisiblePlane(sEngineBLayers);
    #else
    // TODO: Port GXS_SetVisiblePlane to PAL
    #endif
}

void GXLayers_TurnBothDispOn(void)
{
    #ifdef PLATFORM_DS
    GX_DispOn();
    #else
    // TODO: Port GX_DispOn to PAL
    #endif
    #ifdef PLATFORM_DS
    GXS_DispOn();
    #else
    // TODO: Port GXS_DispOn to PAL
    #endif
}

void GXLayers_SwapDisplay(void)
{
    if (gSystem.whichScreenIs3D == DS_SCREEN_MAIN) {
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port GX_SetDispSelect to PAL
        #endif
        GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
        #else
        // TODO: Port GX_DISP_SELECT_MAIN_SUB to PAL
        #endif
    } else {
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port GX_SetDispSelect to PAL
        #endif
        GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
        #else
        // TODO: Port GX_DISP_SELECT_SUB_MAIN to PAL
        #endif
    }
}

int GXLayers_EngineAGetLayers(void)
{
    return sEngineALayers;
}
