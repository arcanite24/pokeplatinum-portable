#include "heap.h"

#ifdef PLATFORM_DS
#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "error_message_reset.h"
#include "unk_020366A0.h"
#else
// SDL Build: Simple heap implementation using malloc/free
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Simple SDL implementation - just use malloc/free
void Heap_Init(void) {
    printf("[Heap] Initialized (SDL malloc/free)\n");
}

void Heap_InitSystem(const HeapParam *templates, u32 nTemplates, u32 totalNumHeaps, u32 preSize) {
    (void)templates;
    (void)nTemplates;
    (void)totalNumHeaps;
    (void)preSize;
    Heap_Init();
}

BOOL Heap_Create(enum HeapID parent, enum HeapID child, u32 size) {
    (void)parent;
    (void)child;
    (void)size;
    return TRUE;  // Always succeed for SDL
}

BOOL Heap_CreateAtEnd(enum HeapID parent, enum HeapID child, u32 size) {
    (void)parent;
    (void)child;
    (void)size;
    return TRUE;
}

void Heap_Destroy(enum HeapID heapID) {
    (void)heapID;
}

void *Heap_Alloc(u32 heapID, u32 size) {
    (void)heapID;
    return malloc(size);
}

void *Heap_AllocAtEnd(u32 heapID, u32 size) {
    (void)heapID;
    return malloc(size);
}

void Heap_Free(void *ptr) {
    free(ptr);
}

void Heap_FreeExplicit(u32 heapID, void *ptr) {
    (void)heapID;
    free(ptr);
}

u32 Heap_GetSize(enum HeapID heapID) {
    (void)heapID;
    return 0xFFFFFFFF;  // Unlimited for SDL
}

u32 Heap_GetMaxAllocatableSize(enum HeapID heapID) {
    (void)heapID;
    return 0xFFFFFFFF;
}

u32 Heap_GetTotalSize(enum HeapID heapID) {
    (void)heapID;
    return 0xFFFFFFFF;
}

u32 Heap_GetCurSize(enum HeapID heapID) {
    (void)heapID;
    return 0;  // Unknown
}

u32 Heap_GetPreSize(enum HeapID heapID) {
    (void)heapID;
    return 0;
}

u16 Heap_GetNumBlocks(enum HeapID heapID) {
    (void)heapID;
    return 0;
}

u8 Heap_GetIndex(enum HeapID heapID) {
    (void)heapID;
    return (u8)heapID;
}

enum HeapID Heap_GetIdFromIndex(u8 heapIdx) {
    return (enum HeapID)heapIdx;
}

u32 HeapExp_FndGetTotalFreeSize(u32 heapID) {
    (void)heapID;
    return 0xFFFFFFFF;  // Unlimited for SDL
}

void Heap_Realloc(void *ptr, u32 newSize) {
    // SDL: realloc not implemented yet
    (void)ptr;
    (void)newSize;
}

BOOL GF_heap_c_dummy_return_true(u32 heapID) {
    (void)heapID;
    return TRUE;
}

#endif  // !PLATFORM_DS

#ifdef PLATFORM_DS
// Original DS code continues below...

typedef struct {
    NNSFndHeapHandle *heapHandles;
    NNSFndHeapHandle *parentHeapHandles;
    void **subHeapRawPtrs;
    u16 *numMemBlocks;
    u8 *heapIdxs;
    u16 totalNumHeaps;
    u16 nTemplates;
    u16 maxHeaps;
    u16 unallocatedHeapID;
} HeapInfo;

typedef struct {
    char filler_00[12];
    u32 heapID : 8;
    u32 filler_0D : 24;
} MemoryBlock;

static int FindFirstAvailableHeapHandle(void);
static BOOL CreateHeapInternal(enum HeapID parent, enum HeapID child, u32 size, s32 alignment);
static void *AllocFromHeapInternal(NNSFndHeapHandle heap, u32 size, s32 alignment, u32 heapID);

static HeapInfo sHeapInfo = { 0 };

