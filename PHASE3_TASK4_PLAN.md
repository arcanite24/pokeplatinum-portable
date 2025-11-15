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

### Task 3.4.1: Core System Integration ⏳ TODO
**Priority:** CRITICAL - Must be done first  
**Estimated Time:** 3-4 days  

#### Files to Modify
- `src/system.c` - System initialization and input handling
- `src/main.c` - Main loop and frame rendering
- `src/graphics.c` - Graphics resource loading

#### Sub-Tasks
- [ ] **A. Input System Integration** (4 hours)
  - Modify `ReadKeypadAndTouchpad()` to use PAL input
  - Map PAL button states to `gSystem` structure
  - Test input with simple debug output
  - **Validation:** Arrow keys scroll background test

- [ ] **B. Graphics Initialization** (4 hours)
  - Modify `System_Init()` to initialize PAL graphics
  - Create PAL background config
  - Create PAL sprite manager
  - **Validation:** Black screens render without crash

- [ ] **C. Main Loop Integration** (6 hours)
  - Modify main loop to call PAL frame functions
  - Integrate `PAL_Graphics_BeginFrame()` / `EndFrame()`
  - Add `PAL_Bg_RenderAll()` / `PAL_Sprite_RenderAll()`
  - **Validation:** Test app still runs at 60 FPS

#### Success Criteria
✅ Game compiles with SDL build  
✅ Window opens and remains stable  
✅ Input detected from keyboard/gamepad  
✅ No crashes during initialization  

---

### Task 3.4.2: Graphics Resource Loading ⏳ TODO
**Priority:** HIGH - Required for any visuals  
**Estimated Time:** 5-6 days  

#### Files to Modify
- `src/graphics.c` - Loading functions
- `include/graphics.h` - API declarations
- `src/narc.c` - Archive replacement (optional)

#### Sub-Tasks
- [ ] **A. NARC Graphics Loading** (8 hours)
  - Implement `Graphics_LoadTilesToBgLayer()` SDL version
  - Add filesystem path mapping for NARC IDs
  - Load raw tile data from files
  - Call PAL background loading functions
  - **Validation:** Load single test tile file

- [ ] **B. Palette Loading** (4 hours)
  - Implement `Graphics_LoadPaletteToBgLayer()` SDL version
  - Parse palette files (RGB555 format)
  - Convert to PAL palette format
  - **Validation:** Load test palette, verify colors

- [ ] **C. Sprite Graphics Loading** (8 hours)
  - Implement `Graphics_LoadSpriteFromNARC()` SDL version
  - Parse NCGR format (or use pre-converted)
  - Create PAL sprite from graphics data
  - **Validation:** Load single sprite, render to screen

#### Success Criteria
✅ Tile data loads from filesystem  
✅ Palettes apply correctly (colors match DS)  
✅ No memory leaks during resource loading  

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

### Task 3.4.4: Asset Conversion Pipeline ⏳ TODO
**Priority:** HIGH - Required for real assets  
**Estimated Time:** 3-4 days  

#### New Tools to Create
1. `tools/narc_to_sdl.py` - Extract NARC archives
2. `tools/ncgr_converter.py` - Convert graphics
3. `tools/nclr_converter.py` - Convert palettes

#### Sub-Tasks
- [ ] **A. NARC Extraction Tool** (6 hours)
  - Research NARC format structure
  - Implement extraction using `ndspy` library
  - Map NARC IDs to directory structure
  - Extract all title screen assets
  - **Validation:** Extract NARC_INDEX_DEMO__TITLE__TITLEDEMO

- [ ] **B. NCGR Converter** (4 hours)
  - Parse NCGR header and tile data
  - Extract raw 4bpp/8bpp tile data
  - Save as binary files
  - **Validation:** Convert title screen tiles

- [ ] **C. NCLR Converter** (4 hours)
  - Parse NCLR palette data
  - Convert RGB555 to PAL format
  - Save as binary or JSON
  - **Validation:** Convert title screen palettes

- [ ] **D. Automation Script** (2 hours)
  - Create batch conversion script
  - Document asset pipeline
  - Add to build system (optional)

#### Success Criteria
✅ All tools working and documented  
✅ Title screen assets extracted  
✅ Assets in correct directory structure  
✅ Conversion reproducible  

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
