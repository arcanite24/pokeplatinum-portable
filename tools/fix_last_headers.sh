#!/bin/bash

# Fix remaining headers with multiple nitro includes
set -e

files=(
    "include/game_overlay.h"
    "include/persisted_map_features_init.h"
    "include/persisted_map_features.h"
    "include/terrain_attributes.h"
    "include/unk_02030EE0.h"
    "include/unk_02033200.h"
    "include/unk_0205B33C.h"
)

for file in "${files[@]}"; do
    if [ ! -f "$file" ]; then
        continue
    fi
    
    # Check if already fixed
    if grep -q "^#ifdef PLATFORM_DS" "$file" 2>/dev/null; then
        echo "Already fixed: $file"
        continue
    fi
    
    echo "Fixing: $file"
    
    # Create temp file with proper wrapping
    (
        # Read file and wrap nitro includes
        in_nitro_block=false
        while IFS= read -r line; do
            if [[ "$line" =~ ^\#include[[:space:]]\<nitro/ ]]; then
                if [ "$in_nitro_block" = false ]; then
                    echo "#ifdef PLATFORM_DS"
                    in_nitro_block=true
                fi
                echo "$line"
            else
                if [ "$in_nitro_block" = true ]; then
                    echo "#else"
                    echo '#include "platform/platform_types.h"'
                    echo "#endif"
                    in_nitro_block=false
                fi
                echo "$line"
            fi
        done < "$file"
        
        # Close block if still open
        if [ "$in_nitro_block" = true ]; then
            echo "#else"
            echo '#include "platform/platform_types.h"'
            echo "#endif"
        fi
    ) > "${file}.tmp"
    
    mv "${file}.tmp" "$file"
done

echo "Done!"
