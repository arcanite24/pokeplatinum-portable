/**
 * @file pal_input_sdl.c
 * @brief SDL3 implementation of input abstraction layer
 */

#include "platform/pal_input.h"

#ifdef PLATFORM_SDL

#include <SDL3/SDL.h>
#include <string.h>

// Input state
static struct {
    u32 held_buttons;
    u32 pressed_buttons;
    u32 released_buttons;
    u32 prev_buttons;
    PAL_TouchState touch;
    BOOL initialized;
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

BOOL PAL_Input_Init(void) {
    if (g_input.initialized) {
        return TRUE;
    }
    
    memset(&g_input, 0, sizeof(g_input));
    g_input.initialized = TRUE;
    return TRUE;
}

void PAL_Input_Shutdown(void) {
    g_input.initialized = FALSE;
}

void PAL_Input_Update(void) {
    const u32 prev_buttons = g_input.held_buttons;
    g_input.held_buttons = 0;
    
    // Read keyboard state
    const bool* keyboard = SDL_GetKeyboardState(NULL);
    
    for (size_t i = 0; i < sizeof(key_map) / sizeof(key_map[0]); i++) {
        if (keyboard[key_map[i].scancode]) {
            g_input.held_buttons |= key_map[i].button;
        }
    }
    
    // Calculate pressed and released
    g_input.pressed_buttons = g_input.held_buttons & ~prev_buttons;
    g_input.released_buttons = prev_buttons & ~g_input.held_buttons;
    
    // Update touch state
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
        g_input.touch.held = FALSE;
        g_input.touch.pressed = FALSE;
        g_input.touch.released = prev_touch.held;
    }
}

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

void PAL_Input_AddGamepad(void* pad) {
    // TODO: Implement gamepad support
}

void PAL_Input_RemoveGamepad(void* pad) {
    // TODO: Implement gamepad support
}

#endif // PLATFORM_SDL
