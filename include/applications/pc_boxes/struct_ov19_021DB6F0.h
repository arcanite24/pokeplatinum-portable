#ifndef POKEPLATINUM_STRUCT_OV19_021DB6F0_H
#define POKEPLATINUM_STRUCT_OV19_021DB6F0_H

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
#include "message.h"
#include "strbuf.h"
#include "string_template.h"

typedef struct {
    BgConfig *unk_00;
    u8 padding_04[4];
    const BoxApplication *unk_08;
    UnkStruct_ov19_021D61B0 *unk_0C;
    MessageLoader *unk_10;
    MessageLoader *unk_14;
    Window *unk_18;
    u16 unk_1C;
    u16 unk_1E;
    int unk_20;
    void *unk_24;
    NNSG2dCharacterData *unk_28;
    const StringTemplate *unk_2C;
    Strbuf *unk_30;
    Strbuf *unk_34;
} UnkStruct_ov19_021DB6F0;

#endif // POKEPLATINUM_STRUCT_OV19_021DB6F0_H
