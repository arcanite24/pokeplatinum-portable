#ifndef POKEPLATINUM_DEXMODE_CHECKER_H
#define POKEPLATINUM_DEXMODE_CHECKER_H

#include "platform/platform_types.h"

#include "struct_decls/pokedexdata_decl.h"

#include "savedata.h"

u32 SaveData_GetDexMode(SaveData *saveData);
u32 IsNationalDexObtained(const Pokedex *dex);
u32 GetDexNumber(u32 isNationalDexObtained, u32 monSpecies);

#endif // POKEPLATINUM_DEXMODE_CHECKER_H
