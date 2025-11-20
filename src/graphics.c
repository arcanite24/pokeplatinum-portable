#include "graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform/pal_file.h"
#include "platform/pal_background.h"
#include "platform/pal_graphics.h"

#include "heap.h"
#include "narc.h"

// Global state for palette loading (SDL3 workaround)
// Tracks the last bgConfig/layer used so Graphics_LoadPalette knows where to apply
static void* g_last_bgConfig = NULL;
static u8 g_last_bgLayer = 0;

// SDL3: Helper function to map NARC ID + member to filesystem path
/**
 * GetAssetPath - Map NARC ID and member index to filesystem path
 * 
 * RESOURCE MANAGEMENT STRATEGY:
 * - All paths point to resources/ directory (gitignored)
 * - Assets extracted from legal ROM via tools/convert_assets.sh
 * - DO NOT reference res/ (DS build only) or build/ (DS output only)
 * - This function is extensible - add more NARC mappings as systems are ported
 * 
 * @param narcID        NARC archive ID (e.g., NARC_INDEX_DEMO__TITLE__TITLEDEMO)
 * @param narcMemberIdx Member index within the NARC (0-based)
 * @param assetType     Asset type suffix (e.g., "tiles.bin", "palette.pal", "tilemap.bin")
 * @param outPath       Output buffer for the constructed path
 * @param pathSize      Size of output buffer
 */
static void GetAssetPath(enum NarcID narcID, u32 narcMemberIdx, const char* assetType, char* outPath, size_t pathSize)
{
    // Map well-known NARC IDs to extracted directories
    const char* narcPath = NULL;
    const char* assetName = NULL;
    
    // Title screen NARC (NARC_INDEX_DEMO__TITLE__TITLEDEMO = 48)
    if (narcID == NARC_INDEX_DEMO__TITLE__TITLEDEMO) {
        // Tilemaps are only in raw directory, use numeric indexing
        if (strcmp(assetType, "tilemap.bin") == 0) {
            snprintf(outPath, pathSize, "resources/graphics/title_screen_raw/%04d.bin", narcMemberIdx);
            return;
        }
        
        narcPath = "resources/graphics/title_screen";
        
        // Map member index to asset name (based on titledemo.naix.h)
        switch (narcMemberIdx) {
            // NCLR palettes
            case 4:  assetName = "gf_presents"; break;          // gf_presents_NCLR
            case 6:  assetName = "bottom_screen_border"; break; // bottom_screen_border_NCLR
            case 7:  assetName = "top_screen_border"; break;    // top_screen_border_NCLR
            case 8:  assetName = "logo_jp"; break;              // logo_jp_NCLR
            case 11: assetName = "logo"; break;                 // logo_NCLR
            case 14: assetName = "copyright"; break;            // copyright_NCLR
            
            // NCGR tiles
            case 5:  assetName = "gf_presents"; break;          // gf_presents_NCGR
            case 9:  assetName = "logo_jp"; break;              // logo_jp_NCGR
            case 12: assetName = "logo"; break;                 // logo_NCGR
            case 15: assetName = "copyright"; break;            // copyright_NCGR
            case 23: assetName = "top_screen_border"; break;    // top_screen_border_NCGR
            case 26: assetName = "bottom_screen_border"; break; // bottom_screen_border_NCGR
            
            default:
                // Unknown member, use numeric fallback from raw directory
                snprintf(outPath, pathSize, "resources/graphics/title_screen_raw/%04d.bin", narcMemberIdx);
                return;
        }
        
        // Build path with asset name
        if (assetName) {
            // Special handling for file extensions - don't duplicate if assetType already has it
            if (strcmp(assetType, "palette.pal") == 0) {
                // File is already named "palette_1.pal", don't add "_palette.pal"
                snprintf(outPath, pathSize, "%s/%s.pal", narcPath, assetName);
            } else if (strcmp(assetType, "tiles.bin") == 0) {
                // File is "top_border_tiles.bin"
                snprintf(outPath, pathSize, "%s/%s_tiles.bin", narcPath, assetName);
            } else {
                // Generic case
                snprintf(outPath, pathSize, "%s/%s_%s", narcPath, assetName, assetType);
            }
            return;
        }
    }
    
    // Add more NARC mappings here as you extract them
    // Example for battle graphics:
    // else if (narcID == NARC_INDEX_BATTLE_GRAPHICS) {
    //     narcPath = "resources/graphics/battle";
    //     switch (narcMemberIdx) {
    //         case 0: assetName = "battle_bg"; break;
    //         case 1: assetName = "pokemon_sprites"; break;
    //         // ... etc
    //     }
    //     snprintf(outPath, pathSize, "%s/%s_%s", narcPath, assetName, assetType);
    //     return;
    // }
    
    // Fallback: use generic path (for assets not yet specifically mapped)
    // This allows incremental asset extraction as more game systems are ported
    snprintf(outPath, pathSize, "resources/graphics/narc_%03d/%04d_%s", narcID, narcMemberIdx, assetType);
}

