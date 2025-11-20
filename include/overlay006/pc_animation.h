#ifndef POKEPLATINUM_PC_ANIMATION_H
#define POKEPLATINUM_PC_ANIMATION_H

#include "platform/platform_types.h"

#include "field/field_system_decl.h"

void FieldSystem_LoadPCAnimation(FieldSystem *fieldSystem, const u8 tag);
void FieldSystem_PlayPCBootUpAnimation(FieldSystem *fieldSystem, const u8 tag);
void FieldSystem_PlayPCShutDownAnimation(FieldSystem *fieldSystem, const u8 tag);

#endif // POKEPLATINUM_PC_ANIMATION_H
