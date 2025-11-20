#ifndef POKEPLATINUM_DYNAMIC_MAP_FEATURES_H
#define POKEPLATINUM_DYNAMIC_MAP_FEATURES_H

#include "platform/platform_types.h"

#ifdef PLATFORM_DS
#include <nitro/fx/fx.h>
#else
#include "platform/platform_types.h"  /* SDL: fx types */
#endif
#include <nitro/types.h>

#include "field/field_system_decl.h"

typedef void (*DynamicMapFeaturesInitFunc)(FieldSystem *);
typedef void (*DynamicMapFeaturesFreeFunc)(FieldSystem *);
typedef BOOL (*DynamicMapFeaturesCheckCollisionFunc)(FieldSystem *, const int, const int, const fx32, BOOL *);

void DynamicMapFeatures_Init(FieldSystem *fieldSystem);
void DynamicMapFeatures_Free(FieldSystem *fieldSystem);
BOOL DynamicMapFeatures_CheckCollision(FieldSystem *fieldSystem, const int tileX, const int tileZ, const fx32 height, BOOL *isColliding);
BOOL DynamicMapFeatures_WillPlayerJumpEternaGymClock(FieldSystem *fieldSystem, const int tileX, const int tileZ, const fx32 unused3, int direction);

#endif // POKEPLATINUM_DYNAMIC_MAP_FEATURES_H
