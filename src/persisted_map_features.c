#include "persisted_map_features.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

void PersistedMapFeatures_Init(PersistedMapFeatures *persistedMapFeatures)
{
    MI_CpuClear8(persistedMapFeatures, sizeof(PersistedMapFeatures));
}

void *PersistedMapFeatures_InitWithID(PersistedMapFeatures *persistedMapFeatures, int id)
{
    PersistedMapFeatures_Init(persistedMapFeatures);
    persistedMapFeatures->id = id;

    return persistedMapFeatures->buffer;
}

void *PersistedMapFeatures_GetBuffer(PersistedMapFeatures *persistedMapFeatures, int id)
{
    GF_ASSERT(persistedMapFeatures->id == id);
    return persistedMapFeatures->buffer;
}

int PersistedMapFeatures_GetID(const PersistedMapFeatures *persistedMapFeatures)
{
    return persistedMapFeatures->id;
}
