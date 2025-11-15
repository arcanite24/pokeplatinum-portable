/**
 * @file pal_input_sdl.c
 * @brief SDL3 implementation of input abstraction layer
 * 
 * Supports:
 * - Keyboard input (with customizable mappings)
 * - Mouse input for touch screen
 * - Gamepad input (up to 4 controllers)
 * - Right-stick touch cursor mode
 */

#include "platform/pal_input.h"

#ifdef PLATFORM_SDL

#include <SDL3/SDL.h>
#include <string.h>
#include <stdio.h>

#define MAX_GAMEPADS 4

// Input state
static struct {
    u32 held_buttons;
    u32 pressed_buttons;
    u32 released_buttons;
    u32 prev_buttons;
    PAL_TouchState touch;
    BOOL initialized;
    
    // Gamepad support
    SDL_Gamepad* gamepads[MAX_GAMEPADS];
    int gamepad_count;
    
    // Touch cursor mode (right-stick controls cursor)
    BOOL touch_cursor_enabled;
    float cursor_x, cursor_y;  // Floating point for smooth movement
    float cursor_speed;         // Pixels per frame
} g_input;

// Key mapping: SDL scancode to PAL button
static const struct {
    SDL_Scancode scancode;
    PAL_ButtonMask button;
} key_map[] = {
    { SDL_SCANCODE_Z,      PAL_BUTTON_A },
    { SDL_SCANCODE_RETURN, PAL_BUTTON_A },
    { SDL_SCANCODE_X,      PAL_BUTTON_B },
    { SDL_SCANCODE_BACKSPACE, PAL_BUTTON_B },
    { SDL_SCANCODE_A,      PAL_BUTTON_X },
    { SDL_SCANCODE_S,      PAL_BUTTON_Y },
    { SDL_SCANCODE_Q,      PAL_BUTTON_L },
    { SDL_SCANCODE_W,      PAL_BUTTON_R },
    { SDL_SCANCODE_SPACE,  PAL_BUTTON_START },
    { SDL_SCANCODE_LSHIFT, PAL_BUTTON_SELECT },
    { SDL_SCANCODE_RSHIFT, PAL_BUTTON_SELECT },
    { SDL_SCANCODE_UP,     PAL_BUTTON_UP },
    { SDL_SCANCODE_DOWN,   PAL_BUTTON_DOWN },
    { SDL_SCANCODE_LEFT,   PAL_BUTTON_LEFT },
    { SDL_SCANCODE_RIGHT,  PAL_BUTTON_RIGHT },
};

// Gamepad button mapping: SDL gamepad button to PAL button
static const struct {
    SDL_GamepadButton sdl_button;
    PAL_ButtonMask pal_button;
} gamepad_button_map[] = {
    { SDL_GAMEPAD_BUTTON_SOUTH,         PAL_BUTTON_A },      // A (Xbox) / Cross (PS)
    { SDL_GAMEPAD_BUTTON_EAST,          PAL_BUTTON_B },      // B (Xbox) / Circle (PS)
    { SDL_GAMEPAD_BUTTON_WEST,          PAL_BUTTON_X },      // X (Xbox) / Square (PS)
    { SDL_GAMEPAD_BUTTON_NORTH,         PAL_BUTTON_Y },      // Y (Xbox) / Triangle (PS)
    { SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, PAL_BUTTON_L },
    { SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,PAL_BUTTON_R },
    { SDL_GAMEPAD_BUTTON_START,         PAL_BUTTON_START },
    { SDL_GAMEPAD_BUTTON_BACK,          PAL_BUTTON_SELECT },
    { SDL_GAMEPAD_BUTTON_DPAD_UP,       PAL_BUTTON_UP },
    { SDL_GAMEPAD_BUTTON_DPAD_DOWN,     PAL_BUTTON_DOWN },
    { SDL_GAMEPAD_BUTTON_DPAD_LEFT,     PAL_BUTTON_LEFT },
    { SDL_GAMEPAD_BUTTON_DPAD_RIGHT,    PAL_BUTTON_RIGHT },
};

// Helper functions
static void UpdateKeyboardInput(void);
static void UpdateGamepadInput(void);
static void UpdateTouchInput(void);
static void UpdateTouchCursor(void);

// ============================================================================
// Initialization and Shutdown
// ============================================================================

