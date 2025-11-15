#ifndef POKEPLATINUM_BATTLE_MESSAGE_H
#define POKEPLATINUM_BATTLE_MESSAGE_H

#ifdef PLATFORM_DS
#include <nitro/types.h>
#else
#include "platform/platform_types.h"
#endif

typedef struct BattleMessage {
    u8 commandCode;
    u8 tags;
    u16 id;
    int params[6];
    int digits;
    int battler;
} BattleMessage;

#endif // POKEPLATINUM_BATTLE_MESSAGE_H
