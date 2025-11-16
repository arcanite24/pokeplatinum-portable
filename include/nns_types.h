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

typedef struct {
    void* data;
    struct {
        u32 baseAddrOfVram[4];  // VRAM addresses for each screen type
    } vramLocation;
} NNSG2dImageProxy;
typedef struct { void* data; } NNSG2dImagePaletteProxy;
typedef struct { 
    void* pRawData;  /* Pointer to raw palette data */
} NNSG2dCharacterData;
typedef struct {
    void* pRawData;  /* Pointer to raw palette data */
} NNSG2dPaletteData;
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

/* NNSG2d Screen/Tilemap Data */
typedef struct {
    void* rawData;      /* Pointer to raw screen/tilemap data (matches pRawData) */
    u16* screenData;    /* Alternative field name */
    u32 screenSize;
    u16 screenWidth;
    u16 screenHeight;
} NNSG2dScreenData;

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
typedef struct { void* data; } NNSG3dAnmObj;  /* Animation object */

// NNSGfd types (VRAM allocation)
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

/* ========================================================================
 * GX Graphics Types
 * ======================================================================== */

/* Color types */
typedef u16 GXRgb;
typedef struct { u8 r, g, b, a; } GXRgba;

/* RGB color conversion (5 bits per channel) */
#define GX_RGB(r, g, b) ((u16)(((r) & 0x1F) | (((g) & 0x1F) << 5) | (((b) & 0x1F) << 10)))

/* RGBA color conversion (5 bits per channel + 1 bit alpha) */
#define GX_RGBA(r, g, b, a) ((u16)(((r) & 0x1F) | (((g) & 0x1F) << 5) | (((b) & 0x1F) << 10) | (((a) & 0x1) << 15)))

/* RGB bit shift constants */
#define GX_RGB_R_SHIFT 0
#define GX_RGB_G_SHIFT 5
#define GX_RGB_B_SHIFT 10

/* OAM types */
typedef struct { void* data; } GXOamAttr;

typedef enum {
    GX_OAM_MODE_NORMAL = 0,
    GX_OAM_MODE_AFFINE = 1,
    GX_OAM_MODE_DISABLE = 2,
    GX_OAM_MODE_AFFINE_DOUBLE = 3,
    GX_OAM_MODE_OBJWND = 2,  /* Object window mode (same as DISABLE) */
    GX_OAM_MODE_XLU = 4  /* Translucent mode */
} GXOamMode;

/* VRAM allocation types (DS hardware memory mapping) */
typedef u32 GXVRamBG;
typedef u32 GXVRamBGExtPltt;
typedef u32 GXVRamSubBG;
typedef u32 GXVRamSubBGExtPltt;
typedef u32 GXVRamOBJ;
typedef u32 GXVRamOBJExtPltt;
typedef u32 GXVRamSubOBJ;
typedef u32 GXVRamSubOBJExtPltt;
typedef u32 GXVRamTex;
typedef u32 GXVRamTexPltt;

/* Background layer planes */
#define GX_PLANEMASK_NONE 0
#define GX_PLANEMASK_BG0  (1 << 0)
#define GX_PLANEMASK_BG1  (1 << 1)
#define GX_PLANEMASK_BG2  (1 << 2)
#define GX_PLANEMASK_BG3  (1 << 3)
#define GX_PLANEMASK_OBJ  (1 << 4)

/* Blend plane masks (for alpha blending) */
#define GX_BLEND_PLANEMASK_NONE   0
#define GX_BLEND_PLANEMASK_BG0    (1 << 0)
#define GX_BLEND_PLANEMASK_BG1    (1 << 1)
#define GX_BLEND_PLANEMASK_BG2    (1 << 2)
#define GX_BLEND_PLANEMASK_BG3    (1 << 3)
#define GX_BLEND_PLANEMASK_OBJ    (1 << 4)
#define GX_BLEND_PLANEMASK_BD     (1 << 5)
/* GX_BLEND_BGALL and GX_BLEND_ALL defined in enums.h */

/* Background color modes */
typedef enum {
    GX_BG_COLORMODE_16 = 0,
    GX_BG_COLORMODE_256 = 1
} GXBgColorMode;

/* Background screen base addresses (2KB boundaries) */
#define GX_BG_SCRBASE_0x0000  0x0000
#define GX_BG_SCRBASE_0x0800  0x0800
#define GX_BG_SCRBASE_0x1000  0x1000
#define GX_BG_SCRBASE_0x1800  0x1800
#define GX_BG_SCRBASE_0x2000  0x2000
#define GX_BG_SCRBASE_0x2800  0x2800
#define GX_BG_SCRBASE_0x3000  0x3000
#define GX_BG_SCRBASE_0x3800  0x3800
#define GX_BG_SCRBASE_0x4000  0x4000
#define GX_BG_SCRBASE_0x4800  0x4800
#define GX_BG_SCRBASE_0x5000  0x5000
#define GX_BG_SCRBASE_0x5800  0x5800
#define GX_BG_SCRBASE_0x6000  0x6000
#define GX_BG_SCRBASE_0x6800  0x6800
#define GX_BG_SCRBASE_0x7000  0x7000
#define GX_BG_SCRBASE_0x7800  0x7800

