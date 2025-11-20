/**
 * @file pal_background.h
 * @brief Platform Abstraction Layer for Background/Tilemap System
 * 
 * Provides abstraction for the Nintendo DS background system with
 * multiple layers, scrolling, and tile-based rendering.
 */

#ifndef PAL_BACKGROUND_H
#define PAL_BACKGROUND_H

#include "platform_config.h"
#include "platform_types.h"
#include "pal_graphics.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
// Note: PAL_BgConfig is typedef'd in bg_window.h as BgConfig (which is struct BgConfig)
struct BgConfig;
typedef struct BgConfig PAL_BgConfig;  // Make it refer to the actual struct
typedef struct PAL_BgTemplate PAL_BgTemplate;

/**
 * @brief Background layer enumeration
 * 
 * DS has 4 background layers per screen (0-3 for main, 4-7 for sub)
 */
typedef enum {
    PAL_BG_LAYER_MAIN_0 = 0,
    PAL_BG_LAYER_MAIN_1,
    PAL_BG_LAYER_MAIN_2,
    PAL_BG_LAYER_MAIN_3,
    PAL_BG_LAYER_SUB_0,
    PAL_BG_LAYER_SUB_1,
    PAL_BG_LAYER_SUB_2,
    PAL_BG_LAYER_SUB_3,
    PAL_BG_LAYER_MAX
} PAL_BgLayer;

/**
 * @brief Background type
 */
typedef enum {
    PAL_BG_TYPE_STATIC = 0,      // Regular scrolling background
    PAL_BG_TYPE_AFFINE,           // Affine transformation (rotation/scale)
    PAL_BG_TYPE_STATIC_WITH_AFFINE
} PAL_BgType;

/**
 * @brief Background screen size
 */
typedef enum {
    PAL_BG_SCREEN_SIZE_128x128 = 0,
    PAL_BG_SCREEN_SIZE_256x256,
    PAL_BG_SCREEN_SIZE_256x512,
    PAL_BG_SCREEN_SIZE_512x256,
    PAL_BG_SCREEN_SIZE_512x512,
    PAL_BG_SCREEN_SIZE_1024x1024
} PAL_BgScreenSize;

/**
 * @brief Background color mode
 */
typedef enum {
    PAL_BG_COLOR_MODE_4BPP = 0,  // 16 colors per tile
    PAL_BG_COLOR_MODE_8BPP       // 256 colors per tile
} PAL_BgColorMode;

/**
 * @brief Background control parameters
 */
typedef enum {
    PAL_BG_CONTROL_COLOR_MODE = 0,
    PAL_BG_CONTROL_SCREEN_BASE,
    PAL_BG_CONTROL_CHAR_BASE,
    PAL_BG_CONTROL_SCREEN_SIZE
} PAL_BgControlParam;

/**
 * @brief Background offset update operations
 */
typedef enum {
    PAL_BG_OFFSET_SET_X = 0,
    PAL_BG_OFFSET_ADD_X,
    PAL_BG_OFFSET_SUB_X,
    PAL_BG_OFFSET_SET_Y,
    PAL_BG_OFFSET_ADD_Y,
    PAL_BG_OFFSET_SUB_Y
} PAL_BgOffsetOp;

/**
 * @brief Background template for initialization
 * 
 * Mirrors the DS BgTemplate structure
 */
struct PAL_BgTemplate {
    u32 x;
    u32 y;
    u32 bufferSize;
    u32 baseTile;
    u8 screenSize;
    u8 colorMode;
    u8 screenBase;
    u8 charBase;
    u8 bgExtPltt;
    u8 priority;
    u8 areaOver;
    BOOL mosaic;
};

// ============================================================================
// Initialization and Management
// ============================================================================

/**
 * @brief Create a new background configuration
 * 
 * @param heapID Heap ID for memory allocation
 * @return Pointer to new BgConfig, or NULL on failure
 */
PAL_BgConfig* PAL_Bg_CreateConfig(u32 heapID);

/**
 * @brief Destroy a background configuration
 * 
 * @param bgConfig Background configuration to destroy
 */
void PAL_Bg_DestroyConfig(PAL_BgConfig* bgConfig);

/**
 * @brief Initialize a background layer from template
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Layer to initialize
 * @param bgTemplate Template with initialization parameters
 * @param bgType Type of background (static, affine, etc.)
 */
void PAL_Bg_InitFromTemplate(PAL_BgConfig* bgConfig, u8 bgLayer, 
                             const PAL_BgTemplate* bgTemplate, u8 bgType);

/**
 * @brief Free tilemap buffer for a layer
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Layer to free
 */
void PAL_Bg_FreeTilemapBuffer(PAL_BgConfig* bgConfig, u8 bgLayer);

/**
 * @brief Set background control parameter
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Layer to modify
 * @param param Parameter to set
 * @param value New value
 */
void PAL_Bg_SetControlParam(PAL_BgConfig* bgConfig, u8 bgLayer, 
                            PAL_BgControlParam param, u8 value);

// ============================================================================
// Layer Control
// ============================================================================

/**
 * @brief Set layer priority (0 = highest)
 * 
 * @param bgLayer Layer to modify
 * @param priority Priority value (0-3)
 */
