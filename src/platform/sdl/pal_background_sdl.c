/**
 * @file pal_background_sdl.c
 * @brief SDL3 implementation of background/tilemap system
 */

#include "platform/pal_background.h"
#include "platform/pal_memory.h"
#include "platform/pal_graphics.h"
#include "bg_window.h"  // Need full BgConfig definition
#include <SDL3/SDL.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Tile constants
#define TILE_SIZE 8  // DS uses 8x8 pixel tiles
#define TILES_PER_ROW_4BPP 64  // For 4bpp: 32 bytes per tile, 64 tiles per 2KB
#define TILES_PER_ROW_8BPP 32  // For 8bpp: 64 bytes per tile, 32 tiles per 2KB

// Tilemap entry format (16-bit)
// Bits 0-9: Tile index (1024 tiles)
// Bit 10: Horizontal flip
// Bit 11: Vertical flip
// Bits 12-15: Palette number (for 4bpp only)
#define TILEMAP_TILE_MASK     0x03FF
#define TILEMAP_HFLIP_MASK    0x0400
#define TILEMAP_VFLIP_MASK    0x0800
#define TILEMAP_PALETTE_SHIFT 12
#define TILEMAP_PALETTE_MASK  0xF000

// Internal PAL state for each background layer (extends game's Background struct)
typedef struct {
    BOOL enabled;           // Is this layer active?
    SDL_Texture* renderTexture;  // Cached rendered tilemap
    void* tileData;         // Tile graphics data
    u32 tileDataSize;       // Size of tile data buffer
    PAL_Palette palette;    // Layer palette (up to 16 sub-palettes for 4bpp)
    u16* paletteData;       // Palette data (RGB555 format) - kept for reference if needed
    BOOL dirty;             // Needs re-render?
    int texWidth;
    int texHeight;
} PAL_BgLayerState;

// Global state for all background layers
static PAL_BgLayerState g_palBgLayers[8] = {0};

// Helper functions
static void GetScreenDimensions(u8 screenSize, int* width, int* height);
static void RenderTilemap(Background* bg, PAL_BgLayerState* state);
static void DecodeTile4bpp(const u8* tileData, u8 paletteIdx, const PAL_Palette* palette, 
                           u32* output, BOOL hflip, BOOL vflip);
static void DecodeTile8bpp(const u8* tileData, const PAL_Palette* palette, 
                           u32* output, BOOL hflip, BOOL vflip);

// ============================================================================
// Initialization and Management
// ============================================================================

PAL_BgConfig* PAL_Bg_CreateConfig(u32 heapID) {
    // Allocate standard BgConfig
    PAL_BgConfig* config = (PAL_BgConfig*)PAL_Malloc(sizeof(BgConfig), heapID);
    if (!config) {
        return NULL;
    }
    
    memset(config, 0, sizeof(BgConfig));
    config->heapID = heapID;
    
    // Initialize all PAL layer states as disabled
    for (int i = 0; i < 8; i++) {
        g_palBgLayers[i].enabled = FALSE;
        g_palBgLayers[i].renderTexture = NULL;
        g_palBgLayers[i].tileData = NULL;
        g_palBgLayers[i].tileDataSize = 0;
        g_palBgLayers[i].paletteData = NULL;
        g_palBgLayers[i].dirty = TRUE;
        g_palBgLayers[i].texWidth = 0;
        g_palBgLayers[i].texHeight = 0;
        memset(&g_palBgLayers[i].palette, 0, sizeof(PAL_Palette));
    }
    
    return config;
}

void PAL_Bg_DestroyConfig(PAL_BgConfig* bgConfig) {
    if (!bgConfig) return;
    
    // Free all PAL layer resources
    for (int i = 0; i < 8; i++) {
        PAL_Bg_FreeTilemapBuffer(bgConfig, i);
        
        if (g_palBgLayers[i].tileData) {
            PAL_Free(g_palBgLayers[i].tileData);
            g_palBgLayers[i].tileData = NULL;
        }
        
        if (g_palBgLayers[i].renderTexture) {
            SDL_DestroyTexture(g_palBgLayers[i].renderTexture);
            g_palBgLayers[i].renderTexture = NULL;
        }
    }
    
    PAL_Free(bgConfig);
}

