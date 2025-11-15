/**
 * @file pal_sprite_sdl.c
 * @brief SDL3 implementation of sprite/OAM system
 */

#include "platform/pal_sprite.h"
#include "platform/pal_memory.h"
#include "platform/pal_graphics.h"
#include <SDL3/SDL.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Maximum sprites per manager (DS has 128 per screen)
#define MAX_SPRITES_DEFAULT 256

/**
 * @brief Single sprite structure
 */
struct PAL_Sprite {
    // Graphics data
    void* graphics_data;
    u32 graphics_size;
    PAL_Palette palette;
    
    // Sprite properties
    PAL_SpriteSize size;
    PAL_SpriteColorMode color_mode;
    int width, height;
    
    // Position and transform
    int x, y;
    float rotation;           // Degrees
    float scale_x, scale_y;
    BOOL affine_enabled;
    
    // Rendering properties
    u8 priority;              // 0-3
    u8 palette_num;           // For 4bpp
    u8 alpha;                 // 0-255
    PAL_SpriteFlip flip;
    PAL_SpriteBlendMode blend_mode;
    PAL_Screen screen;
    
    // State
    BOOL visible;
    BOOL active;              // Slot in use
    int frame;                // Current animation frame
    
    // Rendering cache
    SDL_Texture* texture;
    BOOL texture_dirty;
    
    // Manager link
    PAL_SpriteManager* manager;
    int index;                // Index in manager's array
};

/**
 * @brief Sprite manager structure
 */
struct PAL_SpriteManager {
    PAL_Sprite* sprites;
    int max_sprites;
    int active_count;
    u32 heapID;
    
    // Rendering
    SDL_Renderer* renderer;
    
    // Sorted list for priority rendering
    PAL_Sprite** render_order;
    BOOL render_order_dirty;
};

// Helper functions
static void GetSpriteDimensions(PAL_SpriteSize size, int* width, int* height);
static u32 GetSpriteDataSize(PAL_SpriteSize size, PAL_SpriteColorMode color_mode);
static void RenderSpriteTexture(PAL_Sprite* sprite);
static void DecodeSpriteGraphics4bpp(const u8* src, u8 palette_num, const PAL_Palette* palette,
                                     int width, int height, PAL_SpriteFlip flip, u32* output);
static void DecodeSpriteGraphics8bpp(const u8* src, const PAL_Palette* palette,
                                     int width, int height, PAL_SpriteFlip flip, u32* output);
static int CompareSpritesByPriority(const void* a, const void* b);

// ============================================================================
// Sprite Manager
// ============================================================================

PAL_SpriteManager* PAL_Sprite_CreateManager(int max_sprites, u32 heapID) {
    if (max_sprites <= 0) {
        max_sprites = MAX_SPRITES_DEFAULT;
    }
    
    PAL_SpriteManager* manager = (PAL_SpriteManager*)PAL_Malloc(
        sizeof(PAL_SpriteManager), heapID);
    if (!manager) {
        return NULL;
    }
    
    manager->sprites = (PAL_Sprite*)PAL_Malloc(
        sizeof(PAL_Sprite) * max_sprites, heapID);
    if (!manager->sprites) {
        PAL_Free(manager);
        return NULL;
    }
    
    manager->render_order = (PAL_Sprite**)PAL_Malloc(
        sizeof(PAL_Sprite*) * max_sprites, heapID);
    if (!manager->render_order) {
        PAL_Free(manager->sprites);
        PAL_Free(manager);
        return NULL;
    }
    
    // Initialize all sprites as inactive
    memset(manager->sprites, 0, sizeof(PAL_Sprite) * max_sprites);
    for (int i = 0; i < max_sprites; i++) {
        manager->sprites[i].active = FALSE;
        manager->sprites[i].manager = manager;
        manager->sprites[i].index = i;
    }
    
    manager->max_sprites = max_sprites;
    manager->active_count = 0;
    manager->heapID = heapID;
    manager->renderer = PAL_Graphics_GetRenderer();
    manager->render_order_dirty = TRUE;
    
    return manager;
}

