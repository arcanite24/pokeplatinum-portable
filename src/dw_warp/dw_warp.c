#include "dw_warp/dw_warp.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "constants/graphics.h"

#include "struct_defs/struct_02099F80.h"

#include "camera.h"
#include "easy3d_object.h"
#include "g3d_pipeline.h"
#include "gx_layers.h"
#include "heap.h"
#include "narc.h"
#include "overlay_manager.h"
#include "render_text.h"
#include "screen_fade.h"
#include "sound_playback.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "system.h"
#include "touch_pad.h"
#include "unk_0202419C.h"

#define DWARP_SND_EFFECT_DELAY 15
#define DWARP_ANM_DURATION     85

typedef struct DistortionWorldWarp {
    G3DPipelineBuffers *p3DCallback;
    Camera *camera;
    SysTask *task;
    int frameCnt;
    int soundEffectCnt;
    Easy3DObject animationObj;
    Easy3DAnim animationAnimation;
    Easy3DAnim animationAnimation2;
    Easy3DModel animationModel;
    u32 moveFrame;
    int cameraAngle;
    int cameraAngleCalc;
    fx32 cameraDistance;
    int cameraPerspective;
    NNSFndAllocator allocator;
} DistortionWorldWarp;

static void DWWarp_Update(SysTask *task, void *data);
static void DWWarp_VBlankIntr(void *data);
static void DWWarp_VramSetBank(void);
static void DWWarp_InitCamera(DistortionWorldWarp *warp);
static void DWWarp_DeleteCamera(DistortionWorldWarp *warp);
static void DWWarp_InitModel(DistortionWorldWarp *warp);
static void DWWarp_DeleteModel(DistortionWorldWarp *warp);
static void Model3D_Update(DistortionWorldWarp *warp);
static G3DPipelineBuffers *DWWarp_Init3D(int heapID);
static void DWWarp_Setup3D(void);
static void DWWarp_Exit3D(G3DPipelineBuffers *param0);
static void DWWarp_CameraMove(DistortionWorldWarp *warp);

BOOL DWWarp_Init(ApplicationManager *appMan, int *state)
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
    #ifdef PLATFORM_DS
    G2_BlendNone();
    #else
    // TODO: Port G2_BlendNone to PAL
    #endif
    G2S_BlendNone();

    Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_DISTORTION_WORLD_WARP, HEAP_SIZE_DISTORTION_WORLD_WARP);

    DistortionWorldWarp *dww = ApplicationManager_NewData(appMan, sizeof(DistortionWorldWarp), HEAP_ID_DISTORTION_WORLD_WARP);
    MI_CpuClear8(dww, sizeof(DistortionWorldWarp));
    dww->p3DCallback = DWWarp_Init3D(HEAP_ID_DISTORTION_WORLD_WARP);

    SetAutorepeat(4, 8);
    DWWarp_VramSetBank();
    EnableTouchPad();
    InitializeTouchPad(4);

    DWWarp_InitModel(dww);
    DWWarp_InitCamera(dww);
    StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_IN, FADE_TYPE_BRIGHTNESS_IN, COLOR_BLACK, 16, 1, HEAP_ID_DISTORTION_WORLD_WARP);

    gSystem.whichScreenIs3D = DS_SCREEN_MAIN;

    GXLayers_SwapDisplay();
    GXLayers_TurnBothDispOn();
    RenderControlFlags_SetCanABSpeedUpPrint(TRUE);
    RenderControlFlags_SetAutoScrollFlags(AUTO_SCROLL_DISABLED);
    RenderControlFlags_SetSpeedUpOnTouch(FALSE);

    dww->task = SysTask_Start(DWWarp_Update, dww, 60000);
    SetVBlankCallback(DWWarp_VBlankIntr, dww);

    return TRUE;
}

enum DWWarpState {
    DWARP_SEQ_SCREENWIPE = 0,
    DWARP_SEQ_LOOP,
    DWARP_SEQ_CLEAR_SCREEN,
    DWARP_SEQ_WAIT
};

BOOL DWWarp_Main(ApplicationManager *appMan, int *state)
{
    DistortionWorldWarp *warp = ApplicationManager_Data(appMan);

    switch (*state) {
    case DWARP_SEQ_SCREENWIPE:
        if (IsScreenFadeDone() == TRUE) {
            (*state)++;
        }
        break;
    case DWARP_SEQ_LOOP:
        if (warp->soundEffectCnt == DWARP_SND_EFFECT_DELAY) {
            Sound_PlayEffect(SEQ_SE_PL_SYUWA2);
        }

        warp->soundEffectCnt++;
        warp->frameCnt++;

        if (warp->frameCnt > DWARP_ANM_DURATION) {
            (*state)++;
        }
        break;
    case DWARP_SEQ_CLEAR_SCREEN:
        StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_OUT, FADE_TYPE_BRIGHTNESS_OUT, COLOR_BLACK, 20, 1, HEAP_ID_DISTORTION_WORLD_WARP);
        (*state)++;
        break;
    case DWARP_SEQ_WAIT:
        if (IsScreenFadeDone() == TRUE) {
            return TRUE;
        }
        break;
    }

    DWWarp_CameraMove(warp);

    return FALSE;
}

