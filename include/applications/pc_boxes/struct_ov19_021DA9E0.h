#ifndef POKEPLATINUM_STRUCT_OV19_021DA9E0_H
#define POKEPLATINUM_STRUCT_OV19_021DA9E0_H

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
#include "platform/platform_types.h"

/* Common NNSG2d types */
typedef struct { void* data; } NNSG2dImageProxy;
typedef struct { void* data; } NNSG2dImagePaletteProxy;
typedef struct { void* data; } NNSG2dCharacterData;
typedef struct { void* data; } NNSG2dPaletteData;
typedef struct { s16 x, y; } NNSG2dSVec2;
typedef struct { s32 x, y; } NNSG2dLVec2;
typedef struct { fx32 x, y; } NNSG2dFVec2;
typedef struct { void* data; } NNSG2dImageAttr;

/* Common NNSG3d types */
typedef struct { u8 val[16]; char name[16]; } NNSG3dResName;
typedef struct { void* data; } NNSG3dResMdl;
typedef struct { void* data; } NNSG3dResTex;
typedef struct { void* data; } NNSG3dRenderObj;

/* GX color types */
typedef u16 GXRgb;
typedef struct { u8 r, g, b; } GXRgba;
#endif

#include "applications/pc_boxes/box_application.h"
#include "applications/pc_boxes/struct_ov19_021D61B0_decl.h"

#include "bg_window.h"
#include "font_special_chars.h"
#include "pokemon_sprite.h"
#include "sprite.h"
#include "strbuf.h"
#include "sys_task_manager.h"

typedef struct {
    BOOL unk_00;
    Window *unk_04;
    BgConfig *unk_08;
    SpriteList *unk_0C;
    const BoxApplication *unk_10;
    UnkStruct_ov19_021D61B0 *unk_14;
    FontSpecialCharsContext *unk_18;
    FontSpecialCharsContext *unk_1C;
    PokemonSpriteManager *unk_20;
    PokemonSprite *unk_24;
    void *unk_28;
    NNSG2dCellDataBank *unk_2C;
    Sprite *unk_30;
    void *unk_34;
    NNSG2dCellDataBank *unk_38;
    Sprite *unk_3C[2];
    Strbuf *unk_44;
    Strbuf *unk_48;
    SysTask *unk_4C;
    u8 padding_50[4];
} UnkStruct_ov19_021DA9E0;

#endif // POKEPLATINUM_STRUCT_OV19_021DA9E0_H
