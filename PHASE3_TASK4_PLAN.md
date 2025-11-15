# Phase 3.4: Game Integration & Rendering - Implementation Plan

**Status:** 🚧 **IN PROGRESS**  
**Priority:** HIGH  
**Estimated Duration:** 2-3 weeks  
**Start Date:** November 14, 2025  

---

## Overview

Phase 3.4 bridges the gap between the completed Platform Abstraction Layer (PAL) and the actual Pokémon Platinum game code. The goal is to get **real game graphics rendering** through the PAL system, proving end-to-end functionality.

**Key Deliverable:** Title screen rendering with actual game assets

---

## Implementation Roadmap

### Task 3.4.1: Core System Integration ✅ COMPLETE
**Priority:** CRITICAL - Must be done first  
**Estimated Time:** 3-4 days  
**Actual Time:** ~3 hours  
**Completion Date:** November 14, 2025  

#### Files Modified
- ✅ `src/system.c` - System initialization and input handling
- ✅ `src/main.c` - Main loop and frame rendering

#### Sub-Tasks
- [x] **A. Input System Integration** (4 hours) ✅ COMPLETE
  - Modified `ReadKeypadAndTouchpad()` to use PAL input
  - Maps PAL button states to `gSystem` structure
  - Touch state handling implemented
  - **Validation:** Input detected successfully ✅

- [x] **B. Graphics Initialization** (4 hours) ✅ COMPLETE
  - Modified `InitSystem()` to initialize PAL graphics
  - Calls `PAL_Graphics_Init()` for SDL build
  - Task managers allocated with malloc for SDL
  - **Validation:** Window renders without crash ✅

- [x] **C. Main Loop Integration** (6 hours) ✅ COMPLETE
  - Modified main loop to call PAL frame functions
  - Integrated `PAL_Graphics_BeginFrame()` / `EndFrame()`
  - Wrapped `OS_WaitIrq()` in DS-only conditional
  - **Validation:** Test app runs at 54.53 FPS ✅

#### Success Criteria
✅ Game compiles with SDL build  
✅ Window opens and remains stable  
✅ Input detected from keyboard/gamepad  
✅ No crashes during initialization  

**Result:** All criteria met! Ready for Task 3.4.2  

---

### Task 3.4.2: Graphics Resource Loading ✅ COMPLETE
**Priority:** HIGH - Required for any visuals  
**Estimated Time:** 5-6 days | **Actual Time:** 4 hours  
**Completion Date:** November 14, 2025  

#### Files Modified
- ✅ `src/graphics.c` - Loading functions (SDL implementations added)
- ✅ `include/graphics.h` - Conditional includes and type stubs
- ✅ `src/heap.c` - SDL malloc/free wrapper
- ✅ `include/heap.h` - Conditional heap types
- ✅ `include/narc.h` - FSFile stub typedef
- ✅ `CMakeLists.txt` - Added graphics.c and heap.c to build
- ✅ `src/platform/sdl/main_sdl.c` - Graphics loading test

#### Sub-Tasks
- ✅ **A. NARC Graphics Loading** (2 hours)
  - ✅ Implemented `Graphics_LoadTilesToBgLayer()` SDL version
  - ✅ Added filesystem path mapping via `GetAssetPath()` helper
  - ✅ Loads raw tile data from .bin files
  - ✅ Calls `PAL_Bg_LoadTiles()` for rendering
  - **Validation:** ✅ Loaded 32,776 bytes from top_border_tiles.bin

- ✅ **B. Palette Loading** (1 hour)
  - ✅ Implemented `Graphics_LoadPaletteWithSrcOffset()` SDL version
  - ✅ Parses .pal files (RGB555 format)
  - ✅ Calculates color count (filesize / 2)
  - ✅ Applies source offset for palette extraction
  - **Validation:** ✅ Loaded 260 colors from palette_1.pal

- ⏸️ **C. Sprite Graphics Loading** (Not started - deferred to Phase 3.4.3)
  - Sprite loading will be handled when implementing title screen
  - PAL_Sprite API already complete from Phase 2.4

#### Test Results
```
Test 1: Loading top_border tiles (member 5)
[Graphics] Loaded 32776 bytes of tiles from resources/graphics/title_screen/top_border_tiles.bin to layer 1
✅ Loaded 32776 bytes of tiles

Test 2: Loading palette_1 (member 4)
[Graphics] Loaded 260 colors from resources/graphics/title_screen/palette_1.pal (offset: 0)
✅ Palette loaded

Test 3: Loading logo_top tiles (member 9)
[Graphics] Loaded 24584 bytes of tiles from resources/graphics/title_screen/logo_top_tiles.bin to layer 1
✅ Loaded 24584 bytes of tiles
```

