#ifndef POKEPLATINUM_STRUCT_0203E6C0_H
#define POKEPLATINUM_STRUCT_0203E6C0_H

#include "platform/platform_types.h"

#include "field/field_system_decl.h"

#include "savedata.h"

typedef struct {
    FieldSystem *fieldSystem;
    SaveData *saveData;
    int unk_08;
    u8 unk_0C;
    u8 padding_0D[3];
} UnkStruct_0203E6C0;

#endif // POKEPLATINUM_STRUCT_0203E6C0_H
