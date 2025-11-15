/**
 * @file pal_sprite.h
 * @brief Platform Abstraction Layer for Sprite/OAM System
 * 
 * Provides abstraction for the Nintendo DS hardware sprite system (OAM)
 * with support for up to 128 sprites per screen, affine transformations,
 * and priority-based rendering.
 */

#ifndef PAL_SPRITE_H
#define PAL_SPRITE_H

#include "platform_config.h"
#include "platform_types.h"
#include "pal_graphics.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct PAL_Sprite PAL_Sprite;
typedef struct PAL_SpriteManager PAL_SpriteManager;

/**
 * @brief Sprite size enumeration (DS standard sizes)
 */
typedef enum {
    PAL_SPRITE_SIZE_8x8 = 0,
    PAL_SPRITE_SIZE_16x16,
    PAL_SPRITE_SIZE_32x32,
    PAL_SPRITE_SIZE_64x64,
    PAL_SPRITE_SIZE_16x8,
    PAL_SPRITE_SIZE_32x8,
    PAL_SPRITE_SIZE_32x16,
    PAL_SPRITE_SIZE_64x32,
    PAL_SPRITE_SIZE_8x16,
    PAL_SPRITE_SIZE_8x32,
    PAL_SPRITE_SIZE_16x32,
    PAL_SPRITE_SIZE_32x64,
    PAL_SPRITE_SIZE_MAX
} PAL_SpriteSize;

/**
 * @brief Sprite color mode
 */
typedef enum {
    PAL_SPRITE_COLOR_4BPP = 0,  // 16 colors (4 bits per pixel)
    PAL_SPRITE_COLOR_8BPP       // 256 colors (8 bits per pixel)
} PAL_SpriteColorMode;

/**
 * @brief Sprite flip flags
 */
typedef enum {
    PAL_SPRITE_FLIP_NONE = 0x00,
    PAL_SPRITE_FLIP_H = 0x01,
    PAL_SPRITE_FLIP_V = 0x02,
    PAL_SPRITE_FLIP_HV = 0x03
} PAL_SpriteFlip;

/**
 * @brief Sprite blend mode
 */
typedef enum {
    PAL_SPRITE_BLEND_NONE = 0,
    PAL_SPRITE_BLEND_ALPHA,
    PAL_SPRITE_BLEND_ADD,
    PAL_SPRITE_BLEND_SUB
} PAL_SpriteBlendMode;

/**
 * @brief Sprite template for creation
 */
typedef struct {
    const void* graphics_data;      // Tile data
    u32 graphics_size;
    const void* palette_data;        // Palette (RGB555)
    u16 palette_size;
    
    PAL_SpriteSize size;
    PAL_SpriteColorMode color_mode;
    
    int x, y;                        // Initial position
    u8 priority;                     // 0-3, lower = higher priority
    u8 palette_num;                  // Palette number (for 4bpp)
    
    PAL_Screen screen;               // Which screen to render on
} PAL_SpriteTemplate;

// ============================================================================
// Sprite Manager
// ============================================================================

/**
 * @brief Create a new sprite manager
 * 
 * @param max_sprites Maximum number of sprites (per screen: 128)
 * @param heapID Heap ID for allocations
 * @return Pointer to new sprite manager, or NULL on failure
 */
PAL_SpriteManager* PAL_Sprite_CreateManager(int max_sprites, u32 heapID);

/**
 * @brief Destroy sprite manager and all sprites
 * 
 * @param manager Sprite manager to destroy
 */
void PAL_Sprite_DestroyManager(PAL_SpriteManager* manager);

/**
 * @brief Get maximum sprite count
 * 
 * @param manager Sprite manager
 * @return Maximum number of sprites
 */
int PAL_Sprite_GetMaxCount(PAL_SpriteManager* manager);

/**
 * @brief Get current active sprite count
 * 
 * @param manager Sprite manager
 * @return Number of active sprites
 */
int PAL_Sprite_GetActiveCount(PAL_SpriteManager* manager);

// ============================================================================
// Sprite Creation and Destruction
// ============================================================================

/**
 * @brief Create a new sprite from template
 * 
 * @param manager Sprite manager
 * @param template Sprite template with initialization data
 * @return Pointer to new sprite, or NULL if no slots available
 */
PAL_Sprite* PAL_Sprite_Create(PAL_SpriteManager* manager, 
                              const PAL_SpriteTemplate* template);

/**
 * @brief Destroy a sprite
 * 
 * @param manager Sprite manager
 * @param sprite Sprite to destroy
 */
void PAL_Sprite_Destroy(PAL_SpriteManager* manager, PAL_Sprite* sprite);

/**
 * @brief Destroy all sprites
 * 
 * @param manager Sprite manager
 */
void PAL_Sprite_DestroyAll(PAL_SpriteManager* manager);

// ============================================================================
// Sprite Properties
// ============================================================================

/**
 * @brief Set sprite position
 * 
 * @param sprite Sprite to modify
 * @param x X coordinate
 * @param y Y coordinate
 */
void PAL_Sprite_SetPosition(PAL_Sprite* sprite, int x, int y);

/**
 * @brief Get sprite position
 * 
 * @param sprite Sprite to query
 * @param x Output X coordinate
 * @param y Output Y coordinate
 */
