#ifndef POKEPLATINUM_STRUCT_RADAR_CHAIN_RECORDS_H
#define POKEPLATINUM_STRUCT_RADAR_CHAIN_RECORDS_H

#define NUM_RADAR_RECORDS 3

#include "platform/platform_types.h"

typedef struct ChainRecord {
    u16 species;
    u16 chainCount;
} ChainRecord;

typedef struct RadarChainRecords {
    ChainRecord records[NUM_RADAR_RECORDS];
} RadarChainRecords;

#endif // POKEPLATINUM_STRUCT_RADAR_CHAIN_RECORDS_H
