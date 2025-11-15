#include "graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform/pal_file.h"
#include "platform/pal_background.h"
#include "platform/pal_graphics.h"

#include "heap.h"
#include "narc.h"

// SDL3: Helper function to map NARC ID + member to filesystem path
static void GetAssetPath(enum NarcID narcID, u32 narcMemberIdx, const char* assetType, char* outPath, size_t pathSize)
{
    // Map well-known NARC IDs to extracted directories
    const char* narcPath = NULL;
    const char* assetName = NULL;
    
    // Title screen NARC (NARC_INDEX_DEMO__TITLE__TITLEDEMO = 48)
    if (narcID == NARC_INDEX_DEMO__TITLE__TITLEDEMO) {
        narcPath = "resources/graphics/title_screen";
        
        // Map member index to asset name
        switch (narcMemberIdx) {
            case 4:  assetName = "palette_1"; break;
            case 5:  assetName = "top_border"; break;
            case 6:  assetName = "palette_2"; break;
            case 7:  assetName = "palette_3"; break;
            case 8:  assetName = "palette_4"; break;
            case 9:  assetName = "logo_top"; break;
            case 11: assetName = "palette_5"; break;
            case 12: assetName = "logo_bottom"; break;
            case 14: assetName = "palette_6"; break;
            case 15: assetName = "unknown_gfx"; break;
            case 23: assetName = "press_start"; break;
            case 26: assetName = "small_graphic"; break;
            default:
                // Unknown member, use numeric fallback
                snprintf(outPath, pathSize, "%s/%04d_%s", narcPath, narcMemberIdx, assetType);
                return;
        }
        
        // Build path with asset name
        if (assetName) {
            snprintf(outPath, pathSize, "%s/%s_%s", narcPath, assetName, assetType);
            return;
        }
    }
    
    // Add more NARC mappings here as you extract them
    // else if (narcID == XXX) { ... }
    
    // Fallback: use generic path
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
    
    Heap_Free(tileData);
    
    printf("[Graphics] Loaded %u bytes of tiles from %s to layer %u\n", loadSize, tilePath, bgLayer);
    return loadSize;
}

void Graphics_LoadTilemapToBgLayer(enum NarcID narcID, u32 narcMemberIdx, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
    (void)compressed;  // Unused in SDL3
    (void)narcID; (void)narcMemberIdx; (void)bgConfig; (void)bgLayer;
    (void)offset; (void)size; (void)heapID;
    
    // SDL stub - not implemented yet
    printf("[Graphics] LoadTilemapToBgLayer not implemented for SDL\n");
}

void Graphics_LoadPalette(enum NarcID narcID, u32 narcMemberIdx, enum PaletteLoadLocation loadLocation, u32 palOffset, u32 size, u32 heapID)
{
    Graphics_LoadPaletteWithSrcOffset(narcID, narcMemberIdx, loadLocation, 0, palOffset, size, heapID);
}

void Graphics_LoadPaletteWithSrcOffset(enum NarcID narcID, u32 narcMemberIdx, enum PaletteLoadLocation loadLocation, u32 srcOffset, u32 palOffset, u32 size, u32 heapID)
{
    (void)loadLocation;  // TODO: Use this to determine which screen/layer
    (void)palOffset;     // TODO: Implement offset within palette
    
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
    
    // Determine actual size to load (in number of colors)
    u32 numColors = fileSize / 2; // RGB555 is 2 bytes per color
    u32 loadSize = (size == 0) ? numColors : size;
    
    // Apply source offset (TODO: use srcPtr for palette application)
    (void)srcOffset;  // Will be used when we implement palette application
    
    // For SDL, we need to determine which background layer this applies to
    // loadLocation enum: PALLOAD_MAIN_BG, PALLOAD_SUB_BG, PALLOAD_MAIN_OBJ, PALLOAD_SUB_OBJ
    // For now, we'll assume it's for the main background (this may need refinement)
    
    printf("[Graphics] Loaded %u colors from %s\n", loadSize, palettePath);
    
    // TODO: Apply palette to appropriate layer based on loadLocation
    // This may require tracking which BgConfig was recently used
    // For now, just load and free
    
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
