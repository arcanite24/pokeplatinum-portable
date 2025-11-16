#ifndef POKEPLATINUM_POKEMON_SPRITE_DATA_H
#define POKEPLATINUM_POKEMON_SPRITE_DATA_H

#ifndef PLATFORM_DS
#include "platform/platform_types.h"
#endif

typedef struct PokemonSpriteData {
    u8 *tiles;
    int narcID;
    int palette;
    int yOffset;
} PokemonSpriteData;

#endif // POKEPLATINUM_POKEMON_SPRITE_DATA_H
