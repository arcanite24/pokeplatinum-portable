#include "overlay005/map_prop_material_shape.h"

#include <nitro.h>
#include <string.h>

#include "constants/heap.h"

#include "heap.h"

MapPropMaterialShape *MapPropMaterialShape_Alloc(void)
{
    MapPropMaterialShape *propMatShp = Heap_Alloc(HEAP_ID_FIELD1, sizeof(MapPropMaterialShape));
    GF_ASSERT(propMatShp != NULL);

    return propMatShp;
}

void MapPropMaterialShape_Load(const char *path, MapPropMaterialShape *propMatShp)
{
    FSFile file;
    #ifdef PLATFORM_DS
    FS_InitFile(&file);
    #else
    // TODO: Port FS_InitFile to PAL
    #endif

    #ifdef PLATFORM_DS
    if (FS_OpenFile(&file, path)) {
    #else
    // TODO: Port FS_OpenFile to PAL
    #endif
        u16 idsLocatorsCount;
        #ifdef PLATFORM_DS
        int readLength = FS_ReadFile(&file, &idsLocatorsCount, 2);
        #else
        // TODO: Port FS_ReadFile to PAL
        #endif
        GF_ASSERT(readLength >= 0);

        u16 idsCount;
        #ifdef PLATFORM_DS
        readLength = FS_ReadFile(&file, &idsCount, 2);
        #else
        // TODO: Port FS_ReadFile to PAL
        #endif
        GF_ASSERT(readLength >= 0);

        int idsLocatorsSize = sizeof(MapPropMaterialShapeIDsLocator) * idsLocatorsCount;
        int idsSize = sizeof(MapPropMaterialShapeIDs) * idsCount;

        propMatShp->idsLocators = Heap_Alloc(HEAP_ID_FIELD1, idsLocatorsSize);
        GF_ASSERT(propMatShp->idsLocators != NULL);

        propMatShp->ids = Heap_Alloc(HEAP_ID_FIELD1, idsSize);
        GF_ASSERT(propMatShp->ids != NULL);

        #ifdef PLATFORM_DS
        readLength = FS_ReadFile(&file, propMatShp->idsLocators, idsLocatorsSize);
        #else
        // TODO: Port FS_ReadFile to PAL
        #endif
        GF_ASSERT(readLength >= 0);

        #ifdef PLATFORM_DS
        readLength = FS_ReadFile(&file, propMatShp->ids, idsSize);
        #else
        // TODO: Port FS_ReadFile to PAL
        #endif
        GF_ASSERT(readLength >= 0);

        #ifdef PLATFORM_DS
        FS_CloseFile(&file);
        #else
        // TODO: Port FS_CloseFile to PAL
        #endif
    } else {
        GF_ASSERT(FALSE);
    }
}

void MapPropMaterialShape_Free(MapPropMaterialShape *propMatShp)
{
    Heap_Free(propMatShp->ids);
    Heap_Free(propMatShp->idsLocators);
    Heap_Free(propMatShp);
}

void MapProp_GetMaterialShapeIDsCount(const int modelID, const MapPropMaterialShape *propMatShp, u16 *idsCount)
{
    *idsCount = propMatShp->idsLocators[modelID].idsCount;
}

void MapProp_GetMaterialShapeIDsLocator(const int modelID, const MapPropMaterialShape *propMatShp, u16 *idsCount, u16 *idsIndex)
{
    *idsCount = propMatShp->idsLocators[modelID].idsCount;
    *idsIndex = propMatShp->idsLocators[modelID].idsIndex;
}

MapPropMaterialShapeIDs const *MapPropMaterialShape_GetMaterialShapeIDsAt(const u16 index, const MapPropMaterialShape *propMatShp)
{
    return &propMatShp->ids[index];
}
