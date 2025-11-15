# Asset Conversion Tools

This directory contains tools for extracting and converting Nintendo DS assets from the Pokemon Platinum ROM into formats suitable for the SDL3 port.

## Overview

The asset pipeline consists of three main tools:

1. **narc_extractor.py** - Extracts NARC archives from the ROM
2. **ncgr_converter.py** - Converts NCGR graphics to raw tile data
3. **nclr_converter.py** - Converts NCLR palettes to RGB555 binary format
4. **convert_assets.sh** - Automation script for batch conversion

## Prerequisites

### Required Software
- Python 3.8 or newer
- pip (Python package manager)

### Install Dependencies

```bash
pip install ndspy
```

### ROM Requirement
You need a legally obtained copy of **Pokemon Platinum (US version)**:
- File: `platinum.us.nds`
- Game Code: `CPUE` (US) or `CPUJ` (JP)
- Not included in this repository

## Quick Start - Title Screen Example

For a complete working example of the asset pipeline, see the title screen conversion:

```bash
# 1. Extract title screen NARC (ID 319)
python3 tools/narc_extractor.py pokeplatinum.us.nds 319 resources/graphics/title_screen_raw/

# 2. Convert graphics and palettes
python3 tools/convert_title_screen.py

# Output: 12 converted files ready for PAL in resources/graphics/title_screen/
```

**Result:** Successfully extracted 29 files and converted 6 graphics (1572 tiles total) + 6 palettes (260 colors each).

## Usage

### Step 1: Identify NARC Files

First, list all files in the ROM to find NARC archives:

```bash
python3 narc_extractor.py path/to/platinum.us.nds --list
```

This will display all files in the ROM. Look for:
- Files with `.narc` extension
- Files with `NARC` magic bytes
- Common paths: `data/a/0/0/*`, `data/a/0/1/*`, etc.

**Example Output:**
```
  demo/title/titledemo.narc                          | ID:  319 | Size:   219192 | Magic: NARC
  demo/title/op_demo.narc                            | ID:  318 | Size:  1257224 | Magic: NARC
```

### Step 2: Extract a NARC

Once you've identified a NARC ID, extract it:

```bash
python3 narc_extractor.py path/to/platinum.us.nds <narc_id> output_dir/
```

Example (title screen):
```bash
python3 narc_extractor.py pokeplatinum.us.nds 319 resources/graphics/title_screen_raw/
```

**Output:**
```
Loading ROM: pokeplatinum.us.nds
ROM loaded successfully
Game Code: CPUE
Game Title: POKEMON PL

Searching for file ID 319...
Found file: demo/title/titledemo.narc
Confirmed NARC file
Parsing NARC archive...
NARC contains 29 files
✓ Successfully extracted 29 files to resources/graphics/title_screen_raw/
```

This creates:
- `output_dir/0000.bin`, `0001.bin`, etc. - Extracted files
- `output_dir/narc_info.txt` - Metadata about the NARC

### Step 3: Convert Graphics (NCGR)

Convert extracted NCGR files to raw tile data:

**Single file:**
```bash
python3 ncgr_converter.py input.ncgr output_tiles.bin
```

**Batch convert directory:**
```bash
python3 ncgr_converter.py --batch resources/graphics/narc_000/ resources/converted/
```

**With detailed info:**
```bash
python3 ncgr_converter.py input.ncgr output_tiles.bin --info
```

Output files:
- `output_tiles.bin` - Raw tile data (32 bytes/tile for 4bpp, 64 bytes/tile for 8bpp)
- `output_tiles_info.txt` - Metadata and usage instructions

### Step 4: Convert Palettes (NCLR)

Convert palette files to RGB555 binary or JSON:

**Binary format (for PAL):**
```bash
python3 nclr_converter.py input.nclr output.pal
```

**JSON format (human-readable):**
```bash
python3 nclr_converter.py input.nclr output.json --json
```

**Batch convert:**
```bash
python3 nclr_converter.py --batch resources/graphics/narc_000/ resources/converted/
```

Output files:
- `output.pal` - Raw RGB555 data (16-bit little-endian per color)
- `output_info.txt` - Palette metadata and color list

### Step 5: Automated Conversion (Recommended)

For batch processing multiple NARCs:

```bash
./convert_assets.sh path/to/platinum.us.nds
```

**Note:** You'll need to edit `convert_assets.sh` to specify which NARC IDs to extract. See the script comments for details.

## File Formats

### NCGR (Nitro Character Graphics Resource)
- **4bpp**: 4 bits per pixel (16 colors), 32 bytes per 8x8 tile, requires palette
- **8bpp**: 8 bits per pixel (256 colors), 64 bytes per 8x8 tile, requires palette
- **Layout**: Tiles are stored row-by-row, 2 pixels per byte (4bpp) or 1 pixel per byte (8bpp)

### NCLR (Nitro Color Resource)
- **Format**: RGB555 (5 bits per channel: R, G, B)
- **Encoding**: 16-bit little-endian per color
- **Palettes**: 16 colors for 4bpp, 256 colors for 8bpp
- **Conversion**: RGB555 → RGB888 for SDL3

### NARC (Nitro Archive)
- **Container**: Multiple files packed into one archive
- **Structure**: Header + file table + data blocks
- **Usage**: Pokemon Platinum stores all graphics, maps, text, etc. in NARC files

## Integration with PAL

