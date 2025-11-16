#ifndef POKEPLATINUM_HEAP_H
#define POKEPLATINUM_HEAP_H

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
#include "platform/platform_types.h"
#endif

#include "constants/heap.h"

#ifdef PLATFORM_DS
typedef struct HeapParam {
    u32 size;
    OSArenaId arena;
} HeapParam;
#else
typedef struct HeapParam {
    u32 size;
    int arena;  // SDL doesn't have OS arenas
} HeapParam;
#endif

void Heap_InitSystem(const HeapParam *templates, u32 nTemplates, u32 totalNumHeaps, u32 preSize);
BOOL Heap_Create(enum HeapID parent, enum HeapID child, u32 size);
BOOL Heap_CreateAtEnd(enum HeapID parent, enum HeapID child, u32 size);
void Heap_Destroy(enum HeapID heapID);
void *Heap_Alloc(u32 heapID, u32 size);
void *Heap_AllocAtEnd(u32 heapID, u32 size);
void Heap_Free(void *ptr);
void Heap_FreeExplicit(u32 heapID, void *ptr);
u32 HeapExp_FndGetTotalFreeSize(u32 heapID);

#ifdef PLATFORM_DS
void HeapExp_FndInitAllocator(NNSFndAllocator *pAllocator, u32 heapID, int alignment);
#else
#include "nns_types.h"  /* SDL: NNSFndAllocator defined here */
static inline void HeapExp_FndInitAllocator(NNSFndAllocator *pAllocator, u32 heapID, int alignment) {
    (void)pAllocator; (void)heapID; (void)alignment;
}
#endif

void Heap_Realloc(void *ptr, u32 newSize);
BOOL GF_heap_c_dummy_return_true(u32 heapID);

#ifdef PLATFORM_DS
NNSFndHeapHandle Heap_GetHandle(enum HeapID heapID);
#else
typedef void* NNSFndHeapHandle;
static inline NNSFndHeapHandle Heap_GetHandle(enum HeapID heapID) {
    (void)heapID;
    return NULL;
}
#endif

#endif // POKEPLATINUM_HEAP_H
