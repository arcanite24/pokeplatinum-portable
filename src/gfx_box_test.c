#include "gfx_box_test.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif

#define BOX_TEST_RESULT_READY 0

static void SetBoxTestParams(const fx16 x, const fx16 y, const fx16 z, const fx16 width, const fx16 height, const fx16 depth, GXBoxTestParam *boxTestParam);
static u32 PerformBoxTest(const GXBoxTestParam *boxTestParam);
static void ReduceFx32ToFx16AndScale(const fx32 in, fx16 *outVal, int *outScale);

u32 GFXBoxTest_IsModelInView(const NNSG3dResMdl *model, const VecFx32 *positionPtr, const MtxFx33 *rotation, const VecFx32 *scale)
{
    GXBoxTestParam boxTestParam;

    VecFx32 position = *positionPtr;
    NNSG3dResMdlInfo *modelInfo = NNS_G3dGetMdlInfo(model);

    SetBoxTestParams(modelInfo->boxX, modelInfo->boxY, modelInfo->boxZ, modelInfo->boxW, modelInfo->boxH, modelInfo->boxD, &boxTestParam);

    #ifdef PLATFORM_DS
    NNS_G3dGlbSetBaseTrans(&position);
    #else
    // TODO: Port NNS_G3dGlbSetBaseTrans to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGlbSetBaseRot(rotation);
    #else
    // TODO: Port NNS_G3dGlbSetBaseRot to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGlbSetBaseScale(scale);
    #else
    // TODO: Port NNS_G3dGlbSetBaseScale to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGlbFlush();
    #else
    // TODO: Port NNS_G3dGlbFlush to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGePushMtx();
    #else
    // TODO: Port NNS_G3dGePushMtx to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGeScale(modelInfo->boxPosScale, modelInfo->boxPosScale, modelInfo->boxPosScale);
    #else
    // TODO: Port NNS_G3dGeScale to PAL
    #endif

    u32 boxTestResult = PerformBoxTest(&boxTestParam);
    #ifdef PLATFORM_DS
    NNS_G3dGePopMtx(1);
    #else
    // TODO: Port NNS_G3dGePopMtx to PAL
    #endif

    return boxTestResult;
}

u32 GFXBoxTest_IsBoxAtPositionInView(const VecFx32 *position, const GFXTestBox *box)
{
    GXBoxTestParam boxTestParam;
    SetBoxTestParams(0, 0, 0, box->width, box->height, box->depth, &boxTestParam);

    #ifdef PLATFORM_DS
    NNS_G3dGlbSetBaseTrans(position);
    #else
    // TODO: Port NNS_G3dGlbSetBaseTrans to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGlbFlush();
    #else
    // TODO: Port NNS_G3dGlbFlush to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGePushMtx();
    #else
    // TODO: Port NNS_G3dGePushMtx to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGeScale(box->xScale * FX32_ONE, box->yScale * FX32_ONE, box->zScale * FX32_ONE);
    #else
    // TODO: Port NNS_G3dGeScale to PAL
    #endif

    u32 boxTestResult = PerformBoxTest(&boxTestParam);
    #ifdef PLATFORM_DS
    NNS_G3dGePopMtx(1);
    #else
    // TODO: Port NNS_G3dGePopMtx to PAL
    #endif

    return boxTestResult;
}

void GFXBoxTest_MakeBox(const fx32 width, const fx32 height, const fx32 depth, GFXTestBox *box)
{
    ReduceFx32ToFx16AndScale(width, &box->width, &box->xScale);
    ReduceFx32ToFx16AndScale(height, &box->height, &box->yScale);
    ReduceFx32ToFx16AndScale(depth, &box->depth, &box->zScale);
}

static void SetBoxTestParams(const fx16 x, const fx16 y, const fx16 z, const fx16 width, const fx16 height, const fx16 depth, GXBoxTestParam *boxTestParam)
{
    boxTestParam->x = x;
    boxTestParam->y = y;
    boxTestParam->z = z;
    boxTestParam->width = width;
    boxTestParam->height = height;
    boxTestParam->depth = depth;
}

static u32 PerformBoxTest(const GXBoxTestParam *boxTestParam)
{
    s32 boxTestResult = 1;

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port NNS_G3dGePolygonAttr to PAL
    #endif
    NNS_G3dGePolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 0, GX_POLYGON_ATTR_MISC_FAR_CLIPPING | GX_POLYGON_ATTR_MISC_DISP_1DOT);
    #else
    // TODO: Port GX_POLYGON_ATTR_MISC_DISP_1DOT to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port NNS_G3dGeBegin to PAL
    #endif
    NNS_G3dGeBegin(GX_BEGIN_TRIANGLES);
    #else
    // TODO: Port GX_BEGIN_TRIANGLES to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGeEnd();
    #else
    // TODO: Port NNS_G3dGeEnd to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGeBoxTest(boxTestParam);
    #else
    // TODO: Port NNS_G3dGeBoxTest to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGeFlushBuffer();
    #else
    // TODO: Port NNS_G3dGeFlushBuffer to PAL
    #endif

    while (G3X_GetBoxTestResult(&boxTestResult) != BOX_TEST_RESULT_READY) {}

    return boxTestResult;
}

static void ReduceFx32ToFx16AndScale(const fx32 in, fx16 *outVal, int *outScale)
{
    u8 scaleExponent = 0;
    u32 asU32 = (u32)in;

    while (asU32 >= FX16_MAX) {
        asU32 /= 2;
        scaleExponent++;
    }

    *outVal = (fx16)asU32; // Safe conversion: asU32 is below the max fx16 value
    (*outScale) = 1;

    for (int i = 0; i < scaleExponent; i++) {
        (*outScale) *= 2;
    }
}