/* Background character base addresses */
#define GX_BG_CHARBASE_0x00000  0x00000
#define GX_BG_CHARBASE_0x04000  0x04000
#define GX_BG_CHARBASE_0x08000  0x08000
#define GX_BG_CHARBASE_0x0c000  0x0c000
#define GX_BG_CHARBASE_0x10000  0x10000

/* Background extended palette slots */
typedef enum {
    GX_BG_EXTPLTT_01 = 0,
    GX_BG_EXTPLTT_23 = 1
} GXBgExtPltt;

typedef enum {
    GX_BUFFER_MODE_Z = 0,
    GX_BUFFER_MODE_W = 1
} GXBufferMode;

/* Alternate names used in code */
#define GX_BUFFERMODE_Z  GX_BUFFER_MODE_Z
#define GX_BUFFERMODE_W  GX_BUFFER_MODE_W

/* 3D shading modes */
#define GX_SHADING_TOON      0
#define GX_SHADING_HIGHLIGHT 1

/* 3D fog modes */
#define GX_FOGBLEND_COLOR_ALPHA  0
#define GX_FOGBLEND_ALPHA        1
#define GX_FOGSLOPE_0x8000       0x8000

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

/* VRAM bank allocation constants */
#define GX_VRAM_BG_128_A             (1 << 0)
#define GX_VRAM_BG_128_B             (1 << 1)
#define GX_VRAM_BGEXTPLTT_NONE       0
#define GX_VRAM_SUB_BG_32_H          (1 << 7)
#define GX_VRAM_SUB_BG_128_C         (1 << 2)
#define GX_VRAM_SUB_BGEXTPLTT_NONE   0
#define GX_VRAM_OBJ_64_E             (1 << 4)
#define GX_VRAM_OBJEXTPLTT_NONE      0
#define GX_VRAM_SUB_OBJ_16_I         (1 << 8)
#define GX_VRAM_SUB_OBJEXTPLTT_NONE  0
#define GX_VRAM_TEX_01_BC            ((1 << 1) | (1 << 2))
#define GX_VRAM_TEX_0_B              (1 << 1)
#define GX_VRAM_TEXPLTT_01_FG        ((1 << 5) | (1 << 6))

/* Hardware VRAM addresses (DS memory map) */
#define HW_BG_VRAM          0x06000000
#define HW_BG_VRAM_SIZE     0x80000
#define HW_DB_BG_VRAM       0x06200000
#define HW_DB_BG_VRAM_SIZE  0x20000
#define HW_OBJ_VRAM         0x06400000
#define HW_OBJ_VRAM_SIZE    0x40000
#define HW_DB_OBJ_VRAM      0x06600000
#define HW_DB_OBJ_VRAM_SIZE 0x20000
#define HW_BG_PLTT          0x05000000
#define HW_BG_PLTT_SIZE     0x200
#define HW_DB_BG_PLTT       0x05000200
#define HW_DB_BG_PLTT_SIZE  0x200
#define HW_OBJ_PLTT         0x05000200
#define HW_OBJ_PLTT_SIZE    0x200
#define HW_DB_OBJ_PLTT      0x05000600
#define HW_DB_OBJ_PLTT_SIZE 0x200

/* Display modes */
#define GX_DISPMODE_GRAPHICS  0
#define GX_DISPMODE_VRAM      1
#define GX_DISPMODE_OFF       2

/* Background modes */
#define GX_BGMODE_0  0
#define GX_BGMODE_1  1
#define GX_BGMODE_2  2
#define GX_BGMODE_3  3
#define GX_BGMODE_4  4
#define GX_BGMODE_5  5

/* Background 0 modes */
#define GX_BG0_AS_3D  (1 << 3)

/* Window masks */
#define GX_WNDMASK_NONE  0
#define GX_WNDMASK_W0    (1 << 0)
#define GX_WNDMASK_W1    (1 << 1)
#define GX_WNDMASK_OBJ   (1 << 2)
#define GX_WNDMASK_OBJW  (1 << 3)
#define GX_WNDMASK_OW    (1 << 3)  /* Alias for OBJW */

/* Window plane masks */
#define GX_WND_PLANEMASK_NONE  0
#define GX_WND_PLANEMASK_BG0   (1 << 0)
#define GX_WND_PLANEMASK_BG1   (1 << 1)
#define GX_WND_PLANEMASK_BG2   (1 << 2)
#define GX_WND_PLANEMASK_BG3   (1 << 3)
#define GX_WND_PLANEMASK_OBJ   (1 << 4)
#define GX_WND_PLANEMASK_BD    (1 << 5)
// GX_WND_PLANEMASK_ALL is in enums.h

