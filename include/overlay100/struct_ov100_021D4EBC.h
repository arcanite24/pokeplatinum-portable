#ifndef POKEPLATINUM_STRUCT_OV100_021D4EBC_H
#define POKEPLATINUM_STRUCT_OV100_021D4EBC_H

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
    GXDispMode unk_00;
    GXBGMode unk_04;
    GXBG0As unk_08;
    GXCaptureSize unk_0C;
    GXCaptureMode unk_10;
    GXCaptureSrcA unk_14;
    GXCaptureSrcB unk_18;
    GXCaptureDest unk_1C;
    int unk_20;
    int unk_24;
    int unk_28;
} UnkStruct_ov100_021D4EBC;

#endif // POKEPLATINUM_STRUCT_OV100_021D4EBC_H
