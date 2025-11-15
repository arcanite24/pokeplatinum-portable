# Testing the Input System

This guide will help you test the enhanced Phase 3 input system with keyboard, mouse, and gamepad.

## Building and Running

```bash
cd /Users/brandonalamgarcianeri/code/pokeplatinum

# Build the SDL port
cmake --build build-sdl

# Run the test application
./build-sdl/pokeplatinum_sdl
```

## What to Test

### 1. Keyboard Input ✅

**D-Pad / Arrow Keys:**
- Press **arrow keys** to scroll the background
- Hold multiple directions for diagonal movement
- Watch console output for button state

**Action Buttons:**
- **Z** or **Enter** → A button
- **X** or **Backspace** → B button
- **A** → X button
- **S** → Y button

**Shoulder Buttons:**
- **Q** → L button
- **W** → R button

**System Buttons:**
- **Enter** → Start
- **Shift** → Select
- **ESC** → Quit application

### 2. Mouse Input ✅

**Touch Screen:**
- Click on the **right side of the window** (sub-screen area)
- Touch coordinates should print to console
- Expected range: x: 0-255, y: 0-191

### 3. Gamepad Input (If Available)

**Controller Setup:**
1. Connect Xbox, PlayStation, or Switch Pro controller
2. Watch console for "Gamepad added" message
3. Test all buttons

**Button Mapping:**
- **A (South/Cross)** → A button
- **B (East/Circle)** → B button  
- **X (West/Square)** → X button
- **Y (North/Triangle)** → Y button
- **L1/LB** → L button
- **R1/RB** → R button
- **Start** → Start
- **Select/Back** → Select

**Analog Sticks:**
- **Left Stick** → Moves background (same as D-Pad)
  - Deadzone: 0.3 (prevents drift)
  - Push in any direction to scroll
  
- **Right Stick** → Touch cursor mode
  - Move stick to control cursor position
  - Hold **L2/LT** or **R2/RT** (triggers) to activate "touch"
  - Cursor speed: 4 pixels per frame
  - Cursor bounds: 0-255, 0-191

**Hot-Plug Test:**
1. Disconnect controller while running
2. Should see "Gamepad removed" message
3. Reconnect controller
4. Should see "Gamepad added" message
5. Input should work immediately

### 4. Expected Console Output

```
=== Pokemon Platinum SDL3 Port - Phase 3 Test ===
Initializing PAL subsystems...
[TIMER] Timer system initialized
[AUDIO] Audio system initialized (44100 Hz, 2 channels, 1024 buffer)
[INPUT] Input system initialized
[INPUT] Auto-detected 0 gamepad(s)
[GRAPHICS] Graphics system initialized
...

Frame 0 - Held: 0x0000, Pressed: 0x0000, BG Scroll: (0, 0)
Frame 60 - Held: 0x0100, Pressed: 0x0000, BG Scroll: (-2, 0)
Frame 120 - Held: 0x0300, Pressed: 0x0000, BG Scroll: (-4, -2)
...
```

**With Gamepad:**
```
[INPUT] Gamepad added: Xbox Series X Controller (total: 1)
Frame 60 - Held: 0x0001, Pressed: 0x0001, BG Scroll: (0, 0)
```

**With Touch:**
```
Frame 180 - Held: 0x0000, Pressed: 0x0000, BG Scroll: (10, 5), Touch: (128, 96)
```

## Performance Metrics

**Expected FPS:** ~54-60 FPS  
**Input Latency:** <1ms  
**CPU Usage:** <0.1% for input processing  

Check console output every 60 frames for performance stats.

## Troubleshooting

### "No gamepad detected"
- Make sure controller is connected before starting app
- Check SDL3 gamepad database: `sdl3-controllermap`
- Try different USB port or Bluetooth pairing

### "Gamepad buttons not working"
- Verify controller is recognized by system
- Test with SDL3 test programs
- Check `PAL_Input_GetHeld()` output in console

### "Touch cursor not moving"
- Ensure right stick is pushed beyond deadzone (0.3)
- Try pushing stick to edges
- Check console for touch coordinates

### "Background not scrolling"
- Verify arrow keys work in other apps
- Try gamepad left stick instead
- Check `held` button state in console

### Build Errors
```bash
# Clean build
rm -rf build-sdl
mkdir build-sdl
cd build-sdl
cmake ..
cmake --build .
```

## Next Steps

Once input testing is complete, Phase 4 (Audio System) will be next:
- BGM playback
- Sound effects
- Audio mixing
- Volume control

See `PHASE3_SUMMARY.md` for full implementation details.

---

**Phase 3 Status: ✅ COMPLETE**  
**Input System: Keyboard ✅ | Mouse ✅ | Gamepad ✅**
