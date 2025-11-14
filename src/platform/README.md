# SDL3 Build Instructions

This directory contains the platform abstraction layer (PAL) for building Pokémon Platinum as a portable SDL3 application.

## Prerequisites

### Install SDL3

**macOS (Homebrew):**
```bash
brew install sdl3
```

**Ubuntu/Debian:**
```bash
# Build from source (SDL3 not in most package managers yet)
git clone https://github.com/libsdl-org/SDL.git -b SDL3
cd SDL
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```

**Windows:**
- Download SDL3 development libraries from https://github.com/libsdl-org/SDL/releases
- Extract to a known location
- Set `SDL3_DIR` environment variable to the SDL3 cmake folder

## Building

### Using CMake (Recommended for SDL)

```bash
# From repository root
mkdir build-sdl
cd build-sdl
cmake .. -DBUILD_SDL_VERSION=ON -DBUILD_DS_VERSION=OFF
cmake --build .
```

### Running

```bash
# From build directory
./pokeplatinum_sdl
```

## Current Status

**Phase 1: Foundation & Build System** ✅

- [x] Platform configuration headers (`platform_config.h`, `platform_types.h`)
- [x] PAL structure created (`src/platform/sdl/`)
- [x] PAL headers for all subsystems
- [x] Basic SDL3 implementations (stubs)
- [x] CMake build system with dual-target support
- [x] Minimal SDL3 test application

**Next Steps:**

- Phase 2: Display System (implement graphics rendering)
- Phase 3: Input System (complete gamepad support)
- Phase 4: Audio System (implement audio playback)
- Phase 5: File System (NARC extraction and loading)
- Phase 6: Memory Management (heap abstraction)
- Phase 7: Testing & Optimization

## Architecture

```
include/platform/          # PAL public headers
├── platform_config.h      # Platform detection and configuration
├── platform_types.h       # Cross-platform type definitions
├── pal_graphics.h         # Graphics abstraction
├── pal_input.h            # Input abstraction
├── pal_audio.h            # Audio abstraction
├── pal_file.h             # File I/O abstraction
├── pal_timer.h            # Timing abstraction
└── pal_memory.h           # Memory management abstraction

src/platform/sdl/          # SDL3 implementations
├── pal_graphics_sdl.c     # SDL renderer-based graphics
├── pal_input_sdl.c        # Keyboard/mouse/gamepad input
├── pal_audio_sdl.c        # SDL audio playback
├── pal_file_sdl.c         # Standard filesystem I/O
├── pal_timer_sdl.c        # SDL timing functions
├── pal_memory_sdl.c       # Standard malloc/free
└── main_sdl.c             # SDL entry point
```

## Controls (Test Application)

- **Arrow Keys** - D-Pad
- **Z / Enter** - A Button
- **X / Backspace** - B Button
- **A** - X Button
- **S** - Y Button
- **Q** - L Button
- **W** - R Button
- **Space** - Start
- **Shift** - Select
- **Mouse** - Touch Screen (right screen area)
- **ESC** - Quit

## Development Notes

### Conditional Compilation

Code can be conditionally compiled for different platforms:

```c
#ifdef PLATFORM_DS
    // Nintendo DS-specific code
    BgConfig* bgConfig = BgConfig_New(heapID);
#else
    // SDL portable code
    PAL_BgConfig* bgConfig = PAL_Bg_Create();
#endif
```

### Adding New PAL Functions

1. Add function declaration to appropriate `include/platform/pal_*.h` header
2. Implement SDL version in `src/platform/sdl/pal_*_sdl.c`
3. (Later) Implement DS wrapper in `src/platform/ds/pal_*_ds.c`
4. Use in game code with conditional compilation

### Build System

- **CMake** - For SDL3 builds (cross-platform)
- **Meson** - For Nintendo DS builds (original system)

Both build systems can coexist and are independent.

## Troubleshooting

**SDL3 not found:**
- Make sure SDL3 is installed and SDL3_DIR is set correctly
- On macOS with Homebrew: `export SDL3_DIR=/opt/homebrew/Cellar/sdl3/3.x.x/lib/cmake/SDL3`

**Linker errors:**
- Ensure all PAL implementation files are included in CMakeLists.txt
- Check that `PLATFORM_SDL` and `TARGET_SDL` are defined

**Window doesn't appear:**
- Check SDL3 initialization messages in console output
- Verify graphics drivers support OpenGL or DirectX

## Resources

- [SDL3 Documentation](https://wiki.libsdl.org/SDL3/FrontPage)
- [PORTING_PLAN.md](../PORTING_PLAN.md) - Full porting strategy
- [pret Discord](https://discord.gg/d5dubZ3) - Decompilation community

## License

See [LICENSE](../LICENSE) for details.
