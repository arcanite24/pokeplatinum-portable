#include "easy3d.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <nnsys.h>
#include <string.h>

#include "constants/graphics.h"

#include "g3d_pipeline.h"
#include "gx_layers.h"
#include "system.h"

static void Easy3D_SetupEngine(void);

void Easy3D_LoadModelFromPath(const u8 heapID, const char *path, NNSG3dResMdl **outModel, NNSG3dResFileHeader **outResource)
{
    *outResource = ReadFileToHeap(heapID, path);
    NNS_G3D_NULL_ASSERT(*outResource);

    #ifdef PLATFORM_DS
    NNSG3dResTex *texture = NNS_G3dGetTex(*outResource);
    #else
    // TODO: Port NNS_G3dGetTex to PAL
    #endif
    if (texture != NULL && Easy3D_IsTextureUploadedToVRAM(texture) == FALSE) {
        DC_FlushRange(*outResource, (*outResource)->fileSize);
        #ifdef PLATFORM_DS
        NNS_G3dResDefaultSetup(*outResource);
        #else
        // TODO: Port NNS_G3dResDefaultSetup to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port NNS_G3dGetMdlByIdx to PAL
    #endif
    *outModel = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(*outResource), 0);
    #else
    // TODO: Port NNS_G3dGetMdlSet to PAL
    #endif
}

void Easy3D_LoadModelFromResource(NNSG3dResMdl **outModel, NNSG3dResFileHeader **resource)
{
    #ifdef PLATFORM_DS
    NNSG3dResTex *texture = NNS_G3dGetTex(*resource);
    #else
    // TODO: Port NNS_G3dGetTex to PAL
    #endif

    if (texture != NULL && Easy3D_IsTextureUploadedToVRAM(texture) == FALSE) {
        DC_FlushRange(*resource, (*resource)->fileSize);
        #ifdef PLATFORM_DS
        NNS_G3dResDefaultSetup(*resource);
        #else
        // TODO: Port NNS_G3dResDefaultSetup to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port NNS_G3dGetMdlByIdx to PAL
    #endif
    *outModel = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(*resource), 0);
    #else
    // TODO: Port NNS_G3dGetMdlSet to PAL
    #endif
}

void Easy3D_InitRenderObjFromPath(const u8 heapID, const char *path, NNSG3dRenderObj *obj, NNSG3dResMdl **outModel, NNSG3dResFileHeader **outResource)
{
    Easy3D_LoadModelFromPath(heapID, path, outModel, outResource);
    #ifdef PLATFORM_DS
    NNS_G3dRenderObjInit(obj, *outModel);
    #else
    // TODO: Port NNS_G3dRenderObjInit to PAL
    #endif
}

void Easy3D_InitRenderObjFromResource(NNSG3dRenderObj *renderObj, NNSG3dResMdl **model, NNSG3dResFileHeader **resource)
{
    Easy3D_LoadModelFromResource(model, resource);
    GF_ASSERT(model);
    #ifdef PLATFORM_DS
    NNS_G3dRenderObjInit(renderObj, *model);
    #else
    // TODO: Port NNS_G3dRenderObjInit to PAL
    #endif
}

BOOL Easy3D_IsTextureUploadedToVRAM(NNSG3dResTex *texture)
{
    #ifdef PLATFORM_DS
    return (texture->texInfo.flag & NNS_G3D_RESTEX_LOADED)
    #else
    // TODO: Port NNS_G3D_RESTEX_LOADED to PAL
    #endif
        || (texture->tex4x4Info.flag & NNS_G3D_RESTEX4x4_LOADED);
}

void Easy3D_DrawRenderObj(NNSG3dRenderObj *renderObj, const VecFx32 *pos, const MtxFx33 *rot, const VecFx32 *scale)
{
    #ifdef PLATFORM_DS
    NNS_G3dGlbSetBaseTrans(pos);
    #else
    // TODO: Port NNS_G3dGlbSetBaseTrans to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGlbSetBaseRot(rot);
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
    NNS_G3dDraw(renderObj);
    #else
    // TODO: Port NNS_G3dDraw to PAL
    #endif
}