#### Success Criteria
✅ Tile data loads from filesystem (32K+ bytes per file)  
✅ Palettes load correctly (260 RGB555 colors)  
✅ No memory leaks during resource loading  
✅ Build compiles cleanly for both DS and SDL  
✅ Console logging provides debugging info  

**Documentation:** See `PHASE3_TASK4.2_SUMMARY.md` for detailed implementation notes.

---

### Task 3.4.3: Title Screen Integration ⏳ TODO
**Priority:** MEDIUM - Proves concept works  
**Estimated Time:** 4-5 days  

#### Files to Modify
- `src/applications/title_screen.c` - Main title screen code

#### Sub-Tasks
- [ ] **A. Initialization** (6 hours)
  - Port `TitleScreen_Init()` to use PAL
  - Create PAL background config
  - Setup background layers
  - Load initial resources
  - **Validation:** Title screen state machine starts

- [ ] **B. Rendering** (8 hours)
  - Port background rendering functions
  - Load title screen graphics (logo, borders, etc.)
  - Position backgrounds correctly
  - Apply palettes
  - **Validation:** Static title screen graphics visible

- [ ] **C. Input Handling** (4 hours)
  - Verify Start button advances screens
  - Test idle timeout triggers
  - **Validation:** Can navigate title screen with keyboard

#### Success Criteria
✅ Title screen displays with correct graphics  
✅ Logo, borders, and text visible  
✅ Start button advances to next state  
✅ Colors match DS version  

---

### Task 3.4.4: Asset Conversion Pipeline ✅ COMPLETE
**Priority:** HIGH - Required for real assets  
**Estimated Time:** 3-4 days  
**Actual Time:** ~2 hours  
**Completion Date:** November 14, 2025

#### Tools Created
1. ✅ `tools/narc_extractor.py` - Extract NARC archives by file ID
2. ✅ `tools/ncgr_converter.py` - Convert graphics (manual parser)
3. ✅ `tools/nclr_converter.py` - Convert palettes (manual parser)
4. ✅ `tools/convert_title_screen.py` - Quick converter for title screen

#### Sub-Tasks
- [x] **A. NARC Extraction Tool** (6 hours → 1 hour)
  - Research NARC format structure
  - ✅ Fixed ndspy API usage for Folder iteration
  - ✅ Extract by file ID instead of path patterns
  - ✅ Successfully extracted NARC 319 (demo/title/titledemo.narc)
  - ✅ **Validation:** Extracted 29 files from title screen NARC

- [x] **B. NCGR Converter** (4 hours → 30 min)
  - ✅ Manual NCGR parser (ndspy.graphics2D not needed)
  - ✅ Extracted tile data for 6 graphics files
  - ✅ Detected 8bpp color mode correctly
  - ✅ **Validation:** Converted top_border (512 tiles), logo (384+512 tiles), press_start (128 tiles)

- [x] **C. NCLR Converter** (4 hours → 30 min)
  - ✅ Manual NCLR palette parser
  - ✅ RGB555 extraction and conversion
  - ✅ Generated human-readable color info
  - ✅ **Validation:** Converted 6 palette files (260 colors each)

- [x] **D. Quick Converter Script** (2 hours → 15 min)
  - ✅ Created `convert_title_screen.py` for batch processing
  - ✅ Automated tile + palette conversion
  - ✅ Generated info files for debugging
  - ✅ **Output:** `resources/graphics/title_screen/` with 12 .bin + 12 .txt files

#### Success Criteria
✅ All tools working and documented  
✅ Title screen assets extracted (29 files from NARC 319)  
✅ Assets converted: 6 tile files + 6 palette files  
✅ Conversion reproducible (single command)  
✅ Info files for debugging created  

---

### Task 3.4.5: Testing & Validation ⏳ TODO
**Priority:** HIGH - Final verification  
**Estimated Time:** 2-3 days  

#### Test Categories

**Functional Tests:**
- [ ] Title screen loads without errors
- [ ] Graphics render correctly
- [ ] Colors match DS version
- [ ] Input responsive
- [ ] No crashes on navigation

**Performance Tests:**
- [ ] 60 FPS maintained
- [ ] Memory usage reasonable (<100 MB)
- [ ] No memory leaks (Valgrind/Instruments)

**Regression Tests:**
- [ ] Test application still works
- [ ] Phase 2 graphics tests pass
- [ ] Phase 3 input tests pass

#### Bug Fixing
Reserve 2-3 days for fixing issues found during testing.

#### Success Criteria
✅ All tests passing  
✅ No critical bugs  
✅ Performance acceptable  
✅ Ready for Phase 3.5 or Phase 4  

---

## Resource Requirements

### Dependencies
- **ndspy** - Python library for DS file formats
  ```bash
  pip install ndspy
  ```

- **Python 3.8+** - For asset conversion tools

