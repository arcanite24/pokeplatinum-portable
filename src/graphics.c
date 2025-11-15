#include "graphics.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include "platform/pal_file.h"
#include "platform/pal_background.h"
#include "platform/pal_graphics.h"
#endif

#include <string.h>

#ifdef PLATFORM_DS
#include "bg_window.h"
#endif
#include "heap.h"
#include "narc.h"

#ifdef PLATFORM_DS
// DS-specific forward declarations
static u32 LoadTilesToBgLayer(void *ncgrBuffer, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size);
static void LoadTilemapToBgLayer(void *nscrBuffer, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size);
static u32 LoadObjectTiles(void *ncgrBuffer, enum DSScreen display, u32 offset, u32 size);
static void LoadPaletteWithSrcOffset(void *nclrBuffer, enum PaletteLoadLocation loadLocation, u32 srcOffset, u32 offset, u32 size);
static void LoadPartialPalette(void *nclrBuffer, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, NNSG2dImagePaletteProxy *paletteProxy);
static u32 LoadImageMapping(void *ncgrBuffer, enum ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, NNSG2dImageProxy *imageProxy);
static void LoadImageMappingAndSetVramMode(void *ncgrBuffer, enum ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, NNSG2dImageProxy *imageProxy);
static void *GetCharacterData(void *ncgrBuffer, NNSG2dCharacterData **outCharData);
static void *GetScreenData(void *nscrBuffer, NNSG2dScreenData **outScreenData);
static void *GetPaletteData(void *nclrBuffer, NNSG2dPaletteData **outPaletteData);
static void *GetCellBank(void *ncerBuffer, NNSG2dCellDataBank **outCellBank);
static void *GetAnimBank(void *nanrBuffer, NNSG2dAnimBankData **outAnimBank);
#endif

#ifndef PLATFORM_DS
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
#endif

u32 Graphics_LoadTilesToBgLayer(enum NarcID narcID, u32 narcMemberIdx, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
#ifdef PLATFORM_DS
    void *ncgrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, FALSE);
    return LoadTilesToBgLayer(ncgrBuffer, bgConfig, bgLayer, offset, size);
#else
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
#endif
}

#ifdef PLATFORM_DS
void Graphics_LoadTilemapToBgLayer(enum NarcID narcID, u32 narcMemberIdx, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
    void *nscrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, TRUE);
    LoadTilemapToBgLayer(nscrBuffer, bgConfig, bgLayer, offset, size);
}
#else
void Graphics_LoadTilemapToBgLayer(enum NarcID narcID, u32 narcMemberIdx, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
    // SDL stub - not implemented yet
    (void)narcID; (void)narcMemberIdx; (void)bgConfig; (void)bgLayer;
    (void)offset; (void)size; (void)compressed; (void)heapID;
    printf("[Graphics] LoadTilemapToBgLayer not implemented for SDL\n");
}
#endif

void Graphics_LoadPalette(enum NarcID narcID, u32 narcMemberIdx, enum PaletteLoadLocation loadLocation, u32 palOffset, u32 size, u32 heapID)
{
    Graphics_LoadPaletteWithSrcOffset(narcID, narcMemberIdx, loadLocation, 0, palOffset, size, heapID);
}

void Graphics_LoadPaletteWithSrcOffset(enum NarcID narcID, u32 narcMemberIdx, enum PaletteLoadLocation loadLocation, u32 srcOffset, u32 palOffset, u32 size, u32 heapID)
{
#ifdef PLATFORM_DS
    void *nclrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, FALSE, heapID, TRUE);
    LoadPaletteWithSrcOffset(nclrBuffer, loadLocation, srcOffset, palOffset, size);
#else
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
    
    // Apply source offset
    u16 *srcPtr = paletteData + srcOffset;
    
    // For SDL, we need to determine which background layer this applies to
    // loadLocation enum: PALLOAD_MAIN_BG, PALLOAD_SUB_BG, PALLOAD_MAIN_OBJ, PALLOAD_SUB_OBJ
    // For now, we'll assume it's for the main background (this may need refinement)
    
    printf("[Graphics] Loaded %u colors from %s (offset: %u)\n", loadSize, palettePath, srcOffset);
    
    // TODO: Apply palette to appropriate layer based on loadLocation
    // This may require tracking which BgConfig was recently used
    // For now, just load and free
    
    Heap_Free(paletteData);