void Heap_InitSystem(const HeapParam *templates, u32 nTemplates, u32 totalNumHeaps, u32 preSize)
{
    void *ptr;
    u32 i;
    u32 usableHeaps = nTemplates + 24;
    if (totalNumHeaps < usableHeaps) {
        totalNumHeaps = usableHeaps;
    }

    if (preSize) {
        while (preSize & 3) {
            preSize++;
        }
        #ifdef PLATFORM_DS
        OS_AllocFromMainArenaLo(preSize, 4);
        #else
        // TODO: Port OS_AllocFromMainArenaLo to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    sHeapInfo.heapHandles = OS_AllocFromMainArenaLo((sizeof(NNSFndHeapHandle) * (usableHeaps + 1)) + (sizeof(NNSFndHeapHandle) * usableHeaps) + (sizeof(void *) * usableHeaps) + (sizeof(u16) * (totalNumHeaps)) + totalNumHeaps, 4);
    #else
    // TODO: Port OS_AllocFromMainArenaLo to PAL
    #endif
    sHeapInfo.parentHeapHandles = sHeapInfo.heapHandles + (usableHeaps + 1);
    sHeapInfo.subHeapRawPtrs = (void **)((u8 *)(sHeapInfo.parentHeapHandles) + (sizeof(NNSFndHeapHandle) * usableHeaps));
    sHeapInfo.numMemBlocks = (u16 *)((u8 *)(sHeapInfo.subHeapRawPtrs) + (sizeof(void *) * usableHeaps));
    sHeapInfo.heapIdxs = (u8 *)(sHeapInfo.numMemBlocks) + (sizeof(u16) * (totalNumHeaps));
    sHeapInfo.totalNumHeaps = totalNumHeaps;
    sHeapInfo.nTemplates = nTemplates;
    sHeapInfo.unallocatedHeapID = usableHeaps;
    sHeapInfo.maxHeaps = usableHeaps;

    for (i = 0; i < nTemplates; i++) {
        switch (templates[i].arena) {
        #ifdef PLATFORM_DS
        case OS_ARENA_MAIN:
        #else
        // TODO: Port OS_ARENA_MAIN to PAL
        #endif
        default:
            #ifdef PLATFORM_DS
            ptr = OS_AllocFromMainArenaLo(templates[i].size, 4);
            #else
            // TODO: Port OS_AllocFromMainArenaLo to PAL
            #endif
            break;
        #ifdef PLATFORM_DS
        case OS_ARENA_MAINEX:
        #else
        // TODO: Port OS_ARENA_MAINEX to PAL
        #endif
            #ifdef PLATFORM_DS
            ptr = OS_AllocFromMainExArenaHi(templates[i].size, 4);
            #else
            // TODO: Port OS_AllocFromMainExArenaHi to PAL
            #endif
            break;
        }

        if (ptr != NULL) {
            #ifdef PLATFORM_DS
            sHeapInfo.heapHandles[i] = NNS_FndCreateExpHeap(ptr, templates[i].size);
            #else
            // TODO: Port NNS_FndCreateExpHeap to PAL
            #endif
            sHeapInfo.heapIdxs[i] = i;
        } else {
            GF_ASSERT(FALSE);
        }
    }

    for (i = nTemplates; i < (usableHeaps + 1); i++) {
        #ifdef PLATFORM_DS
        sHeapInfo.heapHandles[i] = NNS_FND_HEAP_INVALID_HANDLE;
        #else
        // TODO: Port NNS_FND_HEAP_INVALID_HANDLE to PAL
        #endif
        sHeapInfo.heapIdxs[i] = sHeapInfo.unallocatedHeapID;
    }

    while (i < totalNumHeaps) {
        sHeapInfo.heapIdxs[i++] = sHeapInfo.unallocatedHeapID;
    }

    for (i = 0; i < totalNumHeaps; i++) {
        sHeapInfo.numMemBlocks[i] = 0;
    }
}

static int FindFirstAvailableHeapHandle(void)
{
    for (int i = sHeapInfo.nTemplates; i < sHeapInfo.maxHeaps; i++) {
        #ifdef PLATFORM_DS
        if (sHeapInfo.heapHandles[i] == NNS_FND_HEAP_INVALID_HANDLE) {
        #else
        // TODO: Port NNS_FND_HEAP_INVALID_HANDLE to PAL
        #endif
            return i;
        }
    }

    return -1;
}

BOOL Heap_Create(enum HeapID parent, enum HeapID child, u32 size)
{
    return CreateHeapInternal(parent, child, size, 4);
}

BOOL Heap_CreateAtEnd(enum HeapID parent, enum HeapID child, u32 size)
{
    return CreateHeapInternal(parent, child, size, -4);
}

static BOOL CreateHeapInternal(enum HeapID parent, enum HeapID child, u32 size, s32 alignment)
{
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_GetProcMode to PAL
    #endif
    GF_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    #else
    // TODO: Port OS_PROCMODE_IRQ to PAL
    #endif

    if (sHeapInfo.heapIdxs[child] == sHeapInfo.unallocatedHeapID) {
        NNSFndHeapHandle parentHeap = sHeapInfo.heapHandles[sHeapInfo.heapIdxs[parent]];

        #ifdef PLATFORM_DS
        if (parentHeap != NNS_FND_HEAP_INVALID_HANDLE) {
        #else
        // TODO: Port NNS_FND_HEAP_INVALID_HANDLE to PAL
        #endif
            #ifdef PLATFORM_DS
            void *newHeapAddr = NNS_FndAllocFromExpHeapEx(parentHeap, size, alignment);
            #else
            // TODO: Port NNS_FndAllocFromExpHeapEx to PAL
            #endif

            if (newHeapAddr != NULL) {
                int i = FindFirstAvailableHeapHandle();

                if (i >= 0) {
                    #ifdef PLATFORM_DS
                    sHeapInfo.heapHandles[i] = NNS_FndCreateExpHeap(newHeapAddr, size);
                    #else
                    // TODO: Port NNS_FndCreateExpHeap to PAL
                    #endif

                    #ifdef PLATFORM_DS
                    if (sHeapInfo.heapHandles[i] != NNS_FND_HEAP_INVALID_HANDLE) {
                    #else
                    // TODO: Port NNS_FND_HEAP_INVALID_HANDLE to PAL
                    #endif
                        sHeapInfo.parentHeapHandles[i] = parentHeap;
                        sHeapInfo.subHeapRawPtrs[i] = newHeapAddr;
                        sHeapInfo.heapIdxs[child] = i;

                        return TRUE;
                    } else {
                        GF_ASSERT(FALSE);
                    }
                } else {
                    GF_ASSERT(FALSE);
                }
            } else {
                GF_ASSERT(FALSE);
            }
        } else {
            GF_ASSERT(FALSE);
        }
    } else {
        GF_ASSERT(FALSE);
    }

    return FALSE;
}

void Heap_Destroy(enum HeapID heapID)
{
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_GetProcMode to PAL
    #endif
    GF_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    #else
    // TODO: Port OS_PROCMODE_IRQ to PAL
    #endif

    u8 index = sHeapInfo.heapIdxs[heapID];
    NNSFndHeapHandle handle = sHeapInfo.heapHandles[index];

    #ifdef PLATFORM_DS
    if (handle != NNS_FND_HEAP_INVALID_HANDLE) {
    #else
    // TODO: Port NNS_FND_HEAP_INVALID_HANDLE to PAL
    #endif
        NNSFndHeapHandle parentHeap;
        void *childRaw;

        #ifdef PLATFORM_DS
        NNS_FndDestroyExpHeap(handle);
        #else
        // TODO: Port NNS_FndDestroyExpHeap to PAL
        #endif

        parentHeap = sHeapInfo.parentHeapHandles[sHeapInfo.heapIdxs[heapID]];
        childRaw = sHeapInfo.subHeapRawPtrs[sHeapInfo.heapIdxs[heapID]];

        #ifdef PLATFORM_DS
        if ((parentHeap != NNS_FND_HEAP_INVALID_HANDLE) && (childRaw != NULL)) {
        #else
        // TODO: Port NNS_FND_HEAP_INVALID_HANDLE to PAL
        #endif
            #ifdef PLATFORM_DS
            NNS_FndFreeToExpHeap(parentHeap, childRaw);
            #else
            // TODO: Port NNS_FndFreeToExpHeap to PAL
            #endif
        } else {
            GF_ASSERT(FALSE);
        }

        sHeapInfo.heapHandles[sHeapInfo.heapIdxs[heapID]] = (void *)0;
        sHeapInfo.parentHeapHandles[sHeapInfo.heapIdxs[heapID]] = (void *)0;
        sHeapInfo.subHeapRawPtrs[sHeapInfo.heapIdxs[heapID]] = (void *)0;

        sHeapInfo.heapIdxs[heapID] = sHeapInfo.unallocatedHeapID;
    }
}

static void *AllocFromHeapInternal(NNSFndHeapHandle heap, u32 size, s32 alignment, u32 heapID)
{
    void *ptr;
    OSIntrMode intr_mode;

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    GF_ASSERT(heap != NNS_FND_HEAP_INVALID_HANDLE);
    #else
    // TODO: Port NNS_FND_HEAP_INVALID_HANDLE to PAL
    #endif
    #else
    // TODO: Port NNS_FND_HEAP_INVALID_HANDLE to PAL
    #endif

    #ifdef PLATFORM_DS
    intr_mode = OS_DisableInterrupts();
    #else
    // TODO: Port OS_DisableInterrupts to PAL
    #endif
    #ifdef PLATFORM_DS
    ptr = NNS_FndAllocFromExpHeapEx(heap, size + sizeof(MemoryBlock), alignment);
    #else
    // TODO: Port NNS_FndAllocFromExpHeapEx to PAL
    #endif

    #ifdef PLATFORM_DS
    OS_RestoreInterrupts(intr_mode);
    #else
    // TODO: Port OS_RestoreInterrupts to PAL
    #endif

    if (ptr != NULL) {
        ((MemoryBlock *)ptr)->heapID = heapID;
        ptr += sizeof(MemoryBlock);
    }

    return ptr;
}

static void AllocFail(void)
{
    if (CommMan_IsInitialized()) {
        ErrorMessageReset_PrintErrorAndReset();
    }
}

void *Heap_Alloc(u32 heapID, u32 size)
{
    void *ptr = NULL;

    if (heapID < sHeapInfo.totalNumHeaps) {
        u8 index = sHeapInfo.heapIdxs[heapID];
        NNSFndHeapHandle heap = sHeapInfo.heapHandles[index];
        ptr = AllocFromHeapInternal(heap, size, 4, heapID);
    }

    if (ptr != NULL) {
        sHeapInfo.numMemBlocks[heapID]++;
    } else {
        AllocFail();
    }

    return ptr;
}

void *Heap_AllocAtEnd(u32 heapID, u32 size)
{
    void *ptr = NULL;

    if (heapID < sHeapInfo.totalNumHeaps) {
        u8 index = sHeapInfo.heapIdxs[heapID];
        NNSFndHeapHandle heap = sHeapInfo.heapHandles[index];
        ptr = AllocFromHeapInternal(heap, size, -4, heapID);
    }

    if (ptr != NULL) {
        sHeapInfo.numMemBlocks[heapID]++;
    } else {
        AllocFail();
    }

    return ptr;
}

void Heap_Free(void *ptr)
{
    ptr -= sizeof(MemoryBlock);
    u32 heapID = ((MemoryBlock *)ptr)->heapID;

    if (heapID < sHeapInfo.totalNumHeaps) {
        u8 index = sHeapInfo.heapIdxs[heapID];
        NNSFndHeapHandle heap = sHeapInfo.heapHandles[index];

        #ifdef PLATFORM_DS
        GF_ASSERT(heap != NNS_FND_HEAP_INVALID_HANDLE);
        #else
        // TODO: Port NNS_FND_HEAP_INVALID_HANDLE to PAL
        #endif

        if (sHeapInfo.numMemBlocks[heapID] == 0) {
            GF_heap_c_dummy_return_true(heapID);
        }

        GF_ASSERT(sHeapInfo.numMemBlocks[heapID]);

        sHeapInfo.numMemBlocks[heapID]--;

        {
            OSIntrMode intrMode;

            #ifdef PLATFORM_DS
            intrMode = OS_DisableInterrupts();
            #else
            // TODO: Port OS_DisableInterrupts to PAL
            #endif
            #ifdef PLATFORM_DS
            NNS_FndFreeToExpHeap(heap, ptr);
            #else
            // TODO: Port NNS_FndFreeToExpHeap to PAL
            #endif
            #ifdef PLATFORM_DS
            OS_RestoreInterrupts(intrMode);
            #else
            // TODO: Port OS_RestoreInterrupts to PAL
            #endif
        }
    } else {
        GF_ASSERT(FALSE);
    }
}

void Heap_FreeExplicit(u32 heapID, void *ptr)
{
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_GetProcMode to PAL
    #endif
    GF_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    #else
    // TODO: Port OS_PROCMODE_IRQ to PAL
    #endif

    if (heapID < sHeapInfo.totalNumHeaps) {
        u8 index = sHeapInfo.heapIdxs[heapID];
        NNSFndHeapHandle heap = sHeapInfo.heapHandles[index];

        #ifdef PLATFORM_DS
        GF_ASSERT(heap != NNS_FND_HEAP_INVALID_HANDLE);
        #else
        // TODO: Port NNS_FND_HEAP_INVALID_HANDLE to PAL
        #endif

        ptr -= sizeof(MemoryBlock);

        if (((MemoryBlock *)ptr)->heapID != heapID) {
            GF_ASSERT(FALSE);
        }

        #ifdef PLATFORM_DS
        NNS_FndFreeToExpHeap(heap, ptr);
        #else
        // TODO: Port NNS_FndFreeToExpHeap to PAL
        #endif

        GF_ASSERT(sHeapInfo.numMemBlocks[heapID]);
        sHeapInfo.numMemBlocks[heapID]--;
    } else {
        GF_ASSERT(FALSE);
    }
}

u32 HeapExp_FndGetTotalFreeSize(u32 heapID)
{
    if (heapID < sHeapInfo.totalNumHeaps) {
        u8 index = sHeapInfo.heapIdxs[heapID];
        NNSFndHeapHandle heap = sHeapInfo.heapHandles[index];
        #ifdef PLATFORM_DS
        return NNS_FndGetTotalFreeSizeForExpHeap(heap);
        #else
        // TODO: Port NNS_FndGetTotalFreeSizeForExpHeap to PAL
        #endif
    }

    GF_ASSERT(FALSE);
    return 0;
}

void HeapExp_FndInitAllocator(NNSFndAllocator *pAllocator, u32 heapID, int alignment)
{
    if (heapID < sHeapInfo.totalNumHeaps) {
        u8 index = sHeapInfo.heapIdxs[heapID];
        NNSFndHeapHandle heap = sHeapInfo.heapHandles[index];
        #ifdef PLATFORM_DS
        NNS_FndInitAllocatorForExpHeap(pAllocator, heap, alignment);
        #else
        // TODO: Port NNS_FndInitAllocatorForExpHeap to PAL
        #endif
    } else {
        GF_ASSERT(FALSE);
    }
}

void Heap_Realloc(void *ptr, u32 newSize)
{
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_GetProcMode to PAL
    #endif
    GF_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    #else
    // TODO: Port OS_PROCMODE_IRQ to PAL
    #endif

    newSize += sizeof(MemoryBlock);
    ptr -= sizeof(MemoryBlock);

    #ifdef PLATFORM_DS
    if (NNS_FndGetSizeForMBlockExpHeap(ptr) >= newSize) {
    #else
    // TODO: Port NNS_FndGetSizeForMBlockExpHeap to PAL
    #endif
        u32 heapID = ((MemoryBlock *)ptr)->heapID;
        u8 index = sHeapInfo.heapIdxs[heapID];
        NNSFndHeapHandle heap = sHeapInfo.heapHandles[index];

        #ifdef PLATFORM_DS
        NNS_FndResizeForMBlockExpHeap(heap, ptr, newSize);
        #else
        // TODO: Port NNS_FndResizeForMBlockExpHeap to PAL
        #endif
    } else {
        GF_ASSERT(FALSE);
    }
}

BOOL GF_heap_c_dummy_return_true(u32 heapID)
{
    return TRUE;
}

#endif  // PLATFORM_DS