### Loading Tiles
```c
// Load converted tile data
PAL_File file = PAL_File_Open("resources/graphics/narc_000/0000_tiles.bin", "rb");
size_t size = PAL_File_Size(file);
void* tile_data = Heap_Alloc(heapID, size);
PAL_File_Read(tile_data, 1, size, file);
PAL_File_Close(file);

// Load to background layer
PAL_Bg_LoadTiles(bgConfig, layer, tile_data, size, 0);
Heap_Free(tile_data);
```

### Loading Palettes
```c
// Load converted palette
PAL_File file = PAL_File_Open("resources/graphics/narc_000/0001_palette.pal", "rb");
size_t palette_size = PAL_File_Size(file);
u16* palette_data = Heap_Alloc(heapID, palette_size);
PAL_File_Read(palette_data, 1, palette_size, file);
PAL_File_Close(file);

// Load to background layer
PAL_Bg_LoadPalette(bgConfig, layer, palette_data, palette_size, 0);
Heap_Free(palette_data);
```

## Directory Structure

After conversion, assets are organized like this:

```
resources/
├── graphics/
│   ├── narc_000_raw/          # Raw extracted NARC files
│   │   ├── 0000.bin
│   │   ├── 0001.bin
│   │   └── narc_info.txt
│   └── narc_000/              # Converted assets
│       ├── 0000_tiles.bin     # Converted graphics
│       ├── 0000_info.txt      # Tile metadata
│       ├── 0001_palette.pal   # Converted palette
│       └── 0001_info.txt      # Palette metadata
```

## Tested NARC Files

| NARC ID | Path | Size | Contents | Status |
|---------|------|------|----------|--------|
| 319 | `demo/title/titledemo.narc` | 219KB | Title screen graphics (29 files) | ✅ Working |
| 318 | `demo/title/op_demo.narc` | 1.2MB | Opening demo | ⏳ Untested |
| 139 | `arc/demo_tengan_gra.narc` | 953KB | Giratina demo graphics | ⏳ Untested |

Add more as you test them!

## Troubleshooting

### "ndspy library not installed"
```bash
pip install ndspy
```

**Note:** Successfully tested with ndspy 4.2.0 on Python 3.11+

### "ROM file not found"
Verify the ROM path is correct and the file exists.

### "Could not find NARC X in ROM"
The NARC ID may be incorrect. Use `--list` to browse the ROM structure.

**Known Working:** File ID 319 extracts successfully to 29 files.

### "Error parsing NARC"
The file may not be a valid NARC. Check the file magic bytes (should be `NARC` or `0x4E415243`).

### Converted files are wrong size
- Check if graphics are compressed (NARC extraction handles this automatically)
- Verify the color depth (4bpp vs 8bpp)
- Check tile count in `_info.txt` files

### Manual Parsers vs ndspy
The `convert_title_screen.py` uses manual NCGR/NCLR parsing because ndspy 4.x doesn't expose direct graphics2D conversion APIs. The manual parsers work correctly for tested files.

## Development Tips

### Finding Title Screen Assets
1. List ROM files: `python3 narc_extractor.py rom.nds --list`
2. Look for paths like `data/a/0/0/*` (common for graphics)
3. Extract suspected NARCs and check the file sizes
4. Title screen graphics are usually large (10KB-100KB+)
5. Palettes are small (32 bytes for 16 colors, 512 bytes for 256 colors)

### Identifying File Types
Files extracted from NARCs may not have extensions. Check magic bytes:
- **NCGR**: `RGCN` (0x4E434752, reversed)
- **NCLR**: `RLCN` (0x4E434C52, reversed)
- **NSCR**: `RCSN` (0x4E534352, reversed) - Screen/tilemap data
- **NCER**: `RECN` - Cell data (sprite metadata)
- **NANR**: `RNAN` - Animation data

### Testing Converted Assets
Use the test application to verify conversions:
```bash
./build_sdl.sh
./build-sdl/pokeplatinum_sdl
```

Load assets in the test app and verify colors/graphics appear correctly.

## References

- **GBATEK**: DS hardware documentation - https://problemkaputt.de/gbatek.htm
- **ndspy Documentation**: Python DS library - https://github.com/RoadrunnerWMC/ndspy
- **DS Format Docs**: Various DS file format specifications

## Testing Results (November 14, 2025)

### Successfully Converted Assets

**Title Screen (NARC 319):**
- ✅ Extracted 29 files from `demo/title/titledemo.narc`
- ✅ Identified 6 NCGR graphics + 6 NCLR palettes
- ✅ Converted using `convert_title_screen.py`:
  - `top_border_tiles.bin`: 512 tiles (8bpp) - 33KB
  - `logo_top_tiles.bin`: 384 tiles (8bpp) - 25KB
  - `logo_bottom_tiles.bin`: 512 tiles (8bpp) - 33KB
  - `press_start_tiles.bin`: 128 tiles (8bpp) - 8KB
  - Plus 2 smaller graphics
  - 6 palettes (260 colors each, RGB555 format)
- ✅ Output: `resources/graphics/title_screen/` with 24 files ready for PAL

**Performance:**
- Extraction: ~2 seconds
- Conversion: ~1 second
- Total: ~3 seconds for complete title screen assets

## Contributing

When adding support for new NARC files:
1. Document the NARC ID and purpose in the "Tested NARC Files" table
2. Add extraction to `convert_assets.sh` if needed
3. Test conversion with the tools
4. Update this README with results

## License

These tools are part of the Pokemon Platinum SDL3 port project. See main repository LICENSE.
