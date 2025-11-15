#ifndef POKEPLATINUM_OV19_021DB8E4_H
#define POKEPLATINUM_OV19_021DB8E4_H

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
#include "applications/pc_boxes/struct_ov19_021DBA9C.h"

#include "bg_window.h"
#include "narc.h"
#include "sprite.h"

BOOL ov19_021DB8E4(UnkStruct_ov19_021DBA9C *param0, UnkStruct_ov19_021D61B0 *param1, const BoxApplication *param2, BgConfig *param3, SpriteList *param4, NARC *param5);
void ov19_021DBA9C(UnkStruct_ov19_021DBA9C *param0);
void ov19_021DBAD0(UnkStruct_ov19_021DBA9C *param0);
void ov19_021DBB48(UnkStruct_ov19_021DBA9C *param0);
BOOL ov19_021DBB68(UnkStruct_ov19_021DBA9C *param0);
void ov19_021DBB70(UnkStruct_ov19_021DBA9C *param0);
void ov19_021DBB94(UnkStruct_ov19_021DBA9C *param0);
BOOL ov19_021DBBA0(UnkStruct_ov19_021DBA9C *param0);
void ov19_021DBBA8(UnkStruct_ov19_021DBA9C *param0, u32 param1, u32 param2, NNS_G2D_VRAM_TYPE param3, NNSG2dImageProxy *param4);
NNSG2dCellDataBank *ov19_021DBD3C(UnkStruct_ov19_021DBA9C *param0);
void ov19_021DC29C(UnkStruct_ov19_021DBA9C *param0);
BOOL ov19_021DC364(UnkStruct_ov19_021DBA9C *param0);
void ov19_021DC3F4(UnkStruct_ov19_021DBA9C *param0);
BOOL ov19_021DC43C(UnkStruct_ov19_021DBA9C *param0);

#endif // POKEPLATINUM_OV19_021DB8E4_H
