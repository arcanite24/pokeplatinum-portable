#ifndef POKEPLATINUM_MOVE_DISPLAY_INFO
#define POKEPLATINUM_MOVE_DISPLAY_INFO

#ifndef PLATFORM_DS
#include "platform/platform_types.h"

#include "platform/platform_types.h"
#endif

#include "constants/moves.h"

typedef struct MoveDisplayInfo {
    u16 move[LEARNED_MOVES_MAX];
    u16 curPP[LEARNED_MOVES_MAX];
    u16 maxPP[LEARNED_MOVES_MAX];
} MoveDisplayInfo;

#endif // POKEPLATINUM_MOVE_DISPLAY_INFO
