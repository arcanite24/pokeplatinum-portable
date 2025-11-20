#include <nitro.h>
#include <string.h>

#include "gx_layers.h"
#include "pokemon.h"
#include "pokemon_sprite.h"
#include "unk_0202419C.h"

void ov105_02245CD0(void);
void ov105_02245D50(PokemonSpriteManager *param0);
PokemonSprite *ov105_02245D88(PokemonSpriteManager *param0, int param1, Pokemon *param2, int param3, int param4, int param5);
void ov105_02245DB8(PokemonSprite *param0, int param1);

void ov105_02245CD0(void)
{
    #ifdef PLATFORM_DS
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_BG0, 1);
    #else
    // TODO: Port GX_PLANEMASK_BG0 to PAL
    #endif
    #ifdef PLATFORM_DS
    G2_SetBG0Priority(0);
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
    return;
}

void ov105_02245D50(PokemonSpriteManager *param0)
{
    G3_ResetG3X();

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

    PokemonSpriteManager_DrawSprites(param0);
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_SORTMODE_MANUAL to PAL
    #endif
    G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z);
    #else
    // TODO: Port GX_BUFFERMODE_Z to PAL
    #endif

    return;
}

PokemonSprite *ov105_02245D88(PokemonSpriteManager *param0, int param1, Pokemon *param2, int param3, int param4, int param5)
{
    PokemonSpriteTemplate v0;
    PokemonSprite *v1;
    int v2;
    u32 v3;

    Pokemon_BuildSpriteTemplate(&v0, param2, 2);

    v2 = 0;
    v1 = PokemonSpriteManager_CreateSprite(param0, &v0, param3, param4 + v2, param5, param1, NULL, NULL);

    return v1;
}

void ov105_02245DB8(PokemonSprite *param0, int param1)
{
    PokemonSprite_SetAttribute(param0, MON_SPRITE_FLIP_H, param1);
    return;
}