/* Graphics sort and buffer modes - defined in enums.h as GXSortMode and GXBufferMode */

/* ========================================================================
 * Touch Screen (TP) Types
 * ======================================================================== */

typedef struct {
    u16 x, y;
    u16 touch;  // 1 if touching, 0 if not
    u16 validity;
} TPData;

/* ========================================================================
 * Wireless (WM) Types
 * ======================================================================== */

typedef struct {
    u8 data[256];  // BSS descriptor for wireless connection
} WMBssDesc;

/* ========================================================================
 * Sound System Types
 * ======================================================================== */

typedef enum {
    NNS_SND_WAVE_FORMAT_PCM8 = 0,
    NNS_SND_WAVE_FORMAT_PCM16 = 1,
    NNS_SND_WAVE_FORMAT_ADPCM = 2
} NNSSndWaveFormat;

/* ========================================================================
 * 3D Graphics Enums
 * ======================================================================== */

typedef enum {
    GX_POLYGONMODE_MODULATE = 0,
    GX_POLYGONMODE_DECAL = 1,
    GX_POLYGONMODE_TOON = 2,
    GX_POLYGONMODE_SHADOW = 3
} GXPolygonMode;

typedef enum {
    GX_CULL_NONE = 0,
    GX_CULL_FRONT = 1,
    GX_CULL_BACK = 2,
    GX_CULL_ALL = 3
} GXCull;

typedef enum {
    GX_LIGHTID_0 = (1 << 0),
    GX_LIGHTID_1 = (1 << 1),
    GX_LIGHTID_2 = (1 << 2),
    GX_LIGHTID_3 = (1 << 3)
} GXLightId;

/* 3D polygon attributes */
typedef u32 GXPolygonAttrMisc;

/* ========================================================================
 * Microphone (MIC) Types
 * ======================================================================== */

typedef enum {
    MIC_RESULT_SUCCESS = 0,
    MIC_RESULT_BUSY = 1,
    MIC_RESULT_ILLEGAL_STATUS = 2,
    MIC_RESULT_SEND_ERROR = 3,
    MIC_RESULT_INVALID_PARAM = 4,
    MIC_RESULT_ILLEGAL_PARAMETER = 5
} MICResult;

typedef struct {
    void* buffer;
    u32 size;
    u32 rate;
    BOOL loop_enable;
} MICAutoParam;

typedef enum {
    MIC_SAMPLING_TYPE_8BIT = 0,
    MIC_SAMPLING_TYPE_12BIT = 1,
    MIC_SAMPLING_TYPE_SIGNED_8BIT = 2,
    MIC_SAMPLING_TYPE_SIGNED_12BIT = 3
} MICSamplingType;

typedef void (*MICCallback)(MICResult result, void* arg);

/* ========================================================================
 * Input System Types (PAD)
 * ======================================================================== */

typedef enum {
    PAD_BUTTON_A      = 0x0001,
    PAD_BUTTON_B      = 0x0002,
    PAD_BUTTON_SELECT = 0x0004,
    PAD_BUTTON_START  = 0x0008,
    PAD_BUTTON_RIGHT  = 0x0010,
    PAD_BUTTON_LEFT   = 0x0020,
    PAD_BUTTON_UP     = 0x0040,
    PAD_BUTTON_DOWN   = 0x0080,
    PAD_BUTTON_R      = 0x0100,
    PAD_BUTTON_L      = 0x0200,
    PAD_BUTTON_X      = 0x0400,
    PAD_BUTTON_Y      = 0x0800
} PADButton;

#define PAD_BUTTON_START 0x0008

/* PAD_KEY aliases (used in some code paths) */
#define PAD_KEY_A      PAD_BUTTON_A
#define PAD_KEY_B      PAD_BUTTON_B
#define PAD_KEY_SELECT PAD_BUTTON_SELECT
#define PAD_KEY_START  PAD_BUTTON_START
#define PAD_KEY_RIGHT  PAD_BUTTON_RIGHT
#define PAD_KEY_LEFT   PAD_BUTTON_LEFT
#define PAD_KEY_UP     PAD_BUTTON_UP
#define PAD_KEY_DOWN   PAD_BUTTON_DOWN
#define PAD_KEY_R      PAD_BUTTON_R
#define PAD_KEY_L      PAD_BUTTON_L
#define PAD_KEY_X      PAD_BUTTON_X
#define PAD_KEY_Y      PAD_BUTTON_Y

#endif /* !PLATFORM_DS */

#endif /* POKEPLATINUM_NNS_TYPES_H */
