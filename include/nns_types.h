#ifndef POKEPLATINUM_NNS_TYPES_H
#define POKEPLATINUM_NNS_TYPES_H

/**
 * @file nns_types.h
 * @brief Common NitroSystem type definitions for SDL builds
 * 
 * This header provides stub type definitions for NitroSystem (NNS) types
 * used throughout the codebase. These are only used for SDL builds;
 * DS builds use the actual NitroSystem library types.
 */

#ifndef PLATFORM_DS

#include "platform/platform_types.h"

/* Alignment macro (DS uses ALIGN_4 for struct alignment) */
#define ALIGN_4

/* ========================================================================
 * NNSG2d Image/Sprite Types
 * ======================================================================== */

typedef struct { void* data; } NNSG2dImageProxy;
typedef struct { void* data; } NNSG2dImagePaletteProxy;
typedef struct { void* data; } NNSG2dCharacterData;
typedef struct { void* data; } NNSG2dPaletteData;
typedef struct { void* data; } NNSG2dImageAttr;

/* NNSG2d Cell/Animation Types */
typedef struct { void* data; } NNSG2dCellDataBank;
typedef struct { void* data; } NNSG2dCellAnimBankData;
typedef struct { void* data; } NNSG2dCellAnimation;
typedef struct { void* data; } NNSG2dMultiCellDataBank;
typedef struct { void* data; } NNSG2dMultiCellAnimBankData;
typedef struct { void* data; } NNSG2dMultiCellAnimation;
typedef struct { void* data; } NNSG2dCellTransferState;
typedef struct { void* data; } NNSG2dNode;

/* NNSG2d Rendering Types */
typedef struct { void* data; } NNSG2dRenderSurface;
typedef struct { void* data; } NNSG2dViewRect;
typedef struct { void* data; } NNSG2dRendererInstance;
typedef struct { void* data; } NNSG2dOamManagerInstance;

typedef enum {
    NNS_G2D_SURFACETYPE_MAIN2D = 0,
    NNS_G2D_SURFACETYPE_SUB2D = 1,
    NNS_G2D_SURFACETYPE_MAIN3D = 2
} NNSG2dSurfaceType;

/* NNSG2d Vector Types */
typedef struct { s16 x, y; } NNSG2dSVec2;
typedef struct { s32 x, y; } NNSG2dLVec2;
typedef struct { fx32 x, y; } NNSG2dFVec2;

/* NNSG2d VRAM Types */
enum NNS_G2D_VRAM_TYPE {
    NNS_G2D_VRAM_TYPE_2DMAIN = 0,
    NNS_G2D_VRAM_TYPE_2DSUB = 1,
    NNS_G2D_VRAM_TYPE_MAX = 2
};
typedef enum NNS_G2D_VRAM_TYPE NNS_G2D_VRAM_TYPE;

/* ========================================================================
 * NNSG3d Model/Resource Types
 * ======================================================================== */

#define NNS_G3D_RESNAME_VALSIZE 16

// NNSG3d types (3D graphics)
typedef struct {
    union {
        u32 val;
        char name[4];
    };
} NNSG3dResName;

typedef struct { void* data; } NNSG3dResMdl;
typedef struct { void* data; } NNSG3dResTex;
typedef struct { void* data; } NNSG3dRenderObj;
typedef struct { void* data; } NNSG3dResFileHeader;

// NNSGfd types (VRAM allocation)
typedef u32 NNSGfdTexKey;
typedef u32 NNSGfdPlttKey;
typedef struct { void* data; } NNSGfdFrmTexVramState;
typedef struct { void* data; } NNSGfdFrmPlttVramState;

/* ========================================================================
 * GX Graphics Types
 * ======================================================================== */

/* Color types */
typedef u16 GXRgb;
typedef struct { u8 r, g, b, a; } GXRgba;

/* OAM types */
typedef struct { void* data; } GXOamAttr;

typedef enum {
    GX_OAM_MODE_NORMAL = 0,
    GX_OAM_MODE_AFFINE = 1,
    GX_OAM_MODE_DISABLE = 2,
    GX_OAM_MODE_AFFINE_DOUBLE = 3
} GXOamMode;

typedef enum {
    GX_BUFFER_MODE_Z = 0,
    GX_BUFFER_MODE_W = 1
} GXBufferMode;

/* Polygon attribute types */
typedef u32 GXPolygonAttrMisc;

/* Math/Random types */
typedef struct {
    u32 seed;
    u32 state;
} MATHRandContext32;

/* VRAM mode types */
typedef enum {
    GX_OBJVRAMMODE_CHAR_2D = 0,
    GX_OBJVRAMMODE_CHAR_1D_32K = 1,
    GX_OBJVRAMMODE_CHAR_1D_64K = 2,
    GX_OBJVRAMMODE_CHAR_1D_128K = 3,
    GX_OBJVRAMMODE_CHAR_1D_256K = 4
} GXOBJVRamModeChar;

#endif /* !PLATFORM_DS */

#endif /* POKEPLATINUM_NNS_TYPES_H */