void PAL_Sprite_DestroyManager(PAL_SpriteManager* manager) {
    if (!manager) return;
    
    // Destroy all sprites first
    for (int i = 0; i < manager->max_sprites; i++) {
        PAL_Sprite* sprite = &manager->sprites[i];
        if (sprite->active) {
            if (sprite->graphics_data) {
                PAL_Free(sprite->graphics_data);
            }
            if (sprite->texture) {
                SDL_DestroyTexture(sprite->texture);
            }
        }
    }
    
    PAL_Free(manager->render_order);
    PAL_Free(manager->sprites);
    PAL_Free(manager);
}

int PAL_Sprite_GetMaxCount(PAL_SpriteManager* manager) {
    return manager ? manager->max_sprites : 0;
}

int PAL_Sprite_GetActiveCount(PAL_SpriteManager* manager) {
    return manager ? manager->active_count : 0;
}

// ============================================================================
// Sprite Creation and Destruction
// ============================================================================

PAL_Sprite* PAL_Sprite_Create(PAL_SpriteManager* manager, 
                              const PAL_SpriteTemplate* template) {
    if (!manager || !template) {
        return NULL;
    }
    
    // Find free sprite slot
    PAL_Sprite* sprite = NULL;
    for (int i = 0; i < manager->max_sprites; i++) {
        if (!manager->sprites[i].active) {
            sprite = &manager->sprites[i];
            break;
        }
    }
    
    if (!sprite) {
        return NULL; // No free slots
    }
    
    // Initialize sprite
    memset(sprite, 0, sizeof(PAL_Sprite));
    sprite->active = TRUE;
    sprite->manager = manager;
    sprite->index = sprite - manager->sprites;
    
    // Copy graphics data
    if (template->graphics_data && template->graphics_size > 0) {
        sprite->graphics_data = PAL_Malloc(template->graphics_size, manager->heapID);
        if (sprite->graphics_data) {
            memcpy(sprite->graphics_data, template->graphics_data, template->graphics_size);
            sprite->graphics_size = template->graphics_size;
        }
    }
    
    // Convert palette
    if (template->palette_data && template->palette_size > 0) {
        const u16* rgb555_palette = (const u16*)template->palette_data;
        u32 num_colors = template->palette_size / sizeof(u16);
        if (num_colors > 256) num_colors = 256;
        
        for (u32 i = 0; i < num_colors; i++) {
            sprite->palette.colors[i] = PAL_ColorFromRGB555(rgb555_palette[i]);
        }
        sprite->palette.num_colors = num_colors;
    }
    
    // Set properties
    sprite->size = template->size;
    sprite->color_mode = template->color_mode;
    GetSpriteDimensions(sprite->size, &sprite->width, &sprite->height);
    
    sprite->x = template->x;
    sprite->y = template->y;
    sprite->priority = template->priority;
    sprite->palette_num = template->palette_num;
    sprite->screen = template->screen;
    
    // Default values
    sprite->rotation = 0.0f;
    sprite->scale_x = 1.0f;
    sprite->scale_y = 1.0f;
    sprite->affine_enabled = FALSE;
    sprite->alpha = 255;
    sprite->flip = PAL_SPRITE_FLIP_NONE;
    sprite->blend_mode = PAL_SPRITE_BLEND_NONE;
    sprite->visible = TRUE;
    sprite->frame = 0;
    
    // Create SDL texture
    if (manager->renderer) {
        sprite->texture = SDL_CreateTexture(manager->renderer,
                                           SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           sprite->width, sprite->height);
        if (sprite->texture) {
            SDL_SetTextureBlendMode(sprite->texture, SDL_BLENDMODE_BLEND);
        }
    }
    
    sprite->texture_dirty = TRUE;
    manager->active_count++;
    manager->render_order_dirty = TRUE;
    
    return sprite;
}

void PAL_Sprite_Destroy(PAL_SpriteManager* manager, PAL_Sprite* sprite) {
    if (!manager || !sprite || !sprite->active) {
        return;
    }
    
    if (sprite->graphics_data) {
        PAL_Free(sprite->graphics_data);
    }
    
    if (sprite->texture) {
        SDL_DestroyTexture(sprite->texture);
    }
    
    sprite->active = FALSE;
    manager->active_count--;
    manager->render_order_dirty = TRUE;
}

void PAL_Sprite_DestroyAll(PAL_SpriteManager* manager) {
    if (!manager) return;
    
    for (int i = 0; i < manager->max_sprites; i++) {
        if (manager->sprites[i].active) {
            PAL_Sprite_Destroy(manager, &manager->sprites[i]);
        }
    }
}

