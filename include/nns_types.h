#ifndef NNS_TYPES_H
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

/* NNSG2d Graphics Data Types */
#ifdef PLATFORM_DS
// Use original DS types
#else
// PAL stub types for graphics data
typedef struct { void* data; u32 size; } NNSG2dCharacterData;
typedef struct { void* data; u32 size; } NNSG2dScreenData;
typedef struct { void* data; u32 size; } NNSG2dPaletteData;
typedef struct { void* data; u32 size; } NNSG2dCellDataBank;
typedef struct { void* data; u32 size; } NNSG2dAnimBankData;
typedef struct { void* data; u32 size; } NNSG2dCellAnimBankData;
typedef struct { void* data; u32 size; } NNSG2dMultiCellDataBank;
typedef struct { void* data; u32 size; } NNSG2dMultiCellAnimBankData;

/* Image proxy and animation types */
typedef struct { void* data; } NNSG2dImageProxy;
typedef struct { void* data; } NNSG2dImagePaletteProxy;
typedef struct { void* data; } NNSG2dImageAttr;
typedef struct { void* data; } NNSG2dAnimController;
typedef struct { void* data; } NNSG2dCellAnimation;
typedef struct { void* data; } NNSG2dMultiCellAnimation;
typedef struct { void* data; } NNSG2dNode;
typedef struct { void* data; } NNSG2dCellTransferState;

/* Font types */
typedef struct { void* data; } NNSG2dRenderSurface;
typedef struct { void* data; } NNSG2dViewRect;
typedef struct { void* data; } NNSG2dRendererInstance;
typedef struct { void* data; } NNSG2dOamManagerInstance;

/* Font types */
typedef struct { void* data; } NNSG2dFont;
typedef struct { void* data; } NNSG2dGlyph;
typedef struct { void* data; } NNSG2dTextCanvas;
#endif

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
typedef struct { void* data; } NNSG3dResTex;
typedef struct { void* data; } NNSG3dResMdlSet;
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
typedef enum {
    GX_BUFFERMODE_Z = 0,
    GX_BUFFERMODE_W = 1
} GXBufferMode;

typedef enum {
    GX_SORTMODE_AUTO = 0,
    GX_SORTMODE_MANUAL = 1
} GXSortMode;

typedef enum {
    GX_OBJVRAMMODE_CHAR_2D = 0,
    GX_OBJVRAMMODE_CHAR_1D_32K = 1,
    GX_OBJVRAMMODE_CHAR_1D_64K = 2,
    GX_OBJVRAMMODE_CHAR_1D_128K = 3,
    GX_OBJVRAMMODE_CHAR_1D_256K = 4
} GXOBJVRamModeChar;

typedef enum {
    GX_OAM_MODE_NORMAL = 0,
    GX_OAM_MODE_XLU = 1,
    GX_OAM_MODE_OBJWIN = 2,
    GX_OAM_MODE_BITMAPOBJ = 3
} GXOamMode;

typedef enum {
    GX_POLYGONMODE_MODULATE = 0,
    GX_POLYGONMODE_DECAL = 1,
    GX_POLYGONMODE_TOON = 2,
    GX_POLYGONMODE_SHADOW = 3
} GXPolygonMode;

typedef enum {
    GX_CULL_ALL = 0,
    GX_CULL_FRONT = 1,
    GX_CULL_BACK = 2,
    GX_CULL_NONE = 3
} GXCull;

typedef enum {
    GX_VRAM_BG_NONE = 0,
    GX_VRAM_BG_128_A = 1
} GXVRamBG;

typedef enum {
    GX_VRAM_BGEXTPLTT_NONE = 0
} GXVRamBGExtPltt;

typedef enum {
    GX_VRAM_SUB_BG_NONE = 0
} GXVRamSubBG;

typedef enum {
    GX_VRAM_SUB_BGEXTPLTT_NONE = 0
} GXVRamSubBGExtPltt;

typedef enum {
    GX_VRAM_OBJ_NONE = 0,
    GX_VRAM_OBJ_32_FG = 1,
    GX_VRAM_OBJ_64_E = 2,
    GX_VRAM_OBJ_80_EF = 3,
    GX_VRAM_OBJ_16_F = 4,
    GX_VRAM_OBJ_16_G = 5
} GXVRamOBJ;

typedef enum {
    GX_VRAM_OBJEXTPLTT_NONE = 0
} GXVRamOBJExtPltt;

typedef enum {
    GX_VRAM_SUB_OBJ_NONE = 0
} GXVRamSubOBJ;

typedef enum {
    GX_VRAM_SUB_OBJEXTPLTT_NONE = 0
} GXVRamSubOBJExtPltt;

typedef enum {
    GX_VRAM_TEX_NONE = 0,
    GX_VRAM_TEX_0_A = 1,
    GX_VRAM_TEX_0_B = 2,
    GX_VRAM_TEX_0_C = 3,
    GX_VRAM_TEX_0_D = 4
} GXVRamTex;

typedef enum {
    GX_VRAM_TEXPLTT_NONE = 0,
    GX_VRAM_TEXPLTT_0_F = 1,
    GX_VRAM_TEXPLTT_0_G = 2,
    GX_VRAM_TEXPLTT_0123_E = 3
} GXVRamTexPltt;

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

#define GX_BLEND_BGALL (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3)
#define GX_BLEND_ALL (GX_BLEND_BGALL | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD)

#define GX_WND_PLANEMASK_NONE       0
#define GX_WND_PLANEMASK_BG0        (1 << 0)
#define GX_WND_PLANEMASK_BG1        (1 << 1)
#define GX_WND_PLANEMASK_BG2        (1 << 2)
#define GX_WND_PLANEMASK_BG3        (1 << 3)
#define GX_WND_PLANEMASK_OBJ        (1 << 4)

#define GX_WND_PLANEMASK_BGALL (GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3)
#define GX_WND_PLANEMASK_ALL (GX_WND_PLANEMASK_BGALL | GX_WND_PLANEMASK_OBJ)

#define GX_BG_COLORMODE_16 0
#define GX_BG_COLORMODE_256 1

typedef struct {
    u16 priority : 2;
    u16 charBase : 4;
    u16 mosaic : 1;
    u16 colorMode : 1;
    u16 screenBase : 5;
    u16 extPltt : 1;
    u16 screenSize : 2;
} GXBg01Control;

typedef GXBg01Control GXBg23Control;

typedef struct {
    u16 priority : 2;
    u16 charBase : 4;
    u16 mosaic : 1;
    u16 colorMode : 1;
    u16 screenBase : 5;
    u16 overflow : 1;
    u16 screenSize : 2;
} GXBg23ControlText;

typedef struct {
    u16 priority : 2;
    u16 charBase : 4;
    u16 mosaic : 1;
    u16 overflow : 1;
    u16 screenBase : 5;
    u16 areaOver : 1;
    u16 screenSize : 2;
} GXBg23ControlAffine;

typedef GXBg23ControlAffine GXBg23Control256x16Pltt;
#endif

#endif // NNS_TYPES_H
