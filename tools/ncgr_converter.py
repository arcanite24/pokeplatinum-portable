#!/usr/bin/env python3
"""
NCGR Converter for Pokemon Platinum SDL3 Port

Converts Nintendo DS NCGR (Nitro Character Graphics Resource) files to raw tile data
suitable for the PAL background and sprite systems.

NCGR Format:
- 4bpp: 4 bits per pixel (16 colors), 32 bytes per 8x8 tile
- 8bpp: 8 bits per pixel (256 colors), 64 bytes per 8x8 tile

Usage:
    python3 ncgr_converter.py <input.ncgr> <output.bin> [--info]

Dependencies:
    pip install ndspy
"""

import sys
import os
import struct

try:
    import ndspy.ncgr
except ImportError:
    print("Error: ndspy library not installed")
    print("Install with: pip install ndspy")
    sys.exit(1)


def convert_ncgr_to_raw(ncgr_path, output_path, show_info=False):
    """
    Convert NCGR file to raw tile data
    
    Args:
        ncgr_path: Path to input .ncgr file
        output_path: Path to output .bin file
        show_info: Print detailed information about the NCGR
    """
    print(f"Loading NCGR: {ncgr_path}")
    
    try:
        with open(ncgr_path, 'rb') as f:
            ncgr_data = f.read()
        
        ncgr = ndspy.ncgr.NCGR(ncgr_data)
    except Exception as e:
        print(f"Error loading NCGR: {e}")
        sys.exit(1)
    
    # Get tile data
    tile_data = ncgr.tileData
    
    # Determine color depth
    bits_per_pixel = 4 if ncgr.depth == 4 else 8
    bytes_per_tile = 32 if bits_per_pixel == 4 else 64
    tile_count = len(tile_data) // bytes_per_tile
    
    print(f"NCGR Information:")
    print(f"  Color Depth: {bits_per_pixel}bpp")
    print(f"  Tile Data Size: {len(tile_data)} bytes")
    print(f"  Tile Count: {tile_count} tiles")
    print(f"  Bytes per Tile: {bytes_per_tile}")
    
    if show_info:
        # Additional info
        print(f"\nDetailed Information:")
        print(f"  Width (tiles): {ncgr.tileWidth if hasattr(ncgr, 'tileWidth') else 'N/A'}")
        print(f"  Height (tiles): {ncgr.tileHeight if hasattr(ncgr, 'tileHeight') else 'N/A'}")
        print(f"  Tiled: {ncgr.tiled}")
        
        # Show first few bytes
        print(f"\n  First 32 bytes (hex):")
        for i in range(0, min(32, len(tile_data)), 16):
            hex_str = ' '.join(f'{b:02x}' for b in tile_data[i:i+16])
            print(f"    {i:04x}: {hex_str}")
    
    # Write raw tile data
    print(f"\nWriting raw tile data to: {output_path}")
    with open(output_path, 'wb') as f:
        f.write(tile_data)
    
    print(f"✓ Converted {len(tile_data)} bytes ({tile_count} tiles) successfully")
    
    # Create metadata file
    metadata_path = output_path.replace('.bin', '_info.txt')
    with open(metadata_path, 'w') as f:
        f.write(f"NCGR Conversion Metadata\n")
        f.write(f"========================\n\n")
        f.write(f"Source File: {ncgr_path}\n")
        f.write(f"Output File: {output_path}\n")
        f.write(f"Color Depth: {bits_per_pixel}bpp\n")
        f.write(f"Tile Count: {tile_count}\n")
        f.write(f"Bytes per Tile: {bytes_per_tile}\n")
        f.write(f"Total Size: {len(tile_data)} bytes\n")
        f.write(f"Tiled Layout: {ncgr.tiled}\n")
        f.write(f"\nFormat Notes:\n")
        if bits_per_pixel == 4:
            f.write("  - 4bpp format: 2 pixels per byte (nibbles)\n")
            f.write("  - Requires palette (16 colors)\n")
            f.write("  - Each 8x8 tile = 32 bytes\n")
        else:
            f.write("  - 8bpp format: 1 pixel per byte\n")
            f.write("  - Requires palette (256 colors)\n")
            f.write("  - Each 8x8 tile = 64 bytes\n")
    
    print(f"✓ Created metadata file: {metadata_path}")


def batch_convert_directory(input_dir, output_dir):
    """
    Convert all NCGR files in a directory
    """
    input_path = os.path.abspath(input_dir)
    output_path = os.path.abspath(output_dir)
    
    os.makedirs(output_path, exist_ok=True)
    
    # Find all .ncgr and .bin files (extracted NARC members might not have extensions)
    files_to_convert = []
    
    for filename in sorted(os.listdir(input_path)):
        file_path = os.path.join(input_path, filename)
        
        if not os.path.isfile(file_path):
            continue
        
        # Check if file starts with NCGR magic
        try:
            with open(file_path, 'rb') as f:
                magic = f.read(4)
                if magic == b'RGCN':  # NCGR magic (reversed)
                    files_to_convert.append((filename, file_path))
        except:
            continue
    
    if not files_to_convert:
        print(f"No NCGR files found in {input_path}")
        return
    
    print(f"Found {len(files_to_convert)} NCGR files")
    print(f"Converting to: {output_path}\n")
    
    for i, (filename, file_path) in enumerate(files_to_convert):
        # Preserve numbering if it exists (e.g., 0000.bin -> 0000_tiles.bin)
        if filename.endswith('.bin'):
            base_name = filename[:-4]
        else:
            base_name = filename
        
        output_file = os.path.join(output_path, f"{base_name}_tiles.bin")
        
        print(f"[{i+1}/{len(files_to_convert)}] Converting {filename}...")
        try:
            convert_ncgr_to_raw(file_path, output_file, show_info=False)
            print()
        except Exception as e:
            print(f"  Error: {e}\n")
            continue


def main():
    if len(sys.argv) < 2:
        print("NCGR Converter for Pokemon Platinum SDL3 Port")
        print()
        print("Usage:")
        print("  Convert single file:")
        print("    python3 ncgr_converter.py <input.ncgr> <output.bin> [--info]")
        print()
        print("  Batch convert directory:")
        print("    python3 ncgr_converter.py --batch <input_dir> <output_dir>")
        print()
        print("Examples:")
        print("  python3 ncgr_converter.py title_logo.ncgr title_logo_tiles.bin")
        print("  python3 ncgr_converter.py title_logo.ncgr title_logo_tiles.bin --info")
        print("  python3 ncgr_converter.py --batch resources/narc_000/ resources/converted/")
        sys.exit(1)
    
    if sys.argv[1] == '--batch':
        if len(sys.argv) != 4:
            print("Error: --batch requires input_dir and output_dir")
            sys.exit(1)
        
        input_dir = sys.argv[2]
        output_dir = sys.argv[3]
        
        if not os.path.isdir(input_dir):
            print(f"Error: Input directory not found: {input_dir}")
            sys.exit(1)
        
        batch_convert_directory(input_dir, output_dir)
    else:
        if len(sys.argv) < 3:
            print("Error: Requires input and output file paths")
            sys.exit(1)
        
        input_file = sys.argv[1]
        output_file = sys.argv[2]
        show_info = '--info' in sys.argv
        
        if not os.path.exists(input_file):
            print(f"Error: Input file not found: {input_file}")
            sys.exit(1)
        
        convert_ncgr_to_raw(input_file, output_file, show_info)


if __name__ == '__main__':
    main()
