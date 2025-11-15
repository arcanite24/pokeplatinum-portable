#!/usr/bin/env python3
"""
NARC Extractor for Pokemon Platinum SDL3 Port

Extracts NARC archives from a Nintendo DS ROM and saves files to a directory structure
suitable for the SDL3 build.

Usage:
    python3 narc_extractor.py <rom_path> <narc_id> <output_dir>
    
Example:
    python3 narc_extractor.py platinum.us.nds 0 resources/graphics/narc_000/

Dependencies:
    pip install ndspy
"""

import sys
import os
from pathlib import Path

try:
    import ndspy.rom
    import ndspy.narc
except ImportError:
    print("Error: ndspy library not installed")
    print("Install with: pip install ndspy")
    sys.exit(1)


def extract_narc_by_id(rom_path, narc_id, output_dir):
    """
    Extract a NARC archive by its ID from the ROM
    
    Args:
        rom_path: Path to the .nds ROM file
        narc_id: NARC index to extract
        output_dir: Directory to save extracted files
    """
    print(f"Loading ROM: {rom_path}")
    
    try:
        rom = ndspy.rom.NintendoDSRom.fromFile(rom_path)
    except Exception as e:
        print(f"Error loading ROM: {e}")
        sys.exit(1)
    
    print(f"ROM loaded successfully")
    print(f"Game Code: {rom.idCode.decode('ascii')}")
    print(f"Game Title: {rom.name.decode('ascii').strip()}")
    
    # Find NARC file by ID
    print(f"\nSearching for file ID {narc_id}...")
    
    # Access file directly by ID
    if narc_id >= len(rom.files) or rom.files[narc_id] is None:
        print(f"Error: File ID {narc_id} not found in ROM")
        print(f"ROM has {len(rom.files)} files")
        sys.exit(1)
    
    narc_data = rom.files[narc_id]
    
    # Try to get filename
    try:
        narc_filename = rom.filenames.filenameOf(narc_id)
        if narc_filename:
            print(f"Found file: {narc_filename}")
        else:
            print(f"Found file ID {narc_id} (no filename)")
    except:
        narc_filename = f"file_{narc_id}"
        print(f"Found file ID {narc_id} (no filename)")
    
    # Check if it's a NARC
    if narc_data[:4] != b'NARC':
        magic = narc_data[:4].decode('ascii', errors='ignore') if len(narc_data) >= 4 else "???"
        print(f"Warning: File does not appear to be a NARC (magic: {magic})")
        print(f"Continuing anyway...")
    else:
        print(f"Confirmed NARC file")
    
    # Parse NARC
    print(f"Parsing NARC archive...")
    try:
        narc = ndspy.narc.NARC(narc_data)
    except Exception as e:
        print(f"Error parsing NARC: {e}")
        sys.exit(1)
    
    print(f"NARC contains {len(narc.files)} files")
    
    # Create output directory
    os.makedirs(output_dir, exist_ok=True)
    print(f"Output directory: {output_dir}")
    
    # Extract all files
    for i, file_data in enumerate(narc.files):
        output_path = os.path.join(output_dir, f"{i:04d}.bin")
        
        with open(output_path, 'wb') as f:
            f.write(file_data)
        
        if i % 10 == 0 or i == len(narc.files) - 1:
            print(f"Extracted {i + 1}/{len(narc.files)} files...", end='\r')
    
    print(f"\n✓ Successfully extracted {len(narc.files)} files to {output_dir}")
    
    # Create metadata file
    metadata_path = os.path.join(output_dir, "narc_info.txt")
    with open(metadata_path, 'w') as f:
        f.write(f"NARC ID: {narc_id}\n")
        f.write(f"Source ROM: {rom_path}\n")
        f.write(f"ROM Game Code: {rom.idCode.decode('ascii')}\n")
        f.write(f"NARC Path: {narc_filename}\n")
        f.write(f"File Count: {len(narc.files)}\n")
        f.write(f"\nFile Sizes:\n")
        for i, file_data in enumerate(narc.files):
            f.write(f"  {i:04d}.bin: {len(file_data)} bytes\n")
    
    print(f"✓ Created metadata file: {metadata_path}")


def list_rom_files(rom_path):
    """
    List all files in the ROM (for debugging)
    """
    print(f"Loading ROM: {rom_path}")
    rom = ndspy.rom.NintendoDSRom.fromFile(rom_path)
    
    print(f"\nFiles in ROM:")
    print(f"Total files: {len(rom.files)}\n")
    
    # Just list all files by ID
    for file_id, file_data in enumerate(rom.files):
        if file_data is None:
            continue
            
        # Try to get the file path
        try:
            path = rom.filenames.filenameOf(file_id)
            if path is None:
                path = "<no name>"
        except:
            path = "<no name>"
        
        magic = file_data[:4] if len(file_data) >= 4 else b''
        magic_str = magic.decode('ascii', errors='ignore') if magic.isalpha() else magic.hex()
        
        print(f"  {path:50s} | ID: {file_id:4d} | Size: {len(file_data):8d} | Magic: {magic_str}")


def main():
    if len(sys.argv) < 2:
        print("NARC Extractor for Pokemon Platinum SDL3 Port")
        print()
        print("Usage:")
        print("  Extract NARC:")
        print("    python3 narc_extractor.py <rom_path> <narc_id> <output_dir>")
        print()
        print("  List ROM files (for finding NARCs):")
        print("    python3 narc_extractor.py <rom_path> --list")
        print()
        print("Examples:")
        print("  python3 narc_extractor.py platinum.us.nds 0 resources/graphics/narc_000/")
        print("  python3 narc_extractor.py platinum.us.nds --list")
        sys.exit(1)
    
    rom_path = sys.argv[1]
    
    if not os.path.exists(rom_path):
        print(f"Error: ROM file not found: {rom_path}")
        sys.exit(1)
    
    if len(sys.argv) == 3 and sys.argv[2] == '--list':
        list_rom_files(rom_path)
    elif len(sys.argv) == 4:
        try:
            narc_id = int(sys.argv[2])
        except ValueError:
            print(f"Error: NARC ID must be a number, got: {sys.argv[2]}")
            sys.exit(1)
        
        output_dir = sys.argv[3]
        extract_narc_by_id(rom_path, narc_id, output_dir)
    else:
        print("Error: Invalid arguments")
        print("Use --help for usage information")
        sys.exit(1)


if __name__ == '__main__':
    main()
