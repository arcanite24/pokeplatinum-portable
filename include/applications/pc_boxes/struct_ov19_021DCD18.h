#ifndef POKEPLATINUM_STRUCT_OV19_021DCD18_H
#define POKEPLATINUM_STRUCT_OV19_021DCD18_H

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

#include "sprite.h"

typedef struct {
    Sprite *unk_00;
    NNSG2dImageProxy unk_04;
    u32 unk_28;
    u16 unk_2C;
    u16 unk_2E;
    u16 unk_30;
    u8 padding_32[2];
} UnkStruct_ov19_021DCD18;

#endif // POKEPLATINUM_STRUCT_OV19_021DCD18_H