BOOL PAL_Input_Init(void) {
    if (g_input.initialized) {
        return TRUE;
    }
    
    memset(&g_input, 0, sizeof(g_input));
    
    // Initialize gamepad subsystem
    if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
        printf("Warning: Failed to initialize gamepad subsystem: %s\n", SDL_GetError());
    }
    
    // Auto-detect and open gamepads
    int num_joysticks;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&num_joysticks);
    if (joysticks) {
        for (int i = 0; i < num_joysticks && g_input.gamepad_count < MAX_GAMEPADS; i++) {
            if (SDL_IsGamepad(joysticks[i])) {
                SDL_Gamepad* gamepad = SDL_OpenGamepad(joysticks[i]);
                if (gamepad) {
                    g_input.gamepads[g_input.gamepad_count++] = gamepad;
                    printf("Gamepad %d connected: %s\n", g_input.gamepad_count,
                           SDL_GetGamepadName(gamepad));
                }
            }
        }
        SDL_free(joysticks);
    }
    
    // Initialize touch cursor mode
    g_input.touch_cursor_enabled = TRUE;
    g_input.cursor_x = 128.0f;  // Center of screen
    g_input.cursor_y = 96.0f;
    g_input.cursor_speed = 4.0f;
    
    g_input.initialized = TRUE;
    return TRUE;
}

void PAL_Input_Shutdown(void) {
    // Close all gamepads
    for (int i = 0; i < g_input.gamepad_count; i++) {
        if (g_input.gamepads[i]) {
            SDL_CloseGamepad(g_input.gamepads[i]);
            g_input.gamepads[i] = NULL;
        }
    }
    g_input.gamepad_count = 0;
    
    SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
    g_input.initialized = FALSE;
}

// ============================================================================
// Main Update Loop
// ============================================================================

void PAL_Input_Update(void) {
    const u32 prev_buttons = g_input.held_buttons;
    g_input.held_buttons = 0;
    
    // Update from all input sources
    UpdateKeyboardInput();
    UpdateGamepadInput();
    UpdateTouchInput();
    UpdateTouchCursor();
    
    // Calculate pressed and released
    g_input.pressed_buttons = g_input.held_buttons & ~prev_buttons;
    g_input.released_buttons = prev_buttons & ~g_input.held_buttons;
}

// ============================================================================
// Helper Functions
// ============================================================================

static void UpdateKeyboardInput(void) {
    const bool* keyboard = SDL_GetKeyboardState(NULL);
    
    for (size_t i = 0; i < sizeof(key_map) / sizeof(key_map[0]); i++) {
        if (keyboard[key_map[i].scancode]) {
            g_input.held_buttons |= key_map[i].button;
        }
    }
}

static void UpdateGamepadInput(void) {
    // Check all connected gamepads
    for (int pad_idx = 0; pad_idx < g_input.gamepad_count; pad_idx++) {
        SDL_Gamepad* gamepad = g_input.gamepads[pad_idx];
        if (!gamepad) continue;
        
        // Read buttons
        for (size_t i = 0; i < sizeof(gamepad_button_map) / sizeof(gamepad_button_map[0]); i++) {
            if (SDL_GetGamepadButton(gamepad, gamepad_button_map[i].sdl_button)) {
                g_input.held_buttons |= gamepad_button_map[i].pal_button;
            }
        }
        
        // Read left stick as D-Pad (with deadzone)
        const float STICK_DEADZONE = 0.3f;
        float left_x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX) / 32767.0f;
        float left_y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY) / 32767.0f;
        
        if (left_x < -STICK_DEADZONE) g_input.held_buttons |= PAL_BUTTON_LEFT;
        if (left_x > STICK_DEADZONE)  g_input.held_buttons |= PAL_BUTTON_RIGHT;
        if (left_y < -STICK_DEADZONE) g_input.held_buttons |= PAL_BUTTON_UP;
        if (left_y > STICK_DEADZONE)  g_input.held_buttons |= PAL_BUTTON_DOWN;
    }
}

static void UpdateTouchInput(void) {
    PAL_TouchState prev_touch = g_input.touch;
    
    float mouse_x, mouse_y;
    Uint32 mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
    
    // Check if mouse is in sub-screen area (right screen, 256-512 x range)
    if (mouse_x >= 256 && mouse_x < 512 && mouse_y >= 0 && mouse_y < 192) {
        g_input.touch.x = (int)(mouse_x - 256);
        g_input.touch.y = (int)mouse_y;
        g_input.touch.held = (mouse_state & SDL_BUTTON_LMASK) != 0;
        g_input.touch.pressed = g_input.touch.held && !prev_touch.held;
        g_input.touch.released = !g_input.touch.held && prev_touch.held;
    } else {
        // If not using mouse, check if using gamepad cursor
        if (!g_input.touch.held) {
            g_input.touch.held = FALSE;
            g_input.touch.pressed = FALSE;
            g_input.touch.released = prev_touch.held;
        }
    }
}

