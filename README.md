# Pokémon Platinum SDL3 Port

This is an **SDL3 port** of the Pokémon Platinum decompilation, making the game **portable to modern platforms**. This fork focuses exclusively on SDL3 portability and does not maintain the original Nintendo DS build.

**Note:** This is a port fork. For the original decompilation project with DS build support, see the [upstream pret repository](https://github.com/pret/pokeplatinum).

## Project Goals

1. **Cross-platform portability** - Run on Windows, macOS, Linux, iOS, and Android using SDL3
2. **Clean, maintainable code** - Direct SDL3 implementation without conditional compilation complexity
3. **Preserve game functionality** - Maintain original gameplay while modernizing the platform layer

For the comprehensive SDL3 porting roadmap, see [`PORTING_PLAN.md`](PORTING_PLAN.md).

## Build & Run

### SDL3 Port (This Repository)

**Current Status:**
- ✅ Phase 1: Foundation & Build System complete
- ✅ Phase 2: Display System complete (graphics primitives, backgrounds, sprites)
- ✅ Phase 3: Input System complete (keyboard, mouse, gamepad with hot-plug)
- 🚧 Phase 3.4: Game Integration (graphics loading - in progress)
- 📋 Phase 4: Audio System (planned)
- 📋 Phase 5: File System (planned)

See [`QUICKSTART_SDL.md`](QUICKSTART_SDL.md) for detailed build instructions and [`PORTING_PLAN.md`](PORTING_PLAN.md) for the full roadmap.

**Target Platforms:**
- ✅ macOS (tested - Apple Silicon and Intel)
- 🚧 Windows (planned)
- 🚧 Linux (planned)
- 📋 iOS (future)
- 📋 Android (future)

**Quick Start:**
```bash
# Install dependencies (macOS)
brew install cmake sdl3

# Or on Linux
sudo apt install cmake libsdl3-dev  # Ubuntu/Debian
sudo dnf install cmake SDL3-devel   # Fedora

# Build and run
./build_sdl.sh
./build-sdl/pokeplatinum_sdl
```

## Documentation

- **[QUICKSTART_SDL.md](QUICKSTART_SDL.md)** - Build instructions and quick start guide
- **[PORTING_PLAN.md](PORTING_PLAN.md)** - Comprehensive porting strategy and roadmap
- **[PAL.md](PAL.md)** - Platform Abstraction Layer documentation
- **[PHASE*_SUMMARY.md](.)** - Phase completion summaries with implementation details

## Original Nintendo DS Build

This fork does not support building the original Nintendo DS ROM. For DS build support, see the [upstream pret/pokeplatinum repository](https://github.com/pret/pokeplatinum).

The original DS build produced these ROMs:
* **pokeplatinum.us.nds** (Rev 1): `sha1: 0862ec35b24de5c7e2dcb88c9eea0873110d755c`
* **pokeplatinum.us.nds** (Rev 0): `sha1: ce81046eda7d232513069519cb2085349896dec7`

## Community

For contacts and other pret projects, see [pret.github.io](https://pret.github.io/). In addition to the pret Discord, also see the [VoidMatrix Discord (#decomp)](https://discord.gg/prUAgd5).
