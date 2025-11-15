#!/usr/bin/env python3
"""
NCLR Converter for Pokemon Platinum SDL3 Port

Converts Nintendo DS NCLR (Nitro Color Resource) palette files to formats
suitable for the PAL graphics system.

NCLR Format:
- RGB555: 5 bits per channel (0-31 range)
- Palettes can be 16 colors (4bpp) or 256 colors (8bpp)

Output Formats:
- Binary (.pal): Raw RGB555 data (16-bit per color)
- JSON (.json): Human-readable format with RGB888 values

Usage:
    python3 nclr_converter.py <input.nclr> <output.pal> [--json] [--info]

Dependencies:
    pip install ndspy
"""

import sys
import os
import json
import struct

try:
    import ndspy.nclr
except ImportError:
    print("Error: ndspy library not installed")
    print("Install with: pip install ndspy")
    sys.exit(1)


def rgb555_to_rgb888(rgb555):
    """
    Convert DS RGB555 color to RGB888
    
    Args:
        rgb555: 16-bit RGB555 color value
    
    Returns:
        tuple: (r, g, b) with 8-bit values
    """
    r5 = (rgb555 >> 0) & 0x1F
    g5 = (rgb555 >> 5) & 0x1F
    b5 = (rgb555 >> 10) & 0x1F
    
    # Scale 5-bit to 8-bit (0-31 -> 0-255)
    r8 = (r5 * 255) // 31
    g8 = (g5 * 255) // 31
    b8 = (b5 * 255) // 31
    
    return (r8, g8, b8)


def rgb555_to_hex(rgb555):
    """Convert RGB555 to hex string"""
    r, g, b = rgb555_to_rgb888(rgb555)
    return f"#{r:02x}{g:02x}{b:02x}"


def convert_nclr_to_binary(nclr_path, output_path, show_info=False):
    """
    Convert NCLR file to raw RGB555 binary data
    
    Args:
        nclr_path: Path to input .nclr file
        output_path: Path to output .pal file
        show_info: Print detailed information
    """
    print(f"Loading NCLR: {nclr_path}")
    
    try:
        with open(nclr_path, 'rb') as f:
            nclr_data = f.read()
        
        nclr = ndspy.nclr.NCLR(nclr_data)
    except Exception as e:
        print(f"Error loading NCLR: {e}")
        sys.exit(1)
    
    # Get palette data (list of RGB555 values)
    palette = nclr.palette
    color_count = len(palette)
    
    print(f"NCLR Information:")
    print(f"  Color Count: {color_count}")
    print(f"  Format: RGB555 (5 bits per channel)")
    
    # Determine palette type
    if color_count == 16:
        palette_type = "4bpp (16 colors)"
    elif color_count == 256:
        palette_type = "8bpp (256 colors)"
    elif color_count % 16 == 0:
        palette_type = f"Multiple 4bpp palettes ({color_count // 16} palettes)"
    else:
        palette_type = f"Custom ({color_count} colors)"
    
    print(f"  Type: {palette_type}")
    
    if show_info:
        print(f"\nColor Preview (first 16 colors):")
        for i in range(min(16, color_count)):
            rgb555 = palette[i]
            hex_color = rgb555_to_hex(rgb555)
            r, g, b = rgb555_to_rgb888(rgb555)
            print(f"    Color {i:3d}: RGB555=0x{rgb555:04x} RGB888=({r:3d},{g:3d},{b:3d}) {hex_color}")
    
    # Write binary palette (raw RGB555 values, little-endian)
    print(f"\nWriting binary palette to: {output_path}")
    with open(output_path, 'wb') as f:
        for color in palette:
            f.write(struct.pack('<H', color))  # 16-bit little-endian
    
    bytes_written = color_count * 2
    print(f"✓ Wrote {bytes_written} bytes ({color_count} colors)")
    
    # Create metadata file
    metadata_path = output_path.replace('.pal', '_info.txt')
    with open(metadata_path, 'w') as f:
        f.write(f"NCLR Palette Metadata\n")
        f.write(f"====================\n\n")
        f.write(f"Source File: {nclr_path}\n")
        f.write(f"Output File: {output_path}\n")
        f.write(f"Color Count: {color_count}\n")
        f.write(f"Palette Type: {palette_type}\n")
        f.write(f"Format: RGB555 (16-bit little-endian)\n")
        f.write(f"File Size: {bytes_written} bytes\n")
        f.write(f"\nUsage with PAL:\n")
        f.write(f"  PAL_Bg_LoadPalette(bgConfig, layer, palette_data, {bytes_written}, 0);\n")
        f.write(f"\nColor List (RGB888):\n")
        for i, color in enumerate(palette):
            r, g, b = rgb555_to_rgb888(color)
            hex_color = rgb555_to_hex(color)
            f.write(f"  {i:3d}: RGB({r:3d},{g:3d},{b:3d}) {hex_color}\n")
    
    print(f"✓ Created metadata file: {metadata_path}")
    
    return palette


