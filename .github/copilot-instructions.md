# GitHub Copilot Instructions for Pokémon Platinum SDL3 Port

## Project Overview
This is an **SDL3 port** of the Pokémon Platinum decompilation project. This fork focuses **exclusively on SDL3 portability** and does not maintain the original Nintendo DS build.

**Key Strategy:** We directly replace Nintendo DS-specific code with SDL3 implementations. No conditional compilation (`#ifdef PLATFORM_DS`) - this keeps the codebase clean and maintainable.

The codebase is ~99% C. Current focuses: **porting DS-specific subsystems to SDL3, implementing platform abstraction layers, and integrating game logic with PAL**.

### Porting Initiative
See [`PORTING_PLAN.md`](../PORTING_PLAN.md) for comprehensive SDL3 porting strategy. Target platforms: Windows, macOS, Linux, iOS, Android.

**Current Status:** ✅ **Phases 1-3 Complete** - Foundation, Graphics, and Input systems working.
- ~5,128 lines of PAL code across 21 files
- CMake build system operational
- Working test app at ~54 FPS with backgrounds, sprites, and full input support
- **Phase 1:** Build system, PAL infrastructure ✅
- **Phase 2:** Display system (primitives, backgrounds, sprites, 3D API) ✅
- **Phase 3:** Input system (keyboard, mouse, gamepad with hot-plug) ✅
- **Next:** Phase 4 - Audio System (BGM, sound effects)

### Key Documentation Files
**MUST READ** these files for context when working on SDL3 port:
- **[`PAL.md`](../PAL.md)** - Complete Platform Abstraction Layer documentation, architecture, implementation guidelines, and current status
- **[`PORTING_PLAN.md`](../PORTING_PLAN.md)** - Comprehensive porting strategy with phase-by-phase implementation plan

**When implementing new PAL features:**
1. Reference `PAL.md` for architecture and guidelines
2. Follow existing PAL patterns from completed phases
4. Update `PAL.md` with new subsystem details
5. Update `PORTING_PLAN.md` to mark phase/task complete

## Critical Build Commands

### SDL3 Build (This Repository)
```bash
./build_sdl.sh                    # Automated build script (requires cmake + sdl3)
./build-sdl/pokeplatinum_sdl      # Run application
cmake --build build-sdl           # Manual build after initial setup
```

Don't use head or tail commands when getting the build output since you might miss some important messages.

### Code Formatting
```bash
make format         # Run clang-format (IMPORTANT - maintain code style)
```

## Architecture: SDL3-Only System

