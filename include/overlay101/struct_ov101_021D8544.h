#ifndef POKEPLATINUM_STRUCT_OV101_021D8544_H
#define POKEPLATINUM_STRUCT_OV101_021D8544_H

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
#include "platform/platform_types.h"
#include "nns_types.h"  /* SDL: All NNSG2d/NNSG3d/GX types */
#endif

#include "sprite.h"

typedef struct {
    int unk_00;
    Sprite *unk_04;
    const NNSG2dImageProxy *unk_08;
} UnkStruct_ov101_021D8544;

#endif // POKEPLATINUM_STRUCT_OV101_021D8544_H