#endif
}

// All remaining functions are DS-specific
#ifdef PLATFORM_DS

u32 Graphics_LoadObjectTiles(enum NarcID narcID, u32 narcMemberIdx, enum DSScreen display, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
    void *ncgrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, TRUE);
    return LoadObjectTiles(ncgrBuffer, display, offset, size);
}

void Graphics_LoadPartialPalette(enum NarcID narcID, u32 narcMemberIdx, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, u32 heapID, NNSG2dImagePaletteProxy *paletteProxy)
{
    void *nclrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, FALSE, heapID, TRUE);
    LoadPartialPalette(nclrBuffer, vramType, baseAddr, paletteProxy);
}

u32 Graphics_LoadImageMapping(enum NarcID narcID, u32 narcMemberIdx, BOOL compressed, enum ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, u32 heapID, NNSG2dImageProxy *imageProxy)
{
    void *ncgrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, TRUE);
    return LoadImageMapping(ncgrBuffer, layout, size, vramType, baseAddr, imageProxy);
}

void Graphics_LoadImageMappingAndSetVramMode(enum NarcID narcID, u32 narcMemberIdx, BOOL compressed, enum ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, u32 heapID, NNSG2dImageProxy *imageProxy)
{
    void *ncgrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, TRUE);
    LoadImageMappingAndSetVramMode(ncgrBuffer, layout, size, vramType, baseAddr, imageProxy);
}

void *Graphics_GetCharData(enum NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dCharacterData **outCharData, u32 heapID)
{
    void *ncgrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, FALSE);
    return GetCharacterData(ncgrBuffer, outCharData);
}

void *Graphics_GetScrnData(enum NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dScreenData **outScreenData, u32 heapID)
{
    void *nscrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, FALSE);
    return GetScreenData(nscrBuffer, outScreenData);
}

void *Graphics_GetPlttData(enum NarcID narcID, u32 narcMemberIdx, NNSG2dPaletteData **outPaletteData, u32 heapID)
{
    void *nclrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, FALSE, heapID, FALSE);
    return GetPaletteData(nclrBuffer, outPaletteData);
}

void *Graphics_GetCellBank(enum NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dCellDataBank **outCellBank, u32 heapID)
{
    void *ncerBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, FALSE);
    return GetCellBank(ncerBuffer, outCellBank);
}

void *Graphics_GetAnimBank(enum NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dAnimBankData **outAnimBank, u32 heapID)
{
    void *nanrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, FALSE);
    return GetAnimBank(nanrBuffer, outAnimBank);
}

void *LoadCompressedMemberFromNARC(enum NarcID narcID, u32 narcMemberIdx, u32 heapID)
{
    return LoadMemberFromNARC(narcID, narcMemberIdx, TRUE, heapID, FALSE);
}

void *LoadMemberFromNARC(enum NarcID narcID, u32 narcMemberIdx, BOOL compressed, u32 heapID, BOOL allocAtEnd)
{
    void *data;

    if (compressed || allocAtEnd == TRUE) {
        data = Heap_AllocAtEnd(heapID, NARC_GetMemberSizeByIndexPair(narcID, narcMemberIdx));
    } else {
        data = Heap_Alloc(heapID, NARC_GetMemberSizeByIndexPair(narcID, narcMemberIdx));
    }

    if (data != NULL) {
        NARC_ReadWholeMemberByIndexPair(data, narcID, narcMemberIdx);

        if (compressed) {
            void *uncompBuf;

            if (allocAtEnd == FALSE) {
                uncompBuf = Heap_Alloc(heapID, MI_GetUncompressedSize(data));
            } else {
                uncompBuf = Heap_AllocAtEnd(heapID, MI_GetUncompressedSize(data));
            }

            if (uncompBuf) {
                MI_UncompressLZ8(data, uncompBuf);
                Heap_Free(data);
            }

            data = uncompBuf;
        }
    }

    return data;
}

