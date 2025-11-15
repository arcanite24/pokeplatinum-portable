#ifndef POKEPLATINUM_CHARCODE_H
#define POKEPLATINUM_CHARCODE_H

#include "platform/platform_types.h"

typedef u16 charcode_t;

charcode_t *CharCode_SkipFormatArg(const charcode_t *cstr);
u32 CharCode_FormatArgType(const charcode_t *cstr);
BOOL CharCode_IsFormatArg(const charcode_t *cstr);
u32 CharCode_FormatArgParam(const charcode_t *cstr, u32 paramIdx);

#endif // POKEPLATINUM_CHARCODE_H
