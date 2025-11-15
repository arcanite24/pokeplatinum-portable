#ifndef POKEPLATINUM_UNK_0202419C_H
#define POKEPLATINUM_UNK_0202419C_H

#ifdef PLATFORM_DS
#include <nitro/gx.h>
#else
#include "nns_types.h"
// Additional GX types not in nns_types.h
typedef enum {
    GX_SORTMODE_AUTO,
    GX_SORTMODE_MANUAL
} GXSortMode;
#endif

void sub_0202419C(void);
void G3_ResetG3X(void);
void G3_RequestSwapBuffers(GXSortMode param0, GXBufferMode param1);
void sub_020241CC(void);

#endif // POKEPLATINUM_UNK_0202419C_H
