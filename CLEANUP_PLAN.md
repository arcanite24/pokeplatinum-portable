# Code Cleanup Plan - Remove Conditional Compilation

**Created:** November 14, 2025  
**Purpose:** Document strategy for removing `#ifdef PLATFORM_DS` conditional compilation and transitioning to pure SDL3 code

---

## Overview

During initial porting phases, we used conditional compilation (`#ifdef PLATFORM_DS`) to maintain dual DS and SDL builds. This added significant complexity (~200 lines of boilerplate across multiple files). **We've now decided to focus exclusively on SDL3**, so this conditional compilation should be removed.

This document outlines how to systematically clean up the codebase.

---

## Affected Files

### High Priority (Active Development)

1. **src/graphics.c** (~697 lines total)
   - Lines 1-16: Conditional includes
   - Lines 33-190: SDL implementations (keep)
   - Lines 188-687: DS-specific code wrapped in `#ifdef PLATFORM_DS` (DELETE)
   - **Action**: Remove lines 188-687 entirely

2. **include/graphics.h** (~82 lines)
   - Lines 4-14: Stub typedefs for NitroSDK types
   - Lines 20-27: Conditional background includes
   - **Action**: Remove stubs, use real SDL/PAL types directly

3. **src/heap.c** (~697 lines total)
   - Lines 1-120: SDL malloc/free wrapper (keep)
   - Lines 124-697: Original DS heap code (DELETE)
   - **Action**: Remove lines 124-697 entirely

4. **include/heap.h** (~48 lines)
   - Lines 4-9: Conditional includes
   - Lines 12-23: Conditional HeapParam struct
   - Lines 34-48: NNSFndHeapHandle typedef and inline wrappers
   - **Action**: Simplify to SDL-only types

5. **include/narc.h**
   - Lines 6-11: FSFile stub typedef
   - **Action**: Remove conditional, define FSFile for SDL permanently

### Medium Priority

6. **include/platform/platform_config.h**
   - Currently defines both `PLATFORM_DS` and `PLATFORM_SDL` paths
   - **Action**: Remove DS path, always define `PLATFORM_SDL`

7. **CMakeLists.txt**
   - Lines 28-43: PAL_SDL_SOURCES
   - Lines 45-51: GAME_SOURCES (graphics.c, heap.c)
   - **Action**: Merge GAME_SOURCES into main sources list, no need for separation

### Low Priority (Future)

8. **src/system.c** (not yet ported)
   - Will need PAL integration
   - **Action**: Directly use PAL, no conditional compilation

9. **src/main.c** (not yet ported)
   - Will need PAL integration
   - **Action**: Directly use PAL, no conditional compilation

---

## Cleanup Strategy

### Phase 1: Remove Dead DS Code (Immediate)

**Files to clean:**
- src/graphics.c: Delete lines 188-687 (DS implementation)
- src/heap.c: Delete lines 124-697 (DS heap system)
- include/graphics.h: Remove stub typedefs
- include/heap.h: Simplify to SDL-only types
- include/narc.h: Remove conditional FSFile typedef

**Benefits:**
- ~1000 lines of code removed
- Simpler file navigation
- No more dead code in repository

### Phase 2: Simplify Type System (Short Term)

**Current Problem:**
```c
// Stub typedefs to make headers compile
typedef void* NNSG2dImageProxy;
typedef void* NNSG2dPaletteData;
typedef void* FSFile;
```

**Solution:**
Replace with real SDL/PAL types or remove entirely:
```c
// graphics.h - use PAL types directly
// No need for NNSG2dImageProxy - use SDL_Texture* or PAL_Texture*

// narc.h - define FSFile properly for SDL
typedef struct FSFile {
    PAL_File file;
    size_t size;
} FSFile;
```

### Phase 3: Simplify Build System (Medium Term)

**Current:**
```cmake
set(PAL_SDL_SOURCES ...)
set(GAME_SOURCES ...)
add_executable(pokeplatinum_sdl ${PAL_SDL_SOURCES} ${GAME_SOURCES} ...)
```

