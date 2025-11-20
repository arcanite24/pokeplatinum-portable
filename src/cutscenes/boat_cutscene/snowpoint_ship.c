#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "struct_defs/struct_02099F80.h"

#include "cutscenes/boat_cutscene.h"
#include "overlay009/camera_configuration.h"

#include "camera.h"
#include "easy3d.h"
#include "gx_layers.h"
#include "heap.h"
#include "narc.h"
#include "overlay_manager.h"
#include "screen_fade.h"
#include "sound_playback.h"
#include "system.h"
#include "unk_0202419C.h"
#include "unk_0208C098.h"

#define BOAT_TRAVEL_CUTSCENE_NUM_ANIMATIONS 3

typedef struct BoatCutscene_SnowpointShip {
    Camera *camera;
    NNSG3dRenderObj renderObj;
    NNSG3dResMdl *model;
    NNSG3dResFileHeader *modelFile;
    void *animationFiles[BOAT_TRAVEL_CUTSCENE_NUM_ANIMATIONS];
    NNSFndAllocator allocator;
    NNSG3dAnmObj *anmObjs[BOAT_TRAVEL_CUTSCENE_NUM_ANIMATIONS];
    VecFx32 unused;
    u8 travelDir;
    u8 firstAnimationFinished;
} BoatCutscene_SnowpointShip;

typedef struct BoatCutscene_SnowpointShip_NarcMemberIndexes {
    u16 modelMemberIndex;
    u16 animationMemberIndexes[BOAT_TRAVEL_CUTSCENE_NUM_ANIMATIONS];
} BoatCutscene_SnowpointShip_NarcMemberIndexes;

static void BoatCutscene_SnowpointShip_Init3D(void);
static void BoatCutscene_SnowpointShip_SetGXBanks(void);
static void BoatCutscene_SnowpointShip_LoadAnimations(BoatCutscene_SnowpointShip *cutsceneData);

static const CameraConfiguration cameraConfig = {
    .distance = BOAT_TRAVEL_CUTSCENE_CAMERA_BASE_DISTANCE,
    .cameraAngle = {
        BOAT_TRAVEL_CUTSCENE_CAMERA_BASE_ANGLE_X,
        BOAT_TRAVEL_CUTSCENE_CAMERA_BASE_ANGLE_Y,
        BOAT_TRAVEL_CUTSCENE_CAMERA_BASE_ANGLE_Z,
    },
    .projectionMtx = BOAT_TRAVEL_CUTSCENE_CAMERA_BASE_PROJECTION_MTX,
    .fovY = BOAT_TRAVEL_CUTSCENE_CAMERA_BASE_FOVY,
};

static const GXRgb edgeColorTable[8] = {
    #ifdef PLATFORM_DS
    GX_RGB(0, 0, 0),
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_RGB(4, 4, 4),
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_RGB(4, 4, 4),
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_RGB(4, 4, 4),
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_RGB(4, 4, 4),
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_RGB(4, 4, 4),
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_RGB(4, 4, 4),
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    #ifdef PLATFORM_DS
    GX_RGB(4, 4, 4)
    #else
    // TODO: Port GX_RGB to PAL
    #endif
};

static BoatCutscene_SnowpointShip_NarcMemberIndexes narcMemberIndexes[4] = {
    {
        1,
        { 3, 2, 0 },
    },
    {
        5,
        { 7, 6, 4 },
    },
    {
        1,
        { 3, 2, 0 },
    },
    {
        1,
        { 3, 2, 0 },
    },
};

