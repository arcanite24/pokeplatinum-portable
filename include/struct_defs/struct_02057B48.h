#ifndef POKEPLATINUM_STRUCT_02057B48_H
#define POKEPLATINUM_STRUCT_02057B48_H

#include "platform/platform_types.h"

#include "platform/platform_types.h"

typedef struct CommPlayerLocation {
    u16 x;
    u16 z;
    s8 dir;
    u8 moveSpeed;
    u8 unk_06;
    u8 collisionFlag;
} CommPlayerLocation;

#endif // POKEPLATINUM_STRUCT_02057B48_H
