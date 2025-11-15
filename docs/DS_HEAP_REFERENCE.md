# DS Heap System Reference

**Purpose:** This document preserves the original Nintendo DS heap implementation for reference. The heap system was removed from `src/heap.c` but is documented here to understand the original memory management behavior.

**Date Archived:** November 15, 2025  
**Original Location:** `src/heap.c` lines 124-468

---

## Overview

The DS used NitroSDK's **Expanded Heap** system with:
- **Fixed RAM:** 4 MB total (ARM9 + ARM7)
- **Arena-based allocation:** OS_ARENA_MAIN, OS_ARENA_MAINEX
- **Heap hierarchy:** Parent/child heap relationships
- **Memory tracking:** Per-block headers with heap ID and canaries

---

## Heap Architecture

### HeapInfo Structure
```c
typedef struct {
    NNSFndHeapHandle *heapHandles;        // Array of heap handles
    NNSFndHeapHandle *parentHeapHandles;  // Parent heap tracking
    void **subHeapRawPtrs;                // Raw memory pointers
    u16 *numMemBlocks;                    // Block count per heap
    u8 *heapIdxs;                         // Heap ID → index mapping
    u16 totalNumHeaps;                    // Total heaps in system
    u16 nTemplates;                       // Number of templates
    u16 maxHeaps;                         // Maximum heaps allowed
    u16 unallocatedHeapID;                // Next available ID
} HeapInfo;
```

### Memory Block Structure
```c
typedef struct {
    char filler_00[12];    // NNS header data
    u32 heapID : 8;        // Which heap owns this block
    u32 filler_0D : 24;    // Padding
} MemoryBlock;
```

---

## Key Functions

### Initialization
```c
void Heap_InitSystem(const HeapParam *templates, u32 nTemplates, 
                     u32 totalNumHeaps, u32 preSize);
// Initializes heap system with OS arenas
// Allocates tracking structures from OS_ARENA_MAIN
```

### Heap Creation
```c
BOOL Heap_Create(const HeapParam *params, enum HeapID heapID, u32 size);
// Creates heap from template parameters
// Supports parent/child hierarchy

static BOOL CreateHeapInternal(enum HeapID parent, enum HeapID child, 
                               u32 size, s32 alignment);
// Internal: Creates child heap within parent heap
```

### Allocation
```c
void *Heap_Alloc(enum HeapID heapID, u32 size);
// Allocates from specified heap
// Adds memory block header with heap ID

void *Heap_AllocAtEndAndClear(enum HeapID heapID, u32 size);
// Allocates from end of heap (bottom-up)
// Clears memory to zero

static void *AllocFromHeapInternal(NNSFndHeapHandle heap, u32 size, 
                                   s32 alignment, u32 heapID);
// Internal: Adds block header and calls NNS_FndAllocFromExpHeap()
```

### Deallocation
```c
void Heap_Free(void *ptr);
// Frees memory block
// Uses NNS_FndFreeToExpHeap()
```

### Queries
```c
u32 Heap_GetCurrentSize(enum HeapID heapID);
// Returns total size of heap

u32 Heap_GetTotalSize(enum HeapID heapID);
// Returns size including free space

u32 Heap_GetMaxAllocatableSize(enum HeapID heapID);
// Returns largest contiguous free block

BOOL Heap_IsValid(void *ptr);
// Checks if pointer is valid heap allocation
```

### Heap Management
```c
void Heap_Destroy(enum HeapID heapID);
// Destroys heap and frees all memory

void Heap_Clear(enum HeapID heapID);
// Frees all blocks but keeps heap alive

void Heap_ClearAtEnd(enum HeapID heapID);
// Frees blocks allocated from end
```

### Resizing
```c
void Heap_ResizeLargestAllocation(enum HeapID heapID, u32 newSize);
// Resizes the largest allocated block
// Used for dynamic buffer resizing
```

---

## Heap Hierarchy Example

```
OS_ARENA_MAIN (4 MB)
├── HEAP_ID_SYSTEM (512 KB)
│   ├── HEAP_ID_APPLICATION (256 KB)
│   │   └── HEAP_ID_FIELD1 (128 KB)
│   └── HEAP_ID_FIELD2 (128 KB)
└── HEAP_ID_BATTLE (1 MB)
    ├── HEAP_ID_BATTLE_SUB1 (512 KB)
    └── HEAP_ID_BATTLE_SUB2 (512 KB)
```

