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
    SDL_SetRenderTarget(g_graphics.renderer, NULL);
    SDL_SetRenderDrawColor(g_graphics.renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_graphics.renderer);
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
    // TODO: Implement sprite rendering
    // This requires converting DS sprite format to SDL textures
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

#endif // PLATFORM_SDL
