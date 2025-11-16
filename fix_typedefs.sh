#!/bin/bash
# Fix duplicate NNSG2d/NNSG3d/GX typedefs in headers

FILES=$(find include -name "*.h" -exec grep -l "typedef struct { void\* data; } NNSG2dImageProxy;" {} \;)

for file in $FILES; do
    echo "Fixing $file..."
    # Use perl to replace the entire block of typedefs with just include nns_types.h
    perl -i -0777 -pe 's/#ifdef PLATFORM_DS\n#include <nnsys\.h>\n#else\n#include "platform\/platform_types\.h"\n\n\/\* Common NNSG2d types \*\/.*?\/\* GX color types \*\/\ntypedef u16 GXRgb;\ntypedef struct \{ u8 r, g, b; \} GXRgba;\n#endif/#ifdef PLATFORM_DS\n#include <nnsys.h>\n#else\n#include "platform\/platform_types.h"\n#include "nns_types.h"  \/\* SDL: All NNSG2d\/NNSG3d\/GX types \*\/\n#endif/s' "$file"
done

echo "Done! Fixed $(echo "$FILES" | wc -l) files."
