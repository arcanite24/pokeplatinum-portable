#ifndef POKEPLATINUM_STRUCT_020147B8_H
#define POKEPLATINUM_STRUCT_020147B8_H

#ifdef PLATFORM_DS
#include <nitro/fx/fx.h>
#else
#include "platform/platform_types.h"  /* SDL: fx types */
#endif

struct SPLParticle;
struct SPLEmitter;

typedef struct SPLBehavior {
    void (*applyFunc)(const void *, struct SPLParticle *, VecFx32 *, struct SPLEmitter *);
    const void *object;
} SPLBehavior;

#endif // POKEPLATINUM_STRUCT_020147B8_H
