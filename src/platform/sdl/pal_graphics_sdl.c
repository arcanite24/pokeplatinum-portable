/**
 * @file pal_graphics_sdl.c
 * @brief SDL3 implementation of graphics abstraction layer
 */

#include "platform/pal_graphics.h"

#ifdef PLATFORM_SDL

#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

// Internal structure for SDL surface
struct PAL_Surface {
    SDL_Texture* texture;
    int x, y;
    int width, height;
    PAL_Screen screen;
};

// Global state
static struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    PAL_Surface screens[PAL_SCREEN_MAX];
    PAL_Palette default_palette;
    BOOL initialized;
} g_graphics;

BOOL PAL_Graphics_Init(int window_width, int window_height) {
    if (g_graphics.initialized) {
        return TRUE;
    }
    
    // Create window
    g_graphics.window = SDL_CreateWindow(
        "Pokemon Platinum",
        window_width, window_height,
        SDL_WINDOW_RESIZABLE
    );
    
    if (!g_graphics.window) {
        return FALSE;
    }
    
    // Create renderer
    g_graphics.renderer = SDL_CreateRenderer(
        g_graphics.window,
        NULL
    );
    
    if (!g_graphics.renderer) {
        SDL_DestroyWindow(g_graphics.window);
        return FALSE;
    }
    
    // Initialize screen surfaces
    // Side-by-side layout by default
    for (int i = 0; i < PAL_SCREEN_MAX; i++) {
        g_graphics.screens[i].texture = SDL_CreateTexture(
            g_graphics.renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            PAL_SCREEN_WIDTH,
            PAL_SCREEN_HEIGHT
        );
        
        if (!g_graphics.screens[i].texture) {
            PAL_Graphics_Shutdown();
            return FALSE;
        }
        
        g_graphics.screens[i].x = i * PAL_SCREEN_WIDTH;
        g_graphics.screens[i].y = 0;
        g_graphics.screens[i].width = PAL_SCREEN_WIDTH;
        g_graphics.screens[i].height = PAL_SCREEN_HEIGHT;
        g_graphics.screens[i].screen = (PAL_Screen)i;
    }
    
    g_graphics.initialized = TRUE;
    return TRUE;
}

void PAL_Graphics_Shutdown(void) {
    if (!g_graphics.initialized) {
        return;
    }
    
    for (int i = 0; i < PAL_SCREEN_MAX; i++) {
        if (g_graphics.screens[i].texture) {
            SDL_DestroyTexture(g_graphics.screens[i].texture);
            g_graphics.screens[i].texture = NULL;
        }
    }
    
    if (g_graphics.renderer) {
        SDL_DestroyRenderer(g_graphics.renderer);
        g_graphics.renderer = NULL;
    }
    
    if (g_graphics.window) {
        SDL_DestroyWindow(g_graphics.window);
        g_graphics.window = NULL;
    }
    
    g_graphics.initialized = FALSE;
}

PAL_Surface* PAL_Graphics_GetScreen(PAL_Screen screen) {
    if (screen >= PAL_SCREEN_MAX) {
        return NULL;
    }
    return &g_graphics.screens[screen];
}

void PAL_Graphics_SetActiveScreen(PAL_Screen screen) {
    if (screen >= PAL_SCREEN_MAX) {
        return;
    }
    SDL_SetRenderTarget(g_graphics.renderer, g_graphics.screens[screen].texture);
}

void PAL_Graphics_BeginFrame(void) {
    // Clear the main window
    SDL_SetRenderTarget(g_graphics.renderer, NULL);
    SDL_SetRenderDrawColor(g_graphics.renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_graphics.renderer);
    
    // Clear both screen textures
    for (int i = 0; i < PAL_SCREEN_MAX; i++) {
        SDL_SetRenderTarget(g_graphics.renderer, g_graphics.screens[i].texture);
        SDL_SetRenderDrawColor(g_graphics.renderer, 0, 0, 0, 255);
        SDL_RenderClear(g_graphics.renderer);
    }
    
    SDL_SetRenderTarget(g_graphics.renderer, NULL);
}

void PAL_Graphics_EndFrame(void) {
    SDL_SetRenderTarget(g_graphics.renderer, NULL);
    
    // Render both screens
    for (int i = 0; i < PAL_SCREEN_MAX; i++) {
        SDL_FRect dest = {
            .x = (float)g_graphics.screens[i].x,
            .y = (float)g_graphics.screens[i].y,
            .w = (float)g_graphics.screens[i].width,
            .h = (float)g_graphics.screens[i].height
        };
        SDL_RenderTexture(g_graphics.renderer, g_graphics.screens[i].texture, NULL, &dest);
    }
    
    SDL_RenderPresent(g_graphics.renderer);
}

void PAL_Graphics_DrawSprite(PAL_Surface* surf, const void* sprite_data, 
                             int x, int y, int width, int height) {
    if (!surf || !sprite_data || !g_graphics.renderer) {
        return;
    }
    
    // For now, assume sprite_data is RGBA8888 format
    // In Phase 2.3, we'll add proper DS sprite format conversion
    SDL_Texture* temp_texture = SDL_CreateTexture(
        g_graphics.renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STATIC,
        width,
        height
    );
    
    if (!temp_texture) {
        return;
    }
    
    // Upload sprite data
    SDL_UpdateTexture(temp_texture, NULL, sprite_data, width * 4);
    
    // Set render target to the surface's texture
    SDL_SetRenderTarget(g_graphics.renderer, surf->texture);
    
    // Draw sprite at position
    SDL_FRect dest = {
        .x = (float)x,
        .y = (float)y,
        .w = (float)width,
        .h = (float)height
    };
    SDL_RenderTexture(g_graphics.renderer, temp_texture, NULL, &dest);
    
    // Cleanup
    SDL_DestroyTexture(temp_texture);
}