BOOL BoatCutscene_SnowpointShip_Init(ApplicationManager *appMan, int *state)
{
    u8 lightID;
    BoatCutscene_SnowpointShip *cutsceneData;
    BoatTravelCutscene *taskEnv;

    Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_BOAT_CUTSCENE, HEAP_SIZE_BOAT_CUTSCENE);

    cutsceneData = ApplicationManager_NewData(appMan, sizeof(BoatCutscene_SnowpointShip), HEAP_ID_BOAT_CUTSCENE);
    memset(cutsceneData, 0, sizeof(BoatCutscene_SnowpointShip));

    taskEnv = ApplicationManager_Args(appMan);

    cutsceneData->travelDir = taskEnv->travelDir;
    cutsceneData->firstAnimationFinished = FALSE;
    cutsceneData->camera = Camera_Alloc(HEAP_ID_BOAT_CUTSCENE);

    BoatCutscene_SnowpointShip_Init3D();
    BoatCutscene_SnowpointShip_LoadAnimations(cutsceneData);

    VecFx32 cameraTarget = { 0, 0, 0 };

    Camera_InitWithTarget(&cameraTarget, cameraConfig.distance, &cameraConfig.cameraAngle, cameraConfig.fovY, cameraConfig.projectionMtx, FALSE, cutsceneData->camera);
    Camera_SetAsActive(cutsceneData->camera);

    for (lightID = 0; lightID < BOAT_TRAVEL_CUTSCENE_NUM_LIGHTS; lightID++) {
        #ifdef PLATFORM_DS
        NNS_G3dGlbLightVector(lightID, taskEnv->areaModelAttrs->lightVectors[lightID].x, taskEnv->areaModelAttrs->lightVectors[lightID].y, taskEnv->areaModelAttrs->lightVectors[lightID].z);
        #else
        // TODO: Port NNS_G3dGlbLightVector to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G3dGlbLightColor(lightID, taskEnv->areaModelAttrs->lightColors[lightID]);
        #else
        // TODO: Port NNS_G3dGlbLightColor to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    NNS_G3dGlbMaterialColorDiffAmb(taskEnv->areaModelAttrs->diffuseReflectColor, taskEnv->areaModelAttrs->ambientReflectColor, taskEnv->areaModelAttrs->setDiffuseColorAsVertexColor);
    #else
    // TODO: Port NNS_G3dGlbMaterialColorDiffAmb to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGlbMaterialColorSpecEmi(taskEnv->areaModelAttrs->specularReflectColor, taskEnv->areaModelAttrs->emissionColor, taskEnv->areaModelAttrs->enableSpecularReflectShininessTable);
    #else
    // TODO: Port NNS_G3dGlbMaterialColorSpecEmi to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGlbPolygonAttr(taskEnv->areaModelAttrs->enabledLightsMask, taskEnv->areaModelAttrs->polygonMode, taskEnv->areaModelAttrs->cullMode, taskEnv->areaModelAttrs->polygonID, taskEnv->areaModelAttrs->alpha, taskEnv->areaModelAttrs->miscFlags);
    #else
    // TODO: Port NNS_G3dGlbPolygonAttr to PAL
    #endif

    App_StartScreenFade(FALSE, HEAP_ID_BOAT_CUTSCENE);
    return TRUE;
}

BOOL BoatCutscene_SnowpointShip_Main(ApplicationManager *appMan, int *state)
{
    u8 animIndex;
    BOOL unused;
    MtxFx33 rotation = {
        FX32_ONE,
        0,
        0,
        0,
        FX32_ONE,
        0,
        0,
        0,
        FX32_ONE,
    };
    VecFx32 scale = { FX32_ONE, FX32_ONE, FX32_ONE };
    VecFx32 position = { 0, 0, 0 };
    BoatCutscene_SnowpointShip *cutsceneData = ApplicationManager_Data(appMan);

    switch (*state) {
    case BOAT_TRAVEL_CUTSCENE_STATE_SFX:
        Sound_PlayEffect(SEQ_SE_DP_SHIP01);
        (*state)++;
        break;
    case BOAT_TRAVEL_CUTSCENE_STATE_FADE_OUT:
        if (!cutsceneData->firstAnimationFinished) {
            #ifdef PLATFORM_DS
            if (cutsceneData->anmObjs[0]->frame + FX32_ONE == NNS_G3dAnmObjGetNumFrame(cutsceneData->anmObjs[0])) {
            #else
            // TODO: Port NNS_G3dAnmObjGetNumFrame to PAL
            #endif
                cutsceneData->firstAnimationFinished = TRUE;
                App_StartScreenFade(TRUE, HEAP_ID_BOAT_CUTSCENE);
            }
        } else {
            if (IsScreenFadeDone()) {
                return TRUE;
            }
        }
    }

    for (animIndex = 0; animIndex < BOAT_TRAVEL_CUTSCENE_NUM_ANIMATIONS; animIndex++) {
        #ifdef PLATFORM_DS
        if (cutsceneData->anmObjs[animIndex]->frame + FX32_ONE < NNS_G3dAnmObjGetNumFrame(cutsceneData->anmObjs[animIndex])) {
        #else
        // TODO: Port NNS_G3dAnmObjGetNumFrame to PAL
        #endif
            cutsceneData->anmObjs[animIndex]->frame += FX32_ONE;
        }
    }

    G3_ResetG3X();
    Camera_ComputeViewMatrix();
    Easy3D_DrawRenderObj(&cutsceneData->renderObj, &position, &rotation, &scale);
    #ifdef PLATFORM_DS
    G3_RequestSwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
    #else
    // TODO: Port GX_BUFFERMODE_W to PAL
    #endif

    return FALSE;
}

BOOL BoatCutscene_SnowpointShip_Exit(ApplicationManager *appMan, int *state)
{
    u8 animIndex;
    BoatCutscene_SnowpointShip *cutsceneData = ApplicationManager_Data(appMan);

    for (animIndex = 0; animIndex < BOAT_TRAVEL_CUTSCENE_NUM_ANIMATIONS; animIndex++) {
        #ifdef PLATFORM_DS
        NNS_G3dFreeAnmObj(&cutsceneData->allocator, cutsceneData->anmObjs[animIndex]);
        #else
        // TODO: Port NNS_G3dFreeAnmObj to PAL
        #endif
        Heap_Free(cutsceneData->animationFiles[animIndex]);
    }

    Heap_Free(cutsceneData->modelFile);
    Camera_Delete(cutsceneData->camera);
    ApplicationManager_FreeData(appMan);
    Easy3D_Shutdown();
    Heap_Destroy(HEAP_ID_BOAT_CUTSCENE);

    return TRUE;
}