void *LoadMemberFromNARC_OutFileSize(enum NarcID narcID, u32 narcMemberIdx, BOOL compressed, u32 heapID, BOOL allocAtEnd, u32 *fileSize)
{
    void *data;
    *fileSize = NARC_GetMemberSizeByIndexPair(narcID, narcMemberIdx);

    if (compressed || allocAtEnd == TRUE) {
        data = Heap_AllocAtEnd(heapID, *fileSize);
    } else {
        data = Heap_Alloc(heapID, *fileSize);
    }

    if (data != NULL) {
        NARC_ReadWholeMemberByIndexPair(data, narcID, narcMemberIdx);

        if (compressed) {
            *fileSize = MI_GetUncompressedSize(data);

            void *uncompBuf;
            if (allocAtEnd == FALSE) {
                uncompBuf = Heap_Alloc(heapID, *fileSize);
            } else {
                uncompBuf = Heap_AllocAtEnd(heapID, *fileSize);
            }

            if (uncompBuf) {
                MI_UncompressLZ8(data, uncompBuf);
                Heap_Free(data);
            }

            data = uncompBuf;
        }
    }

    return data;
}

u32 Graphics_LoadTilesToBgLayerFromOpenNARC(NARC *narc, u32 narcMemberIdx, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
    void *ncgrBuffer = LoadMemberFromOpenNARC(narc, narcMemberIdx, compressed, heapID, FALSE);
    return LoadTilesToBgLayer(ncgrBuffer, bgConfig, bgLayer, offset, size);
}

void Graphics_LoadTilemapToBgLayerFromOpenNARC(NARC *narc, u32 narcMemberIdx, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID)
{
    void *nscrBuffer = LoadMemberFromOpenNARC(narc, narcMemberIdx, compressed, heapID, TRUE);
    LoadTilemapToBgLayer(nscrBuffer, bgConfig, bgLayer, offset, size);
}

void Graphics_LoadPaletteFromOpenNARC(NARC *narc, u32 narcMemberIdx, enum PaletteLoadLocation loadLocation, u32 offset, u32 size, u32 heapID)
{
    Graphics_LoadPaletteWithSrcOffsetFromOpenNARC(narc, narcMemberIdx, loadLocation, FALSE, offset, size, heapID);
}

void Graphics_LoadPaletteWithSrcOffsetFromOpenNARC(NARC *narc, u32 narcMemberIdx, enum PaletteLoadLocation loadLocation, u32 srcOffset, u32 offset, u32 size, u32 heapID)
{
    void *nclrBuffer = LoadMemberFromOpenNARC(narc, narcMemberIdx, FALSE, heapID, TRUE);
    LoadPaletteWithSrcOffset(nclrBuffer, loadLocation, srcOffset, offset, size);
}

void Graphics_LoadPartialPaletteFromOpenNARC(NARC *narc, u32 narcMemberIdx, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, u32 heapID, NNSG2dImagePaletteProxy *paletteProxy)
{
    void *nclrBuffer = LoadMemberFromOpenNARC(narc, narcMemberIdx, FALSE, heapID, TRUE);
    LoadPartialPalette(nclrBuffer, vramType, baseAddr, paletteProxy);
}

u32 Graphics_LoadImageMappingFromOpenNARC(NARC *narc, u32 narcMemberIdx, BOOL compressed, enum ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, u32 heapID, NNSG2dImageProxy *imageProxy)
{
    void *ncgrBuffer = LoadMemberFromOpenNARC(narc, narcMemberIdx, compressed, heapID, TRUE);
    return LoadImageMapping(ncgrBuffer, layout, size, vramType, baseAddr, imageProxy);
}

void *Graphics_GetCharDataFromOpenNARC(NARC *narc, u32 narcMemberIdx, BOOL compressed, NNSG2dCharacterData **outCharData, u32 heapID)
{
    void *ncgrBuffer = LoadMemberFromOpenNARC(narc, narcMemberIdx, compressed, heapID, FALSE);
    return GetCharacterData(ncgrBuffer, outCharData);
}

