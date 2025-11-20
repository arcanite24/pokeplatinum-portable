#include "applications/town_map/map_blocks.h"

#include "applications/town_map/defs.h"

#include "heap.h"

TownMapBlockList *TownMap_ReadBlocks(const char *path, enum HeapID heapID)
{
    FSFile tmapBlockFile;
    #ifdef PLATFORM_DS
    FS_InitFile(&tmapBlockFile);
    #else
    // TODO: Port FS_InitFile to PAL
    #endif

    #ifdef PLATFORM_DS
    if (!FS_OpenFile(&tmapBlockFile, path)) {
    #else
    // TODO: Port FS_OpenFile to PAL
    #endif
        GF_ASSERT(FALSE);
        return NULL;
    }

    int blockCount;
    #ifdef PLATFORM_DS
    int readLength = FS_ReadFile(&tmapBlockFile, &blockCount, sizeof(int));
    #else
    // TODO: Port FS_ReadFile to PAL
    #endif
    GF_ASSERT(readLength >= 0);

    TownMapBlockList *blockList = Heap_Alloc(heapID, sizeof(TownMapBlockList));
    memset(blockList, 0, sizeof(TownMapBlockList));

    blockList->entries = Heap_Alloc(heapID, sizeof(TownMapBlock) * blockCount);
    memset(blockList->entries, 0, sizeof(TownMapBlock) * blockCount);

    blockList->count = blockCount;

    for (int i = 0; i < blockList->count; i++) {
        TownMapBlock *block = &(blockList->entries[i]);
        #ifdef PLATFORM_DS
        readLength = FS_ReadFile(&tmapBlockFile, block, sizeof(TownMapBlock));
        #else
        // TODO: Port FS_ReadFile to PAL
        #endif
        block->index = i;
    }

    #ifdef PLATFORM_DS
    (void)FS_CloseFile(&tmapBlockFile);
    #else
    // TODO: Port FS_CloseFile to PAL
    #endif

    return blockList;
}

void TownMap_FreeBlocks(TownMapBlockList *blockList)
{
    Heap_Free(blockList->entries);
    Heap_Free(blockList);
}

TownMapBlock *TownMap_GetMapBlockAtPosition(TownMapBlockList *blockList, int x, int z, u16 unlockedHiddenLocations)
{
    for (int i = 0; i < blockList->count; i++) {
        TownMapBlock *block = &(blockList->entries[i]);

        if (block->x == x && block->z == z) {
            if (block->hiddenLocationFlags == 0 || block->hiddenLocationFlags & unlockedHiddenLocations) {
                return block;
            } else {
                return NULL;
            }
        }
    }

    return NULL;
}
