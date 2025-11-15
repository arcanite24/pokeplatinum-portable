#!/bin/bash
# Script to automatically add conditional compilation around nitro.h includes
# in all .c files in src/ (excluding overlays and applications)

FILES=$(find src -name "*.c" -not -path "*/overlay*/*" -not -path "*/applications/*" -not -path "*/platform/*")

for file in $FILES; do
    if grep -q "^#include <nitro.h>" "$file"; then
        echo "Fixing $file..."
        
        # Use sed to replace the nitro.h include with conditional compilation
        sed -i.bak '
            /^#include <nitro.h>/ {
                i\
#ifdef PLATFORM_DS
                a\
#else\
#include "platform/platform_types.h"\
#endif
            }
        ' "$file"
        
        # Remove backup file
        rm -f "$file.bak"
    fi
done

echo "Done! Fixed all nitro.h includes."