// ============================================================================
// Sprite Properties
// ============================================================================

void PAL_Sprite_SetPosition(PAL_Sprite* sprite, int x, int y) {
    if (!sprite || !sprite->active) return;
    sprite->x = x;
    sprite->y = y;
}

void PAL_Sprite_GetPosition(const PAL_Sprite* sprite, int* x, int* y) {
    if (!sprite || !sprite->active) return;
    if (x) *x = sprite->x;
    if (y) *y = sprite->y;
}

void PAL_Sprite_SetVisible(PAL_Sprite* sprite, BOOL visible) {
    if (!sprite || !sprite->active) return;
    sprite->visible = visible;
}

BOOL PAL_Sprite_GetVisible(const PAL_Sprite* sprite) {
    return (sprite && sprite->active) ? sprite->visible : FALSE;
}

void PAL_Sprite_SetPriority(PAL_Sprite* sprite, u8 priority) {
    if (!sprite || !sprite->active) return;
    if (priority > 3) priority = 3;
    sprite->priority = priority;
    if (sprite->manager) {
        sprite->manager->render_order_dirty = TRUE;
    }
}

u8 PAL_Sprite_GetPriority(const PAL_Sprite* sprite) {
    return (sprite && sprite->active) ? sprite->priority : 0;
}

void PAL_Sprite_SetFlip(PAL_Sprite* sprite, PAL_SpriteFlip flip) {
    if (!sprite || !sprite->active) return;
    sprite->flip = flip;
    sprite->texture_dirty = TRUE;
}

void PAL_Sprite_SetPalette(PAL_Sprite* sprite, u8 palette_num) {
    if (!sprite || !sprite->active) return;
    sprite->palette_num = palette_num;
    sprite->texture_dirty = TRUE;
}

void PAL_Sprite_SetAlpha(PAL_Sprite* sprite, u8 alpha) {
    if (!sprite || !sprite->active) return;
    sprite->alpha = alpha;
    if (sprite->texture) {
        SDL_SetTextureAlphaMod(sprite->texture, alpha);
    }
}

void PAL_Sprite_SetBlendMode(PAL_Sprite* sprite, PAL_SpriteBlendMode blend_mode) {
    if (!sprite || !sprite->active || !sprite->texture) return;
    
    sprite->blend_mode = blend_mode;
    
    SDL_BlendMode sdl_blend;
    switch (blend_mode) {
        case PAL_SPRITE_BLEND_ALPHA:
            sdl_blend = SDL_BLENDMODE_BLEND;
            break;
        case PAL_SPRITE_BLEND_ADD:
            sdl_blend = SDL_BLENDMODE_ADD;
            break;
        case PAL_SPRITE_BLEND_SUB:
            sdl_blend = SDL_BLENDMODE_MUL;  // SDL doesn't have subtract, use multiply
            break;
        default:
            sdl_blend = SDL_BLENDMODE_NONE;
            break;
    }
    
    SDL_SetTextureBlendMode(sprite->texture, sdl_blend);
}

// ============================================================================
// Affine Transformations
// ============================================================================

void PAL_Sprite_SetRotation(PAL_Sprite* sprite, float angle) {
    if (!sprite || !sprite->active) return;
    sprite->rotation = fmodf(angle, 360.0f);
    if (sprite->rotation < 0) sprite->rotation += 360.0f;
}

void PAL_Sprite_SetScale(PAL_Sprite* sprite, float scale_x, float scale_y) {
    if (!sprite || !sprite->active) return;
    sprite->scale_x = scale_x;
    sprite->scale_y = scale_y;
}

void PAL_Sprite_SetAffine(PAL_Sprite* sprite, float angle, 
                         float scale_x, float scale_y) {
    if (!sprite || !sprite->active) return;
    PAL_Sprite_SetRotation(sprite, angle);
    PAL_Sprite_SetScale(sprite, scale_x, scale_y);
    sprite->affine_enabled = TRUE;
}

void PAL_Sprite_EnableAffine(PAL_Sprite* sprite, BOOL enable) {
    if (!sprite || !sprite->active) return;
    sprite->affine_enabled = enable;
}

// ============================================================================
// Animation
// ============================================================================