u32 Graphics_LoadTilesToBgLayer(enum NarcID narcID, u32 narcMemberIdx, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
    (void)compressed;  // Unused in SDL3 - assets are pre-converted
    
    // SDL3: Load tile data from filesystem
    char tilePath[512];
    GetAssetPath(narcID, narcMemberIdx, "tiles.bin", tilePath, sizeof(tilePath));
    
    PAL_File file = PAL_File_Open(tilePath, "rb");
    if (!file) {
        fprintf(stderr, "[Graphics] Failed to load tiles: %s\n", tilePath);
        return 0;
    }
    
    size_t fileSize = PAL_File_Size(file);
    void *tileData = Heap_Alloc(heapID, fileSize);
    if (!tileData) {
        fprintf(stderr, "[Graphics] Failed to allocate %zu bytes for tiles\n", fileSize);
        PAL_File_Close(file);
        return 0;
    }
    
    PAL_File_Read(tileData, 1, fileSize, file);
    PAL_File_Close(file);
    
    // Determine actual size to load
    u32 loadSize = (size == 0) ? fileSize : size;
    
    // Load to PAL background layer
    PAL_BgConfig *palBgConfig = (PAL_BgConfig*)bgConfig;
    PAL_Bg_LoadTiles(palBgConfig, bgLayer, tileData, loadSize, offset);
    
    // Track last used layer for palette loading
    g_last_bgConfig = bgConfig;
    g_last_bgLayer = bgLayer;
    
    Heap_Free(tileData);
    
    printf("[Graphics] Loaded %u bytes of tiles from %s to layer %u\n", loadSize, tilePath, bgLayer);
    return loadSize;
}

