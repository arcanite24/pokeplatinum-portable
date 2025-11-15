#!/bin/bash
# Script to automatically add conditional compilation around nnsys.h includes
# and add common NNSG2d/NNSG3d type stubs to headers

set -e  # Exit on error

FIXED_COUNT=0
SKIPPED_COUNT=0

echo "=========================================="
echo "Fixing NitroSystem (nnsys.h) includes in headers"
echo "=========================================="

# Find all headers that include nnsys.h but don't already have PLATFORM_DS guards
HEADERS=$(find include -name "*.h" -not -path "*/platform/*" -exec grep -l "^#include <nnsys.h>" {} \; 2>/dev/null || true)

for header in $HEADERS; do
    # Skip if already has PLATFORM_DS guard for nnsys
    if grep -q "#ifdef PLATFORM_DS" "$header" && grep -A 2 "#ifdef PLATFORM_DS" "$header" | grep -q "nnsys.h"; then
        echo "SKIP: $header (already has conditional compilation)"
        ((SKIPPED_COUNT++))
        continue
    fi
    
    echo "Fixing: $header"
    
    # Create backup
    cp "$header" "$header.bak"
    
    # Add conditional compilation around nnsys.h
    # Also add common type stubs
    sed -i '' '
        /^#include <nnsys.h>/ {
            i\
#ifdef PLATFORM_DS
            a\
#else\
#include "nns_types.h"  /* SDL: Common NNS type stubs */\
#endif
        }
    ' "$header"
    
    # Check if sed made changes
    if ! diff -q "$header" "$header.bak" > /dev/null 2>&1; then
        echo "  ✓ Fixed $header"
        ((FIXED_COUNT++))
        rm "$header.bak"
    else
        echo "  ⚠ No changes needed for $header"
        mv "$header.bak" "$header"
        ((SKIPPED_COUNT++))
    fi
done

echo ""
echo "=========================================="
echo "Fixing nitro/gx.h includes in headers"
echo "=========================================="

# Find headers that include nitro/gx.h
GX_HEADERS=$(find include -name "*.h" -not -path "*/platform/*" -exec grep -l "^#include <nitro/gx.h>" {} \; 2>/dev/null || true)

for header in $GX_HEADERS; do
    # Skip if already has conditional compilation
    if grep -q "#ifdef PLATFORM_DS" "$header" && grep -A 2 "#ifdef PLATFORM_DS" "$header" | grep -q "nitro/gx.h"; then
        echo "SKIP: $header (already has conditional compilation)"
        ((SKIPPED_COUNT++))
        continue
    fi
    
    echo "Fixing: $header"
    
    cp "$header" "$header.bak"
    
    sed -i '' '
        /^#include <nitro\/gx.h>/ {
            i\
#ifdef PLATFORM_DS
            a\
#else\
#include "nns_types.h"  /* SDL: Common GX type stubs */\
#endif
        }
    ' "$header"
    
    if ! diff -q "$header" "$header.bak" > /dev/null 2>&1; then
        echo "  ✓ Fixed $header"
        ((FIXED_COUNT++))
        rm "$header.bak"
    else
        echo "  ⚠ No changes needed for $header"
        mv "$header.bak" "$header"
        ((SKIPPED_COUNT++))
    fi
done

echo ""
echo "=========================================="
echo "Fixing nitro/rtc.h includes in headers"
echo "=========================================="

# Find headers that include nitro/rtc.h (but not rtc.h itself)
RTC_HEADERS=$(find include -name "*.h" -not -path "*/platform/*" -not -name "rtc.h" -exec grep -l "^#include <nitro/rtc.h>" {} \; 2>/dev/null || true)

for header in $RTC_HEADERS; do
    # Skip if already has conditional compilation
    if grep -q "#ifdef PLATFORM_DS" "$header" && grep -A 2 "#ifdef PLATFORM_DS" "$header" | grep -q "nitro/rtc.h"; then
        echo "SKIP: $header (already has conditional compilation)"
        ((SKIPPED_COUNT++))
        continue
    fi
    
    echo "Fixing: $header"
    
    cp "$header" "$header.bak"
    
    sed -i '' '
        /^#include <nitro\/rtc.h>/ {
            i\
#ifdef PLATFORM_DS
            a\
#else\
#include "rtc.h"  /* SDL stub types already defined in rtc.h */\
#endif
        }
    ' "$header"
    
    if ! diff -q "$header" "$header.bak" > /dev/null 2>&1; then
        echo "  ✓ Fixed $header"
        ((FIXED_COUNT++))
        rm "$header.bak"
    else
        echo "  ⚠ No changes needed for $header"
        mv "$header.bak" "$header"
        ((SKIPPED_COUNT++))
    fi
done

echo ""
echo "=========================================="
echo "Summary"
echo "=========================================="
echo "Headers fixed: $FIXED_COUNT"
echo "Headers skipped: $SKIPPED_COUNT"
echo ""

if [ $FIXED_COUNT -gt 0 ]; then
    echo "✓ Successfully fixed $FIXED_COUNT headers!"
    echo ""
    echo "Next steps:"
    echo "  1. Run: cmake --build build-sdl"
    echo "  2. Fix any remaining compilation errors"
else
    echo "No headers needed fixing (all already have conditional compilation)"
fi