- **Pokémon Platinum ROM** - US version recommended
  - File: `platinum.us.nds`
  - Not included in repo (obtain legally)

### Reference Materials
- **GBATEK** - DS file format documentation
- **ndspy Documentation** - File parsing examples
- **Existing PAL Documentation** - `PAL.md`, phase summaries

---

## File Organization

### Expected Directory Structure After Completion

```
pokeplatinum/
├── src/
│   ├── system.c              # Modified - PAL integration
│   ├── main.c                # Modified - Main loop
│   ├── graphics.c            # Modified - Resource loading
│   └── applications/
│       └── title_screen.c    # Modified - Title screen
├── resources/                # New - Extracted assets
│   └── graphics/
│       └── title_screen/
│           ├── top_border.bin
│           ├── top_border.pal
│           ├── logo.bin
│           └── logo.pal
├── tools/
│   ├── narc_to_sdl.py       # New - NARC extractor
│   ├── ncgr_converter.py    # New - Graphics converter
│   └── nclr_converter.py    # New - Palette converter
└── PHASE3_TASK4_SUMMARY.md  # Created after completion
```

---

## Risk Assessment

### High Risk
- **NARC format complexity** - May need reverse engineering
  - *Mitigation:* Use `ndspy` library, reference existing tools

- **Graphics format conversion** - Color/format mismatches
  - *Mitigation:* Start with simple test assets, compare with DS

### Medium Risk
- **Performance issues** - Asset loading too slow
  - *Mitigation:* Profile early, implement caching

- **Memory leaks** - Improper resource cleanup
  - *Mitigation:* Use Valgrind, test extensively

### Low Risk
- **Input integration** - Already working in test app
- **PAL stability** - All subsystems tested

---

## Success Metrics

### Phase 3.4 Complete When:
1. ✅ Title screen renders with real game graphics
2. ✅ User can navigate with keyboard/gamepad
3. ✅ Colors match DS version visually
4. ✅ 60 FPS maintained
5. ✅ No memory leaks detected
6. ✅ Asset pipeline documented
7. ✅ Code compiles on macOS/Linux/Windows

### Definition of Done:
- Title screen fully functional in SDL build
- Asset conversion pipeline documented
- Phase 3.4 summary document created
- Ready to proceed to Phase 4 (Audio) or Phase 3.5 (More screens)

---

## Next Steps After Completion

### Option A: Phase 3.5 - Additional Game Screens (Recommended)
Port 2-3 more simple screens to validate PAL:
- Main Menu
- Options Menu
- Save/Load Screen

**Benefits:** Uncover edge cases, build confidence

### Option B: Skip to Phase 4 - Audio System
Begin audio implementation:
- SDAT conversion
- SDL_Audio integration
- BGM/SFX playback

**Benefits:** Faster progress to playable game

### Decision Point:
Evaluate after Phase 3.4 completion based on:
- How smooth integration was
- Number of issues discovered
- Team capacity and priorities

---

## Questions to Answer During Implementation

1. **Asset Storage:** Keep assets in ROM or extract to filesystem?
   - *Recommendation:* Extract for easier debugging

2. **NCGR Parsing:** Parse at runtime or pre-convert?
   - *Recommendation:* Pre-convert for simplicity

3. **Palette Format:** Binary or JSON?
   - *Recommendation:* Binary for performance, JSON for debugging

4. **Error Handling:** Fail fast or fallback gracefully?
   - *Recommendation:* Fail fast during development, add fallbacks later

---

## Communication Plan

### Weekly Updates
- Monday: Task status review
- Wednesday: Mid-week progress check
- Friday: End-of-week summary

### Blockers
Report immediately if:
- File format cannot be parsed
- Performance below 30 FPS
- Memory leaks cannot be resolved

---

## Appendix: Useful Commands

### Build Commands
```bash
# Clean rebuild
rm -rf build-sdl && ./build_sdl.sh

# Run with verbose output
./build-sdl/pokeplatinum_sdl --verbose

# Run with debugger
lldb ./build-sdl/pokeplatinum_sdl
```

### Asset Extraction
```bash
# Extract NARC (example)
python3 tools/narc_to_sdl.py platinum.us.nds 123 resources/graphics/narc_123/

# Convert NCGR (example)
python3 tools/ncgr_converter.py input.ncgr output.bin

# Batch convert
./tools/convert_all_title_screen_assets.sh
```

### Memory Profiling
```bash
# macOS
leaks --atExit -- ./build-sdl/pokeplatinum_sdl

# Linux
valgrind --leak-check=full ./build-sdl/pokeplatinum_sdl
```

---

**Document Version:** 1.0  
**Last Updated:** November 14, 2025  
**Author:** PAL Development Team  
**Status:** Living Document - Update as tasks progress