**Benefits:**
- Automatic cleanup of child heaps when parent destroyed
- Prevents memory leaks in overlay system
- Clear separation of subsystem memory

---

## DS-Specific Features

### Arena Management
```c
// Allocate from specific OS arena
OSArenaId arena = (params->arenaId == -1) 
    ? OS_ARENA_MAIN 
    : params->arenaId;
    
void *base = OS_AllocFromArenaLo(arena, size, alignment);
```

### Alignment
- All allocations 4-byte aligned by default
- Can specify custom alignment (16, 32, 64 bytes)
- Important for DMA transfers

### Memory Block Headers
- Each allocation has 12-byte header
- Contains heap ID for tracking
- Enables `Heap_IsValid()` checks

### Error Handling
```c
GF_ASSERT(heap != NULL);  // Fatal error if heap invalid
```

---

## SDL3 Port Differences

| Feature | DS Implementation | SDL3 Implementation |
|---------|------------------|---------------------|
| **Memory Limit** | 4 MB fixed | Unlimited (OS manages) |
| **Heap Hierarchy** | Parent/child heaps | Flat malloc/free |
| **Heap IDs** | Required for all allocs | Ignored (param kept for API compat) |
| **Alignment** | Manual, DMA-critical | OS handles automatically |
| **Block Tracking** | Custom headers | OS manages internally |
| **Arena Selection** | OS_ARENA_MAIN/MAINEX | N/A |
| **Heap Destruction** | Explicit Heap_Destroy() | Free all individually |

---

## SDL3 Heap Wrapper

Our SDL3 implementation is simplified:

```c
void* Heap_Alloc(u32 heapID, size_t size) {
    (void)heapID;  // Ignored on SDL
    return malloc(size);
}

void Heap_Free(void* ptr) {
    free(ptr);
}

u32 Heap_GetTotalSize(u32 heapID) {
    (void)heapID;
    return UINT32_MAX;  // "Unlimited"
}
```

**Rationale:**
- Modern systems have gigabytes of RAM, not 4 MB
- OS provides robust memory management
- No need for complex heap hierarchy on desktop
- Heap IDs preserved for API compatibility only

---

## Porting Notes

### When DS Code Uses Heap IDs

**DS Pattern:**
```c
void* battleData = Heap_Alloc(HEAP_ID_BATTLE, sizeof(BattleData));
// ... use data ...
Heap_Destroy(HEAP_ID_BATTLE);  // Frees battleData automatically
```

**SDL Port:**
```c
void* battleData = Heap_Alloc(HEAP_ID_BATTLE, sizeof(BattleData));
// ... use data ...
Heap_Free(battleData);  // Must free explicitly
// Heap_Destroy() is no-op on SDL
```

**Warning:** SDL port requires explicit `Heap_Free()` for every allocation. Relying on `Heap_Destroy()` will cause memory leaks.

### When DS Code Uses Heap Queries

**DS Pattern:**
```c
u32 freeSpace = Heap_GetMaxAllocatableSize(HEAP_ID_SYSTEM);
if (freeSpace < requiredSize) {
    // Not enough memory!
}
```

**SDL Port:**
```c
u32 freeSpace = Heap_GetMaxAllocatableSize(HEAP_ID_SYSTEM);
// Always returns UINT32_MAX on SDL - check always passes
// Memory allocation will fail at malloc() if truly out of RAM
```

---

## Common Heap Constants

From `include/constants/heap.h`:

```c
enum HeapID {
    HEAP_ID_SYSTEM = 0,
    HEAP_ID_APPLICATION = 1,
    HEAP_ID_FIELD1 = 2,
    HEAP_ID_FIELD2 = 3,
    HEAP_ID_BATTLE = 4,
    // ... more IDs
};
```

**Usage in Game:**
- `HEAP_ID_SYSTEM`: Core system services (always resident)
- `HEAP_ID_APPLICATION`: Current application/screen
- `HEAP_ID_FIELD1/2`: Overworld, walking around
- `HEAP_ID_BATTLE`: Battle system and animations

---

## References

- **NitroSDK:** `include/nnsys/fnd/allocator.h`
- **Original Code:** `src/heap.c` (removed November 15, 2025)
- **SDL Port:** `src/heap.c` (current - malloc/free wrapper)

---

**Note:** This reference is for understanding only. The DS heap system has been completely replaced with malloc/free for SDL3. Do not attempt to reintegrate this code.
