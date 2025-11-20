#ifndef POKEPLATINUM_INT_DISTANCE_H
#define POKEPLATINUM_INT_DISTANCE_H

#ifdef PLATFORM_DS
#include <nitro/types.h>
#else
#include "platform/platform_types.h"
#endif

u32 CalcDistance2D(s32 x0, s32 x1, s32 y0, s32 y1);

#endif // POKEPLATINUM_INT_DISTANCE_H