void *Graphics_GetScrnDataFromOpenNARC(NARC *narc, u32 narcMemberIdx, BOOL compressed, NNSG2dScreenData **outScreenData, u32 heapID)
{
    void *nscrBuffer = LoadMemberFromOpenNARC(narc, narcMemberIdx, compressed, heapID, FALSE);
    return GetScreenData(nscrBuffer, outScreenData);
}

void *Graphics_GetPlttDataFromOpenNARC(NARC *narc, u32 narcMemberIdx, NNSG2dPaletteData **outPaletteData, u32 heapID)
{
    void *nclrBuffer = LoadMemberFromOpenNARC(narc, narcMemberIdx, FALSE, heapID, FALSE);
    return GetPaletteData(nclrBuffer, outPaletteData);
}

void *Graphics_GetCellBankFromOpenNARC(NARC *narc, u32 narcMemberIdx, BOOL compressed, NNSG2dCellDataBank **outCellBank, u32 heapID)
{
    void *ncerBuffer = LoadMemberFromOpenNARC(narc, narcMemberIdx, compressed, heapID, 0);
    return GetCellBank(ncerBuffer, outCellBank);
}

void *Graphics_GetAnimBankFromOpenNARC(NARC *narc, u32 narcMemberIdx, BOOL compressed, NNSG2dAnimBankData **outAnimBank, u32 heapID)
{
    void *nanrBuffer = LoadMemberFromOpenNARC(narc, narcMemberIdx, compressed, heapID, 0);
    return GetAnimBank(nanrBuffer, outAnimBank);
}

void *LoadMemberFromOpenNARC(NARC *narc, u32 narcMemberIdx, BOOL compressed, u32 heapID, BOOL allocAtEnd)
{
    u32 tmp;
    return LoadMemberFromOpenNARC_OutFileSize(narc, narcMemberIdx, compressed, heapID, allocAtEnd, &tmp);
}

void *LoadMemberFromOpenNARC_OutFileSize(NARC *narc, u32 narcMemberIdx, BOOL compressed, u32 heapID, BOOL allocAtEnd, u32 *fileSize)
{
    void *data;
    *fileSize = NARC_GetMemberSize(narc, narcMemberIdx);

    if (compressed || allocAtEnd == TRUE) {
        data = Heap_AllocAtEnd(heapID, *fileSize);
    } else {
        data = Heap_Alloc(heapID, *fileSize);
    }

    if (data != NULL) {
        NARC_ReadWholeMember(narc, narcMemberIdx, data);

        if (compressed) {
            *fileSize = MI_GetUncompressedSize(data);

            void *uncompBuf;
            if (allocAtEnd == FALSE) {
                uncompBuf = Heap_Alloc(heapID, *fileSize);
            } else {
                uncompBuf = Heap_AllocAtEnd(heapID, *fileSize);
            }

            if (uncompBuf) {
                MI_UncompressLZ8(data, uncompBuf);
                Heap_Free(data);
            }

            data = uncompBuf;
        }
    }

    return data;
}

static u32 LoadTilesToBgLayer(void *ncgrBuffer, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size)
{
    if (ncgrBuffer != NULL) {
        NNSG2dCharacterData *tiles;

        if (NNS_G2dGetUnpackedBGCharacterData(ncgrBuffer, &tiles)) {
            if (size == 0) {
                size = tiles->szByte;
            }

            Bg_LoadTiles(bgConfig, bgLayer, tiles->pRawData, size, offset);
        }

        Heap_Free(ncgrBuffer);
    }

    return size;
}