BOOL DWWarp_Exit(ApplicationManager *appMan, int *state)
{
    DistortionWorldWarp *warp = ApplicationManager_Data(appMan);

    SysTask_Done(warp->task);

    DWWarp_DeleteModel(warp);
    DWWarp_DeleteCamera(warp);
    DWWarp_Exit3D(warp->p3DCallback);

    SetVBlankCallback(NULL, NULL);
    DisableHBlank();
    DisableTouchPad();
    RenderControlFlags_SetCanABSpeedUpPrint(FALSE);
    RenderControlFlags_SetAutoScrollFlags(AUTO_SCROLL_DISABLED);
    RenderControlFlags_SetSpeedUpOnTouch(FALSE);
    ApplicationManager_FreeData(appMan);
    Heap_Destroy(HEAP_ID_DISTORTION_WORLD_WARP);

    return TRUE;
}

static void DWWarp_Update(SysTask *task, void *data)
{
    DistortionWorldWarp *dw = data;

    Model3D_Update(dw);
    #ifdef PLATFORM_DS
    G3_RequestSwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
    #else
    // TODO: Port GX_BUFFERMODE_W to PAL
    #endif
}

static void DWWarp_VBlankIntr(void *data)
{
    DistortionWorldWarp *dw = data;
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_SetIrqCheckFlag to PAL
    #endif
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
    #else
    // TODO: Port OS_IE_V_BLANK to PAL
    #endif
}

