#ifndef PAL_INPUT_H
#define PAL_INPUT_H

/**
 * @file pal_input.h
 * @brief Platform Abstraction Layer - Input API
 * 
 * Provides unified input interface for both Nintendo DS and SDL3 platforms.
 */

#include "platform_config.h"
#include "platform_types.h"

// Button bit masks (matching PAD_* constants from NitroSDK)
typedef enum {
    PAL_BUTTON_A      = 0x0001,
    PAL_BUTTON_B      = 0x0002,
    PAL_BUTTON_SELECT = 0x0004,
    PAL_BUTTON_START  = 0x0008,
    PAL_BUTTON_RIGHT  = 0x0010,
    PAL_BUTTON_LEFT   = 0x0020,
    PAL_BUTTON_UP     = 0x0040,
    PAL_BUTTON_DOWN   = 0x0080,
    PAL_BUTTON_R      = 0x0100,
    PAL_BUTTON_L      = 0x0200,
    PAL_BUTTON_X      = 0x0400,
    PAL_BUTTON_Y      = 0x0800,
} PAL_ButtonMask;

// Touch screen state
typedef struct {
    int x;
    int y;
    BOOL pressed;
    BOOL held;
    BOOL released;
} PAL_TouchState;

/**
 * Initialize the input system
 * @return TRUE on success, FALSE on failure
 */
BOOL PAL_Input_Init(void);

/**
 * Shutdown the input system
 */
void PAL_Input_Shutdown(void);

/**
 * Update input state (call once per frame)
 */
void PAL_Input_Update(void);

/**
 * Get currently held buttons
 * @return Bitmask of held buttons
 */
u32 PAL_Input_GetHeld(void);

/**
 * Get buttons pressed this frame
 * @return Bitmask of pressed buttons
 */
u32 PAL_Input_GetPressed(void);

/**
 * Get buttons released this frame
 * @return Bitmask of released buttons
 */
u32 PAL_Input_GetReleased(void);

/**
 * Get touch screen state
 * @return Touch state structure
 */
PAL_TouchState PAL_Input_GetTouch(void);

/**
 * Check if touch screen is currently pressed
 * @return TRUE if pressed, FALSE otherwise
 */
BOOL PAL_Input_IsTouchPressed(void);

#ifdef PLATFORM_SDL
/**
 * Add a gamepad to the input system (SDL only)
 * @param pad SDL gamepad handle
 */
void PAL_Input_AddGamepad(void* pad);

/**
 * Remove a gamepad from the input system (SDL only)
 * @param pad SDL gamepad handle
 */
void PAL_Input_RemoveGamepad(void* pad);
#endif

#endif // PAL_INPUT_H
