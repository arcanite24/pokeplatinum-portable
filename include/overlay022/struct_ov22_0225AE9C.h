#ifndef POKEPLATINUM_STRUCT_OV22_0225AE9C_H
#define POKEPLATINUM_STRUCT_OV22_0225AE9C_H

#ifdef PLATFORM_DS
#include <nitro/fx/fx.h>
#else
#include "platform/platform_types.h"  /* SDL: fx types */
#endif

#include "overlay022/struct_ov22_0225AD68.h"

#include "sprite.h"

typedef struct {
    BOOL unk_00;
    Sprite *unk_04[2];
    VecFx32 unk_0C[2];
    UnkStruct_ov22_0225AD68 unk_24;
    UnkStruct_ov22_0225AD68 unk_38;
    UnkStruct_ov22_0225AD68 unk_4C;
} UnkStruct_ov22_0225AE9C;

#endif // POKEPLATINUM_STRUCT_OV22_0225AE9C_H
