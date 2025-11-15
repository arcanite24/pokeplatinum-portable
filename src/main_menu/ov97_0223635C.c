#include "main_menu/ov97_0223635C.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "main_menu/ov97_02235D18.h"
#include "main_menu/struct_ov97_0223635C.h"

u32 GetGBAPlayerTrainerId(void)
{
    GBASaveBlock2 *saveBlock2 = GetGBASaveBlock2();
    return saveBlock2->playerTrainerId[3] << 24 | saveBlock2->playerTrainerId[2] << 16 | saveBlock2->playerTrainerId[1] << 8 | saveBlock2->playerTrainerId[0];
}

u8 *GetGBAPlayerName(void)
{
    GBASaveBlock2 *saveBlock2 = GetGBASaveBlock2();
    return saveBlock2->playerName;
}
