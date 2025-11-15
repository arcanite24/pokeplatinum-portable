# Pokémon Platinum SDL3 Porting Plan

## Executive Summary

This document outlines a comprehensive plan to port the Pokémon Platinum decompilation project from Nintendo DS-specific hardware to a cross-platform architecture using SDL3. The goal is to create a portable, maintainable codebase that can run on modern platforms (Windows, macOS, Linux, iOS, Android) while preserving the original game's functionality and feel.

## Table of Contents

- [1. Project Analysis](#1-project-analysis)
- [2. Architecture Overview](#2-architecture-overview)
- [3. Phase-by-Phase Porting Strategy](#3-phase-by-phase-porting-strategy)
- [4. Technical Challenges & Solutions](#4-technical-challenges--solutions)
- [5. Implementation Details](#5-implementation-details)
- [6. Testing Strategy](#6-testing-strategy)
- [7. Build System & Tooling](#7-build-system--tooling)
- [8. Resource Recommendations](#8-resource-recommendations)

---

## 1. Project Analysis

### Current State

The project is a **WIP decompilation** of Pokémon Platinum for Nintendo DS, written primarily in **C** with some ARM assembly. Key characteristics:

- **Language:** ~99% C (C99), ~1% ARM assembly
- **Build System:** Meson + Ninja
- **Target Platform:** Nintendo DS hardware
- **Dependencies:**
  - NitroSDK (Nintendo's DS SDK)
  - NitroSystem (graphics/sound libraries)
  - NitroWiFi (networking)
  - NitroDWC (wireless communication)
  - Custom libraries (spl, gds, crypto)

### Hardware Dependencies Identified

#### 1. **Display System**
- **Dual screens:** Main (256x192) and Sub (256x192)
- **2D rendering:** Backgrounds, sprites, OAM (Object Attribute Memory)
- **3D rendering:** Single hardware 3D engine (can be assigned to either screen)
- **VRAM management:** Bank switching for textures/palettes

#### 2. **Input System**
- **Physical buttons:** D-Pad, A, B, X, Y, L, R, Start, Select
- **Touch screen:** Resistive touchscreen on bottom screen
- **Lid sensor:** Fold detection

#### 3. **Audio System**
- **Sound engine:** NNS_Snd (NitroSystem Sound)
- **Formats:** SDAT (Sound Data), SSEQ (sequences), SBNK (banks), SWAR (waves)
- **Channels:** 16 hardware channels
- **Microphone:** Built-in mic for Chatot cry recording

#### 4. **File System**
- **ROM format:** NDS (Nintendo DS ROM)
- **File archive:** NARC (Nitro Archive)
- **Filesystem:** NitroFS overlay system

#### 5. **Memory Architecture**
- **ARM9 main processor:** 4 MB RAM
- **VRAM:** 656 KB shared between engines
- **Heap management:** Custom allocators with canaries
- **Overlays:** Dynamic code loading system

---

## 2. Architecture Overview

### Proposed Abstraction Layers

```
┌─────────────────────────────────────────────────────┐
│           Game Logic Layer (Minimal Changes)         │
│  - Battle System  - Field System  - Menu System     │
│  - Pokemon Data   - Save System   - Script Engine   │
└─────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────┐
│          Platform Abstraction Layer (PAL)            │
│  - Graphics API  - Input API   - Audio API          │
│  - File API      - Timer API   - Memory API         │
└─────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────┐
│              SDL3 Implementation Layer               │
│  - SDL_Renderer/GPU  - SDL_Gamepad  - SDL_Audio     │
│  - SDL_IOStream      - SDL_Timer    - Memory mgmt   │
└─────────────────────────────────────────────────────┘
```

### Key Design Principles

1. **Minimal Game Logic Changes:** Preserve original code structure where possible
2. **Clean Abstraction:** Create well-defined interfaces between layers
3. **Optional Native Features:** Allow platform-specific optimizations
4. **Backward Compatibility:** Maintain ability to build original DS version
5. **Incremental Migration:** Port subsystems independently

---

## 3. Phase-by-Phase Porting Strategy

### Phase 1: Foundation & Build System (Weeks 1-3) ✅ COMPLETED

**Status:** ✅ **COMPLETE** - All tasks finished, test application running successfully

**Goals:**
- ✅ Set up cross-platform build system
- ✅ Create basic PAL structure
- ✅ Establish testing framework

**Completion Date:** November 14, 2025

**Tasks:**

#### 1.1 Build System Migration ✅ COMPLETED
```bash
# Create new CMake or Meson configuration
- Dual-target support (DS original + SDL port)
- Conditional compilation flags
- Cross-platform toolchain files
```

**Files to Create:**
- `CMakeLists.txt` or `meson_sdl.build`
- `include/platform/platform_config.h`
- `include/platform/platform_types.h`

**Example:**
```c
// platform_config.h
#ifdef TARGET_NDS
    #define PLATFORM_DS
    #include <nitro.h>
#else
    #define PLATFORM_SDL
    #include <SDL3/SDL.h>
#endif
```

#### 1.2 Create PAL Structure
```
src/platform/
├── pal_graphics.h/c    # Graphics abstraction
├── pal_input.h/c       # Input abstraction
├── pal_audio.h/c       # Audio abstraction
├── pal_file.h/c        # File I/O abstraction
├── pal_timer.h/c       # Timing abstraction
└── pal_memory.h/c      # Memory management abstraction
```

#### 1.3 Basic SDL Initialization ✅ COMPLETED
```c
// Create minimal SDL window and renderer
- Initialize SDL3 video, audio, events
- Create dual-screen layout (side-by-side or vertical)
- Basic event loop
```

**Deliverable:** ✅ Project compiles with SDL3, shows empty windows

**Achievement Summary:**
- Created 21 new files (headers, implementations, build system, docs)
- Working test application running at ~60 FPS
- Input system detecting keyboard and mouse
- Clean initialization and shutdown of all PAL subsystems
- Documentation: `PHASE1_TASK1_SUMMARY.md`, `QUICKSTART_SDL.md`, `src/platform/README.md`

**Next Steps:** Proceed to Phase 2 (Display System) to implement actual graphics rendering.

---

### Phase 2: Display System (Weeks 4-8) 🚧 IN PROGRESS

**Goals:**
- Implement 2D rendering abstraction
- Port sprite and background systems
- Handle dual-screen layout

#### 2.1 Screen Layout Design

**Option A: Side-by-Side**
```
┌───────────┬───────────┐
│           │           │
│   Main    │    Sub    │
│  Screen   │   Screen  │
│ (256x192) │ (256x192) │
└───────────┴───────────┘
  Total: 512x192
```

**Option B: Vertical Stack**
```
┌───────────┐
│   Main    │
│  Screen   │
│ (256x192) │
├───────────┤
│    Sub    │
│  Screen   │
│ (256x192) │
└───────────┘
  Total: 256x384
```

**Option C: Switchable**
```
Press Tab to toggle between screens in fullscreen mode
```

#### 2.2 Graphics PAL Implementation ✅ COMPLETED

**Status:** ✅ **COMPLETE** - Graphics rendering system working with primitives, textures, and palettes

**Completion Date:** November 14, 2025

**Key Abstractions:**

```c
// pal_graphics.h
typedef enum {
    PAL_SCREEN_MAIN,
    PAL_SCREEN_SUB
} PAL_Screen;

#### 2.3 Background System Port

**Current NDS System:**
```c
// From bg_window.h/c
BgConfig_New()
Bg_InitFromTemplate()
Bg_ScheduleTilemapTransfer()
```

**PAL Wrapper:**
```c
// pal_background.c
typedef struct PAL_BgConfig {
    PAL_Surface* surfaces[8]; // 4 per screen
    void* tilemap_data[8];
    void* tile_data[8];
    int priority[8];
} PAL_BgConfig;

PAL_BgConfig* PAL_Bg_Create(void) {
    #ifdef PLATFORM_DS
        return BgConfig_New();
    #else
        // SDL implementation
        PAL_BgConfig* cfg = malloc(sizeof(PAL_BgConfig));
        // Initialize surfaces
        return cfg;
    #endif
}
```

#### 2.4 Sprite/OAM System Port

**Current System:**
- Hardware OAM (Object Attribute Memory)
- 128 sprites per screen
- Hardware affine transformations

**PAL Implementation:**
```c
// pal_sprite.h
typedef struct PAL_Sprite {
    int x, y;
    int width, height;
    void* graphics_data;
    void* palette_data;
    int priority;
    float rotation;
    float scale_x, scale_y;
    bool visible;
    PAL_Screen screen;
} PAL_Sprite;

typedef struct PAL_SpriteManager {
    PAL_Sprite sprites[256]; // 128 per screen
    int sprite_count;
} PAL_SpriteManager;

// Use SDL_Renderer or custom software rendering
void PAL_Sprite_Draw(PAL_Sprite* sprite, PAL_Surface* surface);
```

#### 2.5 3D Graphics Abstraction

**Challenge:** DS has single hardware 3D engine; need modern equivalent

**Options:**

**A. Software 3D Rendering (Easier)**
- Use existing G3D code from NitroSystem
- Render to texture, blit to SDL surface
- Limited performance

**B. OpenGL/Vulkan Backend (Better)**
- Translate G3D calls to OpenGL ES
- Hardware acceleration
- More complex

**Recommended: Hybrid Approach**
```c
// pal_3d.h
typedef enum {
    PAL_3D_BACKEND_SOFTWARE,
    PAL_3D_BACKEND_OPENGL,
    PAL_3D_BACKEND_VULKAN
} PAL_3D_Backend;

bool PAL_3D_Init(PAL_3D_Backend backend);
void PAL_3D_SetScreen(PAL_Screen screen);

// Translate NNS_G3d calls
void PAL_3D_Begin(void);
void PAL_3D_End(void);
void PAL_3D_LoadModel(const char* path);
void PAL_3D_DrawModel(int model_id, const VecFx32* pos, const MtxFx33* rot);
```

**Implementation Strategy:**
1. Start with software rendering
2. Profile bottlenecks
3. Gradually add GPU acceleration where needed

---

### Phase 3: Input System (Weeks 9-11)

**Goals:**
- Map DS controls to modern input devices
- Implement touch screen emulation
- Support gamepads

#### 3.1 Input Mapping Design

**DS Controls → Modern Input:**
```
DS Button        | Keyboard      | Gamepad (SDL)
-------------------------------------------------
D-Pad           | Arrow Keys    | D-Pad
A Button        | Z / Enter     | A (South)
B Button        | X / Backspace | B (East)
X Button        | A             | X (West)
Y Button        | S             | Y (North)
L Button        | Q             | L1
R Button        | W             | R1
Start           | Enter         | Start
Select          | Shift         | Select/Back
Touch Screen    | Mouse         | Right Stick (cursor mode)
Lid Close       | F1            | (N/A)
```

#### 3.2 Input PAL Implementation

```c
// pal_input.h
typedef enum {
    PAL_BUTTON_A = 0x0001,
    PAL_BUTTON_B = 0x0002,
    PAL_BUTTON_X = 0x0004,
    PAL_BUTTON_Y = 0x0008,
    PAL_BUTTON_L = 0x0010,
    PAL_BUTTON_R = 0x0020,
    PAL_BUTTON_START = 0x0040,
    PAL_BUTTON_SELECT = 0x0080,
    PAL_BUTTON_UP = 0x0100,
    PAL_BUTTON_DOWN = 0x0200,
    PAL_BUTTON_LEFT = 0x0400,
    PAL_BUTTON_RIGHT = 0x0800,
} PAL_ButtonMask;

typedef struct {
    int x, y;
    bool pressed;
    bool held;
    bool released;
} PAL_TouchState;

// Initialize input
bool PAL_Input_Init(void);
void PAL_Input_Update(void);

// Button state queries (mirrors PAD_Read())
uint32_t PAL_Input_GetHeld(void);
uint32_t PAL_Input_GetPressed(void);
uint32_t PAL_Input_GetReleased(void);

// Touch input (mirrors TP_* functions)
PAL_TouchState PAL_Input_GetTouch(void);
bool PAL_Input_IsTouchPressed(void);

// Gamepad support
void PAL_Input_AddGamepad(SDL_Gamepad* pad);
void PAL_Input_RemoveGamepad(SDL_Gamepad* pad);
```

#### 3.3 Touch Screen Emulation

**Modes:**

1. **Mouse Mode:** Click on sub-screen area
2. **Cursor Mode:** Right stick controls cursor
3. **Hybrid:** Mouse + gamepad cursor

```c
// pal_touch.c
void PAL_Touch_Update(void) {
    #ifdef PLATFORM_SDL
        SDL_MouseState mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);
        
        // Map mouse coords to sub-screen coordinates
        if (mouse.x >= SUB_SCREEN_X && 
            mouse.x < SUB_SCREEN_X + 256 &&
            mouse.y >= SUB_SCREEN_Y && 
            mouse.y < SUB_SCREEN_Y + 192) {
            
            touch_state.x = mouse.x - SUB_SCREEN_X;
            touch_state.y = mouse.y - SUB_SCREEN_Y;
            touch_state.pressed = SDL_GetMouseButtons() & SDL_BUTTON_LMASK;
        }
    #else
        // Original DS touch code
        TP_RequestRawSampling(&tpRaw);
        TP_GetCalibratedPoint(&tp, &tpRaw);
    #endif
}
```

#### 3.4 System Integration

**Replace in existing code:**
```c
// src/system.c - ReadKeypadAndTouchpad()

void ReadKeypadAndTouchpad(void) {
    #ifdef PLATFORM_DS
        TPData tpRaw;
        TPData tp;
        u32 padRead = PAD_Read();
        // ... original code ...
    #else
        PAL_Input_Update();
        gSystem.heldKeysRaw = PAL_Input_GetHeld();
        gSystem.pressedKeysRaw = PAL_Input_GetPressed();
        
        PAL_TouchState touch = PAL_Input_GetTouch();
        if (touch.pressed) {
            gSystem.touchX = touch.x;
            gSystem.touchY = touch.y;
            gSystem.touchPressed = true;
        }
    #endif
}
```

---

### Phase 4: Audio System (Weeks 12-16)

**Goals:**
- Replace NitroSystem sound with SDL_Audio
- Convert SDAT format to modern audio
- Implement music and sound effects

#### 4.1 Audio Architecture

**Current System:**
```
SDAT (Sound Data Archive)
├── SSEQ (Sequences/MIDI)
├── SBNK (Sound Banks)
├── SWAR (Wave Archives/Samples)
└── STRM (Streams/Direct Audio)
```

**Conversion Strategy:**

**Option A: Pre-Convert Assets**
- Convert SSEQ to MIDI → Render to OGG/MP3
- Extract SWAR samples to WAV
- Use SDL_mixer for playback

**Option B: Runtime Conversion**
- Implement SSEQ interpreter
- Load SWAR samples dynamically
- More authentic but complex

**Recommended: Hybrid**
- Pre-convert music (STRM) to OGG
- Runtime interpret SSEQ for dynamic music
- Direct WAV playback for sound effects

#### 4.2 Audio PAL Implementation

```c
// pal_audio.h
typedef enum {
    PAL_AUDIO_BGM,
    PAL_AUDIO_SFX,
    PAL_AUDIO_VOICE
} PAL_AudioType;

typedef void* PAL_AudioHandle;

// Initialize audio system
bool PAL_Audio_Init(int frequency, int channels);
void PAL_Audio_Shutdown(void);

// Music control
PAL_AudioHandle PAL_Audio_LoadMusic(const char* path);
void PAL_Audio_PlayMusic(PAL_AudioHandle music, bool loop);
void PAL_Audio_StopMusic(PAL_AudioHandle music);
void PAL_Audio_FadeMusic(PAL_AudioHandle music, int ms, float target_volume);

// Sound effects
PAL_AudioHandle PAL_Audio_LoadSound(const char* path);
void PAL_Audio_PlaySound(PAL_AudioHandle sound, int channel, int loops);
void PAL_Audio_StopSound(int channel);

// Volume control
void PAL_Audio_SetVolume(PAL_AudioType type, float volume);
float PAL_Audio_GetVolume(PAL_AudioType type);
```

#### 4.3 SDAT Conversion Tool

```python
# tools/sdat_converter.py
import sys
from pathlib import Path

def convert_sdat(input_sdat, output_dir):
    """Convert SDAT to modern audio formats"""
    # Parse SDAT structure
    sdat = parse_sdat(input_sdat)
    
    # Extract sequences (music)
    for seq in sdat.sequences:
        # Render MIDI-like sequence to OGG
        render_sequence(seq, output_dir / f"bgm_{seq.id}.ogg")
    
    # Extract samples
    for wave in sdat.wave_archives:
        extract_samples(wave, output_dir / "samples")
    
    # Generate manifest
    create_audio_manifest(output_dir / "audio_manifest.json")

if __name__ == "__main__":
    convert_sdat("data/sound/pl_sound_data.sdat", "resources/audio/")
```

#### 4.4 Integration with Sound System

```c
// Replace sound_system.c functions

void SoundSystem_Init(ChatotCry *chatotCry, Options *options) {
    #ifdef PLATFORM_DS
        NNS_SndInit();
        NNS_SndArcInit(&sSoundSystem.arc, "data/sound/pl_sound_data.sdat", 
                       sSoundSystem.heap, 0);
    #else
        PAL_Audio_Init(44100, 2);
        // Load audio manifest
        load_audio_manifest("resources/audio/audio_manifest.json");
    #endif
    
    // Common initialization
    sSoundSystem.chatotCry = chatotCry;
    Sound_SetPlaybackMode(options->soundMode);
}

BOOL Sound_PlayBGM(u16 bgmID) {
    #ifdef PLATFORM_DS
        // Original NNS_SndArcPlayerStartSeq code
    #else
        char path[256];
        snprintf(path, sizeof(path), "resources/audio/bgm_%d.ogg", bgmID);
        PAL_AudioHandle music = PAL_Audio_LoadMusic(path);
        PAL_Audio_PlayMusic(music, true);
        return TRUE;
    #endif
}
```

#### 4.5 Advanced Features

**Chatot Cry Recording:**
- Use SDL_AudioCapture for mic input
- Save to WAV format
- Playback through standard audio channel

**Music Fading:**
- SDL_mixer supports fade-in/fade-out
- Implement crossfading for seamless transitions

**3D Audio:**
- Pan and volume based on sprite position
- Optional OpenAL backend for positional audio

---

### Phase 5: File System (Weeks 17-19)

**Goals:**
- Replace NitroFS with standard filesystem
- Convert NARC archives
- Handle save data

#### 5.1 File System Architecture

**Current:** ROM-based NitroFS + NARC archives
**Target:** Directory-based filesystem

**Structure:**
```
resources/
├── graphics/
│   ├── battle/
│   ├── field/
│   └── ui/
├── audio/
│   ├── bgm/
│   └── sfx/
├── text/
│   └── messages/
├── pokemon/
│   ├── sprites/
│   └── data/
└── data/
    └── maps/
```

#### 5.2 NARC Extraction Tool

```bash
# tools/extract_narc.sh
#!/bin/bash
# Extract all NARC archives from ROM

ROM_PATH="platinum.nds"
OUTPUT_DIR="resources"

for narc in $(find rom/ -name "*.narc"); do
    echo "Extracting $narc..."
    ndstool -x "$narc" -d "$OUTPUT_DIR"
done
```

#### 5.3 File I/O PAL

```c
// pal_file.h
typedef void* PAL_File;
typedef void* PAL_Dir;

// File operations
PAL_File PAL_File_Open(const char* path, const char* mode);
size_t PAL_File_Read(void* buffer, size_t size, size_t count, PAL_File file);
size_t PAL_File_Write(const void* buffer, size_t size, size_t count, PAL_File file);
void PAL_File_Close(PAL_File file);
long PAL_File_Size(PAL_File file);

// Directory operations
PAL_Dir PAL_Dir_Open(const char* path);
const char* PAL_Dir_ReadNext(PAL_Dir dir);
void PAL_Dir_Close(PAL_Dir dir);

// Path utilities
char* PAL_Path_Combine(const char* base, const char* relative);
bool PAL_Path_Exists(const char* path);
```

#### 5.4 NARC Replacement

```c
// Replace narc.c functions

typedef struct PAL_Archive {
    char base_path[256];
    // Optionally: in-memory file table
} PAL_Archive;

NARC* NARC_ctor(u32 narcID, u32 heapID) {
    #ifdef PLATFORM_DS
        // Original ROM-based loading
    #else
        PAL_Archive* archive = malloc(sizeof(PAL_Archive));
        snprintf(archive->base_path, sizeof(archive->base_path),
                 "resources/data/narc_%03d", narcID);
        return (NARC*)archive;
    #endif
}

void* NARC_AllocAndReadWholeMember(NARC *narc, u32 memberIdx, u32 heapID) {
    #ifdef PLATFORM_DS
        // Original FS_ReadFile code
    #else
        PAL_Archive* archive = (PAL_Archive*)narc;
        char path[512];
        snprintf(path, sizeof(path), "%s/%04d.bin", 
                 archive->base_path, memberIdx);
        
        PAL_File file = PAL_File_Open(path, "rb");
        long size = PAL_File_Size(file);
        void* buffer = Heap_Alloc(heapID, size);
        PAL_File_Read(buffer, 1, size, file);
        PAL_File_Close(file);
        return buffer;
    #endif
}
```

#### 5.5 Save Data

**Current:** Save to DS cart EEPROM
**Target:** Save to user directory

```c
// Platform-specific save locations
#ifdef _WIN32
    // %APPDATA%/PokemonPlatinum/
#elif __APPLE__
    // ~/Library/Application Support/PokemonPlatinum/
#elif __linux__
    // ~/.local/share/PokemonPlatinum/
#elif __ANDROID__
    // SDL_AndroidGetInternalStoragePath()
#elif __IOS__
    // SDL_GetiOSDocumentsPath()
#endif

// pal_save.c
const char* PAL_GetSaveDirectory(void) {
    static char path[512];
    #ifdef PLATFORM_SDL
        const char* pref_path = SDL_GetPrefPath("PokePlatinum", "PokePlatinum");
        strncpy(path, pref_path, sizeof(path));
        SDL_free(pref_path);
    #endif
    return path;
}
```

---

### Phase 6: Memory Management (Weeks 20-21)

**Goals:**
- Abstract heap allocation
- Remove DTCM/ITCM dependencies
- Optimize memory usage for modern systems

#### 6.1 Memory PAL

```c
// pal_memory.h
void* PAL_Malloc(size_t size, int heap_id);
void PAL_Free(void* ptr);
void* PAL_Realloc(void* ptr, size_t new_size);

// Optional: Custom allocators
typedef struct PAL_Allocator {
    void* (*malloc_func)(size_t);
    void (*free_func)(void*);
    void* (*realloc_func)(void*, size_t);
} PAL_Allocator;

void PAL_Memory_SetAllocator(PAL_Allocator* allocator);
```

**Integration:**
```c
// Replace heap.c allocations
void* Heap_Alloc(u32 heapID, size_t size) {
    #ifdef PLATFORM_DS
        // Original OS_AllocFromHeap code
    #else
        return PAL_Malloc(size, heapID);
    #endif
}
```

---

### Phase 7: Testing & Optimization (Weeks 22-24)

**Goals:**
- Ensure feature parity
- Optimize performance
- Fix platform-specific bugs

#### 7.1 Test Framework

```c
// tests/test_framework.h
#define TEST(name) void test_##name(void)
#define ASSERT_EQ(a, b) if ((a) != (b)) { test_fail(__FILE__, __LINE__); }
#define ASSERT_TRUE(x) if (!(x)) { test_fail(__FILE__, __LINE__); }

// tests/test_graphics.c
TEST(sprite_rendering) {
    PAL_Sprite sprite = {0};
    sprite.x = 50;
    sprite.y = 50;
    sprite.visible = true;
    
    PAL_Sprite_Draw(&sprite, PAL_Graphics_GetScreen(PAL_SCREEN_MAIN));
    ASSERT_TRUE(sprite_was_drawn());
}
```

#### 7.2 Automated Testing

```bash
# Run test suite
./build/tests/run_all_tests

# Regression tests
./build/tests/compare_with_reference_frames
```

#### 7.3 Performance Profiling

```bash
# Linux: perf
perf record -g ./build/pokeplatinum_sdl
perf report

# Windows: Visual Studio Profiler
# macOS: Instruments
```

**Target Metrics:**
- 60 FPS stable
- < 100 MB RAM usage
- < 1s startup time

---

## 4. Technical Challenges & Solutions

### Challenge 1: Dual Screen Layout

**Problem:** DS has two physical screens; modern devices have one

**Solutions:**
- **Desktop:** Side-by-side or vertical layout with configurable orientation
- **Mobile:** Full-screen with tab gesture to switch screens
- **VR/AR:** Actual dual-display (future consideration)

### Challenge 2: Touch Screen Precision

**Problem:** Resistive touch vs. capacitive/mouse

**Solutions:**
- Implement hit-box expansion for small targets
- Add visual cursor when using gamepad
- Optional "snap to nearest button" assist

### Challenge 3: 3D Rendering

**Problem:** DS has unique fixed-function 3D hardware

**Solutions:**
- **Short-term:** Software emulation of G3D
- **Long-term:** OpenGL ES 2.0/3.0 translation layer
- **Reference:** Look at melonDS or DeSmuME 3D renderers

### Challenge 4: Save Data Compatibility

**Problem:** Different endianness, filesystem layouts

**Solutions:**
- Implement save converter tool
- Support both DS and SDL save formats
- Cloud save sync option (optional)

### Challenge 5: Audio Latency

**Problem:** SDL_Audio has higher latency than DS hardware

**Solutions:**
- Use small buffer sizes (512-1024 samples)
- Pre-load frequently used sound effects
- Implement audio ahead-of-time synthesis

---

## 5. Implementation Details

### 5.1 Directory Structure

```
pokeplatinum/
├── src/
│   ├── platform/          # New: Platform abstraction layer
│   │   ├── sdl/           # SDL3 implementations
│   │   │   ├── pal_graphics_sdl.c
│   │   │   ├── pal_input_sdl.c
│   │   │   ├── pal_audio_sdl.c
│   │   │   └── pal_file_sdl.c
│   │   ├── ds/            # Original DS implementations
│   │   └── pal_*.h        # PAL interface headers
│   ├── applications/      # Existing game code (minimal changes)
│   ├── overlay*/          # Existing overlays
│   └── ...                # Other existing code
├── include/
│   └── platform/          # PAL public headers
├── resources/             # New: Extracted/converted assets
│   ├── graphics/
│   ├── audio/
│   ├── text/
│   └── data/
├── tools/
│   ├── narc_extractor/    # New: Asset extraction
│   ├── sdat_converter/    # New: Audio conversion
│   └── ...
├── tests/                 # New: Automated tests
├── CMakeLists.txt         # New: Build system
└── PORTING_PLAN.md        # This document
```

### 5.2 Conditional Compilation

```c
// Example from src/applications/title_screen.c

static BOOL TitleScreen_RenderMain(TitleScreen *titleScreen, BgConfig *bgConfig, 
                                   enum HeapID heapID)
{
    #ifdef PLATFORM_DS
        // Original DS rendering using BG layers
        Graphics_LoadTilesToBgLayer(NARC_INDEX_DEMO__TITLE__TITLEDEMO, 
                                    top_screen_border_NCGR, 
                                    bgConfig, TITLE_SCREEN_LAYER_LOGO_BG, 
                                    0, 0, FALSE, heapID);
    #else
        // SDL rendering using surfaces
        PAL_Surface* main_screen = PAL_Graphics_GetScreen(PAL_SCREEN_MAIN);
        PAL_Graphics_LoadTexture(main_screen, 
                                "resources/graphics/title/top_border.png");
    #endif
    
    return TRUE;
}
```

### 5.3 Build Configuration

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(PokemonPlatinum C)

option(BUILD_DS_VERSION "Build for Nintendo DS" OFF)
option(BUILD_SDL_VERSION "Build SDL3 port" ON)

if(BUILD_SDL_VERSION)
    find_package(SDL3 REQUIRED)
    
    add_executable(pokeplatinum_sdl
        src/main.c
        src/platform/sdl/pal_graphics_sdl.c
        src/platform/sdl/pal_input_sdl.c
        src/platform/sdl/pal_audio_sdl.c
        src/platform/sdl/pal_file_sdl.c
        # ... all other source files
    )
    
    target_compile_definitions(pokeplatinum_sdl PRIVATE PLATFORM_SDL)
    target_link_libraries(pokeplatinum_sdl SDL3::SDL3)
    target_include_directories(pokeplatinum_sdl PRIVATE 
        include 
        include/platform
    )
endif()

if(BUILD_DS_VERSION)
    # Original Meson/DS build
    # ...
endif()
```

---

## 6. Testing Strategy

### 6.1 Unit Tests

```c
// tests/test_battle_system.c
TEST(pokemon_damage_calculation) {
    Pokemon attacker = create_test_pokemon(SPECIES_PIKACHU, 50);
    Pokemon defender = create_test_pokemon(SPECIES_SQUIRTLE, 50);
    
    int damage = calculate_damage(attacker, defender, MOVE_THUNDERBOLT);
    
    ASSERT_TRUE(damage >= 50 && damage <= 100); // Expected range
}
```

### 6.2 Integration Tests

```c
// tests/test_save_load.c
TEST(save_and_load_party) {
    Party original_party = create_test_party();
    
    SaveData* save = SaveData_Init();
    SaveData_SetParty(save, &original_party);
    SaveData_Save(save, "test_save.sav");
    
    SaveData* loaded = SaveData_Load("test_save.sav");
    Party* loaded_party = SaveData_GetParty(loaded);
    
    ASSERT_EQ(Party_GetSize(original_party), Party_GetSize(*loaded_party));
}
```

### 6.3 Graphical Regression Tests

```python
# tests/visual_regression.py
import subprocess
import cv2
import numpy as np

def test_title_screen_rendering():
    # Run game and capture frame
    process = subprocess.Popen(['./pokeplatinum_sdl', '--test-mode'])
    screenshot = capture_screenshot()
    
    # Compare with reference
    reference = cv2.imread('tests/references/title_screen.png')
    difference = cv2.absdiff(screenshot, reference)
    
    assert np.mean(difference) < 5.0  # Allow small differences
```

### 6.4 Automated Gameplay Tests

```python
# tests/automated_gameplay.py
from game_controller import GameController

def test_battle_flow():
    game = GameController()
    game.load_state("tests/states/before_battle.sav")
    
    game.press_button('A')  # Start battle
    game.wait_for_screen("battle_menu")
    
    game.select_menu_item("FIGHT")
    game.select_menu_item("TACKLE")
    
    game.wait_for_screen("battle_end")
    assert game.get_pokemon(0).hp > 0  # Player won
```

---

## 7. Build System & Tooling

### 7.1 Asset Pipeline

```bash
# Build and convert all assets
make assets

# Targets:
# - extract_narc: Extract NARC archives
# - convert_audio: Convert SDAT to OGG/WAV
# - convert_graphics: Convert NCGR to PNG
# - build_manifest: Generate resource manifests
```

### 7.2 Development Tools

**Recommended:**
- **IDE:** VSCode with C/C++ extension
- **Debugger:** GDB / LLDB / Visual Studio Debugger
- **Profiler:** 
  - Linux: perf, Valgrind
  - Windows: Visual Studio Profiler
  - macOS: Instruments
- **Graphics Debugger:**
  - RenderDoc (OpenGL/Vulkan)
  - SDL_RenderGeometry debugging overlay

### 7.3 Continuous Integration

```yaml
# .github/workflows/build.yml
name: Build SDL Port

on: [push, pull_request]

jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
    
    runs-on: ${{ matrix.os }}
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install SDL3
      run: |
        # Install SDL3 for each platform
        
    - name: Build
      run: |
        mkdir build
        cd build
        cmake .. -DBUILD_SDL_VERSION=ON
        cmake --build .
    
    - name: Run Tests
      run: |
        cd build
        ctest --output-on-failure
```

---

## 8. Resource Recommendations

### 8.1 Essential Reading

1. **SDL3 Documentation**
   - https://wiki.libsdl.org/SDL3/FrontPage
   - Focus on: SDL_Renderer, SDL_Audio, SDL_Gamepad

2. **DS Hardware Reference**
   - GBATEK (DS Technical Info)
   - melonDS source code (3D rendering)
   - DeSmuME source code (general emulation)

3. **Similar Projects**
   - OpenRCT2 (RollerCoaster Tycoon port)
   - OpenMW (Morrowind engine reimplementation)
   - CorsixTH (Theme Hospital port)

### 8.2 Libraries & Dependencies

**Core:**
- SDL3 (graphics, audio, input)
- stb_image (image loading)
- dr_wav (WAV audio)

**Optional:**
- SDL_mixer (if using simpler audio approach)
- OpenGL ES 2.0/3.0 (hardware 3D)
- GLFW (alternative to SDL for windowing)
- Dear ImGui (debug UI)

### 8.3 Community Resources

- **pret Discord:** Ask questions about decompilation structure
- **SDL Discord:** Technical SDL3 support
- **/r/EmuDev:** Emulation techniques applicable to porting

---

## Appendix A: Quick Start Guide

### Getting Started Immediately

1. **Set up build environment:**
```bash
# Install SDL3
git clone https://github.com/libsdl-org/SDL.git -b SDL3
cd SDL
mkdir build && cd build
cmake ..
make && sudo make install

# Clone project
git clone https://github.com/arcanite24/pokeplatinum-portable.git
cd pokeplatinum-portable
```

2. **Create minimal SDL window:**
```c
// src/platform/sdl/main_sdl.c
#include <SDL3/SDL.h>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD);
    
    SDL_Window* window = SDL_CreateWindow("Pokemon Platinum", 
                                          512, 384, 
                                          SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, 
                                                SDL_RENDERER_ACCELERATED);
    
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
```

3. **Build and run:**
```bash
gcc -o test src/platform/sdl/main_sdl.c $(sdl3-config --cflags --libs)
./test
```

---

## Appendix B: Estimated Timeline

| Phase | Description | Weeks | Complexity |
|-------|-------------|-------|------------|
| 1 | Foundation & Build System | 3 | Medium |
| 2 | Display System | 5 | High |
| 3 | Input System | 3 | Low |
| 4 | Audio System | 5 | High |
| 5 | File System | 3 | Medium |
| 6 | Memory Management | 2 | Low |
| 7 | Testing & Optimization | 3 | Medium |
| **Total** | **Full port** | **24** | **~6 months** |

**Factors affecting timeline:**
- Team size (1 person vs. 5+ people)
- Familiarity with codebase
- Quality of tooling (automated testing, asset conversion)
- Scope creep (adding features beyond DS parity)

---

## Appendix C: Key Files to Modify

### High Priority (Core Systems)
```
src/main.c                          # Entry point
src/system.c                        # System initialization
src/graphics.c                      # Graphics loading
src/render_oam.c                    # Sprite rendering
src/sound_system.c                  # Audio system
src/sound.c                         # Sound playback
src/narc.c                          # File archives
src/heap.c                          # Memory management
```

### Medium Priority (Subsystems)
```
src/bg_window.c                     # Background rendering
src/sprite.c                        # Sprite system
src/touch_pad.c                     # Touch input
src/touch_screen.c                  # Touch screen logic
src/camera.c                        # 3D camera
src/easy3d.c                        # 3D rendering helpers
```

### Low Priority (Game Logic - Minimal Changes)
```
src/applications/*                  # Game screens
src/overlay*/*                      # Game features
src/battle/*                        # Battle system
```

---

## Conclusion

This porting plan provides a structured approach to migrating Pokémon Platinum from Nintendo DS hardware to a portable SDL3-based architecture. The key to success is:

1. **Incremental development** - Port subsystems independently
2. **Strong abstraction** - Clean PAL layer allows both DS and SDL builds
3. **Comprehensive testing** - Automated tests catch regressions
4. **Community engagement** - Leverage pret community knowledge

With this plan, a small dedicated team can achieve a working SDL3 port in approximately 6 months, enabling Pokémon Platinum to run on modern platforms while preserving the original game experience.

---

**Next Steps:**
1. Review this plan with the team
2. Set up development environment
3. Begin Phase 1 implementation
4. Iterate based on findings

Good luck with the porting effort! 🚀