void PAL_Bg_SetPriority(u8 bgLayer, u8 priority);

/**
 * @brief Enable or disable a layer
 * 
 * @param bgLayer Layer to toggle
 * @param enable TRUE to enable, FALSE to disable
 */
void PAL_Bg_ToggleLayer(u8 bgLayer, BOOL enable);

// ============================================================================
// Scrolling
// ============================================================================

/**
 * @brief Set background scroll offset
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Layer to scroll
 * @param op Offset operation (set, add, subtract)
 * @param value Offset value
 */
void PAL_Bg_SetOffset(PAL_BgConfig* bgConfig, u8 bgLayer, 
                      PAL_BgOffsetOp op, int value);

/**
 * @brief Get X offset of a layer
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Layer to query
 * @return Current X offset
 */
int PAL_Bg_GetXOffset(PAL_BgConfig* bgConfig, u8 bgLayer);

/**
 * @brief Get Y offset of a layer
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Layer to query
 * @return Current Y offset
 */
int PAL_Bg_GetYOffset(PAL_BgConfig* bgConfig, u8 bgLayer);

// ============================================================================
// Tilemap Operations
// ============================================================================

/**
 * @brief Load tilemap data to buffer
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Target layer
 * @param src Source data
 * @param size Data size in bytes
 */
void PAL_Bg_LoadTilemapBuffer(PAL_BgConfig* bgConfig, u8 bgLayer, 
                              const void* src, u32 size);

/**
 * @brief Copy tilemap buffer to VRAM/screen
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Layer to update
 */
void PAL_Bg_CopyTilemapBufferToVRAM(PAL_BgConfig* bgConfig, u8 bgLayer);

/**
 * @brief Fill tilemap with a single value
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Target layer
 * @param fillVal Value to fill (tile index + flags)
 */
void PAL_Bg_FillTilemap(PAL_BgConfig* bgConfig, u8 bgLayer, u16 fillVal);

/**
 * @brief Fill a rectangle in the tilemap
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Target layer
 * @param fillVal Value to fill
 * @param x X position in tiles
 * @param y Y position in tiles
 * @param width Width in tiles
 * @param height Height in tiles
 * @param palette Palette index (for 4bpp mode)
 */
void PAL_Bg_FillTilemapRect(PAL_BgConfig* bgConfig, u8 bgLayer, u16 fillVal,
                            u8 x, u8 y, u8 width, u8 height, u8 palette);

/**
 * @brief Clear entire tilemap
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Layer to clear
 */
void PAL_Bg_ClearTilemap(PAL_BgConfig* bgConfig, u8 bgLayer);

/**
 * @brief Get pointer to tilemap buffer
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Target layer
 * @return Pointer to tilemap buffer
 */
void* PAL_Bg_GetTilemapBuffer(PAL_BgConfig* bgConfig, u8 bgLayer);

// ============================================================================
// Tile/Character Data
// ============================================================================

/**
 * @brief Load tile graphics to layer
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Target layer
 * @param src Source tile data
 * @param size Data size in bytes
 * @param tileStart Starting tile index
 */
void PAL_Bg_LoadTiles(PAL_BgConfig* bgConfig, u8 bgLayer, const void* src, 
                     u32 size, u32 tileStart);

/**
 * @brief Load tile graphics directly to VRAM
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Target layer
 * @param src Source tile data
 * @param size Data size in bytes
 * @param offset Offset in VRAM
 */
void PAL_Bg_LoadTilesToVRAM(PAL_BgConfig* bgConfig, u8 bgLayer, 
                           const void* src, u32 size, u32 offset);

/**
 * @brief Clear a range of tiles
 * 
 * @param bgLayer Target layer
 * @param size Number of tiles to clear
 * @param offset Starting offset
 * @param heapID Heap ID for temporary allocation
 */
void PAL_Bg_ClearTilesRange(u8 bgLayer, u32 size, u32 offset, u32 heapID);

// ============================================================================
// Palette
// ============================================================================

/**
 * @brief Load palette for background layer
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Target layer
 * @param src Source palette data (RGB555 format)
 * @param size Size in bytes
 * @param offset Offset in palette memory
 */
void PAL_Bg_LoadPalette(PAL_BgConfig* bgConfig, u8 bgLayer, const void* src, u16 size, u16 offset);

/**
 * @brief Apply mask to palette
 * 
 * @param bgLayer Target layer
 * @param mask Mask value
 */
void PAL_Bg_MaskPalette(u8 bgLayer, u16 mask);

// ============================================================================
// Rendering (SDL-specific)
// ============================================================================

/**
 * @brief Render all background layers to screen
 * 
 * Called internally by PAL_Graphics_EndFrame()
 * 
 * @param bgConfig Background configuration
 */
void PAL_Bg_RenderAll(PAL_BgConfig* bgConfig);

/**
 * @brief Render a specific background layer
 * 
 * @param bgConfig Background configuration
 * @param bgLayer Layer to render
 */
void PAL_Bg_RenderLayer(PAL_BgConfig* bgConfig, u8 bgLayer);

#ifdef __cplusplus
}
#endif

#endif // PAL_BACKGROUND_H
