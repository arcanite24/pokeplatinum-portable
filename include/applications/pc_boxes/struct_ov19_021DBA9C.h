#ifndef POKEPLATINUM_STRUCT_OV19_021DBA9C_H
#define POKEPLATINUM_STRUCT_OV19_021DBA9C_H

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
#include "message.h"
#include "sprite.h"
#include "strbuf.h"
#include "sys_task_manager.h"

typedef struct {
    UnkStruct_ov19_021D61B0 *unk_00;
    BgConfig *unk_04;
    SpriteList *unk_08;
    const BoxApplication *unk_0C;
    Window *unk_10;
    void *unk_14;
    NNSG2dCharacterData *unk_18;
    void *unk_1C;
    NNSG2dCellDataBank *unk_20;
    u8 padding_24[8];
    void *unk_2C;
    NNSG2dScreenData *unk_30;
    void *unk_34;
    NNSG2dScreenData *unk_38;
    Bitmap unk_3C;
    FontSpecialCharsContext *unk_44;
    Sprite *unk_48[18];
    Sprite *unk_90;
    u32 unk_94[18];
    SysTask *unk_DC;
    s32 unk_E0;
    u16 unk_E4[8][48];
    u8 unk_3E4[18][1024];
    u8 unk_4BE4[18];
    u8 padding_4BF6[2];
    MessageLoader *unk_4BF8;
    Strbuf *unk_4BFC;
} UnkStruct_ov19_021DBA9C;

#endif // POKEPLATINUM_STRUCT_OV19_021DBA9C_H
