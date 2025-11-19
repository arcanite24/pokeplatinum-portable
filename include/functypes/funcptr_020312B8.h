#ifndef POKEPLATINUM_FUNCPTR_020312B8_H
#define POKEPLATINUM_FUNCPTR_020312B8_H

#ifdef PLATFORM_DS
#include <nitro/wm.h>
typedef void (*UnkFuncPtr_020312B8)(WMBssDesc *);
#else
typedef void (*UnkFuncPtr_020312B8)(void *);  // SDL: WMBssDesc is DS wireless specific
#endif

#endif // POKEPLATINUM_FUNCPTR_020312B8_H