**Simplified:**
```cmake
set(SOURCES
    # Platform layer
    src/platform/sdl/pal_graphics_sdl.c
    src/platform/sdl/pal_input_sdl.c
    # ... all other files
    
    # Game code
    src/graphics.c
    src/heap.c
    # ... all other files
)

add_executable(pokeplatinum_sdl ${SOURCES})
```

### Phase 4: Update Documentation (Ongoing)

**Already Updated:**
- ✅ PORTING_PLAN.md - SDL3-only strategy
- ✅ README.md - Clarified as SDL3 port fork
- ✅ .github/copilot-instructions.md - Direct replacement approach

**Still Contains Conditional Compilation References:**
- ⏸️ PAL.md - Some examples show `#ifdef PLATFORM_DS`
- ⏸️ PHASE*_SUMMARY.md files - May reference dual-compilation

**Action:** Review and update as encountered during future work.

---

## Detailed Cleanup Instructions

### Cleanup src/graphics.c

**Before:**
```c
// Line 1-16: Conditional includes
#ifdef PLATFORM_SDL
#include <stdio.h>
#include <stdlib.h>
// ...
#endif

// Line 33-190: SDL implementations
#ifdef PLATFORM_SDL
void Graphics_LoadTilesToBgLayer(...) {
    // SDL implementation
}
#endif

// Line 188-687: DS implementations
#ifdef PLATFORM_DS
void Graphics_LoadTilesToBgLayer(...) {
    // DS implementation (500+ lines)
}
// ... many more functions
#endif
```

**After:**
```c
// Line 1-16: Always include SDL headers
#include <stdio.h>
#include <stdlib.h>
#include "platform/pal_file.h"
#include "platform/pal_background.h"
#include "platform/pal_graphics.h"
// No conditionals

// Line 33-190: SDL implementations (no conditionals)
void Graphics_LoadTilesToBgLayer(...) {
    // SDL implementation
}

// DELETE lines 188-687 entirely - DS code removed
```

### Cleanup src/heap.c

**Before:**
```c
// Lines 1-120: SDL wrapper
#ifdef PLATFORM_SDL
void* Heap_Alloc(u32 heapID, size_t size) {
    return malloc(size);
}
// ... more functions
#endif

// Lines 124-697: DS implementation
#ifdef PLATFORM_DS
void* Heap_Alloc(u32 heapID, size_t size) {
    // DS arena allocation (300+ lines)
}
#endif
```

**After:**
```c
// Lines 1-120: SDL implementation (no conditionals)
void* Heap_Alloc(u32 heapID, size_t size) {
    (void)heapID;  // Unused on SDL
    return malloc(size);
}

void Heap_Free(void* ptr) {
    free(ptr);
}

// DELETE lines 124-697 entirely - DS heap removed
```

### Cleanup include/graphics.h

**Before:**
```c
#ifdef PLATFORM_SDL
    // Stub typedefs for compilation
    typedef void* NNSG2dImageProxy;
    typedef void* NNSG2dPaletteData;
    typedef void* NNSG2dCellTransferState;
#else
    #include <nnsys/g2d/g2d_Image.h>
    #include <nnsys/g2d/g2d_Palette.h>
#endif

#ifdef PLATFORM_DS
    #include "bg_window.h"
#else
    #include "platform/pal_background.h"
#endif
```

**After:**
```c
// Always use PAL headers
#include "platform/pal_background.h"
#include "platform/pal_graphics.h"

// Remove stub typedefs - use real SDL/PAL types in function signatures
// If NitroSDK types are still needed, define them properly:
typedef struct NNSG2dImageProxy {
    SDL_Texture* texture;
    int width, height;
} NNSG2dImageProxy;
```

### Cleanup include/heap.h

