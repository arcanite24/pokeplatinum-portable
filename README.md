# Pokémon Platinum

This is a WIP decompilation of Pokémon Platinum with an ongoing effort to make it **portable to modern platforms** using SDL3. The project aims to preserve the original game's functionality while enabling it to run on Windows, macOS, Linux, iOS, and Android.

## Project Goals

1. **Byte-perfect decompilation** - Maintain ROM matching for the original Nintendo DS version
2. **Cross-platform portability** - Port to modern platforms via SDL3 abstraction layer
3. **Clean architecture** - Separate platform-specific code from game logic

For the comprehensive SDL3 porting roadmap, see [`PORTING_PLAN.md`](PORTING_PLAN.md).

For instructions on how to set up the repository, please read [`INSTALL.md`](INSTALL.md).

If you are interested in contributing, please read [`CONTRIBUTING.md`](/CONTRIBUTING.md).

## Build Targets

### Nintendo DS (Original Platform)

This repository builds the following ROMs:

* [**pokeplatinum.us.nds**](https://datomatic.no-intro.org/index.php?page=show_record&s=28&n=4997) (Rev 1): `sha1: 0862ec35b24de5c7e2dcb88c9eea0873110d755c`
* [**pokeplatinum.us.nds**](https://datomatic.no-intro.org/index.php?page=show_record&s=28&n=3541) (Rev 0): `sha1: ce81046eda7d232513069519cb2085349896dec7`

### SDL3 Port (Modern Platforms)

The SDL3 port is currently under development. **Phase 1 (Foundation & Build System) is complete!** ✅

See [`QUICKSTART_SDL.md`](QUICKSTART_SDL.md) for build instructions and [`PORTING_PLAN.md`](PORTING_PLAN.md) for the full roadmap.

**Current Status:**
- ✅ Platform abstraction layer (PAL) established
- ✅ CMake build system with dual-target support
- ✅ Working test application with input and graphics initialization
- 🚧 Graphics rendering (Phase 2 - in progress)
- 🚧 Audio system (Phase 4)
- 🚧 Game logic integration

**Target platforms:**
- Windows
- macOS ✅ (tested)
- Linux
- iOS (future)
- Android (future)

**Quick Start:**
```bash
# Install dependencies
brew install cmake sdl3  # macOS

# Build and run
./build_sdl.sh
./build-sdl/pokeplatinum_sdl
```

## Community

For contacts and other pret projects, see [pret.github.io](https://pret.github.io/). In addition to the pret Discord, also see the [VoidMatrix Discord (#decomp)](https://discord.gg/prUAgd5).
