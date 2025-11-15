#ifndef POKEPLATINUM_STRUCT_OV19_021DC680_H
#define POKEPLATINUM_STRUCT_OV19_021DC680_H

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
#include "applications/pc_boxes/struct_ov19_021D8318.h"
#include "applications/pc_boxes/struct_ov19_021DA384.h"
#include "applications/pc_boxes/struct_ov19_021DCD18.h"

#include "bg_window.h"
#include "sprite.h"

typedef struct {
    UnkStruct_ov19_021D61B0 *unk_00;
    BgConfig *unk_04;
    SpriteList *unk_08;
    const BoxApplication *unk_0C;
    UnkStruct_ov19_021D8318 *unk_10;
    void *unk_14;
    NNSG2dScreenData *unk_18;
    void *unk_1C;
    fx32 unk_20;
    fx32 unk_24;
    u8 unk_28;
    u8 unk_29;
    u8 unk_2A;
    u8 unk_2B;
    BOOL unk_2C;
    u32 unk_30;
    int unk_34;
    UnkStruct_ov19_021DA384 *unk_38;
    UnkStruct_ov19_021DCD18 unk_3C[6];
    u8 unk_174;
    u8 padding_175[3];
} UnkStruct_ov19_021DC680;

#endif // POKEPLATINUM_STRUCT_OV19_021DC680_H
