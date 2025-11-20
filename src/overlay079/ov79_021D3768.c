#include "overlay079/ov79_021D3768.h"

#include <nitro.h>
#include <string.h>

#include "overlay079/struct_ov79_021D3820.h"
#include "overlay079/struct_ov79_021D38D0.h"

#include "camera.h"
#include "gx_layers.h"
#include "pokemon.h"
#include "pokemon_sprite.h"
#include "unk_0202419C.h"

static void ov79_021D3870(UnkStruct_ov79_021D3820 *param0, int param1);
static void ov79_021D38D0(UnkStruct_ov79_021D3820 *param0, UnkStruct_ov79_021D38D0 *param1, int heapIDs);

void ov79_021D3768(UnkStruct_ov79_021D3820 *param0, UnkStruct_ov79_021D38D0 *param1, int heapID)
{
    MI_CpuClear8(param0, sizeof(UnkStruct_ov79_021D3820));

    #ifdef PLATFORM_DS
    NNS_G3dInit();
    #else
    // TODO: Port NNS_G3dInit to PAL
    #endif
    G3X_Init();
    G3X_InitMtxStack();
    #ifdef PLATFORM_DS
    G3X_SetShading(GX_SHADING_TOON);
    #else
    // TODO: Port GX_SHADING_TOON to PAL
    #endif
    G3X_AlphaTest(0, 0);
    G3X_AlphaBlend(1);
    G3X_AntiAlias(1);
    G3X_EdgeMarking(0);
    G3X_SetFog(0, 0, 0, 0);
    G3X_SetClearColor(0, 0, 0x7fff, 63, 0);
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_SORTMODE_AUTO to PAL
    #endif
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
    #else
    // TODO: Port GX_BUFFERMODE_W to PAL
    #endif
    G3_ViewPort(0, 0, 255, 191);

    ov79_021D3870(param0, heapID);
    ov79_021D38D0(param0, param1, heapID);
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
}

void ov79_021D3820(UnkStruct_ov79_021D3820 *param0)
{
    G3_ResetG3X();
    Camera_ComputeViewMatrix();

    #ifdef PLATFORM_DS
    G3_MtxMode(GX_MTXMODE_PROJECTION);
    #else
    // TODO: Port GX_MTXMODE_PROJECTION to PAL
    #endif
    G3_Identity();
    #ifdef PLATFORM_DS
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    #else
    // TODO: Port GX_MTXMODE_POSITION_VECTOR to PAL
    #endif
    G3_Identity();

    #ifdef PLATFORM_DS
    NNS_G3dGlbFlush();
    #else
    // TODO: Port NNS_G3dGlbFlush to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G2dSetupSoftwareSpriteCamera();
    #else
    // TODO: Port NNS_G2dSetupSoftwareSpriteCamera to PAL
    #endif
    PokemonSpriteManager_DrawSprites(param0->unk_04);

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_SORTMODE_MANUAL to PAL
    #endif
    G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z);
    #else
    // TODO: Port GX_BUFFERMODE_Z to PAL
    #endif
}

void ov79_021D385C(UnkStruct_ov79_021D3820 *param0)
{
    Camera_Delete(param0->camera);
    PokemonSpriteManager_Free(param0->unk_04);
}

static void ov79_021D3870(UnkStruct_ov79_021D3820 *param0, int param1)
{
    VecFx32 v0 = { 0, 0, 0x10000 };
    CameraAngle v1 = { 0, 0, 0 };
    fx32 v2 = 0x10000;
    u16 v3 = 0x5c1;

    param0->camera = Camera_Alloc(param1);

    Camera_InitWithPosition(&v0, v2, &v1, v3, 1, param0->camera);
    Camera_SetClipping(0, FX32_CONST(100), param0->camera);
    Camera_ReleaseTarget(param0->camera);
    Camera_SetAsActive(param0->camera);
}

static void ov79_021D38D0(UnkStruct_ov79_021D3820 *param0, UnkStruct_ov79_021D38D0 *param1, int heapID)
{
    PokemonSpriteTemplate v0;
    PokemonSprite *v1;

    param0->unk_04 = PokemonSpriteManager_New(heapID);

    Pokemon_BuildSpriteTemplate(&v0, param1->unk_00, 2);

    v1 = PokemonSpriteManager_CreateSprite(param0->unk_04, &v0, 128, 96, 0, 0, NULL, NULL);
    PokemonSprite_SetAttribute(v1, MON_SPRITE_FLIP_H, param1->unk_08);

    param0->unk_18 = v1;
}
