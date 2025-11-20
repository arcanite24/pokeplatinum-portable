#ifndef POKEPLATINUM_STRUCT_OV113_0225FEE0_H
#define POKEPLATINUM_STRUCT_OV113_0225FEE0_H

#include "platform/platform_types.h"

#ifdef PLATFORM_DS
#include <nitro/fx/fx.h>
#else
#include "platform/platform_types.h"  /* SDL: fx types */
#endif

typedef struct {
    VecFx32 unk_00;
    u8 unk_0C;
    u8 unk_0D;
    u8 unk_0E;
    u8 unk_0F;
} UnkStruct_ov113_0225FEE0;

#endif // POKEPLATINUM_STRUCT_OV113_0225FEE0_H
