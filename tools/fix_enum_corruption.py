#!/usr/bin/env python3

"""
Fix enum corruption caused by the automated DS call wrapping.

The auto_fix_ds_calls.py script wrapped individual enum members with #ifdef,
which breaks enum syntax. This script fixes that by:
1. Detecting corrupted enum blocks
2. Restoring proper enum syntax
3. Wrapping entire enum blocks instead of individual members
"""

import re
import os
import sys

def fix_corrupted_enum(content):
    """Fix enum blocks that got corrupted by individual member wrapping."""
    
    # Pattern to match corrupted enum structures
    # Look for: typedef enum { #ifdef PLATFORM_DS MEMBER #else ... patterns
    enum_pattern = r'(typedef\s+enum\s*\{[^}]*?)(\#ifdef\s+PLATFORM_DS[^}]+?)(\}[^;]*;)'
    
    def fix_enum_block(match):
        enum_start = match.group(1)  # "typedef enum {"
        corrupted_body = match.group(2)  # The corrupted part with #ifdef
        enum_end = match.group(3)  # "} TypeName;"
        
        # Extract enum members from corrupted body
        members = []
        lines = corrupted_body.split('\n')
        
        current_member = None
        for line in lines:
            line = line.strip()
            if line.startswith('#ifdef PLATFORM_DS'):
                continue
            elif line.startswith('#else'):
                continue
            elif line.startswith('// TODO:'):
                continue
            elif line.startswith('#endif'):
                continue
            elif line and not line.startswith('#'):
                # This looks like an enum member
                if '=' in line:
                    members.append(line.rstrip(','))
                elif line.endswith(',') or line.isalpha() or '_' in line:
                    members.append(line.rstrip(','))
        
        # Reconstruct the enum properly
        if members:
            member_list = ',\n    '.join(members)
            fixed_enum = f"""#ifdef PLATFORM_DS
{enum_start}
    {member_list}
{enum_end}
#else
// TODO: Port enum to PAL
{enum_start}
    // Stub enum members
{enum_end}
#endif"""
            return fixed_enum
        else:
            return match.group(0)  # Return original if we can't fix it
    
    # Apply the fix
    fixed_content = re.sub(enum_pattern, fix_enum_block, content, flags=re.DOTALL | re.MULTILINE)
    return fixed_content

def fix_simple_enum_corruption(content):
    """Fix simple cases where enum members got individually wrapped."""
    
    # Pattern: enum_member followed by #ifdef wrapping
    patterns_to_fix = [
        # Fix: MEMBER = value #ifdef PLATFORM_DS -> normal enum member
        (r'(\w+)\s*=\s*(\w+)\s*#ifdef\s+PLATFORM_DS\s*#else\s*//[^\n]*\n\s*#endif', r'\1 = \2'),
        
        # Fix: MEMBER #ifdef PLATFORM_DS -> normal enum member  
        (r'(\w+)\s*#ifdef\s+PLATFORM_DS\s*#else\s*//[^\n]*\n\s*#endif', r'\1'),
        
        # Remove duplicate #ifdef blocks
        (r'#ifdef\s+PLATFORM_DS\s*#ifdef\s+PLATFORM_DS', '#ifdef PLATFORM_DS'),
        
        # Clean up empty #else blocks
        (r'#else\s*//[^\n]*\n\s*#endif\s*#ifdef\s+PLATFORM_DS', ''),
    ]
    
    for pattern, replacement in patterns_to_fix:
        content = re.sub(pattern, replacement, content, flags=re.MULTILINE)
    
    return content

