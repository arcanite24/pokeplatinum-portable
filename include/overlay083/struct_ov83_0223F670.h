#ifndef POKEPLATINUM_STRUCT_OV83_0223F670_H
#define POKEPLATINUM_STRUCT_OV83_0223F670_H

#include "platform/platform_types.h"

#ifdef PLATFORM_DS
#include <nitro/fx/fx.h>
#else
#include "platform/platform_types.h"  /* SDL: fx types */
#endif

#include "overlay083/struct_ov83_0223DA94.h"

#include "sprite.h"

typedef struct {
    BOOL unk_00;
    BOOL unk_04;
    VecFx32 unk_08;
    UnkStruct_ov83_0223DA94 unk_14;
    UnkStruct_ov83_0223DA94 unk_2C;
    Sprite *unk_44;
} UnkStruct_ov83_0223F670;

#endif // POKEPLATINUM_STRUCT_OV83_0223F670_H