void Graphics_LoadTilemapToBgLayer(enum NarcID narcID, u32 narcMemberIdx, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
    (void)compressed;  // Unused in SDL3 - assets are pre-converted
    (void)offset;      // TODO: Implement offset support if needed
    
    // SDL3: Load tilemap data from filesystem
    char tilemapPath[512];
    GetAssetPath(narcID, narcMemberIdx, "tilemap.bin", tilemapPath, sizeof(tilemapPath));
    
    PAL_File file = PAL_File_Open(tilemapPath, "rb");
    if (!file) {
        fprintf(stderr, "[Graphics] Failed to load tilemap: %s\n", tilemapPath);
        return;
    }
    
    size_t fileSize = PAL_File_Size(file);
    void *tilemapData = Heap_Alloc(heapID, fileSize);
    if (!tilemapData) {
        fprintf(stderr, "[Graphics] Failed to allocate %zu bytes for tilemap\n", fileSize);
        PAL_File_Close(file);
        return;
    }
    
    PAL_File_Read(tilemapData, 1, fileSize, file);
    PAL_File_Close(file);
    
    // Check if this is an NSCR file (Nintendo Screen format)
    void* actualTilemapData = tilemapData;
    u32 actualSize = fileSize;
    
    u32* magic = (u32*)tilemapData;
    if (*magic == 0x4E534352) {  // "RCSN" in little-endian (0x52='R', 0x43='C', 0x53='S', 0x4E='N')
        // NSCR format: Skip header to get to actual tilemap data
        // Structure:
        //   0x00-0x03: Magic "RCSN"
        //   0x04-0x05: 0xFEFF (byte order mark)
        //   0x06-0x07: Version
        //   0x08-0x0B: File size
        //   0x0C-0x0F: Header size (usually 0x10)
        //   0x10: Start of "SCRN" section with actual data
        
        if (fileSize >= 0x20) {
            // Skip to SCRN section (usually at 0x10)
            u8* scrn_section = (u8*)tilemapData + 0x10;
            u32 scrn_magic = *(u32*)scrn_section;
            
            if (scrn_magic == 0x5343524E || scrn_magic == 0x4E524353) { // "NRCS" or "SCRN" in little-endian
                // SCRN section structure:
                //   0x00-0x03: Magic "NRCS"
                //   0x04-0x07: Section size
                //   0x08-0x09: Width
                //   0x0A-0x0B: Height
                //   0x0C-0x0F: Unknown
                //   0x10-0x13: Tilemap data size
                //   0x14+: Actual tilemap data
                actualTilemapData = scrn_section + 0x14;  // Skip SCRN header (0x14 bytes)
                actualSize = fileSize - 0x24;  // Total offset from start of file
                printf("[Graphics] Detected NSCR format, skipping headers (data at offset 0x24)\n");
            }
        }
    }
    
    // Determine actual size to load
    u32 loadSize = (size == 0) ? actualSize : size;
    
    // Load to PAL background layer
    PAL_BgConfig *palBgConfig = (PAL_BgConfig*)bgConfig;
    PAL_Bg_LoadTilemapBuffer(palBgConfig, bgLayer, actualTilemapData, loadSize);
    PAL_Bg_CopyTilemapBufferToVRAM(palBgConfig, bgLayer);
    
    // Track last used layer for palette loading
    g_last_bgConfig = bgConfig;
    g_last_bgLayer = bgLayer;
    
    Heap_Free(tilemapData);
    
    printf("[Graphics] Loaded %u bytes of tilemap from %s to layer %u\n", loadSize, tilemapPath, bgLayer);
}

void Graphics_LoadPalette(enum NarcID narcID, u32 narcMemberIdx, enum PaletteLoadLocation loadLocation, u32 palOffset, u32 size, u32 heapID)
{
    Graphics_LoadPaletteWithSrcOffset(narcID, narcMemberIdx, loadLocation, 0, palOffset, size, heapID);
}

