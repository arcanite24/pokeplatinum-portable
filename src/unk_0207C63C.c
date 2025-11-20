#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <nnsys.h>
#include <string.h>

#include "struct_defs/struct_0207C894.h"
#include "struct_defs/struct_0207C8C4.h"

#include "camera.h"
#include "g3d_pipeline.h"
#include "gx_layers.h"
#include "heap.h"
#include "particle_system.h"
#include "spl.h"
#include "unk_0202419C.h"

void sub_0207C63C(void);
void sub_0207C664(void);
void sub_0207C6AC(void);
void sub_0207C730(void);
void sub_0207C770(void);
UnkStruct_0207C8C4 *sub_0207C894(UnkStruct_0207C894 *param0);
void sub_0207C8C4(UnkStruct_0207C8C4 *param0, int param1);
BOOL sub_0207C8E0(UnkStruct_0207C8C4 *param0);
void sub_0207C8F4(UnkStruct_0207C8C4 *param0);
static u32 sub_0207C794(u32 param0, BOOL param1);
static u32 sub_0207C7B0(u32 param0, BOOL param1);
static ParticleSystem *sub_0207C7CC(int heapID);
static ParticleSystem *sub_0207C814(int heapID, int param1, int param2);
static void sub_0207C83C(ParticleSystem *param0);
static void sub_0207C854(SPLEmitter *param0);

void sub_0207C63C(void)
{
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
}

void sub_0207C664(void)
{
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port G2_SetBlendAlpha to PAL
    #endif
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_BLEND_PLANEMASK_NONE to PAL
    #endif
    G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE, GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3, 11, 7);
    #else
    // TODO: Port GX_BLEND_PLANEMASK_BG3 to PAL
    #endif
    #ifdef PLATFORM_DS
    G2S_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE, GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3, 7, 8);
    #else
    // TODO: Port GX_BLEND_PLANEMASK_BG3 to PAL
    #endif
}

G3DPipelineBuffers *sub_0207C690(int heapID)
{
    return G3DPipeline_Init(heapID, TEXTURE_VRAM_SIZE_256K, PALETTE_VRAM_SIZE_32K, sub_0207C6AC);
}

void sub_0207C6AC(void)
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
    G3X_AntiAlias(1);
    G3X_AlphaTest(0, 0);
    G3X_AlphaBlend(1);
    G3X_EdgeMarking(0);
    #ifdef PLATFORM_DS
    G3X_SetFog(0, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0);
    #else
    // TODO: Port GX_FOGBLEND_COLOR_ALPHA to PAL
    #endif

    #ifdef PLATFORM_DS
    G3X_SetClearColor(GX_RGB(0, 0, 0), 0, 0x7fff, 63, 0);
    #else
    // TODO: Port GX_RGB to PAL
    #endif
    G3_ViewPort(0, 0, 255, 191);
}

void sub_0207C730(void)
{
    NNSGfdTexKey v0;
    NNSGfdPlttKey v1;
    u32 v2, v3;

    #ifdef PLATFORM_DS
    v0 = NNS_GfdAllocTexVram(0x2000 * 4, 0, 0);
    #else
    // TODO: Port NNS_GfdAllocTexVram to PAL
    #endif
    #ifdef PLATFORM_DS
    v1 = NNS_GfdAllocPlttVram(0x20 * (4 + 1), 0, 0);
    #else
    // TODO: Port NNS_GfdAllocPlttVram to PAL
    #endif

    #ifdef PLATFORM_DS
    GF_ASSERT(v0 != NNS_GFD_ALLOC_ERROR_TEXKEY);
    #else
    // TODO: Port NNS_GFD_ALLOC_ERROR_TEXKEY to PAL
    #endif
    #ifdef PLATFORM_DS
    GF_ASSERT(v1 != NNS_GFD_ALLOC_ERROR_PLTTKEY);
    #else
    // TODO: Port NNS_GFD_ALLOC_ERROR_PLTTKEY to PAL
    #endif

    #ifdef PLATFORM_DS
    v2 = NNS_GfdGetTexKeyAddr(v0);
    #else
    // TODO: Port NNS_GfdGetTexKeyAddr to PAL
    #endif
    #ifdef PLATFORM_DS
    v3 = NNS_GfdGetPlttKeyAddr(v1);
    #else
    // TODO: Port NNS_GfdGetPlttKeyAddr to PAL
    #endif

    ParticleSystem_ZeroAll();
}