static void DWWarp_VramSetBank(void)
{
    GXLayers_DisableEngineALayers();
    GXLayers_DisableEngineBLayers();

    UnkStruct_02099F80 vramBank = {
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
        GX_VRAM_OBJ_64_E,
        #else
        // TODO: Port GX_VRAM_OBJ_64_E to PAL
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
        GX_VRAM_TEXPLTT_01_FG
        #else
        // TODO: Port GX_VRAM_TEXPLTT_01_FG to PAL
        #endif
    };

    GXLayers_SetBanks(&vramBank);

    MI_CpuClear32((void *)HW_BG_VRAM, HW_BG_VRAM_SIZE);
    MI_CpuClear32((void *)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
    MI_CpuClear32((void *)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
    MI_CpuClear32((void *)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
}

static void DWWarp_InitCamera(DistortionWorldWarp *warp)
{
    static const CameraAngle DWW_CameraAngle = {
        0x10000 - 0x1c7d,
        0,
        0,
    };
    VecFx32 target = { 0, 0, 0 };

    warp->camera = Camera_Alloc(HEAP_ID_DISTORTION_WORLD_WARP);

    Camera_InitWithTarget(&target, 160 << FX32_SHIFT, &DWW_CameraAngle, (22 * 0xffff) / 360, 0, 0, warp->camera);
    Camera_SetClipping(0, FX32_ONE * 300, warp->camera);

    CameraAngle angle = { 0, 0, 0, 0 };

    angle = Camera_GetAngle(warp->camera);
    angle.x = (0x10000 - 0x3fef);

    Camera_SetAngleAroundTarget(&angle, warp->camera);

    Camera_SetAsActive(warp->camera);
}

static void DWWarp_DeleteCamera(DistortionWorldWarp *warp)
{
    Camera_Delete(warp->camera);
}

static void DWWarp_InitModel(DistortionWorldWarp *warp)
{
    HeapExp_FndInitAllocator(&warp->allocator, HEAP_ID_DISTORTION_WORLD_WARP, 4);

    NARC *narc = NARC_ctor(NARC_INDEX_DEMO__TITLE__TITLEDEMO, HEAP_ID_DISTORTION_WORLD_WARP);

    Easy3DModel_LoadFrom(&warp->animationModel, narc, 16, HEAP_ID_DISTORTION_WORLD_WARP);

    #ifdef PLATFORM_DS
    NNS_G3dMdlUseMdlAlpha(warp->animationModel.model);
    #else
    // TODO: Port NNS_G3dMdlUseMdlAlpha to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dMdlUseMdlPolygonID(warp->animationModel.model);
    #else
    // TODO: Port NNS_G3dMdlUseMdlPolygonID to PAL
    #endif

    Easy3DAnim_LoadFrom(&warp->animationAnimation, &warp->animationModel, narc, 18, HEAP_ID_DISTORTION_WORLD_WARP, &warp->allocator);
    Easy3DAnim_SetFrame(&warp->animationAnimation, 0);

    Easy3DAnim_LoadFrom(&warp->animationAnimation2, &warp->animationModel, narc, 17, HEAP_ID_DISTORTION_WORLD_WARP, &warp->allocator);
    Easy3DAnim_SetFrame(&warp->animationAnimation2, 0);
    Easy3DObject_Init(&warp->animationObj, &warp->animationModel);

    Easy3DObject_SetPosition(&warp->animationObj, 0, 0, 0);
    Easy3DObject_SetScale(&warp->animationObj, FX32_ONE, FX32_ONE, FX32_ONE);
    Easy3DObject_SetVisible(&warp->animationObj, TRUE);

    Easy3DObject_AddAnim(&warp->animationObj, &warp->animationAnimation);
    Easy3DObject_AddAnim(&warp->animationObj, &warp->animationAnimation2);

    NARC_dtor(narc);

    warp->cameraAngle = ((0x10000 - 0x3fef) - (0x10000 - 0x1c7d)) / 30;
    warp->cameraAngleCalc = 0x10000 - 0x1c7d;
    warp->cameraDistance = FX32_ONE;
    warp->cameraPerspective = 60 << 8;
}

static void DWWarp_DeleteModel(DistortionWorldWarp *warp)
{
    Easy3DModel_Release(&warp->animationModel);
    Easy3DAnim_Release(&warp->animationAnimation, &warp->allocator);
    Easy3DAnim_Release(&warp->animationAnimation2, &warp->allocator);
}

static void Model3D_Update(DistortionWorldWarp *warp)
{
    VecFx32 scaleVec, transVec;
    MtxFx33 rot33;

    scaleVec.x = FX32_ONE;
    scaleVec.y = FX32_ONE;
    scaleVec.z = FX32_ONE;

    transVec.x = 0;
    transVec.y = 0;
    transVec.z = 0;

    MTX_Identity33(&rot33);

    G3_ResetG3X();
    Camera_SetAsActive(warp->camera);
    Camera_ComputeProjectionMatrix(0, warp->camera);
    Camera_ComputeViewMatrix();

    #ifdef PLATFORM_DS
    NNS_G3dGlbLightVector(0, 0, -FX32_ONE, 0);
    #else
    // TODO: Port NNS_G3dGlbLightVector to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port NNS_G3dGlbLightColor to PAL
    #endif
    NNS_G3dGlbLightColor(0, GX_RGB(28, 28, 28));
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port NNS_G3dGlbMaterialColorDiffAmb to PAL
    #endif
    NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), FALSE);
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port NNS_G3dGlbMaterialColorSpecEmi to PAL
    #endif
    NNS_G3dGlbMaterialColorSpecEmi(GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), FALSE);
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGlbSetBaseTrans(&transVec);
    #else
    // TODO: Port NNS_G3dGlbSetBaseTrans to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGlbSetBaseRot(&rot33);
    #else
    // TODO: Port NNS_G3dGlbSetBaseRot to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G3dGlbSetBaseScale(&scaleVec);
    #else
    // TODO: Port NNS_G3dGlbSetBaseScale to PAL
    #endif

    Easy3DAnim_UpdateLooped(&warp->animationAnimation, FX32_ONE);
    Easy3DAnim_UpdateLooped(&warp->animationAnimation2, FX32_ONE);

    #ifdef PLATFORM_DS
    NNS_G3dGePushMtx();
    #else
    // TODO: Port NNS_G3dGePushMtx to PAL
    #endif

    Easy3DObject_Draw(&warp->animationObj);
    #ifdef PLATFORM_DS
    NNS_G3dGePopMtx(1);
    #else
    // TODO: Port NNS_G3dGePopMtx to PAL
    #endif
}

static G3DPipelineBuffers *DWWarp_Init3D(int heapID)
{
    return G3DPipeline_Init(heapID, TEXTURE_VRAM_SIZE_256K, PALETTE_VRAM_SIZE_32K, DWWarp_Setup3D);
}

static void DWWarp_Setup3D(void)
{
    #ifdef PLATFORM_DS
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_BG0, 1);
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
    G3X_SetClearColor(GX_RGB(0, 0, 0), 0, 0x7fff, 63, FALSE);
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    G3_ViewPort(0, 0, 255, 191);
}

static void DWWarp_Exit3D(G3DPipelineBuffers *param0)
{
    G3DPipelineBuffers_Free(param0);
}

static void DWWarp_CameraMove(DistortionWorldWarp *warp)
{
    VecFx32 v0 = { 0, 0, 0 };
    CameraAngle v1 = { 0, 0, 0, 0 };
    int v2;

    Camera_AdjustFOV(-(warp->cameraPerspective >> 8), warp->camera);
    warp->cameraPerspective -= 0x80;

    if (warp->cameraPerspective < (16 << 8)) {
        warp->cameraPerspective = (16 << 8);
    }

    warp->moveFrame++;
}
