#ifndef POKEPLATINUM_STRUCT_OV109_021D5140_SUB1_H
#define POKEPLATINUM_STRUCT_OV109_021D5140_SUB1_H

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
#include "platform/platform_types.h"
#include "nns_types.h"  /* SDL: All NNSG2d/NNSG3d/GX types */
#endif

typedef struct {
    int unk_00;
    int unk_04;
    int unk_08;
    void *unk_0C;
    NNSG2dPaletteData *unk_10;
} UnkStruct_ov109_021D5140_sub1;

#endif // POKEPLATINUM_STRUCT_OV109_021D5140_SUB1_H