void sub_0207C770(void)
{
    int v0;
    const MtxFx43 *v1;

    G3_ResetG3X();

    v0 = ParticleSystem_DrawAll();

    if (v0 > 0) {
        G3_ResetG3X();
        #ifdef PLATFORM_DS
        NNS_G2dSetupSoftwareSpriteCamera();
        #else
        // TODO: Port NNS_G2dSetupSoftwareSpriteCamera to PAL
        #endif
    }

    ParticleSystem_UpdateAll();
    #ifdef PLATFORM_DS
    G3_RequestSwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z);
    #else
    // TODO: Port GX_BUFFERMODE_Z to PAL
    #endif
}

static u32 sub_0207C794(u32 param0, BOOL param1)
{
    NNSGfdTexKey v0;
    u32 v1;

    #ifdef PLATFORM_DS
    v0 = NNS_GfdAllocTexVram(param0, param1, 0);
    #else
    // TODO: Port NNS_GfdAllocTexVram to PAL
    #endif
    ParticleSystem_RegisterTextureKey(v0);

    #ifdef PLATFORM_DS
    v1 = NNS_GfdGetTexKeyAddr(v0);
    #else
    // TODO: Port NNS_GfdGetTexKeyAddr to PAL
    #endif
    return v1;
}

static u32 sub_0207C7B0(u32 param0, BOOL param1)
{
    NNSGfdPlttKey v0;
    u32 v1;

    #ifdef PLATFORM_DS
    v0 = NNS_GfdAllocPlttVram(param0, param1, 0);
    #else
    // TODO: Port NNS_GfdAllocPlttVram to PAL
    #endif
    ParticleSystem_RegisterPaletteKey(v0);

    #ifdef PLATFORM_DS
    v1 = NNS_GfdGetPlttKeyAddr(v0);
    #else
    // TODO: Port NNS_GfdGetPlttKeyAddr to PAL
    #endif
    return v1;
}

static ParticleSystem *sub_0207C7CC(int heapID)
{
    ParticleSystem *v0;
    void *v1;
    Camera *camera;

    v1 = Heap_Alloc(heapID, 0x4800);
    v0 = ParticleSystem_New(sub_0207C794, sub_0207C7B0, v1, 0x4800, 1, heapID);
    camera = ParticleSystem_GetCamera(v0);

    if (camera != NULL) {
        Camera_SetClipping(FX32_ONE, FX32_ONE * 900, camera);
    }

    return v0;
}

static ParticleSystem *sub_0207C814(int heapID, int param1, int param2)
{
    ParticleSystem *v0 = sub_0207C7CC(heapID);
    void *v1 = ParticleSystem_LoadResourceFromNARC(param1, param2, heapID);

    ParticleSystem_SetResource(v0, v1, (1 << 1) | (1 << 3), 1);

    return v0;
}

void sub_0207C83C(ParticleSystem *param0)
{
    void *v0 = ParticleSystem_GetHeapStart(param0);

    ParticleSystem_Free(param0);
    Heap_Free(v0);
}

static void sub_0207C854(SPLEmitter *param0)
{
    VecFx32 v0 = { 0, 0, 0 };

    VEC_Set(&v0, 0, 8 * 172, 0);
    SPLEmitter_SetPos(param0, &v0);
}

UnkStruct_0207C8C4 *sub_0207C894(UnkStruct_0207C894 *param0)
{
    int v0;
    int v1;
    int v2;
    int v3;
    UnkStruct_0207C8C4 *v4 = Heap_Alloc(param0->heapID, sizeof(UnkStruct_0207C8C4));

    GF_ASSERT(v4 != NULL);

    v4->unk_00 = *param0;
    v4->unk_0C = sub_0207C814(v4->unk_00.heapID, 124, v4->unk_00.unk_04);

    ParticleSystem_SetCameraProjection(v4->unk_0C, 1);

    return v4;
}

void sub_0207C8C4(UnkStruct_0207C8C4 *param0, int param1)
{
    int v0;
    int v1;
    int v2;
    int v3;
    UnkStruct_0207C8C4 *v4 = param0;

    ParticleSystem_CreateEmitterWithCallback(v4->unk_0C, param1, sub_0207C854, v4);
    ParticleSystem_SetCameraProjection(v4->unk_0C, 1);
}

BOOL sub_0207C8E0(UnkStruct_0207C8C4 *param0)
{
    UnkStruct_0207C8C4 *v0 = param0;

    if (ParticleSystem_GetActiveEmitterCount(v0->unk_0C) == 0) {
        return 0;
    }

    return 1;
}

void sub_0207C8F4(UnkStruct_0207C8C4 *param0)
{
    sub_0207C83C(param0->unk_0C);
    Heap_Free(param0);
}
