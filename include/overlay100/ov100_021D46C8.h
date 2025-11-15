#ifndef POKEPLATINUM_OV100_021D46C8_H
#define POKEPLATINUM_OV100_021D46C8_H

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

#include "struct_defs/struct_020985E4.h"

#include "overlay100/struct_ov100_021D46C8.h"
#include "overlay100/struct_ov100_021D4890.h"
#include "overlay100/struct_ov100_021D49B4.h"
#include "overlay100/struct_ov100_021D4DD8.h"

#include "narc.h"

int ov100_021D46C8(UnkStruct_ov100_021D46C8 *param0, UnkStruct_020985E4 *param1, int param2);
void ov100_021D4788(UnkStruct_ov100_021D46C8 *param0);
void ov100_021D47A0(UnkStruct_ov100_021D46C8 *param0);
void ov100_021D4844(UnkStruct_ov100_021D46C8 *param0);
void ov100_021D4890(UnkStruct_ov100_021D4890 *param0);
BOOL ov100_021D4920(UnkStruct_ov100_021D4890 *param0);
void ov100_021D49B4(UnkStruct_ov100_021D49B4 *param0);
void ov100_021D4A84(UnkStruct_ov100_021D49B4 *param0);
void ov100_021D4A90(int param0, UnkStruct_ov100_021D49B4 *param1, NNSFndAllocator *param2);
void ov100_021D4AA4(UnkStruct_ov100_021D49B4 *param0, NNSFndAllocator *param1, int param2);
void ov100_021D4AC8(UnkStruct_ov100_021D49B4 *param0, int param1, NARC *param2);
void ov100_021D4B10(UnkStruct_ov100_021D49B4 *param0, UnkStruct_ov100_021D49B4 *param1, int param2, NARC *param3);
void ov100_021D4B4C(int param0, UnkStruct_ov100_021D49B4 *param1, int param2, NARC *param3, NNSFndAllocator *param4);
void ov100_021D4BA0(int param0, UnkStruct_ov100_021D49B4 *param1, UnkStruct_ov100_021D49B4 *param2, int param3, NARC *param4, NNSFndAllocator *param5);
void ov100_021D4BF0(UnkStruct_ov100_021D46C8 *param0);
void ov100_021D4C94(UnkStruct_ov100_021D46C8 *param0, int param1);
void ov100_021D4DC8(int param0);
void ov100_021D4DD8(UnkStruct_ov100_021D4DD8 *param0, int param1);

#endif // POKEPLATINUM_OV100_021D46C8_H
