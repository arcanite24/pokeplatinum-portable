# GitHub Copilot Instructions for Pokémon Platinum Decompilation

## Project Overview
This is a **WIP decompilation** of Pokémon Platinum (Nintendo DS) with **dual objectives**:
1. **Byte-perfect ROM matching** - Maintain original DS build compatibility
2. **Cross-platform portability** - Port to modern platforms via SDL3 abstraction layer

The codebase is ~99% C with remaining ARM assembly stubs. Current focuses: **documentation, reverse engineering obfuscated code, and implementing platform abstraction layers** for portability.

### Porting Initiative
See [`PORTING_PLAN.md`](../PORTING_PLAN.md) for comprehensive SDL3 porting strategy. Target platforms: Windows, macOS, Linux, iOS, Android.

## Critical Build Commands
```bash
make check          # Build and verify ROM SHA-1 match (REQUIRED before PRs)
make format         # Run clang-format (REQUIRED - CI enforced)
make release        # Default build target
ROM_REVISION=0      # Set to build Rev 0 instead of Rev 1
```

## Architecture: Dual-Target System

### Build Targets
The project supports **two build targets**:
1. **Nintendo DS (Original)** - Byte-perfect ROM matching using NitroSDK
2. **SDL3 Port (Modern)** - Cross-platform build using Platform Abstraction Layer

Use conditional compilation to maintain both targets:
```c
#ifdef PLATFORM_DS
    // Original DS-specific code
#else
    // SDL3 portable implementation
#endif
```

### Platform Abstraction Layer (PAL)
When porting code, create clean abstractions in `src/platform/`:

```c
// pal_graphics.h - Graphics abstraction
typedef enum {
    PAL_SCREEN_MAIN,
    PAL_SCREEN_SUB
} PAL_Screen;

PAL_Surface* PAL_Graphics_GetScreen(PAL_Screen screen);
void PAL_Graphics_DrawSprite(PAL_Surface* surf, const void* data, 
                             int x, int y, int w, int h);

// pal_input.h - Input abstraction
uint32_t PAL_Input_GetHeld(void);     // Mirrors PAD_Read()
uint32_t PAL_Input_GetPressed(void);
PAL_TouchState PAL_Input_GetTouch(void);

// pal_audio.h - Audio abstraction
PAL_AudioHandle PAL_Audio_LoadMusic(const char* path);
void PAL_Audio_PlayMusic(PAL_AudioHandle music, bool loop);

// pal_file.h - File I/O abstraction (replaces NARC on SDL)
PAL_File PAL_File_Open(const char* path, const char* mode);
size_t PAL_File_Read(void* buffer, size_t size, size_t count, PAL_File file);
```

**Porting Guidelines:**
- Keep game logic unchanged - only wrap platform-specific calls
- Maintain identical behavior between DS and SDL builds
- Test both builds to ensure feature parity
- See [`PORTING_PLAN.md`](../PORTING_PLAN.md) for detailed implementation phases

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
2. **Missing NARC cleanup**: Always `NARC_dtor()` after use
3. **Overlay dependencies**: Check `FS_EXTERN_OVERLAY` declarations
4. **Pointer arithmetic**: DS is 32-bit ARM, be careful with sizes
5. **Endianness**: Data is little-endian, but watch for serialization
6. **Platform-specific code**: Wrap DS-specific calls with `#ifdef PLATFORM_DS` when porting

## Contributing to SDL3 Port
When working on portability features:

### Adding PAL Abstractions
1. **Create PAL interface** in `include/platform/pal_*.h` with clean API
2. **Implement DS version** in `src/platform/ds/pal_*_ds.c` wrapping existing code
3. **Implement SDL version** in `src/platform/sdl/pal_*_sdl.c` using SDL3 APIs
4. **Update game code** to use PAL instead of direct DS calls

Example workflow:
```c
// Step 1: Create pal_input.h
typedef enum {
    PAL_BUTTON_A = 0x0001,
    PAL_BUTTON_B = 0x0002,
    // ...
} PAL_ButtonMask;

uint32_t PAL_Input_GetHeld(void);

// Step 2: Wrap DS implementation
#ifdef PLATFORM_DS
uint32_t PAL_Input_GetHeld(void) {
    return PAD_Read();
}
#endif

// Step 3: Implement SDL version
#ifdef PLATFORM_SDL
uint32_t PAL_Input_GetHeld(void) {
    // SDL_Keyboard + SDL_Gamepad logic
    return button_state;
}
#endif

// Step 4: Update game code
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
- **Build both targets**: Verify DS build still matches ROM SHA-1
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
