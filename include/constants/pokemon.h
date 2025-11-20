#ifndef POKEPLATINUM_CONSTANTS_POKEMON_H
#define POKEPLATINUM_CONSTANTS_POKEMON_H

#include "generated/pokemon_data_params.h" // PokemonDataParam
#include "generated/pokemon_stats.h" // PokemonStat
#include "generated/pokemon_types.h" // PokemonType
#include "generated/species_data_params.h" // SpeciesDataParam

#define MAX_PARTY_SIZE      6
#define NUM_BOOSTABLE_STATS 8

#define MAX_IVS_SINGLE_STAT 31
#define MAX_EVS_SINGLE_STAT 255
#define MAX_EVS_ALL_STATS   510

#define MAX_CONTEST_STAT 255

#define MAX_POKEMON_SHEEN    255
#define MAX_POKEMON_MARKINGS 6
#define MAX_POKEMON_LEVEL    100
#define EGG_POKEMON_LEVEL    1

#define EVOLVE_FRIENDSHIP_THRESHOLD 220

#define BASE_FRIENDSHIP_VALUE 70
#define MAX_FRIENDSHIP_VALUE  255
#define LOW_FRIENDSHIP_LIMIT  100
#define MED_FRIENDSHIP_LIMIT  200
#define HIGH_FRIENDSHIP_LIMIT MAX_FRIENDSHIP_VALUE

#define CATCHING_SHOW_MONS       6
#define MAX_GBA_TRANSFER_HISTORY 20

#define MAX_PP_UP_BONUSES 3

#define LEVEL_UP_MOVESET_TERMINATOR 0xffff

// MON_DATA_MOVE*_PP aliases for MON_DATA_MOVE*_CUR_PP (current PP values)
// These are commonly used throughout the codebase for getting/setting move PP
#define MON_DATA_MOVE1_PP MON_DATA_MOVE1_CUR_PP
#define MON_DATA_MOVE2_PP MON_DATA_MOVE2_CUR_PP
#define MON_DATA_MOVE3_PP MON_DATA_MOVE3_CUR_PP
#define MON_DATA_MOVE4_PP MON_DATA_MOVE4_CUR_PP

// Compatibility aliases for GBA constants
#define MON_DATA_MARKINGS MON_DATA_MARKS
#define MON_DATA_COOL_RIBBON     71  // Should match generated constants
#define MON_DATA_BEAUTY_RIBBON   72
#define MON_DATA_CUTE_RIBBON     73
#define MON_DATA_SMART_RIBBON    74
#define MON_DATA_TOUGH_RIBBON    75
#define MON_DATA_CHAMPION_RIBBON 76
#define MON_DATA_WINNING_RIBBON  77
#define MON_DATA_VICTORY_RIBBON  78
#define MON_DATA_ARTIST_RIBBON   79
#define MON_DATA_EFFORT_RIBBON   80
#define MON_DATA_MARINE_RIBBON   81
#define MON_DATA_LAND_RIBBON     82
#define MON_DATA_SKY_RIBBON      83
#define MON_DATA_COUNTRY_RIBBON  84
#define MON_DATA_NATIONAL_RIBBON 85
#define MON_DATA_EARTH_RIBBON    86
#define MON_DATA_WORLD_RIBBON    87

#endif // POKEPLATINUM_CONSTANTS_POKEMON_H