void Easy3D_DrawRenderObjSimple(NNSG3dRenderObj *renderObj, const VecFx32 *pos, const MtxFx33 *rot, const VecFx32 *scale)
{
    #ifdef PLATFORM_DS
    NNS_G3dGlbSetBaseTrans(pos);
    #else
    // TODO: Port NNS_G3dGlbSetBaseTrans to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGlbSetBaseRot(rot);
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
    NNS_G3dDraw1Mat1Shp(renderObj->resMdl, 0, 0, TRUE);
    #else
    // TODO: Port NNS_G3dDraw1Mat1Shp to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGeFlushBuffer();
    #else
    // TODO: Port NNS_G3dGeFlushBuffer to PAL
    #endif
}

static G3DPipelineBuffers *sPipelineBuffers = NULL;

void Easy3D_Init(const u8 heapID)
{
    sPipelineBuffers = G3DPipeline_Init(heapID, TEXTURE_VRAM_SIZE_256K, PALETTE_VRAM_SIZE_64K, Easy3D_SetupEngine);
}

static void Easy3D_SetupEngine(void)
{
    #ifdef PLATFORM_DS
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_BG0, TRUE);
    #else
    // TODO: Port GX_PLANEMASK_BG0 to PAL
    #endif

    #ifdef PLATFORM_DS
    G2_SetBG0Priority(1);
    #else
    // TODO: Port G2_SetBG0Priority to PAL
    #endif
    #ifdef PLATFORM_DS
    G3X_SetShading(GX_SHADING_TOON);
    #else
    // TODO: Port GX_SHADING_TOON to PAL
    #endif
    G3X_AntiAlias(TRUE);
    G3X_AlphaTest(FALSE, 0);
    G3X_AlphaBlend(TRUE);
    G3X_EdgeMarking(FALSE);
    #ifdef PLATFORM_DS
    G3X_SetFog(FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0);
    #else
    // TODO: Port GX_FOGBLEND_COLOR_ALPHA to PAL
    #endif
    #ifdef PLATFORM_DS
    G3X_SetClearColor(GX_RGB(0, 0, 0), 0, G3X_DEPTH_MAX, 63, FALSE);
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    G3_ViewPort(0, 0, HW_LCD_WIDTH - 1, HW_LCD_HEIGHT - 1);
}

void Easy3D_Shutdown(void)
{
    G3DPipelineBuffers_Free(sPipelineBuffers);
}

BOOL Easy3D_BindTextureToResource(void *resource, NNSG3dResTex *texture)
{
    #ifdef PLATFORM_DS
    NNSG3dResMdlSet *modelSet = NNS_G3dGetMdlSet((NNSG3dResFileHeader *)resource);
    #else
    // TODO: Port NNS_G3dGetMdlSet to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3D_NULL_ASSERT(modelSet);
    #else
    // TODO: Port NNS_G3D_NULL_ASSERT to PAL
    #endif

    if (texture) {
        #ifdef PLATFORM_DS
        BOOL v1 = NNS_G3dBindMdlSet(modelSet, texture);
        #else
        // TODO: Port NNS_G3dBindMdlSet to PAL
        #endif
        return TRUE;
    }

    return FALSE;
}