void PAL_Graphics_DrawTilemap(PAL_Surface* surf, const void* tilemap_data) {
    // TODO: Implement tilemap rendering
    // This requires converting DS tilemap format
}

void PAL_Graphics_BlitSurface(PAL_Surface* src, PAL_Surface* dst, 
                              int sx, int sy, int w, int h, int dx, int dy) {
    // TODO: Implement surface blitting
}

void* PAL_Graphics_AllocVRAM(size_t size) {
    // On SDL, just use regular malloc
    return malloc(size);
}

void PAL_Graphics_FreeVRAM(void* ptr) {
    free(ptr);
}

void PAL_Graphics_TransferVRAM(void* src, void* dst, size_t size) {
    // On SDL, just memcpy
    memcpy(dst, src, size);
}

// ============================================================================
// Extended Graphics Functions (Phase 2)
// ============================================================================

/**
 * Create a texture from RGBA pixel data
 */
SDL_Texture* PAL_Graphics_CreateTexture(int width, int height, const void* pixels) {
    if (!g_graphics.renderer) {
        return NULL;
    }
    
    SDL_Texture* texture = SDL_CreateTexture(
        g_graphics.renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STATIC,
        width,
        height
    );
    
    if (texture && pixels) {
        SDL_UpdateTexture(texture, NULL, pixels, width * 4);
    }
    
    return texture;
}

/**
 * Create a texture with streaming access (for dynamic updates)
 */
SDL_Texture* PAL_Graphics_CreateStreamingTexture(int width, int height) {
    if (!g_graphics.renderer) {
        return NULL;
    }
    
    return SDL_CreateTexture(
        g_graphics.renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );
}

/**
 * Destroy a texture
 */
void PAL_Graphics_DestroyTexture(SDL_Texture* texture) {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

/**
 * Draw a texture to a surface
 */
void PAL_Graphics_DrawTexture(PAL_Surface* surf, SDL_Texture* texture,
                              int src_x, int src_y, int src_w, int src_h,
                              int dst_x, int dst_y, int dst_w, int dst_h) {
    if (!surf || !texture || !g_graphics.renderer) {
        return;
    }
    
    SDL_SetRenderTarget(g_graphics.renderer, surf->texture);
    
    SDL_FRect src = { (float)src_x, (float)src_y, (float)src_w, (float)src_h };
    SDL_FRect dst = { (float)dst_x, (float)dst_y, (float)dst_w, (float)dst_h };
    
    SDL_RenderTexture(g_graphics.renderer, texture, &src, &dst);
}

/**
 * Clear a surface to a specific color
 */
void PAL_Graphics_ClearSurface(PAL_Surface* surf, u8 r, u8 g, u8 b, u8 a) {
    if (!surf || !g_graphics.renderer) {
        return;
    }
    
    SDL_SetRenderTarget(g_graphics.renderer, surf->texture);
    SDL_SetRenderDrawColor(g_graphics.renderer, r, g, b, a);
    SDL_RenderClear(g_graphics.renderer);
}

/**
 * Draw a filled rectangle
 */
void PAL_Graphics_FillRect(PAL_Surface* surf, int x, int y, int w, int h,
                           u8 r, u8 g, u8 b, u8 a) {
    if (!surf || !g_graphics.renderer) {
        return;
    }
    
    SDL_SetRenderTarget(g_graphics.renderer, surf->texture);
    SDL_SetRenderDrawColor(g_graphics.renderer, r, g, b, a);
    
    SDL_FRect rect = { (float)x, (float)y, (float)w, (float)h };
    SDL_RenderFillRect(g_graphics.renderer, &rect);
}

/**
 * Draw a line
 */
void PAL_Graphics_DrawLine(PAL_Surface* surf, int x1, int y1, int x2, int y2,
                           u8 r, u8 g, u8 b, u8 a) {
    if (!surf || !g_graphics.renderer) {
        return;
    }
    
    SDL_SetRenderTarget(g_graphics.renderer, surf->texture);
    SDL_SetRenderDrawColor(g_graphics.renderer, r, g, b, a);
    SDL_RenderLine(g_graphics.renderer, (float)x1, (float)y1, (float)x2, (float)y2);
}

/**
 * Set a palette for indexed color rendering
 */
void PAL_Graphics_SetPalette(const u16* palette_rgb555, int num_colors) {
    if (!palette_rgb555 || num_colors <= 0 || num_colors > 256) {
        return;
    }
    
    g_graphics.default_palette.num_colors = num_colors;
    for (int i = 0; i < num_colors; i++) {
        g_graphics.default_palette.colors[i] = PAL_ColorFromRGB555(palette_rgb555[i]);
    }
}

/**
 * Get the current renderer (for advanced SDL operations)
 */
SDL_Renderer* PAL_Graphics_GetRenderer(void) {
    return g_graphics.renderer;
}

/**
 * Get the main window (for advanced SDL operations)
 */
SDL_Window* PAL_Graphics_GetWindow(void) {
    return g_graphics.window;
}

/**
 * Get the screen texture (for rendering backgrounds to screens)
 */
SDL_Texture* PAL_Graphics_GetScreenTexture(PAL_Screen screen) {
    if (screen >= PAL_SCREEN_MAX) return NULL;
    return g_graphics.screens[screen].texture;
}

#endif // PLATFORM_SDL