def restore_nns_types_header():
    """Restore the corrupted nns_types.h header to a working state."""
    
    # For now, let's create a minimal working version
    content = """#ifndef NNS_TYPES_H
#define NNS_TYPES_H

#include "platform/platform_config.h"
#include "platform/platform_types.h"

/* ========================================================================
 * NitroSystem Graphics 2D Types (NNS_G2d)
 * ======================================================================== */

#ifdef PLATFORM_DS
// Original DS NNS types
#include <nnsys.h>

// Re-export types for compatibility
typedef NNSG2dSurfaceType PAL_NNSG2dSurfaceType;
typedef NNS_G2D_VRAM_TYPE PAL_NNS_G2D_VRAM_TYPE;

#else
// SDL PAL versions

/* Surface types */
typedef enum {
    NNS_G2D_SURFACETYPE_MAIN2D = 0,
    NNS_G2D_SURFACETYPE_SUB2D = 1,
    NNS_G2D_SURFACETYPE_MAIN3D = 2
} NNSG2dSurfaceType;

/* VRAM types */
typedef enum {
    NNS_G2D_VRAM_TYPE_2DMAIN = 0,
    NNS_G2D_VRAM_TYPE_2DSUB = 1,
    NNS_G2D_VRAM_TYPE_MAX = 2
} NNS_G2D_VRAM_TYPE;

#endif

/* NNSG2d Vector Types */
typedef struct { s16 x, y; } NNSG2dSVec2;
typedef struct { s32 x, y; } NNSG2dLVec2;
typedef struct { fx32 x, y; } NNSG2dFVec2;

/* ========================================================================
 * NitroSystem Graphics 3D Types (NNS_G3d) 
 * ======================================================================== */

#ifdef PLATFORM_DS
// Use original DS types
#else
// PAL stub types
typedef struct { void* data; } NNSG3dResFileHeader;
typedef struct { void* data; } NNSG3dAnmObj;
typedef struct { void* data; } NNSG3dRenderObj;
typedef struct { void* data; } NNSG3dResMdl;
typedef struct { void* data; } NNSG3dResNode;
typedef struct { void* data; } NNSG3dResMat;
typedef struct { void* data; } NNSG3dResShp;
#endif

/* ========================================================================
 * NitroSystem Foundation Types (NNSFnd)
 * ======================================================================== */

#ifdef PLATFORM_DS
// Use original DS types
#else
// PAL stub types
typedef struct { void* data; } NNSFndAllocator;
#endif

/* ========================================================================
 * NitroSystem Graphics Foundation Types (NNSGfd)
 * ======================================================================== */

#ifdef PLATFORM_DS
// Use original DS types
#else
// PAL versions
typedef u32 NNSGfdTexKey;
typedef u32 NNSGfdPlttKey;
typedef struct { void* data; } NNSGfdFrmTexVramState;
typedef struct { void* data; } NNSGfdFrmPlttVramState;

typedef enum {
    NNS_GFD_DST_2D_OBJ_CHAR_MAIN = 0,
    NNS_GFD_DST_2D_OBJ_CHAR_SUB = 1,
    NNS_GFD_DST_2D_OBJ_PLTT_MAIN = 2,
    NNS_GFD_DST_2D_OBJ_PLTT_SUB = 3,
    NNS_GFD_DST_3D_TEX_VRAM = 4,
    NNS_GFD_DST_3D_TEX_PLTT = 5
} NNS_GFD_DST_TYPE;

/* VRAM allocation error values */
#define NNS_GFD_ALLOC_ERROR_TEXKEY   0xFFFFFFFF
#define NNS_GFD_ALLOC_ERROR_PLTTKEY  0xFFFFFFFF
#endif

/* ========================================================================
 * GX Graphics Types
 * ======================================================================== */

#ifdef PLATFORM_DS
// Use original DS types
#else
// PAL versions
typedef u16 GXRgb;
typedef struct { u8 r, g, b, a; } GXRgba;

/* RGB color conversion (5 bits per channel) */
#define GX_RGB(r, g, b) ((u16)(((r) & 0x1F) | (((g) & 0x1F) << 5) | (((b) & 0x1F) << 10)))

/* RGBA color conversion (5 bits per channel + 1 bit alpha) */
#define GX_RGBA(r, g, b, a) ((u16)(((r) & 0x1F) | (((g) & 0x1F) << 5) | (((b) & 0x1F) << 10) | (((a) & 0x1) << 15)))
#endif

/* ========================================================================
 * Common Graphics Constants
 * ======================================================================== */

#ifdef PLATFORM_DS
// Use original DS constants
#else
// PAL versions
#define GX_LIGHTMASK_NONE   0
#define GX_LIGHTMASK_0      (1 << 0)
#define GX_LIGHTMASK_1      (1 << 1)
#define GX_LIGHTMASK_2      (1 << 2)
#define GX_LIGHTMASK_3      (1 << 3)

#define GX_BLEND_PLANEMASK_NONE     0
#define GX_BLEND_PLANEMASK_BG0      (1 << 0)
#define GX_BLEND_PLANEMASK_BG1      (1 << 1)
#define GX_BLEND_PLANEMASK_BG2      (1 << 2)
#define GX_BLEND_PLANEMASK_BG3      (1 << 3)
#define GX_BLEND_PLANEMASK_OBJ      (1 << 4)
#define GX_BLEND_PLANEMASK_BD       (1 << 5)
#endif

#endif // NNS_TYPES_H
"""
    
    return content

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 fix_enum_corruption.py <file_path>")
        sys.exit(1)
    
    file_path = sys.argv[1]
    
    if not os.path.exists(file_path):
        print(f"Error: File {file_path} does not exist")
        sys.exit(1)
    
    # Special handling for nns_types.h
    if file_path.endswith('nns_types.h'):
        print(f"Restoring corrupted {file_path} with clean version...")
        content = restore_nns_types_header()
        
        with open(file_path, 'w') as f:
            f.write(content)
        
        print(f"✅ Restored {file_path}")
        return
    
    # General enum corruption fix
    print(f"Fixing enum corruption in {file_path}...")
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    original_content = content
    
    # Apply fixes
    content = fix_simple_enum_corruption(content)
    content = fix_corrupted_enum(content)
    
    if content != original_content:
        with open(file_path, 'w') as f:
            f.write(content)
        print(f"✅ Fixed enum corruption in {file_path}")
    else:
        print(f"ℹ️  No enum corruption found in {file_path}")

if __name__ == "__main__":
    main()