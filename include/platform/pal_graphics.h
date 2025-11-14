#ifndef PAL_GRAPHICS_H
#define PAL_GRAPHICS_H

/**
 * @file pal_graphics.h
 * @brief Platform Abstraction Layer - Graphics API
 * 
 * Provides unified graphics interface for both Nintendo DS and SDL3 platforms.
 */

#include "platform_config.h"
#include "platform_types.h"

// Screen identifiers
typedef enum {
    PAL_SCREEN_MAIN = 0,
    PAL_SCREEN_SUB = 1,
    PAL_SCREEN_MAX = 2
} PAL_Screen;

// Surface structure (opaque for now, implemented per-platform)
typedef struct PAL_Surface PAL_Surface;

/**
 * Initialize the graphics system
 * @param window_width Total window width
 * @param window_height Total window height
 * @return true on success, false on failure
 */
BOOL PAL_Graphics_Init(int window_width, int window_height);

/**
 * Shutdown the graphics system
 */
void PAL_Graphics_Shutdown(void);

/**
 * Get a screen surface for rendering
 * @param screen The screen identifier (MAIN or SUB)
 * @return Pointer to the screen surface, or NULL on error
 */
PAL_Surface* PAL_Graphics_GetScreen(PAL_Screen screen);

/**
 * Set the active screen for rendering operations
 * @param screen The screen to make active
 */
void PAL_Graphics_SetActiveScreen(PAL_Screen screen);

/**
 * Begin a frame (call before rendering)
 */
void PAL_Graphics_BeginFrame(void);

/**
 * End a frame and present to display (call after rendering)
 */
void PAL_Graphics_EndFrame(void);

/**
 * Draw a sprite to the specified surface
 * @param surf Target surface
 * @param sprite_data Sprite graphics data
 * @param x X position
 * @param y Y position
 * @param width Sprite width
 * @param height Sprite height
 */
void PAL_Graphics_DrawSprite(PAL_Surface* surf, const void* sprite_data, 
                             int x, int y, int width, int height);

/**
 * Draw a tilemap to the specified surface
 * @param surf Target surface
 * @param tilemap_data Tilemap graphics data
 */
void PAL_Graphics_DrawTilemap(PAL_Surface* surf, const void* tilemap_data);

/**
 * Blit one surface to another
 * @param src Source surface
 * @param dst Destination surface
 * @param sx Source X
 * @param sy Source Y
 * @param w Width
 * @param h Height
 * @param dx Destination X
 * @param dy Destination Y
 */
void PAL_Graphics_BlitSurface(PAL_Surface* src, PAL_Surface* dst, 
                              int sx, int sy, int w, int h, int dx, int dy);

/**
 * Allocate VRAM (or VRAM-equivalent memory)
 * @param size Size in bytes
 * @return Pointer to allocated memory, or NULL on failure
 */
void* PAL_Graphics_AllocVRAM(size_t size);

/**
 * Free VRAM
 * @param ptr Pointer to free
 */
void PAL_Graphics_FreeVRAM(void* ptr);

/**
 * Transfer data to VRAM (on DS, actual DMA transfer; on SDL, memcpy)
 * @param src Source data
 * @param dst Destination in VRAM
 * @param size Size in bytes
 */
void PAL_Graphics_TransferVRAM(void* src, void* dst, size_t size);

#endif // PAL_GRAPHICS_H
