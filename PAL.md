# Platform Abstraction Layer (PAL) Documentation

**Last Updated:** November 14, 2025  
**Status:** Phase 3 Complete - Input System  

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Current Implementation Status](#current-implementation-status)
- [PAL Subsystems](#pal-subsystems)
  - [Graphics System](#graphics-system)
  - [Input System](#input-system)
  - [Audio System](#audio-system)
  - [3D Graphics System](#3d-graphics-system)
  - [Timer System](#timer-system)
- [Implementation Guidelines](#implementation-guidelines)
- [Future Plans](#future-plans)
- [Contributing to PAL](#contributing-to-pal)

---

## Overview

The **Platform Abstraction Layer (PAL)** is a clean API layer that separates game logic from platform-specific implementations, enabling Pokémon Platinum to run on both the original Nintendo DS hardware and modern platforms via SDL3.

### Design Principles

1. **Clean Separation** - Game logic never calls platform-specific APIs directly
2. **Dual-Target Support** - Single codebase builds for both DS and SDL3
3. **API Compatibility** - PAL mirrors original DS function signatures where possible
4. **Zero Game Logic Changes** - Existing game code works unchanged
5. **Incremental Migration** - Subsystems ported independently

### Architecture Layers

```
┌─────────────────────────────────────────────────────┐
│           Game Logic Layer (Unchanged)               │
│  - Battle System  - Field System  - Menu System     │
│  - Pokemon Data   - Save System   - Script Engine   │
└─────────────────────────────────┬───────────────────┘
                                  │
                    ┌─────────────▼─────────────┐
                    │  #ifdef PLATFORM_DS       │
                    │  #else PLATFORM_SDL       │
                    └─────────────┬─────────────┘
                                  │
        ┌─────────────────────────┴─────────────────────────┐
        │                                                    │
┌───────▼────────┐                              ┌───────────▼─────────┐
│   DS Target    │                              │    SDL3 Target      │
├────────────────┤                              ├─────────────────────┤
│ - NitroSDK     │                              │ - SDL3 Renderer     │
│ - NitroSystem  │                              │ - SDL3 Audio        │
│ - ARM9 HW      │                              │ - SDL3 Input        │
└────────────────┘                              │ - Cross-platform    │
                                                 └─────────────────────┘
```

---

## Current Implementation Status

### ✅ Completed Phases

| Phase | Subsystem | Status | Lines of Code | Completion Date |
|-------|-----------|--------|---------------|-----------------|
| 1.1-1.3 | Foundation | ✅ Complete | ~500 | Nov 14, 2025 |
| 2.1 | Screen Layout | ✅ Complete | ~200 | Nov 14, 2025 |
| 2.2 | Graphics Primitives | ✅ Complete | ~500 | Nov 14, 2025 |
| 2.3 | Background System | ✅ Complete | ~1,051 | Nov 14, 2025 |
| 2.4 | Sprite/OAM System | ✅ Complete | ~1,028 | Nov 14, 2025 |
| 2.5 | 3D Graphics API | ✅ Complete (Stub) | ~1,299 | Nov 14, 2025 |
| 3.1-3.4 | Input System | ✅ Complete | ~550 | Nov 14, 2025 |
| **Total** | **Phases 1-3** | **✅ Complete** | **~5,128** | **Nov 14, 2025** |

### 🚧 In Progress

None currently - Phase 3 complete.

### 📋 Planned Phases

- **Phase 4:** Audio System (BGM, sound effects, SDAT conversion)
- **Phase 5:** File System (NARC replacement, save data)
- **Phase 6:** Memory Management (heap abstraction)
- **Phase 7:** Testing & Optimization

---

## Architecture

### Directory Structure

```
include/platform/          # PAL public API headers
├── platform_config.h      # Build configuration (DS vs SDL)
├── platform_types.h       # Type definitions (BOOL, u8, etc.)
├── pal_graphics.h         # Graphics API
├── pal_background.h       # Background/tilemap API
├── pal_sprite.h           # Sprite/OAM API
├── pal_3d.h               # 3D graphics API
├── pal_input.h            # Input API
├── pal_audio.h            # Audio API (stub)
├── pal_timer.h            # Timer/timing API
└── pal_memory.h           # Memory management API (stub)

src/platform/              # PAL implementations
├── sdl/                   # SDL3 implementations
│   ├── pal_graphics_sdl.c
│   ├── pal_background_sdl.c
│   ├── pal_sprite_sdl.c
│   ├── pal_3d_sdl.c
│   ├── pal_input_sdl.c
│   ├── pal_timer_sdl.c
│   └── main_sdl.c         # SDL entry point
├── ds/                    # DS PAL wrappers (future)
│   └── (DS implementations)
└── README.md              # PAL subsystem documentation
```

### Conditional Compilation

```c
// platform_config.h
#ifdef TARGET_NDS
    #define PLATFORM_DS
    #include <nitro.h>
    #include <nnsys.h>
#else
    #define PLATFORM_SDL
    #include <SDL3/SDL.h>
#endif
```

**Usage in Code:**
```c
void MyGameFunction(void) {
    #ifdef PLATFORM_DS
        // Original DS code
        GX_SetBankForBG(GX_VRAM_BG_128_A);
    #else
        // SDL implementation
        PAL_Graphics_InitBanks();
    #endif
}
```

### Build System

**CMake Configuration:**
```cmake
# CMakeLists.txt
option(BUILD_DS_VERSION "Build for Nintendo DS" OFF)
option(BUILD_SDL_VERSION "Build SDL3 port" ON)

if(BUILD_SDL_VERSION)
    target_compile_definitions(pokeplatinum_sdl PRIVATE PLATFORM_SDL)
    target_link_libraries(pokeplatinum_sdl SDL3::SDL3)
endif()
```

**Build Commands:**
```bash
# SDL3 Build
./build_sdl.sh                    # Automated build
./build-sdl/pokeplatinum_sdl      # Run application

# DS Build (original)
make check                        # Build and verify ROM
```

---

## PAL Subsystems

### Graphics System

**Header:** `include/platform/pal_graphics.h`  
**Implementation:** `src/platform/sdl/pal_graphics_sdl.c`  
**Status:** ✅ Complete  
**Lines of Code:** ~500  

#### Core Functions

```c
// Initialization
BOOL PAL_Graphics_Init(void);
void PAL_Graphics_Shutdown(void);

// Frame rendering
void PAL_Graphics_BeginFrame(void);
void PAL_Graphics_EndFrame(void);

// Screen access
PAL_Surface* PAL_Graphics_GetScreen(PAL_Screen screen);

// Drawing primitives
void PAL_Graphics_DrawRect(PAL_Surface* surface, int x, int y, int w, int h, u32 color);
void PAL_Graphics_DrawPixel(PAL_Surface* surface, int x, int y, u32 color);
void PAL_Graphics_FillRect(PAL_Surface* surface, int x, int y, int w, int h, u32 color);
void PAL_Graphics_DrawLine(PAL_Surface* surface, int x1, int y1, int x2, int y2, u32 color);

// Texture management
SDL_Texture* PAL_Graphics_CreateTexture(int width, int height);
void PAL_Graphics_DestroyTexture(SDL_Texture* texture);
void PAL_Graphics_LoadTextureData(SDL_Texture* texture, const void* data, int pitch);

// Palette operations
void PAL_Graphics_LoadPalette(const u16* palette, int count, int offset);
u32 PAL_Graphics_RGB555ToRGB888(u16 rgb555);
```

#### Key Features

- **Dual-screen rendering** (256x192 each, side-by-side layout)
- **60 FPS target** with VSync
- **SDL_Renderer backend** for hardware acceleration
- **RGB555 palette support** (DS color format)
- **Texture caching** for performance

**Performance:** ~60 FPS stable with backgrounds + sprites

---

### Background System

**Header:** `include/platform/pal_background.h`  
**Implementation:** `src/platform/sdl/pal_background_sdl.c`  
**Status:** ✅ Complete  
**Lines of Code:** ~1,051  

#### Core Functions

```c
// Configuration
PAL_BgConfig* PAL_Bg_CreateConfig(u32 heapID);
void PAL_Bg_DestroyConfig(PAL_BgConfig* config);

// Layer initialization
void PAL_Bg_InitFromTemplate(PAL_BgConfig* config, PAL_BgLayer layer, 
                             const PAL_BgTemplate* template, PAL_BgType type);

// Tile/tilemap loading
void PAL_Bg_LoadTiles(PAL_BgConfig* config, PAL_BgLayer layer, 
                     const void* data, u32 size, u32 offset);
void PAL_Bg_LoadTilemap(PAL_BgConfig* config, PAL_BgLayer layer, 
                       const void* data, u32 size, u32 offset);

// Scrolling
void PAL_Bg_SetOffset(PAL_BgConfig* config, PAL_BgLayer layer, 
                     PAL_BgOffsetMode mode, int offset);
void PAL_Bg_GetOffset(PAL_BgConfig* config, PAL_BgLayer layer, int* x, int* y);

// Palette management
void PAL_Bg_LoadPalette(PAL_BgConfig* config, PAL_BgLayer layer, 
                       const u16* palette, u32 size, u32 offset);

// Layer control
void PAL_Bg_ToggleLayer(PAL_BgLayer layer, BOOL visible);
```

#### Key Features

- **8 background layers** (4 per screen)
- **4bpp/8bpp color modes** (16 or 256 colors)
- **Multiple screen sizes** (256x256, 512x256, 256x512, 512x512)
- **Smooth scrolling** with pixel-perfect positioning
- **Priority-based rendering** (0-3)
- **Dynamic tilemap updates**

**Performance:** ~54 FPS with 32x24 checkerboard tilemap + scrolling

---

### Sprite/OAM System

**Header:** `include/platform/pal_sprite.h`  
**Implementation:** `src/platform/sdl/pal_sprite_sdl.c`  
**Status:** ✅ Complete  
**Lines of Code:** ~1,028  

#### Core Functions

```c
// Manager
PAL_SpriteManager* PAL_Sprite_CreateManager(int max_sprites, u32 heapID);
void PAL_Sprite_DestroyManager(PAL_SpriteManager* manager);

// Sprite creation
PAL_Sprite* PAL_Sprite_Create(PAL_SpriteManager* manager, const PAL_SpriteTemplate* template);
void PAL_Sprite_Destroy(PAL_Sprite* sprite);

// Position/visibility
void PAL_Sprite_SetPosition(PAL_Sprite* sprite, int x, int y);
void PAL_Sprite_GetPosition(const PAL_Sprite* sprite, int* x, int* y);
void PAL_Sprite_SetVisible(PAL_Sprite* sprite, BOOL visible);

// Affine transformations
void PAL_Sprite_EnableAffine(PAL_Sprite* sprite, BOOL enable);
void PAL_Sprite_SetRotation(PAL_Sprite* sprite, float degrees);
void PAL_Sprite_SetScale(PAL_Sprite* sprite, float scale_x, float scale_y);

// Appearance
void PAL_Sprite_SetFlip(PAL_Sprite* sprite, PAL_SpriteFlip flip);
void PAL_Sprite_SetPriority(PAL_Sprite* sprite, u8 priority);
void PAL_Sprite_SetAlpha(PAL_Sprite* sprite, u8 alpha);

// Rendering
void PAL_Sprite_Update(PAL_SpriteManager* manager);
void PAL_Sprite_Draw(PAL_SpriteManager* manager, PAL_Screen screen);
```

#### Key Features

- **12 sprite sizes** (8x8, 16x16, 32x32, 64x64, plus rectangular)
- **4bpp/8bpp color modes**
- **Affine transformations** (rotation, scaling)
- **Priority-based rendering** (0-3, auto-sorted)
- **H/V flipping**
- **Alpha blending** and blend modes
- **Texture caching** with dirty flag optimization
- **Up to 256 sprites** (configurable)

**Performance:** ~54 FPS with 4 active sprites + rotation/scaling

---

### 3D Graphics System

**Header:** `include/platform/pal_3d.h`  
**Implementation:** `src/platform/sdl/pal_3d_sdl.c`  
**Status:** ✅ API Complete (Stub Rendering)  
**Lines of Code:** ~1,299  

#### Core Functions

```c
// Initialization
BOOL PAL_3D_Init(PAL_3D_Backend backend, u32 heapID);
void PAL_3D_Shutdown(void);

// Rendering state
void PAL_3D_BeginFrame(void);
void PAL_3D_EndFrame(void);
void PAL_3D_Clear(u8 r, u8 g, u8 b, BOOL clear_depth);

// Camera system
void PAL_3D_SetCamera(const PAL_3D_Camera* camera);
void PAL_3D_GetCamera(PAL_3D_Camera* camera);
void PAL_3D_SetPerspective(float fov, float aspect, float near, float far);

// Lighting
void PAL_3D_SetLight(int light_id, const PAL_3D_Light* light);
void PAL_3D_EnableLight(int light_id, BOOL enable);

// Material system
void PAL_3D_SetMaterial(const PAL_3D_Material* material);

// Model loading/rendering
PAL_3D_Model* PAL_3D_LoadModel(const char* path, u32 heapID);
void PAL_3D_DrawRenderObj(PAL_3D_RenderObj* obj, const PAL_Vec3* position,
                         const PAL_Matrix33* rotation, const PAL_Vec3* scale);

// Matrix operations
void PAL_3D_MatrixIdentity(PAL_Matrix44* matrix);
void PAL_3D_MatrixMultiply(PAL_Matrix44* result, const PAL_Matrix44* a, const PAL_Matrix44* b);
void PAL_3D_MatrixTranslate(PAL_Matrix44* matrix, float x, float y, float z);
// ... and more
```

#### Key Features

- **Complete 3D API** (50+ functions)
- **Camera system** (perspective/orthographic)
- **Lighting** (4 directional lights)
- **Material system** (diffuse/ambient/specular/emission)
- **Matrix operations** (all vector/matrix math)
- **Multiple backends** (software stub, OpenGL planned)

**Status:** API complete, rendering is stubbed out. Pokemon Platinum uses 3D for terrain and props, but game can run with 2D-only rendering initially.

**Future:** OpenGL ES backend recommended for hardware acceleration.

---

### Input System

**Header:** `include/platform/pal_input.h`  
**Implementation:** `src/platform/sdl/pal_input_sdl.c`  
**Status:** ✅ Complete  
**Lines of Code:** ~550  

#### Core Functions

```c
// Initialization
BOOL PAL_Input_Init(void);
void PAL_Input_Shutdown(void);
void PAL_Input_Update(void);

// Button state
u32 PAL_Input_GetHeld(void);
u32 PAL_Input_GetPressed(void);
u32 PAL_Input_GetReleased(void);

// Touch input
PAL_TouchState PAL_Input_GetTouch(void);
BOOL PAL_Input_IsTouchPressed(void);

// Gamepad management
void PAL_Input_AddGamepad(void* pad);
void PAL_Input_RemoveGamepad(void* pad);
```

#### Button Mapping

| PAL Constant | Keyboard | Xbox | PlayStation | Value |
|-------------|----------|------|-------------|-------|
| PAL_BUTTON_A | Z, Enter | A | Cross | 0x0001 |
| PAL_BUTTON_B | X, Backspace | B | Circle | 0x0002 |
| PAL_BUTTON_X | A | X | Square | 0x0004 |
| PAL_BUTTON_Y | S | Y | Triangle | 0x0008 |
| PAL_BUTTON_L | Q | LB | L1 | 0x0010 |
| PAL_BUTTON_R | W | RB | R1 | 0x0020 |
| PAL_BUTTON_START | Enter | Start | Start | 0x0040 |
| PAL_BUTTON_SELECT | Shift | Back | Select | 0x0080 |
| PAL_BUTTON_UP | ↑ | D-Pad/L-Stick | D-Pad/L-Stick | 0x0100 |
| PAL_BUTTON_DOWN | ↓ | D-Pad/L-Stick | D-Pad/L-Stick | 0x0200 |
| PAL_BUTTON_LEFT | ← | D-Pad/L-Stick | D-Pad/L-Stick | 0x0400 |
| PAL_BUTTON_RIGHT | → | D-Pad/L-Stick | D-Pad/L-Stick | 0x0800 |

#### Key Features

- **Multi-input support** (keyboard, mouse, gamepad)
- **Auto-detection** (gamepads detected on startup)
- **Hot-plug support** (connect/disconnect at runtime)
- **Up to 4 gamepads** simultaneously
- **Analog stick support** with deadzones
- **Touch cursor mode** (right stick + triggers)
- **Frame-by-frame tracking** (pressed/held/released)

**Performance:** <0.1% CPU impact per frame

---

### Timer System

**Header:** `include/platform/pal_timer.h`  
**Implementation:** `src/platform/sdl/pal_timer_sdl.c`  
**Status:** ✅ Complete  
**Lines of Code:** ~100  

#### Core Functions

```c
BOOL PAL_Timer_Init(void);
void PAL_Timer_Shutdown(void);
u64 PAL_Timer_GetTicks(void);      // Milliseconds since init
void PAL_Timer_Delay(u32 ms);      // Sleep for ms
```

**Features:** High-resolution timing, cross-platform millisecond precision.

---

## Implementation Guidelines

### Adding a New PAL Subsystem

Follow these steps when creating a new PAL subsystem:

#### 1. Create PAL Header (`include/platform/pal_*.h`)

```c
#ifndef PAL_MYSYSTEM_H
#define PAL_MYSYSTEM_H

#include "platform_config.h"
#include "platform_types.h"

// Type definitions
typedef struct PAL_MyHandle {
    void* internal;
} PAL_MyHandle;

// Function declarations
BOOL PAL_My_Init(void);
void PAL_My_Shutdown(void);
PAL_MyHandle* PAL_My_Create(void);

#endif // PAL_MYSYSTEM_H
```

**Guidelines:**
- Use `PAL_` prefix for all types and functions
- Include platform_config.h and platform_types.h
- Document parameters and return values
- Keep API platform-agnostic

#### 2. Implement SDL Version (`src/platform/sdl/pal_*_sdl.c`)

```c
#include "platform/pal_mysystem.h"

#ifdef PLATFORM_SDL

#include <SDL3/SDL.h>

typedef struct {
    // SDL-specific state
    SDL_Something* sdl_handle;
    // ... other fields
} PAL_MyState;

static PAL_MyState g_state = {0};

BOOL PAL_My_Init(void) {
    // Initialize SDL subsystem
    if (SDL_InitSubSystem(SDL_INIT_WHATEVER) < 0) {
        fprintf(stderr, "[MYSYSTEM] Failed to init: %s\n", SDL_GetError());
        return FALSE;
    }
    
    printf("[MYSYSTEM] Initialized\n");
    return TRUE;
}

void PAL_My_Shutdown(void) {
    // Cleanup
    SDL_QuitSubSystem(SDL_INIT_WHATEVER);
    printf("[MYSYSTEM] Shutdown\n");
}

#endif // PLATFORM_SDL
```

**Guidelines:**
- Wrap in `#ifdef PLATFORM_SDL`
- Use static state structs
- Add debug logging with `[SUBSYSTEM]` prefix
- Handle errors gracefully
- Clean up all resources in shutdown

#### 3. Create DS Wrapper (`src/platform/ds/pal_*_ds.c`)

```c
#include "platform/pal_mysystem.h"

#ifdef PLATFORM_DS

#include <nitro.h>

BOOL PAL_My_Init(void) {
    // Call original DS SDK functions
    DS_InitSubsystem();
    return TRUE;
}

void PAL_My_Shutdown(void) {
    DS_ShutdownSubsystem();
}

#endif // PLATFORM_DS
```

**Guidelines:**
- Mirror SDL implementation structure
- Wrap original DS API calls
- Maintain identical behavior

#### 4. Update Build System

**CMakeLists.txt:**
```cmake
add_executable(pokeplatinum_sdl
    # ... existing files ...
    src/platform/sdl/pal_mysystem_sdl.c
)
```

**Meson (DS build):**
```python
# Add to appropriate section
sources += files('src/platform/ds/pal_mysystem_ds.c')
```

#### 5. Document in Phase Summary

Create or update `PHASEX_TASKX_SUMMARY.md` with:
- Implementation details
- API reference
- Usage examples
- Performance characteristics
- Known limitations

#### 6. Update This File (PAL.md)

Add subsystem to:
- [Current Implementation Status](#current-implementation-status) table
- [PAL Subsystems](#pal-subsystems) section with full API reference

---

### Coding Standards

#### Naming Conventions

```c
// Types: PAL_ prefix, PascalCase
typedef struct PAL_MyStruct {
    int my_field;  // camelCase fields
} PAL_MyStruct;

// Enums: PAL_ prefix, PascalCase, UPPER_SNAKE_CASE members
typedef enum {
    PAL_MY_OPTION_ONE,
    PAL_MY_OPTION_TWO
} PAL_MyEnum;

// Functions: PAL_Subsystem_Action()
BOOL PAL_Graphics_Init(void);
void PAL_Sprite_SetPosition(PAL_Sprite* sprite, int x, int y);
```

#### Error Handling

```c
BOOL PAL_My_Init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "[MYSYSTEM] Init failed: %s\n", SDL_GetError());
        return FALSE;
    }
    return TRUE;
}

// Caller checks return value
if (!PAL_My_Init()) {
    // Handle failure
}
```

#### Memory Management

```c
// Use heap IDs for allocations (matches DS system)
PAL_MyStruct* PAL_My_Create(u32 heapID) {
    PAL_MyStruct* obj = Heap_Alloc(heapID, sizeof(PAL_MyStruct));
    if (!obj) return NULL;
    
    // Initialize
    memset(obj, 0, sizeof(PAL_MyStruct));
    return obj;
}

void PAL_My_Destroy(PAL_MyStruct* obj) {
    if (!obj) return;
    // Cleanup resources
    Heap_Free(obj);
}
```

#### Performance Considerations

1. **Minimize allocations** - Allocate once, reuse
2. **Cache frequently accessed data** - Avoid repeated lookups
3. **Use dirty flags** - Only update when changed
4. **Profile regularly** - Target 60 FPS minimum
5. **Optimize hot paths** - Input/rendering called every frame

---

## Future Plans

### Phase 4: Audio System (Planned)

**Goals:**
- Replace NitroSystem sound with SDL_Audio
- Convert SDAT format or implement runtime interpreter
- BGM playback, sound effects, volume control

**Key APIs:**
```c
BOOL PAL_Audio_Init(int frequency, int channels);
PAL_AudioHandle PAL_Audio_LoadMusic(const char* path);
void PAL_Audio_PlayMusic(PAL_AudioHandle music, BOOL loop);
PAL_AudioHandle PAL_Audio_LoadSound(const char* path);
void PAL_Audio_PlaySound(PAL_AudioHandle sound, int channel);
void PAL_Audio_SetVolume(PAL_AudioType type, float volume);
```

### Phase 5: File System (Planned)

**Goals:**
- Replace NitroFS with standard filesystem
- Convert NARC archives to directory structure
- Platform-specific save locations

**Key APIs:**
```c
PAL_File PAL_File_Open(const char* path, const char* mode);
size_t PAL_File_Read(void* buffer, size_t size, size_t count, PAL_File file);
PAL_Dir PAL_Dir_Open(const char* path);
const char* PAL_Path_GetSaveDirectory(void);
```

### Phase 6: Memory Management (Planned)

**Goals:**
- Abstract heap allocation system
- Remove DTCM/ITCM dependencies
- Optimize for modern RAM sizes

**Key APIs:**
```c
void* PAL_Malloc(size_t size, int heap_id);
void PAL_Free(void* ptr);
void* PAL_Realloc(void* ptr, size_t new_size);
void PAL_Memory_SetAllocator(PAL_Allocator* allocator);
```

### Phase 7: Testing & Optimization (Planned)

**Goals:**
- Ensure feature parity with DS version
- Performance profiling and optimization
- Automated testing framework
- Platform-specific bug fixes

---

## Contributing to PAL

### Before Starting

1. **Read the plan:** Review `PORTING_PLAN.md` for phase details
2. **Check status:** See which phases are complete/in-progress
3. **Review docs:** Read phase summaries (`PHASEX_TASKX_SUMMARY.md`)
4. **Understand architecture:** Study existing PAL implementations

### Workflow

1. **Analyze DS code** - Understand original implementation
2. **Design PAL API** - Create clean, platform-agnostic interface
3. **Implement SDL version** - Focus on SDL3 first
4. **Test thoroughly** - Verify functionality and performance
5. **Document** - Create phase summary and update PAL.md
6. **Update build** - Ensure both DS and SDL builds work

### Testing Checklist

- [ ] Compiles without warnings (SDL and DS builds)
- [ ] Runs at 60 FPS minimum
- [ ] Memory leaks checked (Valgrind/Instruments)
- [ ] No crashes on exit
- [ ] Hot-reload works (for input/file systems)
- [ ] Cross-platform tested (Windows/macOS/Linux)

### Documentation Requirements

Each PAL subsystem must have:

1. **API Header** - Full function signatures with doc comments
2. **Implementation** - Clean, commented SDL3 code
3. **Phase Summary** - `PHASEX_TASKX_SUMMARY.md` with:
   - Overview and objectives
   - Implementation details
   - API usage examples
   - Performance metrics
   - Known limitations
4. **PAL.md Update** - Add subsystem to this file

---

## Resources

### Essential Documentation

- **SDL3 API:** https://wiki.libsdl.org/SDL3/FrontPage
- **GBATEK (DS Hardware):** https://problemkaputt.de/gbatek.htm
- **Project Plan:** `PORTING_PLAN.md`
- **Build Instructions:** `QUICKSTART_SDL.md`

### Existing PAL Implementations

Study these for reference:
- `src/platform/sdl/pal_graphics_sdl.c` - Graphics system
- `src/platform/sdl/pal_background_sdl.c` - Background rendering
- `src/platform/sdl/pal_sprite_sdl.c` - Sprite system
- `src/platform/sdl/pal_input_sdl.c` - Input handling

### Phase Documentation

- `PHASE1_TASK1_SUMMARY.md` - Foundation & build system
- `PHASE2_TASK2_SUMMARY.md` - Graphics primitives
- `PHASE2_TASK3_SUMMARY.md` - Background system
- `PHASE2_TASK4_SUMMARY.md` - Sprite/OAM system
- `PHASE2_TASK5_SUMMARY.md` - 3D graphics API
- `PHASE3_SUMMARY.md` - Input system

---

## Frequently Asked Questions

### Q: Do I need to modify game logic code?

**A:** No! The PAL is designed so game logic remains unchanged. Only add PAL calls where platform-specific code exists, wrapped in `#ifdef PLATFORM_SDL`.

### Q: How do I test without a physical DS?

**A:** Build the SDL version (`./build_sdl.sh`) and run locally. The PAL ensures identical behavior.

### Q: Can I add platform-specific optimizations?

**A:** Yes, but keep the API consistent. Add optional features with capability checks:
```c
if (PAL_Graphics_SupportsHardwareAcceleration()) {
    // Use optimized path
}
```

### Q: What if SDL3 doesn't support a DS feature?

**A:** Implement a reasonable approximation. Document differences in phase summary. Example: DS has dual physical screens, SDL uses side-by-side layout.

### Q: How do I handle performance differences?

**A:** Target 60 FPS minimum. Modern hardware is much faster than DS, so most features will run faster. Profile and optimize if needed.

---

## Version History

- **v1.0** (Nov 14, 2025) - Initial PAL.md creation
  - Phase 1-3 complete (Foundation, Graphics, Input)
  - ~5,128 lines of PAL code
  - Full documentation of implemented subsystems

---

**Status:** ✅ **Phases 1-3 Complete**  
**Next:** Phase 4 - Audio System  
**Progress:** ~43% of porting plan complete (3 of 7 phases)