void PAL_Bg_InitFromTemplate(PAL_BgConfig* bgConfig, u8 bgLayer, 
                             const PAL_BgTemplate* bgTemplate, u8 bgType) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX || !bgTemplate) {
        return;
    }
    
    Background* bg = &bgConfig->bgs[bgLayer];
    PAL_BgLayerState* state = &g_palBgLayers[bgLayer];
    
    // Free existing resources
    if (bg->tilemapBuffer) {
        PAL_Free(bg->tilemapBuffer);
    }
    if (state->renderTexture) {
        SDL_DestroyTexture(state->renderTexture);
        state->renderTexture = NULL;
    }
    
    // Set parameters
    bg->type = bgType;
    bg->screenSize = bgTemplate->screenSize;
    bg->colorMode = bgTemplate->colorMode;
    bg->baseTile = bgTemplate->baseTile;
    
    state->enabled = TRUE;
    state->dirty = TRUE;
    
    // Allocate tilemap buffer
    bg->bufferSize = bgTemplate->bufferSize;
    if (bg->bufferSize > 0) {
        bg->tilemapBuffer = PAL_Malloc(bg->bufferSize, bgConfig->heapID);
        if (bg->tilemapBuffer) {
            memset(bg->tilemapBuffer, 0, bg->bufferSize);
        }
    }
    
    // Create render texture
    int width, height;
    GetScreenDimensions(bg->screenSize, &width, &height);
    state->texWidth = width;
    state->texHeight = height;
    
    SDL_Renderer* renderer = PAL_Graphics_GetRenderer();
    if (renderer) {
        state->renderTexture = SDL_CreateTexture(renderer, 
                                             SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             width, height);
        if (state->renderTexture) {
            SDL_SetTextureBlendMode(state->renderTexture, SDL_BLENDMODE_BLEND);
        }
    }
    
    // Reset offsets
    bg->xOffset = 0;
    bg->yOffset = 0;
}

void PAL_Bg_FreeTilemapBuffer(PAL_BgConfig* bgConfig, u8 bgLayer) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX) {
        return;
    }
    
    Background* bg = &bgConfig->bgs[bgLayer];
    
    if (bg->tilemapBuffer) {
        PAL_Free(bg->tilemapBuffer);
        bg->tilemapBuffer = NULL;
        bg->bufferSize = 0;
    }
}

void PAL_Bg_SetControlParam(PAL_BgConfig* bgConfig, u8 bgLayer, 
                            PAL_BgControlParam param, u8 value) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX) {
        return;
    }
    
    Background* bg = &bgConfig->bgs[bgLayer];
    PAL_BgLayerState* state = &g_palBgLayers[bgLayer];
    
    switch (param) {
        case PAL_BG_CONTROL_COLOR_MODE:
            bg->colorMode = value;
            state->dirty = TRUE;
            break;
        case PAL_BG_CONTROL_SCREEN_SIZE:
            bg->screenSize = value;
            state->dirty = TRUE;
            break;
        case PAL_BG_CONTROL_SCREEN_BASE:
        case PAL_BG_CONTROL_CHAR_BASE:
            // These are DS VRAM-specific, can be ignored for SDL
            break;
    }
}

// ============================================================================
// Layer Control
// ============================================================================

void PAL_Bg_SetPriority(u8 bgLayer, u8 priority) {
    // Priority is handled during rendering by layer order
    // This is a stub for DS compatibility
    (void)bgLayer;
    (void)priority;
}

void PAL_Bg_ToggleLayer(u8 bgLayer, BOOL enable) {
    if (bgLayer >= PAL_BG_LAYER_MAX) return;
    g_palBgLayers[bgLayer].enabled = enable;
}

// ============================================================================
// Scrolling
// ============================================================================