void PAL_Sprite_GetPosition(const PAL_Sprite* sprite, int* x, int* y);

/**
 * @brief Set sprite visibility
 * 
 * @param sprite Sprite to modify
 * @param visible TRUE to show, FALSE to hide
 */
void PAL_Sprite_SetVisible(PAL_Sprite* sprite, BOOL visible);

/**
 * @brief Get sprite visibility
 * 
 * @param sprite Sprite to query
 * @return TRUE if visible, FALSE if hidden
 */
BOOL PAL_Sprite_GetVisible(const PAL_Sprite* sprite);

/**
 * @brief Set sprite priority
 * 
 * @param sprite Sprite to modify
 * @param priority Priority value (0-3, lower = higher priority)
 */
void PAL_Sprite_SetPriority(PAL_Sprite* sprite, u8 priority);

/**
 * @brief Get sprite priority
 * 
 * @param sprite Sprite to query
 * @return Priority value
 */
u8 PAL_Sprite_GetPriority(const PAL_Sprite* sprite);

/**
 * @brief Set sprite flip flags
 * 
 * @param sprite Sprite to modify
 * @param flip Flip flags (PAL_SpriteFlip)
 */
void PAL_Sprite_SetFlip(PAL_Sprite* sprite, PAL_SpriteFlip flip);

/**
 * @brief Set sprite palette number (4bpp mode only)
 * 
 * @param sprite Sprite to modify
 * @param palette_num Palette number (0-15)
 */
void PAL_Sprite_SetPalette(PAL_Sprite* sprite, u8 palette_num);

/**
 * @brief Set sprite alpha (transparency)
 * 
 * @param sprite Sprite to modify
 * @param alpha Alpha value (0-255, 0 = transparent, 255 = opaque)
 */
void PAL_Sprite_SetAlpha(PAL_Sprite* sprite, u8 alpha);

/**
 * @brief Set sprite blend mode
 * 
 * @param sprite Sprite to modify
 * @param blend_mode Blend mode
 */
void PAL_Sprite_SetBlendMode(PAL_Sprite* sprite, PAL_SpriteBlendMode blend_mode);

// ============================================================================
// Affine Transformations
// ============================================================================

/**
 * @brief Set sprite rotation
 * 
 * @param sprite Sprite to modify
 * @param angle Rotation angle in degrees (0-359)
 */
void PAL_Sprite_SetRotation(PAL_Sprite* sprite, float angle);

/**
 * @brief Set sprite scale
 * 
 * @param sprite Sprite to modify
 * @param scale_x X scale (1.0 = normal)
 * @param scale_y Y scale (1.0 = normal)
 */
void PAL_Sprite_SetScale(PAL_Sprite* sprite, float scale_x, float scale_y);

/**
 * @brief Set sprite rotation and scale (combined affine)
 * 
 * @param sprite Sprite to modify
 * @param angle Rotation angle in degrees
 * @param scale_x X scale
 * @param scale_y Y scale
 */
void PAL_Sprite_SetAffine(PAL_Sprite* sprite, float angle, 
                         float scale_x, float scale_y);

/**
 * @brief Enable or disable affine transformations
 * 
 * @param sprite Sprite to modify
 * @param enable TRUE to enable affine, FALSE to disable
 */
void PAL_Sprite_EnableAffine(PAL_Sprite* sprite, BOOL enable);

// ============================================================================
// Animation
// ============================================================================

/**
 * @brief Set sprite frame (for multi-frame sprites)
 * 
 * @param sprite Sprite to modify
 * @param frame Frame index
 */
void PAL_Sprite_SetFrame(PAL_Sprite* sprite, int frame);

/**
 * @brief Get current sprite frame
 * 
 * @param sprite Sprite to query
 * @return Current frame index
 */
int PAL_Sprite_GetFrame(const PAL_Sprite* sprite);

// ============================================================================
// Rendering
// ============================================================================

/**
 * @brief Update all sprites (animations, etc.)
 * 
 * @param manager Sprite manager
 */
void PAL_Sprite_UpdateAll(PAL_SpriteManager* manager);

/**
 * @brief Render all visible sprites
 * 
 * Called internally by PAL_Graphics_EndFrame()
 * 
 * @param manager Sprite manager
 */
void PAL_Sprite_RenderAll(PAL_SpriteManager* manager);

/**
 * @brief Render a specific sprite (for manual rendering)
 * 
 * @param sprite Sprite to render
 */
void PAL_Sprite_Render(const PAL_Sprite* sprite);

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Get sprite dimensions for a size enum
 * 
 * @param size Sprite size enum
 * @param width Output width in pixels
 * @param height Output height in pixels
 */
void PAL_Sprite_GetSizeDimensions(PAL_SpriteSize size, int* width, int* height);

/**
 * @brief Get sprite graphics data size for a size and color mode
 * 
 * @param size Sprite size
 * @param color_mode Color mode
 * @return Data size in bytes
 */
u32 PAL_Sprite_GetGraphicsDataSize(PAL_SpriteSize size, PAL_SpriteColorMode color_mode);

#ifdef __cplusplus
}
#endif

#endif // PAL_SPRITE_H
