# DS Graphics Implementation Reference

**Purpose:** This document preserves the original Nintendo DS graphics implementation for reference during SDL3 porting. These functions were removed from `src/graphics.c` but are documented here to understand the original behavior.

**Date Archived:** November 15, 2025  
**Original Location:** `src/graphics.c` lines 193-697

---

## Overview

The DS implementation used NitroSystem G2D libraries for graphics loading. Key patterns:

1. **NARC Loading:** Graphics stored in NARC archives, extracted to memory
2. **NCGR/NCLR Formats:** Nintendo Compressed Graphics (tiles) and Nintendo Compressed Colors (palettes)
3. **VRAM Management:** Direct VRAM upload via `DC_FlushRange()` and `GX_LoadTiles()`
4. **Proxy Objects:** NNS_G2D proxies track VRAM locations for sprites/backgrounds

---

## Function Signatures

### Object (Sprite) Loading
```c
// Load sprite tiles to VRAM
u32 Graphics_LoadObjectTiles(enum NarcID narcID, u32 narcMemberIdx, 
                             enum DSScreen display, u32 offset, u32 size, 
                             BOOL compressed, u32 heapID);

// Load sprite palette
void Graphics_LoadPartialPalette(enum NarcID narcID, u32 narcMemberIdx, 
                                 NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, 
                                 u32 heapID, NNSG2dImagePaletteProxy *paletteProxy);

// Load sprite image mapping
u32 Graphics_LoadImageMapping(enum NarcID narcID, u32 narcMemberIdx, 
                              BOOL compressed, enum ImageMappingLayout layout, 
                              u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, 
                              u32 heapID, NNSG2dImageProxy *imageProxy);

// Load full palette (512 colors)
void Graphics_LoadPalette(enum NarcID narcID, u32 narcMemberIdx, 
                         NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, 
                         u32 heapID, NNSG2dImagePaletteProxy *paletteProxy);
```

### Multi-Palette Loading
```c
// Load 16 palettes at once (common for UI)
void Graphics_LoadPaletteBuffer(enum NarcID narcID, u32 narcMemberIdx, 
                               NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, 
                               u32 heapID, NNSG2dImagePaletteProxy *paletteProxy);
```

### Animation Loading
```c
// Load multi-cell animation data
void Graphics_LoadMultiCellAnimation(enum NarcID narcID, u32 narcMemberIdx, 
                                    BOOL compressed, NNSG2dMultiCellAnimation *cellAnimation, 
                                    u32 heapID);
```

### NARC Iteration
```c
// Iterate through all members of a NARC archive
void *Graphics_GetNARCMemberPointer(NARC *narc, u32 narcMemberIdx);
```

---

## Key Implementation Details

### NARC Member Loading Pattern
```c
// Standard pattern for loading from NARC
void *LoadMemberFromNARC(enum NarcID narcID, u32 narcMemberIdx, 
                        BOOL compressed, u32 heapID, BOOL allocExtra) {
    NARC *narc = NARC_ctor(narcID, heapID);
    void *buffer;
    
    if (allocExtra) {
        u32 size = NARC_GetMemberSize(narc, narcMemberIdx);
        buffer = Heap_Alloc(heapID, size + 32);  // Extra space for decompression
    } else {
        buffer = NARC_AllocAndReadWholeMember(narc, narcMemberIdx, heapID);
    }
    
    if (compressed) {
        void *decompressed = DecompressBuffer(buffer, heapID);
        Heap_Free(buffer);
        buffer = decompressed;
    }
    
    NARC_dtor(narc);
    return buffer;
}
```

### VRAM Upload Pattern
```c
// Standard pattern for uploading tiles to VRAM
u32 LoadObjectTiles(void *ncgrBuffer, enum DSScreen display, 
                    u32 offset, u32 size) {
    NNSG2dCharacterData *charData;
    NNS_G2dGetUnpackedCharacterData(ncgrBuffer, &charData);
    
    DC_FlushRange(charData->pRawData, size);
    
    if (display == DS_SCREEN_MAIN) {
        GX_LoadOBJ(charData->pRawData, offset, size);
    } else {
        GXS_LoadOBJ(charData->pRawData, offset, size);
    }
    
    return size;
}
```

### Palette Loading Pattern
```c
// Standard pattern for loading palettes
void LoadPartialPalette(void *nclrBuffer, NNS_G2D_VRAM_TYPE vramType, 
                       u32 baseAddr, NNSG2dImagePaletteProxy *paletteProxy) {
    NNSG2dPaletteData *paletteData;
    NNS_G2dGetUnpackedPaletteData(nclrBuffer, &paletteData);
    
    NNS_G2dLoadPalette(paletteData, vramType, baseAddr, paletteProxy);
}
```

---

## SDL3 Porting Notes

### What We've Already Ported

1. ✅ **Graphics_LoadTilesToBgLayer()** - Loads tile data for backgrounds
   - SDL: Uses `PAL_File_Open()` + `PAL_Bg_LoadTiles()`
   - DS: Used `LoadMemberFromNARC()` + `Bg_LoadTilesToBgLayerVRAM()`

2. ✅ **Graphics_LoadPaletteWithSrcOffset()** - Loads palette data
   - SDL: Uses `PAL_File_Open()` + `PAL_Graphics_LoadPalette()`
   - DS: Used `LoadMemberFromNARC()` + `Bg_LoadPalette()`

### What Still Needs Porting

1. **Sprite/Object Graphics Loading**
   - `Graphics_LoadObjectTiles()` - Load sprite graphics
   - `Graphics_LoadImageMapping()` - Load sprite mappings
   - Need PAL equivalents using SDL textures

2. **Multi-Palette Systems**
   - `Graphics_LoadPaletteBuffer()` - Load multiple palettes
   - Used for UI elements with many color schemes

3. **Animation Systems**
   - `Graphics_LoadMultiCellAnimation()` - Load animation data
   - Will need PAL animation system

---

## Compression Formats Used

- **LZ77:** Most common compression (tiles, maps)
- **Huffman:** Less common, some graphics
- **RLE:** Run-length encoding for simple patterns

**SDL Port:** Convert assets offline to uncompressed formats, or implement decompression in PAL.

---

## VRAM Layout (For Reference)

### Main Screen
- **BG VRAM:** 512 KB (Banks A-D)
- **OBJ VRAM:** 128 KB (Banks E-F)
- **Palette RAM:** 2 KB (512 colors × 16-bit)

### Sub Screen  
- **BG VRAM:** 128 KB (Banks C-D shared)
- **OBJ VRAM:** 128 KB (Bank I)
- **Palette RAM:** 2 KB (512 colors × 16-bit)

**SDL Port:** No VRAM limits, use SDL textures and renderer memory.

---

## References

- **NitroSDK Documentation:** `include/nnsys/g2d/`
- **GBATEK:** Hardware specifications for DS graphics
- **Our PAL Documentation:** `PAL.md` for SDL3 equivalents

---

**Note:** This reference is for understanding only. Do not attempt to reintegrate this code. Use PAL abstractions for all new SDL3 implementations.
