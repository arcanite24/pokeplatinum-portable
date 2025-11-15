#ifndef POKEPLATINUM_STRUCT_OV109_021D5140_SUB1_H
#define POKEPLATINUM_STRUCT_OV109_021D5140_SUB1_H

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

typedef struct {
    int unk_00;
    int unk_04;
    int unk_08;
    void *unk_0C;
    NNSG2dPaletteData *unk_10;
} UnkStruct_ov109_021D5140_sub1;

#endif // POKEPLATINUM_STRUCT_OV109_021D5140_SUB1_H