def convert_nclr_to_json(nclr_path, output_path):
    """
    Convert NCLR file to JSON format (human-readable)
    """
    print(f"Loading NCLR: {nclr_path}")
    
    try:
        with open(nclr_path, 'rb') as f:
            nclr_data = f.read()
        
        nclr = ndspy.nclr.NCLR(nclr_data)
    except Exception as e:
        print(f"Error loading NCLR: {e}")
        sys.exit(1)
    
    palette = nclr.palette
    
    # Create JSON structure
    palette_json = {
        "format": "RGB555",
        "color_count": len(palette),
        "colors": []
    }
    
    for i, rgb555 in enumerate(palette):
        r, g, b = rgb555_to_rgb888(rgb555)
        palette_json["colors"].append({
            "index": i,
            "rgb555": rgb555,
            "rgb555_hex": f"0x{rgb555:04x}",
            "rgb888": [r, g, b],
            "hex": rgb555_to_hex(rgb555)
        })
    
    print(f"Writing JSON palette to: {output_path}")
    with open(output_path, 'w') as f:
        json.dump(palette_json, f, indent=2)
    
    print(f"✓ Wrote JSON with {len(palette)} colors")


def batch_convert_directory(input_dir, output_dir, json_output=False):
    """
    Convert all NCLR files in a directory
    """
    input_path = os.path.abspath(input_dir)
    output_path = os.path.abspath(output_dir)
    
    os.makedirs(output_path, exist_ok=True)
    
    # Find all NCLR files
    files_to_convert = []
    
    for filename in sorted(os.listdir(input_path)):
        file_path = os.path.join(input_path, filename)
        
        if not os.path.isfile(file_path):
            continue
        
        # Check if file starts with NCLR magic
        try:
            with open(file_path, 'rb') as f:
                magic = f.read(4)
                if magic == b'RLCN':  # NCLR magic (reversed)
                    files_to_convert.append((filename, file_path))
        except:
            continue
    
    if not files_to_convert:
        print(f"No NCLR files found in {input_path}")
        return
    
    print(f"Found {len(files_to_convert)} NCLR files")
    print(f"Converting to: {output_path}\n")
    
    for i, (filename, file_path) in enumerate(files_to_convert):
        # Preserve numbering
        if filename.endswith('.bin'):
            base_name = filename[:-4]
        else:
            base_name = filename
        
        if json_output:
            output_file = os.path.join(output_path, f"{base_name}_palette.json")
            print(f"[{i+1}/{len(files_to_convert)}] Converting {filename} (JSON)...")
            try:
                convert_nclr_to_json(file_path, output_file)
                print()
            except Exception as e:
                print(f"  Error: {e}\n")
        else:
            output_file = os.path.join(output_path, f"{base_name}_palette.pal")
            print(f"[{i+1}/{len(files_to_convert)}] Converting {filename}...")
            try:
                convert_nclr_to_binary(file_path, output_file, show_info=False)
                print()
            except Exception as e:
                print(f"  Error: {e}\n")


def main():
    if len(sys.argv) < 2:
        print("NCLR Converter for Pokemon Platinum SDL3 Port")
        print()
        print("Usage:")
        print("  Convert to binary (default):")
        print("    python3 nclr_converter.py <input.nclr> <output.pal> [--info]")
        print()
        print("  Convert to JSON:")
        print("    python3 nclr_converter.py <input.nclr> <output.json> --json")
        print()
        print("  Batch convert directory:")
        print("    python3 nclr_converter.py --batch <input_dir> <output_dir> [--json]")
        print()
        print("Examples:")
        print("  python3 nclr_converter.py title_logo.nclr title_logo.pal")
        print("  python3 nclr_converter.py title_logo.nclr title_logo.pal --info")
        print("  python3 nclr_converter.py title_logo.nclr title_logo.json --json")
        print("  python3 nclr_converter.py --batch resources/narc_000/ resources/converted/")
        sys.exit(1)
    
    if sys.argv[1] == '--batch':
        if len(sys.argv) < 4:
            print("Error: --batch requires input_dir and output_dir")
            sys.exit(1)
        
        input_dir = sys.argv[2]
        output_dir = sys.argv[3]
        json_output = '--json' in sys.argv
        
        if not os.path.isdir(input_dir):
            print(f"Error: Input directory not found: {input_dir}")
            sys.exit(1)
        
        batch_convert_directory(input_dir, output_dir, json_output)
    else:
        if len(sys.argv) < 3:
            print("Error: Requires input and output file paths")
            sys.exit(1)
        
        input_file = sys.argv[1]
        output_file = sys.argv[2]
        json_output = '--json' in sys.argv
        show_info = '--info' in sys.argv
        
        if not os.path.exists(input_file):
            print(f"Error: Input file not found: {input_file}")
            sys.exit(1)
        
        if json_output:
            convert_nclr_to_json(input_file, output_file)
        else:
            convert_nclr_to_binary(input_file, output_file, show_info)


if __name__ == '__main__':
    main()
