#ifndef POKEPLATINUM_SPRITE_RESOURCE_H
#define POKEPLATINUM_SPRITE_RESOURCE_H

#include "platform/platform_types.h"

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
#include "nns_types.h"  /* SDL: Common NNS type stubs */
#endif

#include "constants/heap.h"

#include "narc.h"
#include "resource_collection.h"

enum SpriteResourceType {
    SPRITE_RESOURCE_CHAR = 0,
    SPRITE_RESOURCE_PLTT,
    SPRITE_RESOURCE_CELL,
    SPRITE_RESOURCE_ANIM,
    SPRITE_RESOURCE_MULTI_CELL,
    SPRITE_RESOURCE_MULTI_ANIM,
    SPRITE_RESOURCE_MAX,
};

#define MAX_SPRITE_RESOURCE_GEN4 (SPRITE_RESOURCE_ANIM + 1) // MULTI_CELL and MULTI_ANIM are not used in the default rom

typedef struct SpriteResource {
    Resource *rawResource;
    enum SpriteResourceType type;
    void *data;
} SpriteResource;

typedef struct SpriteResourceCollection {
    ResourceCollection *collection;
    SpriteResource *resources;
    int capacity;
    int count;
    enum SpriteResourceType type;
} SpriteResourceCollection;

typedef struct SpriteResourceTableEntryFile {
    int id;
    char filename[64];
    #ifdef PLATFORM_DS
    NNS_G2D_VRAM_TYPE vramType;
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE to PAL
    #endif
    int paletteIndex;
} SpriteResourceTableEntryFile;

typedef struct SpriteResourceTableEntryNARC {
    enum NarcID narcID;
    int memberIndex;
    BOOL compressed;
    int id;
    #ifdef PLATFORM_DS
    NNS_G2D_VRAM_TYPE vramType;
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE to PAL
    #endif
    int paletteIndex;
} SpriteResourceTableEntryNARC;

typedef struct SpriteResourceTable {
    void *entries;
    int count;
    enum SpriteResourceType type;
    u8 loadFromNARC; // Whether the entries are SpriteResourceTableEntryNARC or SpriteResourceTableEntryFile
} SpriteResourceTable;

// Fixed size, non-owning list of SpriteResource pointers
typedef struct SpriteResourceList {
    SpriteResource **resources;
    int capacity;
    int count;
} SpriteResourceList;