BOOL Easy3D_UploadTextureToVRAM(NNSG3dResTex *texture)
{
    if (texture == NULL) {
        return FALSE;
    }

    texture->texInfo.vramKey = 0;

    u32 texRequiredSize, tex4x4RequiredSize, paletteRequiredSize;
    BOOL texOk = TRUE;
    BOOL tex4x4Ok = TRUE;
    BOOL paletteOk = TRUE;
    NNSG3dTexKey texKey;
    NNSG3dTexKey tex4x4Key;
    NNSG3dPlttKey paletteKey;

    if (texture) {
        #ifdef PLATFORM_DS
        texRequiredSize = NNS_G3dTexGetRequiredSize(texture);
        #else
        // TODO: Port NNS_G3dTexGetRequiredSize to PAL
        #endif
        #ifdef PLATFORM_DS
        tex4x4RequiredSize = NNS_G3dTex4x4GetRequiredSize(texture);
        #else
        // TODO: Port NNS_G3dTex4x4GetRequiredSize to PAL
        #endif
        #ifdef PLATFORM_DS
        paletteRequiredSize = NNS_G3dPlttGetRequiredSize(texture);
        #else
        // TODO: Port NNS_G3dPlttGetRequiredSize to PAL
        #endif

        if (texRequiredSize > 0) {
            #ifdef PLATFORM_DS
            texKey = NNS_GfdAllocTexVram(texRequiredSize, FALSE, 0);
            #else
            // TODO: Port NNS_GfdAllocTexVram to PAL
            #endif

            #ifdef PLATFORM_DS
            if (texKey == NNS_GFD_ALLOC_ERROR_TEXKEY) {
            #else
            // TODO: Port NNS_GFD_ALLOC_ERROR_TEXKEY to PAL
            #endif
                texOk = FALSE;
            }
        } else {
            texKey = 0;
        }

        if (tex4x4RequiredSize > 0) {
            #ifdef PLATFORM_DS
            tex4x4Key = NNS_GfdAllocTexVram(tex4x4RequiredSize, TRUE, 0);
            #else
            // TODO: Port NNS_GfdAllocTexVram to PAL
            #endif

            #ifdef PLATFORM_DS
            if (tex4x4Key == NNS_GFD_ALLOC_ERROR_TEXKEY) {
            #else
            // TODO: Port NNS_GFD_ALLOC_ERROR_TEXKEY to PAL
            #endif
                tex4x4Ok = FALSE;
            }
        } else {
            tex4x4Key = 0;
        }

        if (paletteRequiredSize > 0) {
            #ifdef PLATFORM_DS
            #ifdef PLATFORM_DS
            #else
            // TODO: Port NNS_GfdAllocPlttVram to PAL
            #endif
            paletteKey = NNS_GfdAllocPlttVram(paletteRequiredSize, texture->tex4x4Info.flag & NNS_G3D_RESPLTT_USEPLTT4, 0);
            #else
            // TODO: Port NNS_G3D_RESPLTT_USEPLTT4 to PAL
            #endif

            #ifdef PLATFORM_DS
            if (paletteKey == NNS_GFD_ALLOC_ERROR_PLTTKEY) {
            #else
            // TODO: Port NNS_GFD_ALLOC_ERROR_PLTTKEY to PAL
            #endif
                paletteOk = FALSE;
            }
        } else {
            paletteKey = 0;
        }

        if (!texOk) {
            #ifdef PLATFORM_DS
            int result = NNS_GfdFreeTexVram(texKey);
            #else
            // TODO: Port NNS_GfdFreeTexVram to PAL
            #endif
            GF_ASSERT(!result);

            return FALSE;
        }

        if (!tex4x4Ok) {
            #ifdef PLATFORM_DS
            int result = NNS_GfdFreeTexVram(tex4x4Key);
            #else
            // TODO: Port NNS_GfdFreeTexVram to PAL
            #endif
            GF_ASSERT(!result);

            return FALSE;
        }

        if (!paletteOk) {
            #ifdef PLATFORM_DS
            int result = NNS_GfdFreePlttVram(paletteKey);
            #else
            // TODO: Port NNS_GfdFreePlttVram to PAL
            #endif
            GF_ASSERT(!result);

            return FALSE;
        }

        #ifdef PLATFORM_DS
        NNS_G3dTexSetTexKey(texture, texKey, tex4x4Key);
        #else
        // TODO: Port NNS_G3dTexSetTexKey to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G3dPlttSetPlttKey(texture, paletteKey);
        #else
        // TODO: Port NNS_G3dPlttSetPlttKey to PAL
        #endif

        DC_FlushRange(texture, texture->header.size);

        #ifdef PLATFORM_DS
        NNS_G3dTexLoad(texture, TRUE);
        #else
        // TODO: Port NNS_G3dTexLoad to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G3dPlttLoad(texture, TRUE);
        #else
        // TODO: Port NNS_G3dPlttLoad to PAL
        #endif
    }

    return TRUE;
}
