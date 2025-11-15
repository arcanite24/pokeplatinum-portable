#ifndef POKEPLATINUM_OV5_021D5CB0_H
#define POKEPLATINUM_OV5_021D5CB0_H

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

#include "overlay005/struct_ov5_021D5CB0_decl.h"

UnkStruct_ov5_021D5CB0 *ov5_021D5CB0(void);
int ov5_021D5CE4(UnkStruct_ov5_021D5CB0 *param0, NNSG3dResTex *param1);
void ov5_021D5DEC(UnkStruct_ov5_021D5CB0 *param0);
void ov5_021D5E5C(UnkStruct_ov5_021D5CB0 *param0, int param1);
void ov5_021D5E8C(UnkStruct_ov5_021D5CB0 *param0);
void ov5_021D5EAC(UnkStruct_ov5_021D5CB0 *param0);

#endif // POKEPLATINUM_OV5_021D5CB0_H
