#!/bin/bash
# Script to replace inline type definitions with nns_types.h include
# in headers that were previously fixed

set -e

echo "Updating headers to use nns_types.h..."

# Find headers with inline type definitions in the #else block
find include -name "*.h" -type f -exec grep -l "typedef struct { void\* data; } NNSG2dImageProxy;" {} \; 2>/dev/null | while read file; do
    echo "Updating: $file"
    
    # Replace the large block of type definitions with just the include
    perl -i -0pe 's/#else\n#include "platform\/platform_types.h"\n\n\/\* Common NNSG2d types \*\/.*?\/\* GX color types \*\/.*?typedef struct \{ u8 r, g, b; \} GXRgba;\n#endif/#else\n#include "nns_types.h"  \/\* SDL: Common NNS type stubs \*\/\n#endif/gs' "$file"
    
    # Also handle the shorter version (just platform_types.h)
    perl -i -0pe 's/#else\n#include "platform\/platform_types.h"\n\/\* GX types from enums.h already define blend\/window masks \*\/.*?typedef struct \{ void\* data; \} GXOamAttr;\n#endif/#else\n#include "nns_types.h"  \/\* SDL: Common GX type stubs \*\/\n#endif/gs' "$file"
done

echo "Done! All headers now use nns_types.h"