void Graphics_LoadPaletteWithSrcOffset(enum NarcID narcID, u32 narcMemberIdx, enum PaletteLoadLocation loadLocation, u32 srcOffset, u32 palOffset, u32 size, u32 heapID)
{
    (void)loadLocation;  // TODO: Use this to determine which screen/layer
    
    // SDL3: Load palette from filesystem
    char palettePath[512];
    GetAssetPath(narcID, narcMemberIdx, "palette.pal", palettePath, sizeof(palettePath));
    
    PAL_File file = PAL_File_Open(palettePath, "rb");
    if (!file) {
        fprintf(stderr, "[Graphics] Failed to load palette: %s\n", palettePath);
        return;
    }
    
    size_t fileSize = PAL_File_Size(file);
    u16 *paletteData = (u16*)Heap_Alloc(heapID, fileSize);
    if (!paletteData) {
        fprintf(stderr, "[Graphics] Failed to allocate %zu bytes for palette\n", fileSize);
        PAL_File_Close(file);
        return;
    }
    
    PAL_File_Read(paletteData, 1, fileSize, file);
    PAL_File_Close(file);
    
    // Determine actual size to load
    // size is in BYTES, convert to number of colors
    u32 numColors = fileSize / 2; // RGB555 is 2 bytes per color
    u32 loadSize = (size == 0) ? numColors : (size / 2);
    
    // Convert byte offsets to color indices
    u32 srcColorOffset = srcOffset / 2;      // srcOffset is in bytes
    u32 palColorOffset = palOffset / 2;      // palOffset is in bytes (PLTT_OFFSET macro)
    
    printf("[Graphics] Loaded %u colors from %s\n", loadSize, palettePath);
    
    // Apply palette to the last-used background layer
    if (g_last_bgConfig) {
        PAL_BgConfig *palBgConfig = (PAL_BgConfig*)g_last_bgConfig;
        
        // Skip source offset colors, then load
        const u16 *srcPtr = paletteData + srcColorOffset;
        PAL_Bg_LoadPalette(palBgConfig, g_last_bgLayer, srcPtr, loadSize * sizeof(u16), palColorOffset);
        
        printf("[Graphics] Applied palette to layer %d (srcOffset=%d colors, palOffset=%d colors, count=%d)\n",
               g_last_bgLayer, srcColorOffset, palColorOffset, loadSize);
    }
    
    Heap_Free(paletteData);
}

// ============================================================================
// SDL3 Stub Implementations
// ============================================================================
// These functions are stubs that need to be ported from the DS implementation.
// See docs/DS_GRAPHICS_REFERENCE.md for the original DS code.
// TODO: Implement these as needed during porting.

u32 Graphics_LoadObjectTiles(enum NarcID narcID, u32 narcMemberIdx, enum DSScreen display, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
    (void)narcID; (void)narcMemberIdx; (void)display; (void)offset;
    (void)size; (void)compressed; (void)heapID;
    
    printf("[Graphics] TODO: Port Graphics_LoadObjectTiles (NARC %d, member %d)\n", narcID, narcMemberIdx);
    return 0; // Return 0 tiles loaded
}

void Graphics_LoadPartialPalette(enum NarcID narcID, u32 narcMemberIdx, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, u32 heapID, NNSG2dImagePaletteProxy *paletteProxy)
{
    (void)narcID; (void)narcMemberIdx; (void)vramType; (void)baseAddr;
    (void)heapID; (void)paletteProxy;
    
    printf("[Graphics] TODO: Port Graphics_LoadPartialPalette (NARC %d, member %d)\n", narcID, narcMemberIdx);
}

u32 Graphics_LoadImageMapping(enum NarcID narcID, u32 narcMemberIdx, BOOL compressed, enum ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, u32 heapID, NNSG2dImageProxy *imageProxy)
{
    (void)narcID; (void)narcMemberIdx; (void)compressed; (void)layout;
    (void)size; (void)vramType; (void)baseAddr; (void)heapID; (void)imageProxy;
    
    printf("[Graphics] TODO: Port Graphics_LoadImageMapping (NARC %d, member %d)\n", narcID, narcMemberIdx);
    return 0;
}

void Graphics_LoadPaletteBuffer(enum NarcID narcID, u32 narcMemberIdx, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, u32 heapID, NNSG2dImagePaletteProxy *paletteProxy)
{
    (void)narcID; (void)narcMemberIdx; (void)vramType; (void)baseAddr;
    (void)heapID; (void)paletteProxy;
    
    printf("[Graphics] TODO: Port Graphics_LoadPaletteBuffer (NARC %d, member %d)\n", narcID, narcMemberIdx);
}

void Graphics_LoadMultiCellAnimation(enum NarcID narcID, u32 narcMemberIdx, BOOL compressed, void *cellAnimation, u32 heapID)
{
    (void)narcID; (void)narcMemberIdx; (void)compressed;
    (void)cellAnimation; (void)heapID;
    
    printf("[Graphics] TODO: Port Graphics_LoadMultiCellAnimation (NARC %d, member %d)\n", narcID, narcMemberIdx);
}

