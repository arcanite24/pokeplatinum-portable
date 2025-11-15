#!/bin/bash

# Script to fix remaining nitro/ includes in headers

set -e

INCLUDE_DIR="include"
COUNT=0

echo "Fixing remaining nitro/ includes in headers..."

# Find all headers with bare nitro includes
for header in $(find "$INCLUDE_DIR" -name "*.h" -type f); do
    # Skip if already has PLATFORM_DS check near the include
    if grep -B1 "^#include.*nitro/" "$header" 2>/dev/null | grep -q "PLATFORM_DS"; then
        continue
    fi
    
    # Check and fix nitro/fx/fx.h
    if grep -q "^#include <nitro/fx/fx.h>" "$header" 2>/dev/null; then
        echo "Fixing: $header (nitro/fx/fx.h)"
        perl -i -pe 's|^#include <nitro/fx/fx.h>|#ifdef PLATFORM_DS\n#include <nitro/fx/fx.h>\n#else\n#include "platform/platform_types.h"  /* SDL: fx types */\n#endif|g' "$header"
        COUNT=$((COUNT + 1))
    fi
    
    # Check and fix nitro/gx.h
    if grep -q "^#include <nitro/gx.h>" "$header" 2>/dev/null; then
        echo "Fixing: $header (nitro/gx.h)"
        perl -i -pe 's|^#include <nitro/gx.h>|#ifdef PLATFORM_DS\n#include <nitro/gx.h>\n#else\n#include "nns_types.h"  /* SDL: GX types */\n#endif|g' "$header"
        COUNT=$((COUNT + 1))
    fi
    
    # Check and fix nitro/types.h
    if grep -q '^#include "nitro/types.h"' "$header" 2>/dev/null; then
        echo "Fixing: $header (nitro/types.h)"
        perl -i -pe 's|^#include "nitro/types.h"|#ifdef PLATFORM_DS\n#include "nitro/types.h"\n#else\n#include "platform/platform_types.h"  /* SDL: base types */\n#endif|g' "$header"
        COUNT=$((COUNT + 1))
    fi
    
    # Check and fix nitro/rtc.h
    if grep -q "^#include <nitro/rtc.h>" "$header" 2>/dev/null; then
        echo "Fixing: $header (nitro/rtc.h)"
        perl -i -pe 's|^#include <nitro/rtc.h>|#ifdef PLATFORM_DS\n#include <nitro/rtc.h>\n#else\n#include "platform/platform_types.h"  /* SDL: RTC types stubbed */\n#endif|g' "$header"
        COUNT=$((COUNT + 1))
    fi
done

echo "Fixed $COUNT includes in headers"
echo "Done!"
