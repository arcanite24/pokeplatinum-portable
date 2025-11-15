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

// ============================================================================
// Extended Graphics Functions (Phase 2) - SDL3 specific helpers
// ============================================================================

#ifdef PLATFORM_SDL
#include <SDL3/SDL.h>

/**
 * Create a texture from RGBA pixel data
 * @param width Texture width
 * @param height Texture height
 * @param pixels RGBA8888 pixel data (can be NULL to create empty texture)
 * @return SDL texture handle, or NULL on failure
 */
SDL_Texture* PAL_Graphics_CreateTexture(int width, int height, const void* pixels);

/**
 * Create a streaming texture for dynamic updates
 * @param width Texture width
 * @param height Texture height
 * @return SDL texture handle, or NULL on failure
 */
SDL_Texture* PAL_Graphics_CreateStreamingTexture(int width, int height);

/**
 * Destroy a texture
 * @param texture Texture to destroy
 */
void PAL_Graphics_DestroyTexture(SDL_Texture* texture);

/**
 * Draw a texture to a surface with source and destination rectangles
 * @param surf Target surface
 * @param texture Source texture
 * @param src_x Source X
 * @param src_y Source Y
 * @param src_w Source width
 * @param src_h Source height
 * @param dst_x Destination X
 * @param dst_y Destination Y
 * @param dst_w Destination width
 * @param dst_h Destination height
 */
void PAL_Graphics_DrawTexture(PAL_Surface* surf, SDL_Texture* texture,
                              int src_x, int src_y, int src_w, int src_h,
                              int dst_x, int dst_y, int dst_w, int dst_h);

/**
 * Clear a surface to a specific color
 * @param surf Surface to clear
 * @param r Red (0-255)
 * @param g Green (0-255)
 * @param b Blue (0-255)
 * @param a Alpha (0-255)
 */
void PAL_Graphics_ClearSurface(PAL_Surface* surf, u8 r, u8 g, u8 b, u8 a);

/**
 * Draw a filled rectangle
 * @param surf Target surface
 * @param x X position
 * @param y Y position
 * @param w Width
 * @param h Height
 * @param r Red (0-255)
 * @param g Green (0-255)
 * @param b Blue (0-255)
 * @param a Alpha (0-255)
 */
void PAL_Graphics_FillRect(PAL_Surface* surf, int x, int y, int w, int h,
                           u8 r, u8 g, u8 b, u8 a);

/**
 * Draw a line
 * @param surf Target surface
 * @param x1 Start X
 * @param y1 Start Y
 * @param x2 End X
 * @param y2 End Y
 * @param r Red (0-255)
 * @param g Green (0-255)
 * @param b Blue (0-255)
 * @param a Alpha (0-255)
 */
void PAL_Graphics_DrawLine(PAL_Surface* surf, int x1, int y1, int x2, int y2,
                           u8 r, u8 g, u8 b, u8 a);

/**
 * Set the palette for indexed color rendering
 * @param palette_rgb555 Array of RGB555 colors (DS format)
 * @param num_colors Number of colors in palette (max 256)
 */
void PAL_Graphics_SetPalette(const u16* palette_rgb555, int num_colors);

/**
 * Get the SDL renderer (for advanced operations)
 * @return SDL renderer handle
 */
SDL_Renderer* PAL_Graphics_GetRenderer(void);

/**
 * Get the SDL window (for advanced operations)
 * @return SDL window handle
 */
SDL_Window* PAL_Graphics_GetWindow(void);

#endif // PLATFORM_SDL

#endif // PAL_GRAPHICS_H