static void BoatCutscene_SnowpointShip_Init3D(void)
{
    SetVBlankCallback(NULL, NULL);
    DisableHBlank();
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

    BoatCutscene_SnowpointShip_SetGXBanks();
    Easy3D_Init(HEAP_ID_BOAT_CUTSCENE);

    G3X_EdgeMarking(TRUE);
    G3X_SetEdgeColorTable(edgeColorTable);

    GXLayers_SwapDisplay();
}

static void BoatCutscene_SnowpointShip_SetGXBanks(void)
{
    UnkStruct_02099F80 banks = {
        #ifdef PLATFORM_DS
        GX_VRAM_BG_128_C,
        #else
        // TODO: Port GX_VRAM_BG_128_C to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_BGEXTPLTT_NONE,
        #else
        // TODO: Port GX_VRAM_BGEXTPLTT_NONE to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_SUB_BG_32_H,
        #else
        // TODO: Port GX_VRAM_SUB_BG_32_H to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_SUB_BGEXTPLTT_NONE,
        #else
        // TODO: Port GX_VRAM_SUB_BGEXTPLTT_NONE to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_OBJ_16_F,
        #else
        // TODO: Port GX_VRAM_OBJ_16_F to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_OBJEXTPLTT_NONE,
        #else
        // TODO: Port GX_VRAM_OBJEXTPLTT_NONE to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_SUB_OBJ_16_I,
        #else
        // TODO: Port GX_VRAM_SUB_OBJ_16_I to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_SUB_OBJEXTPLTT_NONE,
        #else
        // TODO: Port GX_VRAM_SUB_OBJEXTPLTT_NONE to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_TEX_01_AB,
        #else
        // TODO: Port GX_VRAM_TEX_01_AB to PAL
        #endif
        #ifdef PLATFORM_DS
        GX_VRAM_TEXPLTT_0123_E
        #else
        // TODO: Port GX_VRAM_TEXPLTT_0123_E to PAL
        #endif
    };

    GXLayers_SetBanks(&banks);
}

static void BoatCutscene_SnowpointShip_LoadAnimations(BoatCutscene_SnowpointShip *cutsceneData)
{
    u8 animIndex;
    NNSG3dResTex *texture;
    void *animation;
    NARC *narc = NARC_ctor(NARC_INDEX_ARC__SHIP_DEMO_PL, HEAP_ID_BOAT_CUTSCENE);
    HeapExp_FndInitAllocator(&cutsceneData->allocator, HEAP_ID_BOAT_CUTSCENE, 4);

    cutsceneData->modelFile = NARC_AllocAndReadWholeMember(narc, narcMemberIndexes[cutsceneData->travelDir].modelMemberIndex, HEAP_ID_BOAT_CUTSCENE);
    Easy3D_InitRenderObjFromResource(&cutsceneData->renderObj, &cutsceneData->model, &cutsceneData->modelFile);
    #ifdef PLATFORM_DS
    texture = NNS_G3dGetTex(cutsceneData->modelFile);
    #else
    // TODO: Port NNS_G3dGetTex to PAL
    #endif

    #ifdef PLATFORM_DS
    NNS_G3dMdlUseGlbDiff(cutsceneData->model);
    #else
    // TODO: Port NNS_G3dMdlUseGlbDiff to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dMdlUseGlbAmb(cutsceneData->model);
    #else
    // TODO: Port NNS_G3dMdlUseGlbAmb to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dMdlUseGlbSpec(cutsceneData->model);
    #else
    // TODO: Port NNS_G3dMdlUseGlbSpec to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dMdlUseGlbEmi(cutsceneData->model);
    #else
    // TODO: Port NNS_G3dMdlUseGlbEmi to PAL
    #endif

    for (animIndex = 0; animIndex < BOAT_TRAVEL_CUTSCENE_NUM_ANIMATIONS; animIndex++) {
        cutsceneData->animationFiles[animIndex] = NARC_AllocAndReadWholeMember(narc, narcMemberIndexes[cutsceneData->travelDir].animationMemberIndexes[animIndex], HEAP_ID_BOAT_CUTSCENE);
        #ifdef PLATFORM_DS
        animation = NNS_G3dGetAnmByIdx(cutsceneData->animationFiles[animIndex], 0);
        #else
        // TODO: Port NNS_G3dGetAnmByIdx to PAL
        #endif
        #ifdef PLATFORM_DS
        cutsceneData->anmObjs[animIndex] = NNS_G3dAllocAnmObj(&cutsceneData->allocator, animation, cutsceneData->model);
        #else
        // TODO: Port NNS_G3dAllocAnmObj to PAL
        #endif

        #ifdef PLATFORM_DS
        NNS_G3dAnmObjInit(cutsceneData->anmObjs[animIndex], animation, cutsceneData->model, texture);
        #else
        // TODO: Port NNS_G3dAnmObjInit to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G3dRenderObjAddAnmObj(&cutsceneData->renderObj, cutsceneData->anmObjs[animIndex]);
        #else
        // TODO: Port NNS_G3dRenderObjAddAnmObj to PAL
        #endif
    }

    NARC_dtor(narc);
}