void PAL_Bg_SetOffset(PAL_BgConfig* bgConfig, u8 bgLayer, 
                      PAL_BgOffsetOp op, int value) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX) {
        return;
    }
    
    Background* bg = &bgConfig->bgs[bgLayer];
    
    switch (op) {
        case PAL_BG_OFFSET_SET_X:
            bg->xOffset = value;
            break;
        case PAL_BG_OFFSET_ADD_X:
            bg->xOffset += value;
            break;
        case PAL_BG_OFFSET_SUB_X:
            bg->xOffset -= value;
            break;
        case PAL_BG_OFFSET_SET_Y:
            bg->yOffset = value;
            break;
        case PAL_BG_OFFSET_ADD_Y:
            bg->yOffset += value;
            break;
        case PAL_BG_OFFSET_SUB_Y:
            bg->yOffset -= value;
            break;
    }
}

int PAL_Bg_GetXOffset(PAL_BgConfig* bgConfig, u8 bgLayer) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX) {
        return 0;
    }
    return bgConfig->bgs[bgLayer].xOffset;
}

int PAL_Bg_GetYOffset(PAL_BgConfig* bgConfig, u8 bgLayer) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX) {
        return 0;
    }
    return bgConfig->bgs[bgLayer].yOffset;
}

// ============================================================================
// Tilemap Operations
// ============================================================================

void PAL_Bg_LoadTilemapBuffer(PAL_BgConfig* bgConfig, u8 bgLayer, 
                              const void* src, u32 size) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX || !src) {
        return;
    }
    
    Background* bg = &bgConfig->bgs[bgLayer];
    PAL_BgLayerState* state = &g_palBgLayers[bgLayer];
    
    if (!bg->tilemapBuffer || size > bg->bufferSize) {
        return;
    }
    
    memcpy(bg->tilemapBuffer, src, size);
    state->dirty = TRUE;
}

void PAL_Bg_CopyTilemapBufferToVRAM(PAL_BgConfig* bgConfig, u8 bgLayer) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX) {
        return;
    }
    
    PAL_BgLayerState* state = &g_palBgLayers[bgLayer];
    
    // Mark for re-rendering
    state->dirty = TRUE;
}

void PAL_Bg_FillTilemap(PAL_BgConfig* bgConfig, u8 bgLayer, u16 fillVal) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX) {
        return;
    }
    
    Background* bg = &bgConfig->bgs[bgLayer];
    PAL_BgLayerState* state = &g_palBgLayers[bgLayer];
    
    if (!bg->tilemapBuffer) {
        return;
    }
    
    u16* tilemap = (u16*)bg->tilemapBuffer;
    u32 numEntries = bg->bufferSize / sizeof(u16);
    
    for (u32 i = 0; i < numEntries; i++) {
        tilemap[i] = fillVal;
    }
    
    state->dirty = TRUE;
}

void PAL_Bg_FillTilemapRect(PAL_BgConfig* bgConfig, u8 bgLayer, u16 fillVal,
                            u8 x, u8 y, u8 width, u8 height, u8 palette) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX) {
        return;
    }
    
    Background* bg = &bgConfig->bgs[bgLayer];
    PAL_BgLayerState* state = &g_palBgLayers[bgLayer];
    
    if (!bg->tilemapBuffer) {
        return;
    }
    
    // Get tilemap dimensions
    int screenWidth, screenHeight;
    GetScreenDimensions(bg->screenSize, &screenWidth, &screenHeight);
    int tilesX = screenWidth / TILE_SIZE;
    int tilesY = screenHeight / TILE_SIZE;
    
    u16* tilemap = (u16*)bg->tilemapBuffer;
    
    // Apply palette to fill value (for 4bpp mode)
    u16 tileEntry = fillVal | (palette << TILEMAP_PALETTE_SHIFT);
    
    for (u8 dy = 0; dy < height && (y + dy) < tilesY; dy++) {
        for (u8 dx = 0; dx < width && (x + dx) < tilesX; dx++) {
            int idx = (y + dy) * tilesX + (x + dx);
            tilemap[idx] = tileEntry;
        }
    }
    
    state->dirty = TRUE;
}

void PAL_Bg_ClearTilemap(PAL_BgConfig* bgConfig, u8 bgLayer) {
    PAL_Bg_FillTilemap(bgConfig, bgLayer, 0);
}

