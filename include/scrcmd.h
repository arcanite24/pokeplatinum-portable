#ifndef POKEPLATINUM_SCRCMD_H
#define POKEPLATINUM_SCRCMD_H

#include "platform/platform_types.h"

#include "field_script_context.h"

BOOL sub_02041CC8(ScriptContext *ctx);
BOOL ScriptContext_WaitForApplicationExit(ScriptContext *ctx);

#endif // POKEPLATINUM_SCRCMD_H
