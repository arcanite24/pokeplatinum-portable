#ifndef POKEPLATINUM_UNK_0201DD00_H
#define POKEPLATINUM_UNK_0201DD00_H

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

#include "struct_decls/struct_0201DD00_decl.h"
#include "struct_decls/struct_0201DDF4_decl.h"
#include "struct_defs/struct_02024184.h"

UnkStruct_0201DD00 *sub_0201DD00(int param0, int heapID);
void sub_0201DD3C(UnkStruct_0201DD00 *param0);
UnkStruct_0201DDF4 *sub_0201DD54(UnkStruct_0201DD00 *param0, const UnkStruct_02024184 *param1, const NNSG3dResTex *param2, NNSGfdTexKey param3, NNSGfdPlttKey param4, const fx32 param5);
void sub_0201DDAC(UnkStruct_0201DDF4 *param0);
void sub_0201DDB4(UnkStruct_0201DD00 *param0);
void sub_0201DDD8(UnkStruct_0201DDF4 *param0, const fx32 param1);
void *sub_0201DEAC(NNSG3dResTex *param0, const u8 param1);
void *sub_0201DEFC(NNSG3dResTex *param0, const u8 param1);
void *sub_0201DF50(NNSG3dResTex *param0, const char *param1);
int sub_0201DFE4(NNSG3dResTex *param0, const char *param1);

#endif // POKEPLATINUM_UNK_0201DD00_H