static void LoadTilemapToBgLayer(void *nscrBuffer, BgConfig *bgConfig, u32 bgLayer, u32 offset, u32 size)
{
    if (nscrBuffer != NULL) {
        NNSG2dScreenData *tilemap;

        if (NNS_G2dGetUnpackedScreenData(nscrBuffer, &tilemap)) {
            if (size == 0) {
                size = tilemap->szByte;
            }

            if (Bg_GetTilemapBuffer(bgConfig, bgLayer) != NULL) {
                Bg_LoadTilemapBuffer(bgConfig, bgLayer, tilemap->rawData, size);
            }

            Bg_CopyTilemapBufferRangeToVRAM(bgConfig, bgLayer, tilemap->rawData, size, offset);
        }

        Heap_Free(nscrBuffer);
    }
}

static void (*const sDisplayObjectLoadFunc[])(const void *, u32, u32) = {
    GX_LoadOBJ,
    GXS_LoadOBJ
};

static u32 LoadObjectTiles(void *ncgrBuffer, enum DSScreen display, u32 offset, u32 size)
{

    if (ncgrBuffer != NULL) {
        NNSG2dCharacterData *v1;

        if (NNS_G2dGetUnpackedCharacterData(ncgrBuffer, &v1)) {
            if (size == 0) {
                size = v1->szByte;
            }

            DC_FlushRange(v1->pRawData, size);
            sDisplayObjectLoadFunc[display](v1->pRawData, offset, size);
        }

        Heap_Free(ncgrBuffer);
    }

    return size;
}

static void (*const sPaletteLoadFuncs[])(const void *, u32, u32) = {
    GX_LoadBGPltt,
    GX_LoadOBJPltt,
    GX_LoadBGExtPltt,
    GX_LoadOBJExtPltt,
    GXS_LoadBGPltt,
    GXS_LoadOBJPltt,
    GXS_LoadBGExtPltt,
    GXS_LoadOBJExtPltt
};

static void LoadPaletteWithSrcOffset(void *nclrBuffer, enum PaletteLoadLocation loadLocation, u32 srcOffset, u32 offset, u32 size)
{
    if (nclrBuffer != NULL) {
        NNSG2dPaletteData *palette;

        if (NNS_G2dGetUnpackedPaletteData(nclrBuffer, &palette)) {
            palette->pRawData = (void *)((u32)palette->pRawData + srcOffset);

            if (size == 0) {
                size = palette->szByte - srcOffset;
            }

            DC_FlushRange(palette->pRawData, size);

            switch (loadLocation) {
            case PAL_LOAD_MAIN_BGEXT:
                GX_BeginLoadBGExtPltt();
                sPaletteLoadFuncs[loadLocation](palette->pRawData, offset, size);
                GX_EndLoadBGExtPltt();
                break;

            case PAL_LOAD_SUB_BGEXT:
                GXS_BeginLoadBGExtPltt();
                sPaletteLoadFuncs[loadLocation](palette->pRawData, offset, size);
                GXS_EndLoadBGExtPltt();
                break;

            case PAL_LOAD_MAIN_OBJEXT:
                GX_BeginLoadOBJExtPltt();
                sPaletteLoadFuncs[loadLocation](palette->pRawData, offset, size);
                GX_EndLoadOBJExtPltt();
                break;

            case PAL_LOAD_SUB_OBJEXT:
                GXS_BeginLoadOBJExtPltt();
                sPaletteLoadFuncs[loadLocation](palette->pRawData, offset, size);
                GXS_EndLoadOBJExtPltt();
                break;

            default:
                sPaletteLoadFuncs[loadLocation](palette->pRawData, offset, size);
                break;
            }
        }

        Heap_Free(nclrBuffer);
    }
}

static void LoadPartialPalette(void *nclrBuffer, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, NNSG2dImagePaletteProxy *paletteProxy)
{
    if (nclrBuffer != NULL) {
        NNSG2dPaletteData *palette;
        NNSG2dPaletteCompressInfo *compPalette;
        BOOL extended = NNS_G2dGetUnpackedPaletteCompressInfo(nclrBuffer, &compPalette);

        if (NNS_G2dGetUnpackedPaletteData(nclrBuffer, &palette)) {
            if (extended) {
                NNS_G2dLoadPaletteEx(palette, compPalette, baseAddr, vramType, paletteProxy);
            } else {
                NNS_G2dLoadPalette(palette, baseAddr, vramType, paletteProxy);
            }
        }

        Heap_Free(nclrBuffer);
    }
}

