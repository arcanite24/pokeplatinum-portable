#ifndef POKEPLATINUM_UNK_0203909C_H
#define POKEPLATINUM_UNK_0203909C_H

#include "platform/platform_types.h"

#ifdef PLATFORM_DS
#include <dwc.h>
#endif

#include "savedata.h"

#ifdef PLATFORM_DS
int sub_0203909C(SaveData *saveData, DWCFriendData *param1, int *param2);
#else
int sub_0203909C(SaveData *saveData, void *param1, int *param2);  // SDL: DWCFriendData is DS wireless specific
#endif
int sub_02039140(SaveData *saveData, u64 param1, int *param2);
BOOL sub_020391DC(SaveData *saveData, int *param1, int heapID);
void sub_02039298(SaveData *saveData, int param1, int param2, int heapID, int param4);
int sub_02039390(SaveData *saveData, int param1);

#endif // POKEPLATINUM_UNK_0203909C_H