### Build Target
This repository builds **only the SDL3 port**. The original Nintendo DS build is not supported - for that, see the [upstream pret/pokeplatinum repository](https://github.com/pret/pokeplatinum).

### Porting Strategy: Stub-First Approach

**Step 1: Preserve Knowledge**
- Document original DS implementation in `docs/DS_*_REFERENCE.md` before making changes
- Capture function signatures, key patterns, and implementation details

**Step 2: Create SDL3 Stubs**
- Replace DS functions with SDL3 stubs that match the original API
- Use `printf("[Subsystem] TODO: Port FunctionName")` for unimplemented features
- Add `(void)param;` casts to avoid unused parameter warnings

**Step 3: Port Incrementally**
- Implement stubs as needed during testing and integration
- Test each function individually before moving to the next

**Example - Stub-first approach:**
```c
// Step 1: Document in docs/DS_GRAPHICS_REFERENCE.md
// Original DS function loaded sprite tiles from NARC to VRAM

// Step 2: Create stub with same signature
u32 Graphics_LoadObjectTiles(enum NarcID narcID, u32 narcMemberIdx, 
                             enum DSScreen display, u32 offset, u32 size, 
                             BOOL compressed, u32 heapID)
{
    (void)narcID; (void)narcMemberIdx; (void)display; (void)offset;
    (void)size; (void)compressed; (void)heapID;
    
    printf("[Graphics] TODO: Port Graphics_LoadObjectTiles (NARC %d, member %d)\n", 
           narcID, narcMemberIdx);
    return 0; // Return 0 tiles loaded for now
}

// Step 3: Port when needed - replace stub with real implementation
// Load from filesystem, convert to SDL texture, update sprite system
```

**Benefits:**
- ✅ Code compiles immediately - no broken builds
- ✅ Game runs (even if features are incomplete)
- ✅ Clear TODO markers show porting progress
- ✅ Can test incrementally without implementing everything at once

### Platform Abstraction Layer (PAL)
When porting DS code, use PAL APIs directly without conditional compilation:

```c
// pal_graphics.h - Graphics abstraction
typedef enum {
    PAL_SCREEN_MAIN,
    PAL_SCREEN_SUB
} PAL_Screen;

PAL_Surface* PAL_Graphics_GetScreen(PAL_Screen screen);
void PAL_Graphics_DrawSprite(PAL_Surface* surf, const void* data, 
                             int x, int y, int w, int h);

// pal_input.h - Input abstraction (replaces PAD_Read)
uint32_t PAL_Input_GetHeld(void);
uint32_t PAL_Input_GetPressed(void);
PAL_TouchState PAL_Input_GetTouch(void);

// pal_audio.h - Audio abstraction
PAL_AudioHandle PAL_Audio_LoadMusic(const char* path);
void PAL_Audio_PlayMusic(PAL_AudioHandle music, bool loop);

// pal_file.h - File I/O abstraction (replaces NARC)
PAL_File PAL_File_Open(const char* path, const char* mode);
size_t PAL_File_Read(void* buffer, size_t size, size_t count, PAL_File file);
```

**Porting Guidelines:**
- **Port, don't delete:** Create SDL3 stubs for all DS functions before removing any code
- Document DS implementations in `docs/DS_*_REFERENCE.md` before modification
- Replace DS-specific function calls with PAL equivalents
- Keep game logic structure unchanged
- Maintain identical behavior to DS version
- See [`PORTING_PLAN.md`](../PORTING_PLAN.md) for detailed implementation phases
- See [`PAL.md`](../PAL.md) for complete PAL API reference

**SDL3 Port File Structure:**
```
include/platform/         # PAL public headers (platform_config.h, pal_*.h)
src/platform/sdl/         # SDL3 implementations (pal_*_sdl.c, main_sdl.c)
CMakeLists.txt            # SDL3 build configuration
build_sdl.sh              # Automated SDL build script
build-sdl/                # SDL3 build output (gitignored)
resources/                # Extracted game assets (gitignored - user must provide ROM)
tools/                    # Asset extraction and conversion scripts
```

### Resource Management Strategy
(Refer to RESOURCES.md for more info on this topic)

**CRITICAL:** SDL3 build uses a clean asset extraction approach:

**Asset Directory Structure:**
- **`resources/`** (gitignored) - Runtime assets for SDL3 build
  - `resources/graphics/` - Extracted graphics (tiles, palettes, tilemaps)
  - `resources/audio/` - Converted audio files
  - `resources/text/` - Extracted text/message banks
  - `resources/data/` - Other game data
  - Generated by `tools/convert_assets.sh` from user's legal ROM
  - **Required for SDL3 build to run**

- **`res/`** (reference only) - Original DS build assets
  - Used by Meson/DS build system
  - Reference for implementation patterns only
  - **SDL3 code must NOT depend on `/res/`**

- **`build/`** (DS build output only)
  - Generated by Meson for DS target
  - **SDL3 code must NOT reference `/build/` directory**
  - SDL3 uses `build-sdl/` instead

**Asset Loading Pattern (SDL3):**
```c
// CORRECT - Reference resources/ directory
GetAssetPath(narcID, memberIdx, "tiles.bin", path, sizeof(path));
// Generates: "resources/graphics/title_screen/logo_tiles.bin"

PAL_File file = PAL_File_Open(path, "rb");
// Loads from extracted assets

// WRONG - Don't reference res/ or build/
// BAD: "res/graphics/..."  ← Original DS assets
// BAD: "build/res/..."     ← Meson build output
```

**Asset Extraction Tools:**
- `tools/convert_assets.sh` - Main extraction script
- `tools/narc_extractor.py` - Extract NARC archives
- `tools/ncgr_converter.py` - Convert NCGR tiles to raw binary
- `tools/nclr_converter.py` - Convert NCLR palettes to RGB555
- `tools/convert_title_screen.py` - Example: title screen specific extraction

**Scalability:** As more systems are ported, add corresponding extraction tools to `tools/` and extract to appropriate `resources/` subdirectories. This keeps the codebase clean and separates code from assets.

### Overlay System
Nintendo DS uses **dynamic code loading** via overlays. Understanding this is essential:

```c
// Declare overlay externally
FS_EXTERN_OVERLAY(overlay82);

// Load overlay dynamically
ApplicationManagerTemplate tmpl = {
    .init = InitFunc,
    .main = MainFunc, 
    .exit = ExitFunc,
    FS_OVERLAY_ID(overlay82)  // Links to src/overlay082/
};
```

- **Main binary**: `src/main.c` + non-overlay files
- **Overlays**: `src/overlay000/` through `src/overlay119/` - loaded/unloaded dynamically
- **Applications**: `src/applications/` (title screen, menus, pokédex, etc.)
- Each overlay has matching `include/overlay0XX/` headers

## Memory Management: Heap IDs
All allocations MUST specify a heap ID from `include/constants/heap.h`:
```c
void* ptr = Heap_Alloc(HEAP_ID_FIELD1, 1024);  // ALWAYS use heap constant
Heap_Free(ptr);
// Never use malloc/free directly
```

Common heaps: `HEAP_ID_SYSTEM`, `HEAP_ID_APPLICATION`, `HEAP_ID_BATTLE`, `HEAP_ID_FIELD1/2`

## File System: NARC Archives
ROM data is stored in NARC (Nitro ARChive) files, not regular filesystem:
```c
NARC* archive = NARC_ctor(NARC_INDEX_GRAPHIC__BATTLE, HEAP_ID_BATTLE);
void* data = NARC_AllocAndReadWholeMember(archive, fileIdx, heapID);
NARC_dtor(archive);
```

See `docs/maps/README.md` for map system specifics (BDHC, NSBMD 3D models, etc.)

## Naming Conventions (STRICTLY ENFORCED)
```c
// Structures: PascalCase with typedef AND tag
typedef struct MyStruct {
    u32 myField;        // camelCase members
} MyStruct;

// Enums: PascalCase, NO typedef
enum MyEnum {
    MY_ENUM_MEMBER = 0,  // UPPER_SNAKE_CASE, explicit first value
    MY_NEXT_MEMBER       // no explicit value for subsequent
};

// Functions: PascalCase
int MyFunction(int myParameter) {
    int myLocalVariable;  // camelCase locals
}

// Unknown functions/structs: Use address-based names
static void sub_0200679C(void);              // Unknown function
typedef struct UnkStruct_ov82_0223B164 {     // Unknown struct
    u32 unk_00;  // Unknown field at offset 0x00
    u16 unk_04;  // Unknown field at offset 0x04
} UnkStruct_ov82_0223B164;
```

**Critical**: Many files have `unk_` prefixes indicating reverse-engineering WIP. When documenting, replace with meaningful names.

## Code Formatting
- **Pre-commit hooks**: Install `pre-commit` to auto-format on commit
- **Manual format**: `make format` before every PR
- **CI check**: `.github/workflows/pr-lint.yml` enforces clang-format version
- **Local exceptions**: Use `// clang-format off` ... `// clang-format on` for data tables only

## Hardware Abstraction Patterns
Nintendo DS-specific code uses NitroSDK/NitroSystem wrappers:

### Graphics (2D/3D)
```c
#include <nitro.h>
#include <nnsys.h>

// 2D: BG layers, sprites (OAM)
BgConfig* bgConfig = BgConfig_New(heapID);
Sprite* sprite = Sprite_CreateAffine(...);

// 3D: G3D rendering
NNS_G3dDraw(renderObj);
Camera_ComputeViewMatrix();
```

### Input
```c
u32 pressed = gSystem.pressedKeys;      // This frame
u32 held = gSystem.heldKeys;            // Continuous
if (gSystem.touchPressed) {
    int x = gSystem.touchX, y = gSystem.touchY;
}
```

### Audio
```c
Sound_PlayBGM(SEQ_TOWN01);              // Background music
Sound_PlayEffect(SEQ_SE_DP_SELECT);     // Sound effect
```

## Common Patterns

### Application Manager (Screen Flow)
```c
typedef struct MyAppData {
    BgConfig* bgConfig;
    // app state
} MyAppData;

static BOOL MyApp_Init(ApplicationManager* appMan, int* state) {
    MyAppData* data = ApplicationManager_NewData(appMan, sizeof(MyAppData), heapID);
    // initialize
    return TRUE;  // or FALSE if multi-frame init
}

static BOOL MyApp_Main(ApplicationManager* appMan, int* state) {
    MyAppData* data = ApplicationManager_Data(appMan);
    // main loop logic, return TRUE when done
}

static BOOL MyApp_Exit(ApplicationManager* appMan, int* state) {
    // cleanup
    ApplicationManager_FreeData(appMan);
    return TRUE;
}
```

### State Machines
Switch-case pattern with state variable:
```c
switch (appData->state) {
case 0:
    // initialization
    appData->state++;
    break;
case 1:
    // main processing
    if (condition) appData->state++;
    break;
// ...
}
```

## Documentation Priority
When reverse engineering, document in this order:
1. **Function purpose** - What does it do?
2. **Parameters** - Meaningful names if discoverable
3. **Return values** - What do they mean?
4. **Side effects** - Heap allocations, global state changes
5. **Call context** - When/why is this called?

Example:
```c
// Initializes the battle system for a wild encounter
// Returns: TRUE if initialization complete, FALSE if multi-frame
// Allocates: HEAP_ID_BATTLE for battle data structures
BOOL Battle_InitWildEncounter(BattleSystem* battleSys, Pokemon* wildMon) {
```

## Testing Requirements
- **ROM matching**: `make check` must pass - no binary differences
- **No regressions**: If you can't verify matching, mark as WIP
- **Split work**: Large changes should be broken into atomic commits

## Common Pitfalls
1. **Wrong heap ID**: Always use correct heap constant for context
2. **File I/O**: Use PAL_File_* functions instead of NARC on SDL
3. **Overlay dependencies**: Understand original DS overlay structure
4. **Pointer arithmetic**: Original DS is 32-bit ARM, be careful with sizes
5. **Endianness**: Data is little-endian, watch for serialization
6. **Platform-specific code**: Replace DS calls directly with PAL, don't use #ifdef

## Contributing to SDL3 Port
When working on portability features:

### Documentation Requirements
**CRITICAL:** When implementing new PAL features or working on phases, you MUST:

1. **Read existing documentation first:**
   - [`PAL.md`](../PAL.md) - Architecture, implementation guidelines, current status
   - [`PORTING_PLAN.md`](../PORTING_PLAN.md) - Phase details and requirements
   - Existing `PHASEX_TASKX_SUMMARY.md` files for implementation patterns

2. **Create phase summary documentation:**
   - Create `PHASEX_TASKX_SUMMARY.md` when completing a phase/task
   - Include: Overview, objectives, implementation details, API reference, usage examples, performance metrics, known limitations
   - See existing phase summaries as templates

3. **Update central documentation:**
   - Update `PAL.md` with new subsystem details (add to status table and subsystem section)
   - Update `PORTING_PLAN.md` to mark completed phases/tasks
   - Keep status tables current with completion dates

4. **Keep documentation synchronized:**
   - When you modify a PAL subsystem, update its section in `PAL.md`
   - When you complete a phase task, immediately document it
   - Documentation is NOT optional - it's required for maintainability

**Documentation Pattern:**
```
New Feature Implementation:
1. Review PAL.md for guidelines
2. Implement feature following patterns
3. Create PHASEX_TASKX_SUMMARY.md
4. Update PAL.md subsystem section
5. Update PORTING_PLAN.md status
6. Commit documentation WITH code
```

### Adding PAL Abstractions
1. **Create PAL interface** in `include/platform/pal_*.h` with clean API
2. **Implement SDL version** in `src/platform/sdl/pal_*_sdl.c` using SDL3 APIs
3. **Update game code** to use PAL instead of direct DS calls

Example workflow:
```c
// Step 1: Create pal_input.h
typedef enum {
    PAL_BUTTON_A = 0x0001,
    PAL_BUTTON_B = 0x0002,
    // ...
} PAL_ButtonMask;

uint32_t PAL_Input_GetHeld(void);

// Step 2: Implement SDL version
uint32_t PAL_Input_GetHeld(void) {
    // SDL_Keyboard + SDL_Gamepad logic
    return button_state;
}

// Step 3: Update game code
// OLD: u32 keys = PAD_Read();
// NEW: u32 keys = PAL_Input_GetHeld();
```

### Porting Priorities
Based on [`PORTING_PLAN.md`](../PORTING_PLAN.md), prioritize in this order:
1. **Graphics** - Display system, sprites, backgrounds (Phase 2)
2. **Input** - Buttons, touch screen (Phase 3)
3. **Audio** - BGM, sound effects (Phase 4)
4. **File I/O** - Replace NARC with filesystem (Phase 5)
5. **Memory** - Abstract heap allocations (Phase 6)

### Testing Ported Code
- **Build SDL version**: Verify game functions correctly
- **Feature parity**: SDL version must behave identically to DS version
- **Cross-platform**: Test on Windows, macOS, Linux if possible

## Generated Files
Do NOT edit these directly (auto-generated at build time):
- `generated/` - Enums, message banks, NARC indices
- `build/` - Build artifacts
- `.clangd/` - LSP cache

## Quick Reference
- **First PR?** Read `CONTRIBUTING.md` section "Your First Contribution"
- **Setup issues?** Check `INSTALL.md` platform-specific instructions
- **Map system?** See `docs/maps/` for terrain, props, collision
- **Need context?** Search for usage: `grep -r "FunctionName" src/`
- **Ask questions**: pret Discord or VoidMatrix #decomp

## Resource Locations
```
res/graphics/     # Sprites, backgrounds, UI assets
res/text/         # Message banks (text strings)
res/sound/        # SDAT audio archives
include/constants/    # Enum definitions (species, items, moves, etc.)
include/struct_defs/  # Data structure definitions
```
