# Quick Start - SDL3 Port

## Prerequisites

1. **Install CMake**
   ```bash
   # macOS
   brew install cmake
   
   # Linux
   sudo apt install cmake  # Ubuntu/Debian
   sudo yum install cmake  # CentOS/RHEL
   ```

2. **Install SDL3**
   ```bash
   # macOS
   brew install sdl3
   
   # Or build from source
   git clone https://github.com/libsdl-org/SDL.git -b SDL3
   cd SDL && mkdir build && cd build
   cmake .. && make -j$(nproc) && sudo make install
   ```

3. **Verify installations**
   ```bash
   cmake --version
   # Should output: cmake version 3.x.x
   
   pkg-config --modversion sdl3
   # Should output: 3.x.x
   ```

## Build

### Automated (Recommended)
```bash
./build_sdl.sh
```

### Manual
```bash
mkdir build-sdl && cd build-sdl
cmake .. -DBUILD_SDL_VERSION=ON -DBUILD_DS_VERSION=OFF
cmake --build .
```

## Run

```bash
./build-sdl/pokeplatinum_sdl
```

You should see:
- A 512x192 window (two screens side-by-side)
- Console output showing frame count every 60 frames
- Input state when buttons are pressed

## Test the Build

1. **Window appears**: ✅ Graphics initialization works
2. **Press arrow keys**: Should see "Held: 0xXXXX" in console
3. **Click right half of window**: Should see "Touch: (x, y)" in console
4. **Press ESC**: Clean shutdown with FPS stats

## Controls

| Action | Keyboard | DS Equivalent |
|--------|----------|---------------|
| D-Pad | Arrow Keys | D-Pad |
| A Button | Z or Enter | A |
| B Button | X or Backspace | B |
| X Button | A | X |
| Y Button | S | Y |
| L Trigger | Q | L |
| R Trigger | W | R |
| Start | Space | Start |
| Select | Shift | Select |
| Touch | Mouse (right screen) | Touch Screen |
| Quit | ESC | - |

## Troubleshooting

### SDL3 not found
```bash
# Set SDL3_DIR if installed in custom location
export SDL3_DIR=/path/to/sdl3/lib/cmake/SDL3
```

### Build fails
```bash
# Clean and rebuild
rm -rf build-sdl
./build_sdl.sh
```

### Window doesn't appear
- Check console for error messages
- Verify SDL3 version: `pkg-config --modversion sdl3`
- Update graphics drivers

## What's Working

- ✅ SDL3 initialization
- ✅ Window creation (512x192)
- ✅ Keyboard input
- ✅ Mouse input (touch emulation)
- ✅ Frame timing (60 FPS)
- ✅ Clean shutdown

## What's Not Yet Implemented

- Graphics rendering (black screens currently)
- Audio playback
- Gamepad support
- Game logic integration
- Asset loading

## Next Steps

See `PORTING_PLAN.md` for the full roadmap. Current status: **Phase 1, Task 1.1 Complete** ✅

To continue development, proceed to:
- **Phase 2**: Display System (sprite/tilemap rendering)
- **Phase 3**: Input System (complete gamepad support)
- **Phase 4**: Audio System (music and sound effects)

## More Information

- Full build instructions: `src/platform/README.md`
- Porting strategy: `PORTING_PLAN.md`
- Implementation summary: `PHASE1_TASK1_SUMMARY.md`
- GitHub Copilot instructions: `.github/copilot-instructions.md`

## Need Help?

- Check the documentation files listed above
- Review console output for error messages
- Ensure SDL3 is properly installed
- Join pret Discord: https://discord.gg/d5dubZ3