void* PAL_Bg_GetTilemapBuffer(PAL_BgConfig* bgConfig, u8 bgLayer) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX) {
        return NULL;
    }
    return bgConfig->bgs[bgLayer].tilemapBuffer;
}

// ============================================================================
// Tile/Character Data
// ============================================================================

void PAL_Bg_LoadTiles(PAL_BgConfig* bgConfig, u8 bgLayer, const void* src, 
                     u32 size, u32 tileStart) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX || !src) {
        return;
    }
    
    Background* bg = &bgConfig->bgs[bgLayer];
    PAL_BgLayerState* state = &g_palBgLayers[bgLayer];
    
    // Allocate or reallocate tile data buffer
    u32 neededSize = (tileStart * (bg->colorMode == PAL_BG_COLOR_MODE_4BPP ? 32 : 64)) + size;
    
    if (!state->tileData || state->tileDataSize < neededSize) {
        void* newBuffer = PAL_Malloc(neededSize, bgConfig->heapID);
        if (!newBuffer) return;
        
        if (state->tileData) {
            memcpy(newBuffer, state->tileData, state->tileDataSize);
            PAL_Free(state->tileData);
        }
        
        state->tileData = newBuffer;
        state->tileDataSize = neededSize;
    }
    
    // Copy tile data
    u32 offset = tileStart * (bg->colorMode == PAL_BG_COLOR_MODE_4BPP ? 32 : 64);
    memcpy((u8*)state->tileData + offset, src, size);
    
    state->dirty = TRUE;
}

void PAL_Bg_LoadTilesToVRAM(PAL_BgConfig* bgConfig, u8 bgLayer, 
                           const void* src, u32 size, u32 offset) {
    // For SDL, this is the same as LoadTiles
    PAL_Bg_LoadTiles(bgConfig, bgLayer, src, size, offset / (bgConfig->bgs[bgLayer].colorMode == PAL_BG_COLOR_MODE_4BPP ? 32 : 64));
}

void PAL_Bg_ClearTilesRange(u8 bgLayer, u32 size, u32 offset, u32 heapID) {
    // This would need BgConfig access, which we don't have here
    // For now, this is a stub
    (void)bgLayer;
    (void)size;
    (void)offset;
    (void)heapID;
}

// ============================================================================
// Palette
// ============================================================================

void PAL_Bg_LoadPalette(PAL_BgConfig* bgConfig, u8 bgLayer, const void* src, u16 size, u16 offset) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX || !src) {
        return;
    }
    
    PAL_BgLayerState* state = &g_palBgLayers[bgLayer];
    
    // Convert RGB555 palette to PAL_Color format
    const u16* rgb555Palette = (const u16*)src;
    u32 numColors = size / sizeof(u16);
    if (numColors > 256) numColors = 256;
    
    for (u32 i = 0; i < numColors; i++) {
        u32 palIdx = offset + i;
        if (palIdx < 256) {
            state->palette.colors[palIdx] = PAL_ColorFromRGB555(rgb555Palette[i]);
        }
    }
    
    // Update color count
    u32 maxIdx = offset + numColors;
    if (maxIdx > state->palette.num_colors) {
        state->palette.num_colors = maxIdx;
    }
    
    printf("[BG] LoadPalette: layer=%d, loaded %d colors at offset %d, total=%d colors\n",
           bgLayer, numColors, offset, state->palette.num_colors);
    
    state->dirty = TRUE;
}

void PAL_Bg_MaskPalette(u8 bgLayer, u16 mask) {
    // DS-specific hardware feature, not needed for SDL
    (void)bgLayer;
    (void)mask;
}

// ============================================================================
// Rendering
// ============================================================================

void PAL_Bg_RenderAll(PAL_BgConfig* bgConfig) {
    if (!bgConfig) return;
    
    SDL_Renderer* renderer = PAL_Graphics_GetRenderer();
    if (!renderer) return;
    
    // Render layers in priority order (lower priority = drawn first)
    // Note: Background struct doesn't have priority field in bg_window.h
    // We might need to store priority in PAL_BgLayerState if it's not in Background
    // For now, we'll just iterate 0-3 and assume priority is set somewhere or default
    
    // Since we can't easily get priority from Background struct if it's missing,
    // we'll just render in layer order for now, or check if we can find where priority is stored.
    // BgTemplate has priority. Maybe we should store it in PAL_BgLayerState.
    // Let's assume we added priority to PAL_BgLayerState.
    
    // For now, just render all enabled layers.
    for (int layer = 0; layer < PAL_BG_LAYER_MAX; layer++) {
        PAL_BgLayerState* state = &g_palBgLayers[layer];
        if (state->enabled) {
            PAL_Bg_RenderLayer(bgConfig, layer);
        }
    }
}