void PAL_Sprite_SetFrame(PAL_Sprite* sprite, int frame) {
    if (!sprite || !sprite->active) return;
    sprite->frame = frame;
    sprite->texture_dirty = TRUE;
}

int PAL_Sprite_GetFrame(const PAL_Sprite* sprite) {
    return (sprite && sprite->active) ? sprite->frame : 0;
}

// ============================================================================
// Rendering
// ============================================================================

void PAL_Sprite_UpdateAll(PAL_SpriteManager* manager) {
    if (!manager) return;
    
    // Update render order if dirty
    if (manager->render_order_dirty) {
        int count = 0;
        for (int i = 0; i < manager->max_sprites; i++) {
            if (manager->sprites[i].active && manager->sprites[i].visible) {
                manager->render_order[count++] = &manager->sprites[i];
            }
        }
        
        // Sort by priority (lower priority = rendered first/behind)
        qsort(manager->render_order, count, sizeof(PAL_Sprite*), 
              CompareSpritesByPriority);
        
        manager->render_order_dirty = FALSE;
    }
}

void PAL_Sprite_RenderAll(PAL_SpriteManager* manager) {
    if (!manager || !manager->renderer) return;
    
    // Ensure render order is up to date
    PAL_Sprite_UpdateAll(manager);
    
    // Render all visible sprites in priority order
    for (int i = 0; i < manager->max_sprites; i++) {
        PAL_Sprite* sprite = manager->render_order[i];
        if (!sprite || !sprite->active || !sprite->visible) {
            break; // End of active sprites
        }
        
        PAL_Sprite_Render(sprite);
    }
}

void PAL_Sprite_Render(const PAL_Sprite* sprite) {
    if (!sprite || !sprite->active || !sprite->visible || !sprite->texture) {
        return;
    }
    
    // Update texture if dirty
    if (sprite->texture_dirty) {
        RenderSpriteTexture((PAL_Sprite*)sprite);
        ((PAL_Sprite*)sprite)->texture_dirty = FALSE;
    }
    
    SDL_Renderer* renderer = sprite->manager->renderer;
    
    // Calculate screen offset
    int screen_x_offset = (sprite->screen == PAL_SCREEN_SUB) ? 256 : 0;
    
    // Create destination rectangle
    SDL_FRect dst;
    dst.x = (float)(sprite->x + screen_x_offset);
    dst.y = (float)sprite->y;
    dst.w = (float)(sprite->width * sprite->scale_x);
    dst.h = (float)(sprite->height * sprite->scale_y);
    
    // Center point for rotation
    SDL_FPoint center = { dst.w / 2.0f, dst.h / 2.0f };
    
    // Render with or without affine transformation
    if (sprite->affine_enabled && 
        (sprite->rotation != 0.0f || sprite->scale_x != 1.0f || sprite->scale_y != 1.0f)) {
        SDL_RenderTextureRotated(renderer, sprite->texture, NULL, &dst, 
                                sprite->rotation, &center, SDL_FLIP_NONE);
    } else {
        SDL_RenderTexture(renderer, sprite->texture, NULL, &dst);
    }
}

// ============================================================================
// Utility Functions
// ============================================================================

void PAL_Sprite_GetSizeDimensions(PAL_SpriteSize size, int* width, int* height) {
    GetSpriteDimensions(size, width, height);
}

u32 PAL_Sprite_GetGraphicsDataSize(PAL_SpriteSize size, PAL_SpriteColorMode color_mode) {
    return GetSpriteDataSize(size, color_mode);
}

// ============================================================================
// Helper Functions
// ============================================================================

static void GetSpriteDimensions(PAL_SpriteSize size, int* width, int* height) {
    static const int dimensions[][2] = {
        {8, 8},    // PAL_SPRITE_SIZE_8x8
        {16, 16},  // PAL_SPRITE_SIZE_16x16
        {32, 32},  // PAL_SPRITE_SIZE_32x32
        {64, 64},  // PAL_SPRITE_SIZE_64x64
        {16, 8},   // PAL_SPRITE_SIZE_16x8
        {32, 8},   // PAL_SPRITE_SIZE_32x8
        {32, 16},  // PAL_SPRITE_SIZE_32x16
        {64, 32},  // PAL_SPRITE_SIZE_64x32
        {8, 16},   // PAL_SPRITE_SIZE_8x16
        {8, 32},   // PAL_SPRITE_SIZE_8x32
        {16, 32},  // PAL_SPRITE_SIZE_16x32
        {32, 64}   // PAL_SPRITE_SIZE_32x64
    };
    
    if (size >= PAL_SPRITE_SIZE_MAX) {
        *width = *height = 8;
        return;
    }
    
    *width = dimensions[size][0];
    *height = dimensions[size][1];
}