void Graphics_LoadPartialPaletteFromOpenNARC(NARC *narc, u32 narcMemberIdx, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, u32 heapID, NNSG2dImagePaletteProxy *paletteProxy)
{
    (void)narc; (void)narcMemberIdx; (void)vramType; (void)baseAddr;
    (void)heapID; (void)paletteProxy;
    
    printf("[Graphics] TODO: Port Graphics_LoadPartialPaletteFromOpenNARC (member %d)\n", narcMemberIdx);
}

void Graphics_LoadPaletteFromOpenNARC(NARC *narc, u32 narcMemberIdx, enum PaletteLoadLocation loadLocation, u32 palOffset, u32 size, u32 heapID)
{
    (void)narc; (void)narcMemberIdx; (void)loadLocation;
    (void)palOffset; (void)size; (void)heapID;
    
    printf("[Graphics] TODO: Port Graphics_LoadPaletteFromOpenNARC (member %d)\n", narcMemberIdx);
}

u32 Graphics_LoadImageMappingFromOpenNARC(NARC *narc, u32 narcMemberIdx, BOOL compressed, enum ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, u32 heapID, NNSG2dImageProxy *imageProxy)
{
    (void)narc; (void)narcMemberIdx; (void)compressed; (void)layout;
    (void)size; (void)vramType; (void)baseAddr; (void)heapID; (void)imageProxy;
    
    printf("[Graphics] TODO: Port Graphics_LoadImageMappingFromOpenNARC (member %d)\n", narcMemberIdx);
    return 0;
}

void Graphics_LoadImageMappingAndSetVramMode(enum NarcID narcID, u32 narcMemberIdx, BOOL compressed, enum ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, u32 heapID, NNSG2dImageProxy *imageProxy)
{
    (void)narcID; (void)narcMemberIdx; (void)compressed; (void)layout;
    (void)size; (void)vramType; (void)baseAddr; (void)heapID; (void)imageProxy;
    
    printf("[Graphics] TODO: Port Graphics_LoadImageMappingAndSetVramMode (NARC %d, member %d)\n", narcID, narcMemberIdx);
}

void *Graphics_GetNARCMemberPointer(NARC *narc, u32 narcMemberIdx)
{
    (void)narc; (void)narcMemberIdx;
    
    printf("[Graphics] TODO: Port Graphics_GetNARCMemberPointer (member %d)\n", narcMemberIdx);
    return NULL;
}

u32 Graphics_LoadTilesToBgLayerFromOpenNARC(NARC *narc, u32 narcMemberIdx, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
    (void)narc; (void)narcMemberIdx; (void)bgConfig; (void)bgLayer;
    (void)offset; (void)size; (void)compressed; (void)heapID;
    
    printf("[Graphics] TODO: Port Graphics_LoadTilesToBgLayerFromOpenNARC (member %d)\n", narcMemberIdx);
    return 0;
}

void Graphics_LoadTilemapToBgLayerFromOpenNARC(NARC *narc, u32 narcMemberIdx, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
    (void)narc; (void)narcMemberIdx; (void)bgConfig; (void)bgLayer;
    (void)offset; (void)size; (void)compressed; (void)heapID;
    
    printf("[Graphics] TODO: Port Graphics_LoadTilemapToBgLayerFromOpenNARC (member %d)\n", narcMemberIdx);
}

void Graphics_LoadPaletteWithSrcOffsetFromOpenNARC(NARC *narc, u32 narcMemberIdx, enum PaletteLoadLocation loadLocation, u32 srcOffset, u32 offset, u32 size, u32 heapID)
{
    (void)narc; (void)narcMemberIdx; (void)loadLocation;
    (void)srcOffset; (void)offset; (void)size; (void)heapID;
    
    printf("[Graphics] TODO: Port Graphics_LoadPaletteWithSrcOffsetFromOpenNARC (member %d)\n", narcMemberIdx);
}
