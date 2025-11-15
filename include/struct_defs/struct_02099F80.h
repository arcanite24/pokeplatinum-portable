#ifndef POKEPLATINUM_STRUCT_02099F80_H
#define POKEPLATINUM_STRUCT_02099F80_H

#ifdef PLATFORM_DS
#include <nitro/gx.h>
#else
#include "platform/platform_types.h"
/* GX types from enums.h already define blend/window masks */
typedef u16 GXRgb;
typedef struct { u8 r, g, b, a; } GXRgba;
typedef struct { void* data; } GXOamAttr;
#endif

typedef struct {
    GXVRamBG unk_00;
    GXVRamBGExtPltt unk_04;
    GXVRamSubBG unk_08;
    GXVRamSubBGExtPltt unk_0C;
    GXVRamOBJ unk_10;
    GXVRamOBJExtPltt unk_14;
    GXVRamSubOBJ unk_18;
    GXVRamSubOBJExtPltt unk_1C;
    GXVRamTex unk_20;
    GXVRamTexPltt unk_24;
} UnkStruct_02099F80;

#endif // POKEPLATINUM_STRUCT_02099F80_H
