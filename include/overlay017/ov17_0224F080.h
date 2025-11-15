#ifndef POKEPLATINUM_OV17_0224F080_H
#define POKEPLATINUM_OV17_0224F080_H

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

#include "constants/narc.h"

#include "sprite_system.h"

u32 ov17_0224F080(int param0);
u32 ov17_0224F098(void);
u32 ov17_0224F09C(void);
u8 ov17_0224F0A0(int param0);
enum NarcID ov17_0224F0B8(void);
void ov17_0224F0BC(SpriteSystem *param0, SpriteManager *param1, NNS_G2D_VRAM_TYPE param2, int param3, u32 param4);
void ov17_0224F0F0(SpriteSystem *param0, SpriteManager *param1, u32 param2, u32 param3);
void ov17_0224F138(SpriteManager *param0, u32 param1);
void ov17_0224F140(SpriteManager *param0, u32 param1, u32 param2);
ManagedSprite *ov17_0224F154(SpriteSystem *param0, SpriteManager *param1, int param2, const SpriteTemplate *param3);
void ov17_0224F184(ManagedSprite *param0);

#endif // POKEPLATINUM_OV17_0224F080_H
