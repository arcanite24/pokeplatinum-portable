#ifndef POKEPLATINUM_STRUCT_OV113_0226046C_H
#define POKEPLATINUM_STRUCT_OV113_0226046C_H

#include "platform/platform_types.h"

#ifdef PLATFORM_DS
#include <nitro/fx/fx.h>
#else
#include "platform/platform_types.h"  /* SDL: fx types */
#endif

typedef struct {
    VecFx32 unk_00;
    VecFx32 unk_0C;
    fx32 unk_18;
    u8 unk_1C;
    u8 unk_1D;
    u8 unk_1E[2];
} UnkStruct_ov113_0226046C;

#endif // POKEPLATINUM_STRUCT_OV113_0226046C_H