static u32 GetSpriteDataSize(PAL_SpriteSize size, PAL_SpriteColorMode color_mode) {
    int width, height;
    GetSpriteDimensions(size, &width, &height);
    
    int pixels = width * height;
    if (color_mode == PAL_SPRITE_COLOR_4BPP) {
        return pixels / 2;  // 4 bits per pixel
    } else {
        return pixels;      // 8 bits per pixel
    }
}

static void RenderSpriteTexture(PAL_Sprite* sprite) {
    if (!sprite->texture || !sprite->graphics_data) {
        return;
    }
    
    // Lock texture for writing
    void* pixels;
    int pitch;
    if (!SDL_LockTexture(sprite->texture, NULL, &pixels, &pitch)) {
        return;
    }
    
    // Decode sprite graphics to RGBA
    u32* rgba_buffer = (u32*)pixels;
    
    if (sprite->color_mode == PAL_SPRITE_COLOR_4BPP) {
        DecodeSpriteGraphics4bpp((const u8*)sprite->graphics_data, 
                                sprite->palette_num, &sprite->palette,
                                sprite->width, sprite->height, sprite->flip,
                                rgba_buffer);
    } else {
        DecodeSpriteGraphics8bpp((const u8*)sprite->graphics_data,
                                &sprite->palette,
                                sprite->width, sprite->height, sprite->flip,
                                rgba_buffer);
    }
    
    SDL_UnlockTexture(sprite->texture);
}

static void DecodeSpriteGraphics4bpp(const u8* src, u8 palette_num, const PAL_Palette* palette,
                                     int width, int height, PAL_SpriteFlip flip, u32* output) {
    int palette_offset = palette_num * 16;
    BOOL hflip = (flip & PAL_SPRITE_FLIP_H) != 0;
    BOOL vflip = (flip & PAL_SPRITE_FLIP_V) != 0;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int src_x = hflip ? (width - 1 - x) : x;
            int src_y = vflip ? (height - 1 - y) : y;
            
            int byte_idx = (src_y * width + src_x) / 2;
            int nibble = (src_x & 1);
            
            u8 color_idx = (src[byte_idx] >> (nibble * 4)) & 0x0F;
            
            if (color_idx == 0) {
                output[y * width + x] = 0x00000000;  // Transparent
            } else {
                int pal_idx = palette_offset + color_idx;
                if (pal_idx < 256) {
                    PAL_Color color = palette->colors[pal_idx];
                    output[y * width + x] = (color.a << 24) | (color.b << 16) | 
                                           (color.g << 8) | color.r;
                } else {
                    output[y * width + x] = 0xFF00FF00;  // Magenta (out of bounds)
                }
            }
        }
    }
}

static void DecodeSpriteGraphics8bpp(const u8* src, const PAL_Palette* palette,
                                     int width, int height, PAL_SpriteFlip flip, u32* output) {
    BOOL hflip = (flip & PAL_SPRITE_FLIP_H) != 0;
    BOOL vflip = (flip & PAL_SPRITE_FLIP_V) != 0;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int src_x = hflip ? (width - 1 - x) : x;
            int src_y = vflip ? (height - 1 - y) : y;
            
            u8 color_idx = src[src_y * width + src_x];
            
            if (color_idx == 0) {
                output[y * width + x] = 0x00000000;  // Transparent
            } else {
                PAL_Color color = palette->colors[color_idx];
                output[y * width + x] = (color.a << 24) | (color.b << 16) | 
                                       (color.g << 8) | color.r;
            }
        }
    }
}

static int CompareSpritesByPriority(const void* a, const void* b) {
    const PAL_Sprite* sprite_a = *(const PAL_Sprite**)a;
    const PAL_Sprite* sprite_b = *(const PAL_Sprite**)b;
    
    // Higher priority value = rendered later (on top)
    return (int)sprite_b->priority - (int)sprite_a->priority;
}
