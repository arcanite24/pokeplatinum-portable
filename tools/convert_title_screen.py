#!/usr/bin/env python3
"""
Quick converter for title screen NARC files
"""

import sys
import struct
from pathlib import Path

def read_nclr(file_path):
    """
    Read NCLR palette file and extract RGB555 colors
    """
    with open(file_path, 'rb') as f:
        data = f.read()
    
    # Check magic
    if data[:4] != b'RLCN':
        print(f"Not a valid NCLR file (magic: {data[:4]})")
        return None
    
    # Find PLTT chunk
    offset = 0x10  # Skip header
    while offset < len(data):
        chunk_magic = data[offset:offset+4]
        chunk_size = struct.unpack('<I', data[offset+4:offset+8])[0]
        
        if chunk_magic == b'TTLP':
            # Found palette data
            # Skip chunk header (8 bytes) + palette header (8 bytes)
            palette_offset = offset + 16
            palette_data = data[palette_offset:offset+chunk_size]
            
            # Convert to 16-bit RGB555 values
            num_colors = len(palette_data) // 2
            colors = []
            for i in range(num_colors):
                rgb555 = struct.unpack('<H', palette_data[i*2:i*2+2])[0]
                colors.append(rgb555)
            
            return colors
        
        offset += chunk_size
    
    return None

def read_ncgr(file_path):
    """
    Read NCGR graphics file and extract tile data
    """
    with open(file_path, 'rb') as f:
        data = f.read()
    
    # Check magic
    if data[:4] != b'RGCN':
        print(f"Not a valid NCGR file (magic: {data[:4]})")
        return None, None
    
    # Find CHAR chunk
    offset = 0x10  # Skip header
    while offset < len(data):
        chunk_magic = data[offset:offset+4]
        chunk_size = struct.unpack('<I', data[offset+4:offset+8])[0]
        
        if chunk_magic == b'RAHC':
            # Found character (tile) data
            # Get depth from header
            depth = data[offset+0x14]  # 3=4bpp, 4=8bpp
            color_mode = '4bpp' if depth == 3 else '8bpp'
            
            # Skip chunk header + CHAR header
            tile_offset = offset + 0x18
            tile_data = data[tile_offset:offset+chunk_size]
            
            return tile_data, color_mode
        
        offset += chunk_size
    
    return None, None

def main():
    # Convert 0005.bin (NCGR) and 0004.bin (NCLR)
    input_dir = Path("resources/graphics/title_screen_raw")
    output_dir = Path("resources/graphics/title_screen")
    output_dir.mkdir(parents=True, exist_ok=True)
    
    print("Converting title screen assets...")
    
    # Convert NCGR files (based on titledemo.naix.h)
    ncgr_files = [
        (5, "gf_presents"),          # gf_presents_NCGR
        (9, "logo_jp"),              # logo_jp_NCGR (Japanese logo)
        (12, "logo"),                # logo_NCGR (English logo)
        (15, "copyright"),           # copyright_NCGR
        (23, "top_screen_border"),   # top_screen_border_NCGR
        (26, "bottom_screen_border") # bottom_screen_border_NCGR
    ]
    
    for file_id, name in ncgr_files:
        input_file = input_dir / f"{file_id:04d}.bin"
        if not input_file.exists():
            print(f"Skipping {input_file} (not found)")
            continue
        
        tile_data, color_mode = read_ncgr(input_file)
        if tile_data:
            output_file = output_dir / f"{name}_tiles.bin"
            with open(output_file, 'wb') as f:
                f.write(tile_data)
            
            num_tiles = len(tile_data) // (32 if color_mode == '4bpp' else 64)
            print(f"✓ {name}: {num_tiles} tiles ({color_mode}) → {output_file}")
            
            # Write info
            info_file = output_dir / f"{name}_tiles_info.txt"
            with open(info_file, 'w') as f:
                f.write(f"Source: {input_file.name}\n")
                f.write(f"Format: NCGR\n")
                f.write(f"Color Mode: {color_mode}\n")
                f.write(f"Tile Count: {num_tiles}\n")
                f.write(f"Tile Size: 8x8 pixels\n")
                f.write(f"Data Size: {len(tile_data)} bytes\n")
    
    # Convert NCLR files (based on titledemo.naix.h)
    nclr_files = [
        (4, "gf_presents"),          # gf_presents_NCLR
        (6, "bottom_screen_border"), # bottom_screen_border_NCLR
        (7, "top_screen_border"),    # top_screen_border_NCLR
        (8, "logo_jp"),              # logo_jp_NCLR
        (11, "logo"),                # logo_NCLR
        (14, "copyright")            # copyright_NCLR
    ]
    
    for file_id, name in nclr_files:
        input_file = input_dir / f"{file_id:04d}.bin"
        if not input_file.exists():
            print(f"Skipping {input_file} (not found)")
            continue
        
        colors = read_nclr(input_file)
        if colors:
            # Save as binary RGB555
            output_file = output_dir / f"{name}.pal"
            with open(output_file, 'wb') as f:
                for color in colors:
                    f.write(struct.pack('<H', color))
            
            print(f"✓ {name}: {len(colors)} colors → {output_file}")
            
            # Write info
            info_file = output_dir / f"{name}_info.txt"
            with open(info_file, 'w') as f:
                f.write(f"Source: {input_file.name}\n")
                f.write(f"Format: NCLR (RGB555)\n")
                f.write(f"Color Count: {len(colors)}\n")
                f.write(f"Color Mode: {'4bpp (16 colors)' if len(colors) <= 16 else '8bpp (256 colors)'}\n")
                f.write(f"\nColors (RGB555):\n")
                for i, color in enumerate(colors):
                    r = (color & 0x1F) << 3
                    g = ((color >> 5) & 0x1F) << 3
                    b = ((color >> 10) & 0x1F) << 3
                    f.write(f"  [{i:3d}] 0x{color:04X} = RGB({r:3d}, {g:3d}, {b:3d})\n")
    
    print(f"\n✓ Conversion complete!")
    print(f"Output directory: {output_dir}")

if __name__ == "__main__":
    main()