SpriteResourceCollection *SpriteResourceCollection_New(int capacity, enum SpriteResourceType type, enum HeapID heapID);
void SpriteResourceCollection_Delete(SpriteResourceCollection *spriteResources);
SpriteResource *SpriteResourceCollection_AddFromTable(SpriteResourceCollection *spriteResources, const SpriteResourceTable *table, int index, enum HeapID heapID);
#ifdef PLATFORM_DS
SpriteResource *SpriteResourceCollection_AddTiles(SpriteResourceCollection *spriteResources, enum NarcID narcID, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, enum HeapID heapID);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
#ifdef PLATFORM_DS
SpriteResource *SpriteResourceCollection_AddPalette(SpriteResourceCollection *spriteResources, enum NarcID narcID, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, enum HeapID heapID);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
SpriteResource *SpriteResourceCollection_Add(SpriteResourceCollection *spriteResources, enum NarcID narcID, int memberIdx, BOOL compressed, int id, enum SpriteResourceType type, enum HeapID heapID);
void SpriteResourceCollection_ModifyTiles(SpriteResourceCollection *spriteResources, SpriteResource *spriteRes, enum NarcID narcID, int memberIdx, BOOL compressed, enum HeapID heapID);
void SpriteResourceCollection_ModifyPalette(SpriteResourceCollection *spriteResources, SpriteResource *spriteRes, enum NarcID narcID, int memberIdx, BOOL compressed, enum HeapID heapID);
#ifdef PLATFORM_DS
SpriteResource *SpriteResourceCollection_AddTilesFrom(SpriteResourceCollection *spriteResources, NARC *narc, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, enum HeapID heapID);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
#ifdef PLATFORM_DS
SpriteResource *SpriteResourceCollection_AddTilesFromEx(SpriteResourceCollection *spriteResources, NARC *narc, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, enum HeapID heapID, BOOL allocAtEnd);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
#ifdef PLATFORM_DS
SpriteResource *SpriteResourceCollection_AddPaletteFrom(SpriteResourceCollection *spriteResources, NARC *narc, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, enum HeapID heapID);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
#ifdef PLATFORM_DS
SpriteResource *SpriteResourceCollection_AddPaletteFromEx(SpriteResourceCollection *spriteResources, NARC *narc, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, enum HeapID heapID, BOOL allocAtEnd);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
SpriteResource *SpriteResourceCollection_AddFrom(SpriteResourceCollection *spriteResources, NARC *narc, int memberIdx, BOOL compressed, int id, enum SpriteResourceType type, enum HeapID heapID);
void SpriteResourceCollection_ModifyTilesFrom(SpriteResourceCollection *spriteResources, SpriteResource *spriteRes, NARC *narc, int memberIdx, BOOL compressed, enum HeapID heapID);
int SpriteResourceCollection_Extend(SpriteResourceCollection *spriteResources, const SpriteResourceTable *table, SpriteResourceList *outList, enum HeapID heapID);
void SpriteResoureCollection_ExtendEx(SpriteResourceCollection *spriteResources, const SpriteResourceTable *table, int first, int count, SpriteResourceList *outList, enum HeapID heapID);
SpriteResourceList *SpriteResourceList_New(int capacity, enum HeapID heapID);
void SpriteResourceList_Delete(SpriteResourceList *list);
BOOL SpriteResourceCollection_IsIDUnused(const SpriteResourceCollection *spriteResources, int id);
void SpriteResource_ReleaseData(SpriteResource *spriteRes);
void SpriteResourceCollection_Remove(SpriteResourceCollection *spriteResources, SpriteResource *resource);
void SpriteResourceCollection_Clear(SpriteResourceCollection *spriteResources);
SpriteResource *SpriteResourceCollection_Find(const SpriteResourceCollection *spriteResources, int id);
int SpriteResource_GetID(const SpriteResource *spriteRes);
NNSG2dCharacterData *SpriteResource_GetTileData(const SpriteResource *spriteRes);
NNSG2dPaletteData *SpriteResource_GetPaletteFade(const SpriteResource *spriteRes);
NNSG2dCellDataBank *SpriteResource_GetSpriteData(const SpriteResource *spriteRes);
NNSG2dCellAnimBankData *SpriteResource_GetSpriteAnimData(const SpriteResource *spriteRes);
NNSG2dMultiCellDataBank *SpriteResource_GetMultiSpriteData(const SpriteResource *spriteRes);
NNSG2dMultiCellAnimBankData *SpriteResource_GetMultiSpriteAnimData(const SpriteResource *spriteRes);
enum SpriteResourceType SpriteResource_GetType(const SpriteResource *spriteRes);
#ifdef PLATFORM_DS
NNS_G2D_VRAM_TYPE SpriteResource_GetVRAMType(const SpriteResource *spriteRes);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
int SpriteResource_GetPaletteIndex(const SpriteResource *spriteRes);
#ifdef PLATFORM_DS
void SpriteResource_SetVRAMType(SpriteResource *spriteRes, NNS_G2D_VRAM_TYPE vramType);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
int SpriteResourceTable_Size(void);
SpriteResourceTable *SpriteResourceTable_GetArrayElement(SpriteResourceTable *table, int index);
void SpriteResourceTable_LoadFromBinary(const void *data, SpriteResourceTable *table, enum HeapID heapID);
void SpriteResourceTable_Clear(SpriteResourceTable *table);
int SpriteResourceTable_GetCount(const SpriteResourceTable *table);
int SpriteResourceTable_GetEntryID(const SpriteResourceTable *table, int index);
int SpriteResourceTable_GetNARCEntryMemberIndex(const SpriteResourceTable *table, int index);
BOOL SpriteResourceTable_IsNARCEntryCompressed(const SpriteResourceTable *table, int index);
#ifdef PLATFORM_DS
NNS_G2D_VRAM_TYPE SpriteResourceTable_GetEntryVRAMType(const SpriteResourceTable *table, int index);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
int SpriteResourceTable_GetPaletteIndex(const SpriteResourceTable *table, int index);

#endif // POKEPLATINUM_SPRITE_RESOURCE_H