void PAL_Bg_RenderLayer(PAL_BgConfig* bgConfig, u8 bgLayer) {
    if (!bgConfig || bgLayer >= PAL_BG_LAYER_MAX) {
        return;
    }
    
    Background* bg = &bgConfig->bgs[bgLayer];
    PAL_BgLayerState* state = &g_palBgLayers[bgLayer];
    
    if (!state->enabled || !state->renderTexture) {
        return;
    }
    
    // Re-render tilemap if dirty
    if (state->dirty) {
        RenderTilemap(bg, state);
        state->dirty = FALSE;
    }
    
    // Determine which screen to render to
    SDL_Renderer* renderer = PAL_Graphics_GetRenderer();
    PAL_Screen targetScreen = (bgLayer < PAL_BG_LAYER_SUB_0) ? PAL_SCREEN_MAIN : PAL_SCREEN_SUB;
    
    // Get the target screen texture and set it as render target
    SDL_Texture* screenTexture = PAL_Graphics_GetScreenTexture(targetScreen);
    if (!screenTexture) return;
    
    SDL_SetRenderTarget(renderer, screenTexture);
    
    // Calculate destination rectangle with scrolling
    SDL_FRect dstRect;
    dstRect.x = (float)-bg->xOffset;
    dstRect.y = (float)-bg->yOffset;
    dstRect.w = (float)state->texWidth;
    dstRect.h = (float)state->texHeight;
    
    // Render the tilemap texture to the screen surface
    SDL_RenderTexture(renderer, state->renderTexture, NULL, &dstRect);
    
    // Reset render target
    SDL_SetRenderTarget(renderer, NULL);
}

// ============================================================================
// Helper Functions
// ============================================================================

static void GetScreenDimensions(u8 screenSize, int* width, int* height) {
    switch (screenSize) {
        case PAL_BG_SCREEN_SIZE_128x128:
            *width = 128; *height = 128;
            break;
        case PAL_BG_SCREEN_SIZE_256x256:
            *width = 256; *height = 256;
            break;
        case PAL_BG_SCREEN_SIZE_256x512:
            *width = 256; *height = 512;
            break;
        case PAL_BG_SCREEN_SIZE_512x256:
            *width = 512; *height = 256;
            break;
        case PAL_BG_SCREEN_SIZE_512x512:
            *width = 512; *height = 512;
            break;
        case PAL_BG_SCREEN_SIZE_1024x1024:
            *width = 1024; *height = 1024;
            break;
        default:
            *width = 256; *height = 256;
            break;
    }
}

