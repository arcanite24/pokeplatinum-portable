#!/bin/bash
#
# Asset Conversion Script for Pokemon Platinum SDL3 Port
#
# This script automates the extraction and conversion of game assets from
# the Pokemon Platinum ROM to formats suitable for the SDL3 build.
#
# Usage:
#   ./convert_assets.sh <path_to_platinum.us.nds>
#
# Requirements:
#   - Python 3.8+
#   - ndspy library (pip install ndspy)
#   - Pokemon Platinum US ROM

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Directories
TOOLS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$TOOLS_DIR")"
RESOURCES_DIR="$PROJECT_ROOT/resources"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Pokemon Platinum Asset Conversion${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check arguments
if [ $# -lt 1 ]; then
    echo -e "${RED}Error: ROM path required${NC}"
    echo ""
    echo "Usage:"
    echo "  $0 <path_to_platinum.us.nds>"
    echo ""
    echo "Example:"
    echo "  $0 ~/roms/platinum.us.nds"
    exit 1
fi

ROM_PATH="$1"

# Verify ROM exists
if [ ! -f "$ROM_PATH" ]; then
    echo -e "${RED}Error: ROM file not found: $ROM_PATH${NC}"
    exit 1
fi

echo -e "${GREEN}✓${NC} ROM found: $ROM_PATH"

# Check Python
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}Error: python3 not found${NC}"
    echo "Please install Python 3.8 or newer"
    exit 1
fi

echo -e "${GREEN}✓${NC} Python 3 found: $(python3 --version)"

# Check ndspy
if ! python3 -c "import ndspy" 2>/dev/null; then
    echo -e "${RED}Error: ndspy library not installed${NC}"
    echo "Install with: pip install ndspy"
    exit 1
fi

echo -e "${GREEN}✓${NC} ndspy library installed"
echo ""

# Create resources directory
mkdir -p "$RESOURCES_DIR"

# Function to extract and convert a NARC
extract_and_convert_narc() {
    local narc_id=$1
    local narc_name=$2
    
    echo -e "${YELLOW}Processing NARC $narc_id: $narc_name${NC}"
    
    # Create directories
    local raw_dir="$RESOURCES_DIR/graphics/narc_$(printf "%03d" $narc_id)_raw"
    local converted_dir="$RESOURCES_DIR/graphics/narc_$(printf "%03d" $narc_id)"
    
    mkdir -p "$raw_dir"
    mkdir -p "$converted_dir"
    
    # Extract NARC
    echo "  [1/3] Extracting NARC..."
    python3 "$TOOLS_DIR/narc_extractor.py" "$ROM_PATH" "$narc_id" "$raw_dir" || {
        echo -e "  ${RED}✗${NC} Failed to extract NARC $narc_id"
        return 1
    }
    
    # Convert graphics (NCGR)
    echo "  [2/3] Converting graphics..."
    python3 "$TOOLS_DIR/ncgr_converter.py" --batch "$raw_dir" "$converted_dir" || {
        echo -e "  ${YELLOW}!${NC} No graphics files found (may be normal)"
    }
    
    # Convert palettes (NCLR)
    echo "  [3/3] Converting palettes..."
    python3 "$TOOLS_DIR/nclr_converter.py" --batch "$raw_dir" "$converted_dir" || {
        echo -e "  ${YELLOW}!${NC} No palette files found (may be normal)"
    }
    
    echo -e "  ${GREEN}✓${NC} Completed $narc_name"
    echo ""
}

# =============================================================================
# NARC Conversion List
# =============================================================================
# These are the NARCs needed for initial development
# Add more as needed

echo -e "${BLUE}Starting asset extraction...${NC}"
echo ""

# Title Screen Assets (example - adjust NARC IDs based on actual ROM structure)
# Note: These NARC IDs are placeholders and need to be determined by examining
# the ROM structure with tools like Tinke or ndspy's file browser

# Uncomment and adjust NARC IDs once you've identified them:

# extract_and_convert_narc 0 "Title Screen Graphics"
# extract_and_convert_narc 1 "Main Menu Graphics"
# extract_and_convert_narc 2 "Field Graphics"

echo -e "${YELLOW}========================================${NC}"
echo -e "${YELLOW}IMPORTANT: NARC ID Identification${NC}"
echo -e "${YELLOW}========================================${NC}"
echo ""
echo "Before extracting assets, you need to identify the correct NARC IDs."
echo ""
echo "To list all files in the ROM:"
echo "  python3 $TOOLS_DIR/narc_extractor.py \"$ROM_PATH\" --list"
echo ""
echo "Look for NARC files (files with 'NARC' magic or .narc extension)"
echo "Common locations in Pokemon Platinum:"
echo "  - data/a/0/0/* for various graphics archives"
echo "  - data/a/0/1/* for more graphics"
echo "  - data/a/0/2/* for sound data"
echo ""
echo "Once you've identified NARC IDs, edit this script and uncomment"
echo "the extract_and_convert_narc calls with the correct IDs."
echo ""
echo -e "${GREEN}For now, running ROM file list...${NC}"
echo ""

# List ROM files to help identify NARCs
python3 "$TOOLS_DIR/narc_extractor.py" "$ROM_PATH" --list | head -50

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Next Steps${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "1. Review the ROM file list above"
echo "2. Identify NARC files you want to extract"
echo "3. Edit this script to add extract_and_convert_narc calls"
echo "4. Run this script again"
echo ""
echo "Resources will be saved to: $RESOURCES_DIR"
echo ""
echo -e "${GREEN}✓${NC} Setup complete!"