static void (*const sImageLayoutLoadFuncs[])(const NNSG2dCharacterData *, u32, NNS_G2D_VRAM_TYPE, NNSG2dImageProxy *) = {
    NNS_G2dLoadImage1DMapping,
    NNS_G2dLoadImage2DMapping
};

static u32 LoadImageMapping(void *ncgrBuffer, enum ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, NNSG2dImageProxy *imageProxy)
{
    u32 result = 0;

    if (ncgrBuffer != NULL) {
        NNSG2dCharacterData *tiles;

        if (NNS_G2dGetUnpackedCharacterData(ncgrBuffer, &tiles)) {
            if (size) {
                tiles->szByte = size;
            }

            sImageLayoutLoadFuncs[layout](tiles, baseAddr, vramType, imageProxy);
            result = tiles->szByte;
        }

        Heap_Free(ncgrBuffer);
    }

    return result;
}

static void (*const sImageLayoutLoadFuncs2[])(const NNSG2dCharacterData *, u32, NNS_G2D_VRAM_TYPE, NNSG2dImageProxy *) = {
    NNS_G2dLoadImage1DMapping,
    NNS_G2dLoadImage2DMapping
};

static void LoadImageMappingAndSetVramMode(void *ncgrBuffer, enum ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, NNSG2dImageProxy *imageProxy)
{
    if (ncgrBuffer != NULL) {
        NNSG2dCharacterData *tiles;

        if (NNS_G2dGetUnpackedCharacterData(ncgrBuffer, &tiles)) {
            if (size) {
                tiles->szByte = size;
            }

            switch (vramType) {
            case NNS_G2D_VRAM_TYPE_2DMAIN:
                tiles->mapingType = GX_GetOBJVRamModeChar();
                break;
            case NNS_G2D_VRAM_TYPE_2DSUB:
                tiles->mapingType = GXS_GetOBJVRamModeChar();
                break;
            }

            sImageLayoutLoadFuncs2[layout](tiles, baseAddr, vramType, imageProxy);
        }

        Heap_Free(ncgrBuffer);
    }
}

static void *GetCharacterData(void *ncgrBuffer, NNSG2dCharacterData **outCharData)
{
    if (ncgrBuffer != NULL) {
        if (NNS_G2dGetUnpackedBGCharacterData(ncgrBuffer, outCharData) == 0) {
            Heap_Free(ncgrBuffer);
            return NULL;
        }
    }

    return ncgrBuffer;
}

static void *GetScreenData(void *nscrBuffer, NNSG2dScreenData **outScreenData)
{
    if (nscrBuffer != NULL) {
        if (NNS_G2dGetUnpackedScreenData(nscrBuffer, outScreenData) == 0) {
            Heap_Free(nscrBuffer);
            return NULL;
        }
    }

    return nscrBuffer;
}

static void *GetPaletteData(void *nclrBuffer, NNSG2dPaletteData **outPaletteData)
{
    if (nclrBuffer != NULL) {
        if (NNS_G2dGetUnpackedPaletteData(nclrBuffer, outPaletteData) == 0) {
            Heap_Free(nclrBuffer);
            return NULL;
        }
    }

    return nclrBuffer;
}

static void *GetCellBank(void *ncerBuffer, NNSG2dCellDataBank **outCellBank)
{
    if (ncerBuffer != NULL) {
        if (NNS_G2dGetUnpackedCellBank(ncerBuffer, outCellBank) == 0) {
            Heap_Free(ncerBuffer);
            return NULL;
        }
    }

    return ncerBuffer;
}

static void *GetAnimBank(void *nanrBuffer, NNSG2dAnimBankData **outAnimBank)
{
    if (nanrBuffer != NULL) {
        if (NNS_G2dGetUnpackedAnimBank(nanrBuffer, outAnimBank) == 0) {
            Heap_Free(nanrBuffer);
            return NULL;
        }
    }

    return nanrBuffer;
}

#endif // PLATFORM_DS