static void UpdateTouchCursor(void) {
    if (!g_input.touch_cursor_enabled || g_input.gamepad_count == 0) {
        return;
    }
    
    // Use first gamepad's right stick for touch cursor
    SDL_Gamepad* gamepad = g_input.gamepads[0];
    if (!gamepad) return;
    
    const float STICK_DEADZONE = 0.2f;
    float right_x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX) / 32767.0f;
    float right_y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY) / 32767.0f;
    
    // Apply deadzone
    if (right_x * right_x + right_y * right_y < STICK_DEADZONE * STICK_DEADZONE) {
        right_x = right_y = 0.0f;
    }
    
    // Move cursor
    if (right_x != 0.0f || right_y != 0.0f) {
        g_input.cursor_x += right_x * g_input.cursor_speed;
        g_input.cursor_y += right_y * g_input.cursor_speed;
        
        // Clamp to screen bounds
        if (g_input.cursor_x < 0.0f) g_input.cursor_x = 0.0f;
        if (g_input.cursor_x >= 256.0f) g_input.cursor_x = 255.0f;
        if (g_input.cursor_y < 0.0f) g_input.cursor_y = 0.0f;
        if (g_input.cursor_y >= 192.0f) g_input.cursor_y = 191.0f;
    }
    
    // Check if shoulder trigger is pressed for "touch"
    // Triggers are axes in SDL3, with values 0-32767
    PAL_TouchState prev_touch = g_input.touch;
    Sint16 left_trigger = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
    Sint16 right_trigger = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
    BOOL shoulder_pressed = (left_trigger > 16384) || (right_trigger > 16384); // > 50% pressed
    
    if (shoulder_pressed && (right_x != 0.0f || right_y != 0.0f || prev_touch.held)) {
        g_input.touch.x = (int)g_input.cursor_x;
        g_input.touch.y = (int)g_input.cursor_y;
        g_input.touch.held = TRUE;
        g_input.touch.pressed = !prev_touch.held;
        g_input.touch.released = FALSE;
    } else if (prev_touch.held && !shoulder_pressed) {
        g_input.touch.released = TRUE;
        g_input.touch.held = FALSE;
        g_input.touch.pressed = FALSE;
    }
}

// ============================================================================
// Query Functions
// ============================================================================

u32 PAL_Input_GetHeld(void) {
    return g_input.held_buttons;
}

u32 PAL_Input_GetPressed(void) {
    return g_input.pressed_buttons;
}

u32 PAL_Input_GetReleased(void) {
    return g_input.released_buttons;
}

PAL_TouchState PAL_Input_GetTouch(void) {
    return g_input.touch;
}

BOOL PAL_Input_IsTouchPressed(void) {
    return g_input.touch.held;
}

// ============================================================================
// Gamepad Management
// ============================================================================

void PAL_Input_AddGamepad(void* pad) {
    SDL_Gamepad* gamepad = (SDL_Gamepad*)pad;
    
    if (g_input.gamepad_count >= MAX_GAMEPADS) {
        printf("Warning: Maximum gamepad count reached (%d)\n", MAX_GAMEPADS);
        return;
    }
    
    // Check if already added
    for (int i = 0; i < g_input.gamepad_count; i++) {
        if (g_input.gamepads[i] == gamepad) {
            return; // Already added
        }
    }
    
    g_input.gamepads[g_input.gamepad_count++] = gamepad;
    printf("Gamepad added: %s (total: %d)\n", 
           SDL_GetGamepadName(gamepad), g_input.gamepad_count);
}

void PAL_Input_RemoveGamepad(void* pad) {
    SDL_Gamepad* gamepad = (SDL_Gamepad*)pad;
    
    // Find and remove
    for (int i = 0; i < g_input.gamepad_count; i++) {
        if (g_input.gamepads[i] == gamepad) {
            printf("Gamepad removed: %s\n", SDL_GetGamepadName(gamepad));
            
            // Shift remaining gamepads down
            for (int j = i; j < g_input.gamepad_count - 1; j++) {
                g_input.gamepads[j] = g_input.gamepads[j + 1];
            }
            g_input.gamepads[--g_input.gamepad_count] = NULL;
            return;
        }
    }
}

#endif // PLATFORM_SDL