**Before:**
```c
#ifdef PLATFORM_DS
    #include <nnsys/fnd/allocator.h>
#else
    #include "platform/platform_types.h"
#endif

typedef struct HeapParam {
#ifdef PLATFORM_DS
    u32 size;
    OSArenaId arenaId;
#else
    int unused;
#endif
} HeapParam;

#ifdef PLATFORM_SDL
typedef void* NNSFndHeapHandle;
static inline NNSFndHeapHandle NNS_FndCreateExpHeap(...) {
    return NULL;
}
#endif
```

**After:**
```c
#include "platform/platform_types.h"

typedef struct HeapParam {
    u32 size;
    // No arena on SDL - modern systems have unlimited RAM
} HeapParam;

// Define DS types for SDL (if needed)
typedef void* NNSFndHeapHandle;

// Inline wrappers (if these functions are still called)
static inline NNSFndHeapHandle NNS_FndCreateExpHeap(void* base, u32 size) {
    (void)base; (void)size;
    return NULL;  // Not used on SDL
}
```

---

## Testing After Cleanup

### Build Verification
```bash
# Clean build to catch any issues
rm -rf build-sdl
./build_sdl.sh

# Should compile without errors
# Check for warnings about unused variables (expected from stub functions)
```

### Functionality Verification
```bash
# Run application
./build-sdl/pokeplatinum_sdl

# Verify:
# 1. Window opens
# 2. Graphics render (checkerboard, sprites)
# 3. Input works (keyboard, mouse, gamepad if available)
# 4. No crashes or errors
# 5. Clean shutdown on ESC
```

### Code Quality Checks
```bash
# Run clang-format
make format

# Check for remaining conditionals (should be minimal)
grep -r "PLATFORM_DS" include/ src/
grep -r "PLATFORM_SDL" include/ src/

# Remaining conditionals should ONLY be in:
# - include/platform/platform_config.h (defines SDL)
# - src/platform/sdl/*.c (SDL implementations)
```

---

## Benefits of Cleanup

### Code Quality
- **~1000 fewer lines** of dead code
- **Simpler navigation** - no more jumping over `#ifdef` blocks
- **Easier to understand** - one implementation path, not two
- **Better diffs** - changes don't get lost in conditional blocks

### Development Speed
- **Faster compilation** - fewer files to parse
- **Simpler debugging** - no conditional code paths
- **Easier refactoring** - no need to update two implementations
- **Less cognitive load** - don't need to think about DS compatibility

### Maintainability
- **Single source of truth** - one implementation
- **No sync issues** - can't have DS/SDL implementations drift
- **Clearer intent** - code explicitly targets SDL3
- **Future-proof** - easier to add new platforms (iOS, Android) without DS baggage

---

## Timeline

### Immediate (Next Session)
- ✅ Documentation updated (PORTING_PLAN.md, README.md, copilot-instructions.md)
- 🚧 This cleanup plan created

### Short Term (Before Phase 4)
- Remove dead DS code from src/graphics.c (lines 188-687)
- Remove dead DS code from src/heap.c (lines 124-697)
- Simplify include/graphics.h (remove stub typedefs)
- Simplify include/heap.h (SDL-only types)
- Simplify include/narc.h (permanent FSFile definition)

### Medium Term (During Phase 4-5)
- Simplify build system (merge GAME_SOURCES into main sources)
- Review and update PAL.md examples
- Ensure all new code uses direct PAL calls (no conditionals)

### Long Term (Phase 6-7)
- Final audit: search for any remaining `#ifdef PLATFORM_DS`
- Update all documentation to reflect SDL3-only approach
- Add CI check to prevent conditional compilation from being re-added

---

## Conclusion

The conditional compilation approach was useful during initial exploration but has proven to be overly complex. Removing it will result in a cleaner, more maintainable codebase that's easier to work with and understand.

The cleanup should be done **incrementally** to avoid breaking the build, but can be completed relatively quickly (~2-3 hours of focused work).

---

**Next Steps:**
1. Complete this cleanup plan documentation ✅
2. Update todo list with cleanup tasks
3. Begin cleanup starting with src/graphics.c
4. Test after each file cleanup
5. Commit cleaned files with descriptive messages

**Ready to proceed with Phase 3.4.3 (Title Screen Integration) after cleanup!**
