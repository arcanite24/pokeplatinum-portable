#ifndef POKEPLATINUM_GBA_CONVERT_STRING_H
#define POKEPLATINUM_GBA_CONVERT_STRING_H

#include "platform/platform_types.h"
#include "global/pm_version.h"

BOOL GBA_ConvertStringToDS(const u8 *src, u16 *dst, u32 length, u32 language);

#endif // POKEPLATINUM_GBA_CONVERT_STRING_H