static void RenderTilemap(Background* bg, PAL_BgLayerState* state) {
    if (!state->renderTexture || !bg->tilemapBuffer || !state->tileData) {
        // printf("[BG] RenderTilemap failed: texture=%p, tilemap=%p, tiles=%p\n",
        //        state->renderTexture, bg->tilemapBuffer, state->tileData);
        return;
    }
    
    // Lock texture for writing
    void* pixels;
    int pitch;
    if (!SDL_LockTexture(state->renderTexture, NULL, &pixels, &pitch)) {
        printf("[BG] Failed to lock texture: %s\n", SDL_GetError());
        return;
    }
    
    // Clear texture
    memset(pixels, 0, pitch * state->texHeight);
    
    // Get tilemap dimensions
    int tilesX = state->texWidth / TILE_SIZE;
    int tilesY = state->texHeight / TILE_SIZE;
    
    u16* tilemap = (u16*)bg->tilemapBuffer;
    u32 tileBuffer[TILE_SIZE * TILE_SIZE];  // RGBA buffer for one tile
    
    // Render each tile
    for (int ty = 0; ty < tilesY; ty++) {
        for (int tx = 0; tx < tilesX; tx++) {
            u16 tileEntry = tilemap[ty * tilesX + tx];
            
            u16 tileIdx = tileEntry & TILEMAP_TILE_MASK;
            BOOL hflip = (tileEntry & TILEMAP_HFLIP_MASK) != 0;
            BOOL vflip = (tileEntry & TILEMAP_VFLIP_MASK) != 0;
            u8 paletteIdx = (tileEntry & TILEMAP_PALETTE_MASK) >> TILEMAP_PALETTE_SHIFT;
            
            // Decode tile
            if (bg->colorMode == PAL_BG_COLOR_MODE_4BPP) {
                const u8* tileData = (const u8*)state->tileData + (tileIdx * 32);
                DecodeTile4bpp(tileData, paletteIdx, &state->palette, tileBuffer, hflip, vflip);
            } else {
                const u8* tileData = (const u8*)state->tileData + (tileIdx * 64);
                DecodeTile8bpp(tileData, &state->palette, tileBuffer, hflip, vflip);
            }
            
            // Copy tile to texture
            int destX = tx * TILE_SIZE;
            int destY = ty * TILE_SIZE;
            
            for (int py = 0; py < TILE_SIZE; py++) {
                u32* destRow = (u32*)((u8*)pixels + (destY + py) * pitch);
                memcpy(&destRow[destX], &tileBuffer[py * TILE_SIZE], TILE_SIZE * sizeof(u32));
            }
        }
    }
    
    SDL_UnlockTexture(state->renderTexture);
}

static void DecodeTile4bpp(const u8* tileData, u8 paletteIdx, const PAL_Palette* palette, 
                           u32* output, BOOL hflip, BOOL vflip) {
    // 4bpp: 4 bits per pixel, 2 pixels per byte
    // Tile size: 8x8 pixels = 32 bytes
    
    int paletteOffset = paletteIdx * 16;  // 16 colors per sub-palette
    
    for (int y = 0; y < TILE_SIZE; y++) {
        for (int x = 0; x < TILE_SIZE; x++) {
            int srcX = hflip ? (TILE_SIZE - 1 - x) : x;
            int srcY = vflip ? (TILE_SIZE - 1 - y) : y;
            
            int byteIdx = srcY * 4 + srcX / 2;  // 4 bytes per row
            int nibble = srcX & 1;
            
            u8 colorIdx = (tileData[byteIdx] >> (nibble * 4)) & 0x0F;
            
            // Color index 0 is transparent
            if (colorIdx == 0) {
                output[y * TILE_SIZE + x] = 0x00000000;
            } else {
                int palIdx = paletteOffset + colorIdx;
                if (palIdx < 256) {
                    PAL_Color color = palette->colors[palIdx];
                    output[y * TILE_SIZE + x] = (color.a << 24) | (color.b << 16) | 
                                               (color.g << 8) | color.r;
                } else {
                    output[y * TILE_SIZE + x] = 0xFF00FF00;  // Magenta for out-of-bounds
                }
            }
        }
    }
}

static void DecodeTile8bpp(const u8* tileData, const PAL_Palette* palette, 
                           u32* output, BOOL hflip, BOOL vflip) {
    // 8bpp: 8 bits per pixel, 1 pixel per byte
    // Tile size: 8x8 pixels = 64 bytes
    
    for (int y = 0; y < TILE_SIZE; y++) {
        for (int x = 0; x < TILE_SIZE; x++) {
            int srcX = hflip ? (TILE_SIZE - 1 - x) : x;
            int srcY = vflip ? (TILE_SIZE - 1 - y) : y;
            
            u8 colorIdx = tileData[srcY * TILE_SIZE + srcX];
            
            // Color index 0 is transparent
            if (colorIdx == 0) {
                output[y * TILE_SIZE + x] = 0x00000000;
            } else {
                PAL_Color color = palette->colors[colorIdx];
                output[y * TILE_SIZE + x] = (color.a << 24) | (color.b << 16) | 
                                           (color.g << 8) | color.r;
            }
        }
    }
}
