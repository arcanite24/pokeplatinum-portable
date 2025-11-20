#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

/**
 * @file platform_types.h
 * @brief Platform-agnostic type definitions
 * 
 * This file provides unified type definitions that work across both
 * Nintendo DS and SDL3 platforms.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "platform_config.h"

#ifdef PLATFORM_SDL
    // SDL3 platform type mappings
    typedef uint8_t  u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
    
    typedef int8_t   s8;
    typedef int16_t  s16;
    typedef int32_t  s32;
    typedef int64_t  s64;
    
    // Volatile types (hardware register access)
    typedef volatile u8  vu8;
    typedef volatile u16 vu16;
    typedef volatile u32 vu32;
    typedef volatile u64 vu64;
    
    typedef volatile s8  vs8;
    typedef volatile s16 vs16;
    typedef volatile s32 vs32;
    typedef volatile s64 vs64;
    
    typedef float    f32;
    typedef double   f64;
    
    typedef int      BOOL;
    
    #ifndef TRUE
        #define TRUE  1
    #endif
    #ifndef FALSE
        #define FALSE 0
    #endif
    
    // Memory alignment attribute
    #define ALIGN_4  __attribute__((aligned(4)))
    
    // Don't define NULL - let system headers define it
    // The DS codebase uses NULL for integer 0 in many places,
    // which triggers -Wint-conversion warnings on modern compilers.
    // We suppress these warnings in CMakeLists.txt instead.
    
    // Hardware constants (Nintendo DS LCD)
    #define HW_LCD_WIDTH  256
    #define HW_LCD_HEIGHT 192
    
    // Hardware register addresses (DS BG scroll registers)
    // SDL stub: Dummy addresses, not actually used
    #define REG_BG0HOFS_ADDR ((vu16*)0x04000010)
    #define REG_BG0VOFS_ADDR ((vu16*)0x04000012)
    #define REG_BG1HOFS_ADDR ((vu16*)0x04000014)
    #define REG_BG1VOFS_ADDR ((vu16*)0x04000016)
    #define REG_BG2HOFS_ADDR ((vu16*)0x04000018)
    #define REG_BG2VOFS_ADDR ((vu16*)0x0400001A)
    #define REG_BG3HOFS_ADDR ((vu16*)0x0400001C)
    #define REG_BG3VOFS_ADDR ((vu16*)0x0400001E)
    
    // Fixed-point types (used extensively in DS code)
    typedef s32 fx32;
    typedef s16 fx16;
    
    // Fixed-point conversion macros
    #define FX32_SHIFT 12
    #define FX32_ONE   (1 << FX32_SHIFT)
    #define FX_Whole(x) ((x) >> FX32_SHIFT)
    #define FX_Frac(x)  ((x) & (FX32_ONE - 1))
    #define FX_FX32_TO_F32(x) ((float)(x) / (float)FX32_ONE)
    
    // Fixed-point math functions
    static inline fx32 FX_Div(fx32 a, fx32 b) {
        if (b == 0) return 0x7FFFFFFF; // Max positive value
        return (fx32)(((s64)a << FX32_SHIFT) / b);
    }
    
    static inline fx32 FX_Modf(fx32 a, fx32 b) {
        if (b == 0) return 0;
        return a % b;
    }
    
    // Math utility macros
    #define MATH_ABS(x) ((x) < 0 ? -(x) : (x))
    #define SDK_COMPILER_ASSERT(expr) extern char compiler_assert_##__LINE__ [(expr) ? 1 : -1]

    // Vector and Matrix types
    typedef struct {
        fx32 x, y, z;
    } VecFx32;

    typedef struct {
        fx16 x, y, z;
    } VecFx16;

    typedef struct {
        fx32 m[3][3];
    } MtxFx33;

    typedef struct {
        fx32 m[2][2];
    } MtxFx22;

    // Memory manipulation stubs
    static inline void MI_CpuClear16(void *dest, u32 size) {
        memset(dest, 0, size);
    }

    static inline void MI_CpuCopy8(const void *src, void *dest, u32 size) {
        memcpy(dest, src, size);
    }

    static inline void MI_CpuCopy16(const void *src, void *dest, u32 size) {
        memcpy(dest, src, size);
    }

    static inline void MI_CpuCopy32(const void *src, void *dest, u32 size) {
        memcpy(dest, src, size);
    }

    static inline void MI_CpuClear8(void *dest, u32 size) {
        memset(dest, 0, size);
    }

    static inline void MI_CpuClearFast(void *dest, u32 size) {
        memset(dest, 0, size);
    }

    static inline void MI_CpuFill16(void *dest, u16 data, u32 size) {
        u16 *ptr = (u16*)dest;
        for (u32 i = 0; i < size / 2; i++) ptr[i] = data;
    }

    static inline void DC_FlushRange(const void *startAddr, u32 nBytes) {}

    static inline void MTX_Identity33(MtxFx33 *mtx) {
        memset(mtx, 0, sizeof(MtxFx33));
        mtx->m[0][0] = FX32_ONE;
        mtx->m[1][1] = FX32_ONE;
        mtx->m[2][2] = FX32_ONE;
    }

    // GX Screen Sizes
    #define GX_BG_SCRSIZE_TEXT_256x256 0
    #define GX_BG_SCRSIZE_TEXT_512x256 1
    #define GX_BG_SCRSIZE_TEXT_256x512 2
    #define GX_BG_SCRSIZE_TEXT_512x512 3

    #define GX_BG_SCRSIZE_AFFINE_128x128 0
    #define GX_BG_SCRSIZE_AFFINE_256x256 1
    #define GX_BG_SCRSIZE_AFFINE_512x512 2
    #define GX_BG_SCRSIZE_AFFINE_1024x1024 3

    #define GX_BG_SCRSIZE_256x16PLTT_128x128 0
    #define GX_BG_SCRSIZE_256x16PLTT_256x256 1
    #define GX_BG_SCRSIZE_256x16PLTT_512x512 2
    #define GX_BG_SCRSIZE_256x16PLTT_1024x1024 3

    // GX Screen Bases (offsets in 2KB units)
    #define GX_BG_SCRBASE_0x0000 0
    #define GX_BG_SCRBASE_0x0800 1
    #define GX_BG_SCRBASE_0x1000 2
    #define GX_BG_SCRBASE_0x1800 3
    #define GX_BG_SCRBASE_0x2000 4
    #define GX_BG_SCRBASE_0x2800 5
    #define GX_BG_SCRBASE_0x3000 6
    #define GX_BG_SCRBASE_0x3800 7
    #define GX_BG_SCRBASE_0x4000 8
    #define GX_BG_SCRBASE_0x4800 9
    #define GX_BG_SCRBASE_0x5000 10
    #define GX_BG_SCRBASE_0x5800 11
    #define GX_BG_SCRBASE_0x6000 12
    #define GX_BG_SCRBASE_0x6800 13
    #define GX_BG_SCRBASE_0x7000 14
    #define GX_BG_SCRBASE_0x7800 15
    #define GX_BG_SCRBASE_0x8000 16
    #define GX_BG_SCRBASE_0x8800 17
    #define GX_BG_SCRBASE_0x9000 18
    #define GX_BG_SCRBASE_0x9800 19
    #define GX_BG_SCRBASE_0xa000 20
    #define GX_BG_SCRBASE_0xa800 21
    #define GX_BG_SCRBASE_0xb000 22
    #define GX_BG_SCRBASE_0xb800 23
    #define GX_BG_SCRBASE_0xc000 24
    #define GX_BG_SCRBASE_0xc800 25
    #define GX_BG_SCRBASE_0xd000 26
    #define GX_BG_SCRBASE_0xd800 27
    #define GX_BG_SCRBASE_0xe000 28
    #define GX_BG_SCRBASE_0xe800 29
    #define GX_BG_SCRBASE_0xf000 30
    #define GX_BG_SCRBASE_0xf800 31

    // GX Background Control Types (moved from nns_types.h)
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

    // Sound and TouchPad stubs
    typedef struct {
        u16 format;
        u16 loop;
        u32 rate;
        u32 timer;
        u32 loopStart;
        u32 loopEnd;
    } NNSSndWaveFormat;

    typedef enum {
        MIC_RESULT_SUCCESS = 0,
        MIC_RESULT_BUSY,
        MIC_RESULT_ILLEGAL_PARAMETER,
        MIC_RESULT_ILLEGAL_STATUS
    } MICResult;

    typedef enum {
        MIC_SAMPLING_TYPE_8BIT = 0,
        MIC_SAMPLING_TYPE_12BIT,
        MIC_SAMPLING_TYPE_SIGNED_8BIT,
        MIC_SAMPLING_TYPE_SIGNED_12BIT
    } MICSamplingType;

    typedef void (*MICCallback)(MICResult result, void *arg);

    typedef struct {
        MICSamplingType type;
        void *buffer;
        u32 size;
        u32 rate;
        BOOL loop;
        MICCallback callback;
        void *arg;
    } MICAutoParam;

    typedef struct {
        u16 x;
        u16 y;
        u16 touch;
        u16 validity;
    } TPData;
    
    // Graphics hardware functions (DS specific, stub for SDL)
    static inline void G2_SetBG0Priority(int priority) {
        // Stub: DS sets background layer 0 priority
        (void)priority;
    }

    static inline void G2_SetBG0Control(int screenSize, int colorMode, int screenBase, int charBase, int bgExtPltt) {}
    static inline void G2_BG0Mosaic(int mosaic) {}
    static inline void G2_SetBG0Offset(int hOffset, int vOffset) {}
    
    static inline void G2_SetBG1Control(int screenSize, int colorMode, int screenBase, int charBase, int bgExtPltt) {}
    static inline void G2_SetBG1Priority(int priority) {}
    static inline void G2_BG1Mosaic(int mosaic) {}
    static inline void G2_SetBG1Offset(int hOffset, int vOffset) {}
    
    static inline void G2_SetBG2ControlText(int screenSize, int colorMode, int screenBase, int charBase) {}
    static inline void G2_SetBG2ControlAffine(int screenSize, int areaOver, int screenBase, int charBase) {}
    static inline void G2_SetBG2Control256x16Pltt(int screenSize, int areaOver, int screenBase, int charBase) {}
    static inline void G2_SetBG2Priority(int priority) {}
    static inline void G2_BG2Mosaic(int mosaic) {}
    static inline void G2_SetBG2Offset(int hOffset, int vOffset) {}
    static inline void G2_SetBG2Affine(const void* mtx, int centerX, int centerY, int x1, int y1) {}
    
    static inline void G2_SetBG3ControlText(int screenSize, int colorMode, int screenBase, int charBase) {}
    static inline void G2_SetBG3ControlAffine(int screenSize, int areaOver, int screenBase, int charBase) {}
    static inline void G2_SetBG3Control256x16Pltt(int screenSize, int areaOver, int screenBase, int charBase) {}
    static inline void G2_SetBG3Priority(int priority) {}
    static inline void G2_BG3Mosaic(int mosaic) {}
    static inline void G2_SetBG3Offset(int hOffset, int vOffset) {}
    static inline void G2_SetBG3Affine(const void* mtx, int centerX, int centerY, int x1, int y1) {}
    
    static inline void G2S_SetBG0Control(int screenSize, int colorMode, int screenBase, int charBase, int bgExtPltt) {}
    static inline void G2S_SetBG0Priority(int priority) {}
    static inline void G2S_BG0Mosaic(int mosaic) {}
    static inline void G2S_SetBG0Offset(int hOffset, int vOffset) {}
    
    static inline void G2S_SetBG1Control(int screenSize, int colorMode, int screenBase, int charBase, int bgExtPltt) {}
    static inline void G2S_SetBG1Priority(int priority) {}
    static inline void G2S_BG1Mosaic(int mosaic) {}
    static inline void G2S_SetBG1Offset(int hOffset, int vOffset) {}
    
    static inline void G2S_SetBG2ControlText(int screenSize, int colorMode, int screenBase, int charBase) {}
    static inline void G2S_SetBG2ControlAffine(int screenSize, int areaOver, int screenBase, int charBase) {}
    static inline void G2S_SetBG2Control256x16Pltt(int screenSize, int areaOver, int screenBase, int charBase) {}
    static inline void G2S_SetBG2Priority(int priority) {}
    static inline void G2S_BG2Mosaic(int mosaic) {}
    static inline void G2S_SetBG2Offset(int hOffset, int vOffset) {}
    static inline void G2S_SetBG2Affine(const void* mtx, int centerX, int centerY, int x1, int y1) {}
    
    static inline void G2S_SetBG3ControlText(int screenSize, int colorMode, int screenBase, int charBase) {}
    static inline void G2S_SetBG3ControlAffine(int screenSize, int areaOver, int screenBase, int charBase) {}
    static inline void G2S_SetBG3Control256x16Pltt(int screenSize, int areaOver, int screenBase, int charBase) {}
    static inline void G2S_SetBG3Priority(int priority) {}
    static inline void G2S_BG3Mosaic(int mosaic) {}
    static inline void G2S_SetBG3Offset(int hOffset, int vOffset) {}
    static inline void G2S_SetBG3Affine(const void* mtx, int centerX, int centerY, int x1, int y1) {}
    
    // Getters (SDL stubs returning 0/empty structs)
    static inline GXBg01Control G2_GetBG0Control(void) { GXBg01Control c = {0}; return c; }
    static inline GXBg01Control G2_GetBG1Control(void) { GXBg01Control c = {0}; return c; }
    static inline GXBg23ControlText G2_GetBG2ControlText(void) { GXBg23ControlText c = {0}; return c; }
    static inline GXBg23ControlAffine G2_GetBG2ControlAffine(void) { GXBg23ControlAffine c = {0}; return c; }
    static inline GXBg23Control256x16Pltt G2_GetBG2Control256x16Pltt(void) { GXBg23Control256x16Pltt c = {0}; return c; }
    static inline GXBg23ControlText G2_GetBG3ControlText(void) { GXBg23ControlText c = {0}; return c; }
    static inline GXBg23ControlAffine G2_GetBG3ControlAffine(void) { GXBg23ControlAffine c = {0}; return c; }
    static inline GXBg23Control256x16Pltt G2_GetBG3Control256x16Pltt(void) { GXBg23Control256x16Pltt c = {0}; return c; }

    static inline GXBg01Control G2S_GetBG0Control(void) { GXBg01Control c = {0}; return c; }
    static inline GXBg01Control G2S_GetBG1Control(void) { GXBg01Control c = {0}; return c; }
    static inline GXBg23ControlText G2S_GetBG2ControlText(void) { GXBg23ControlText c = {0}; return c; }
    static inline GXBg23ControlAffine G2S_GetBG2ControlAffine(void) { GXBg23ControlAffine c = {0}; return c; }
    static inline GXBg23Control256x16Pltt G2S_GetBG2Control256x16Pltt(void) { GXBg23Control256x16Pltt c = {0}; return c; }
    static inline GXBg23ControlText G2S_GetBG3ControlText(void) { GXBg23ControlText c = {0}; return c; }
    static inline GXBg23ControlAffine G2S_GetBG3ControlAffine(void) { GXBg23ControlAffine c = {0}; return c; }
    static inline GXBg23Control256x16Pltt G2S_GetBG3Control256x16Pltt(void) { GXBg23Control256x16Pltt c = {0}; return c; }

    // GX Bank Stubs
    static inline void GX_ResetBankForBG(void) {}
    static inline void GX_ResetBankForBGExtPltt(void) {}
    static inline void GX_ResetBankForSubBG(void) {}
    static inline void GX_ResetBankForSubBGExtPltt(void) {}
    static inline void GX_ResetBankForOBJ(void) {}
    static inline void GX_ResetBankForOBJExtPltt(void) {}
    static inline void GX_ResetBankForSubOBJ(void) {}
    static inline void GX_ResetBankForSubOBJExtPltt(void) {}
    static inline void GX_ResetBankForTex(void) {}
    static inline void GX_ResetBankForTexPltt(void) {}
    
    static inline void GX_SetBankForBG(int bank) {}
    static inline void GX_SetBankForBGExtPltt(int bank) {}
    static inline void GX_SetBankForSubBG(int bank) {}
    static inline void GX_SetBankForSubBGExtPltt(int bank) {}
    static inline void GX_SetBankForOBJ(int bank) {}
    static inline void GX_SetBankForOBJExtPltt(int bank) {}
    static inline void GX_SetBankForSubOBJ(int bank) {}
    static inline void GX_SetBankForSubOBJExtPltt(int bank) {}
    static inline void GX_SetBankForTex(int bank) {}
    static inline void GX_SetBankForTexPltt(int bank) {}
    
    // Memory Interface (MI) functions
    static inline void MI_UncompressLZ8(const void* src, void* dest) {
        // Stub: Just copy for now, assuming uncompressed in SDL build or handled elsewhere
        // In a real port, we'd implement LZ77 decompression
        (void)src; (void)dest;
    }
    
    static inline u32 MI_GetUncompressedSize(const void* src) {
        // Stub: Return 0 or read header
        return 0;
    }
    
    static inline void MI_CpuFillFast(void* dest, u32 data, u32 size) {
        // Simple memset-like fill (data is 32-bit, but for 0 we can use memset)
        if (data == 0) {
            memset(dest, 0, size);
        } else {
            u32* ptr = (u32*)dest;
            for (u32 i = 0; i < size / 4; i++) {
                ptr[i] = data;
            }
        }
    }

    // VRAM Character Pointer Getters
    static inline void* G2_GetBG0CharPtr(void) { return NULL; }
    static inline void* G2_GetBG1CharPtr(void) { return NULL; }
    static inline void* G2_GetBG2CharPtr(void) { return NULL; }
    static inline void* G2_GetBG3CharPtr(void) { return NULL; }
    
    static inline void* G2S_GetBG0CharPtr(void) { return NULL; }
    static inline void* G2S_GetBG1CharPtr(void) { return NULL; }
    static inline void* G2S_GetBG2CharPtr(void) { return NULL; }
    static inline void* G2S_GetBG3CharPtr(void) { return NULL; }

    // Palette loading functions
    static inline void GX_LoadBGPltt(const void* src, u32 offset, u32 size) {
        // Stub: DS loads palette to background palette RAM
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GXS_LoadBGPltt(const void* src, u32 offset, u32 size) {
        // Stub: DS loads palette to sub-screen background palette RAM
        (void)src; (void)offset; (void)size;
    }
    
    // GX Background Screen (Tilemap) Loading Functions
    static inline void GX_LoadBG0Scr(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GX_LoadBG1Scr(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GX_LoadBG2Scr(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GX_LoadBG3Scr(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    // GX Background Character (Tile Graphics) Loading Functions
    static inline void GX_LoadBG0Char(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GX_LoadBG1Char(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GX_LoadBG2Char(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GX_LoadBG3Char(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    // GXS Sub-Screen Background Screen (Tilemap) Loading Functions
    static inline void GXS_LoadBG0Scr(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GXS_LoadBG1Scr(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GXS_LoadBG2Scr(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GXS_LoadBG3Scr(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    // GXS Sub-Screen Background Character (Tile Graphics) Loading Functions
    static inline void GXS_LoadBG0Char(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GXS_LoadBG1Char(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GXS_LoadBG2Char(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GXS_LoadBG3Char(const void* src, u32 offset, u32 size) {
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GX_LoadOBJPltt(const void* src, u32 offset, u32 size) {
        // Stub: DS loads palette to sprite palette RAM
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GXS_LoadOBJPltt(const void* src, u32 offset, u32 size) {
        // Stub: DS loads palette to sub-screen sprite palette RAM
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GX_SetVisibleWnd(u32 mask) {
        // Stub: DS sets window visibility mask
        (void)mask;
    }
    
    static inline void GXS_SetVisibleWnd(u32 mask) {
        // Stub: DS sets sub-screen window visibility mask
        (void)mask;
    }
    
    static inline void G2_SetWnd0InsidePlane(u32 mask, int enable) {
        // Stub: DS window 0 inside plane
        (void)mask; (void)enable;
    }
    
    static inline void G2_SetWndOutsidePlane(u32 mask, int enable) {
        // Stub: DS window outside plane
        (void)mask; (void)enable;
    }
    
    static inline void G3X_SetShading(int mode) {
        // Stub: DS 3D shading mode
        (void)mode;
    }
    
    static inline void G3X_AntiAlias(int enable) {
        // Stub: DS 3D anti-aliasing
        (void)enable;
    }
    
    static inline void G3X_AlphaTest(int enable, int ref) {
        // Stub: DS 3D alpha testing
        (void)enable; (void)ref;
    }
    
    static inline void G3X_AlphaBlend(int enable) {
        // Stub: DS 3D alpha blending
        (void)enable;
    }
    
    static inline void G3X_EdgeMarking(int enable) {
        // Stub: DS 3D edge marking (toon shading outline)
        (void)enable;
    }
    
    static inline void G3X_SetFog(int enable, int mode, int slope, int offset) {
        // Stub: DS 3D fog effect
        (void)enable; (void)mode; (void)slope; (void)offset;
    }
    
    static inline void G3X_SetClearColor(u16 color, int alpha, int depth, int polyID, int fogEnable) {
        // Stub: DS 3D clear color
        (void)color; (void)alpha; (void)depth; (void)polyID; (void)fogEnable;
    }
    
    static inline void G3_ViewPort(int x1, int y1, int x2, int y2) {
        // Stub: DS 3D viewport
        (void)x1; (void)y1; (void)x2; (void)y2;
    }
    
    static inline u32 NNS_GfdAllocTexVram(u32 size, int is4x4comp, int use4PlttMode) {
        // Stub: NitroSystem texture VRAM allocation
        (void)size; (void)is4x4comp; (void)use4PlttMode;
        return 0; // Return 0 (no VRAM allocated)
    }
    
    static inline u32 NNS_GfdAllocPlttVram(u32 size, int is4Pltt, int use4PlttMode) {
        // Stub: NitroSystem palette VRAM allocation
        (void)size; (void)is4Pltt; (void)use4PlttMode;
        return 0; // Return 0 (no VRAM allocated)
    }
    
    static inline u32 NNS_GfdGetTexKeyAddr(u32 key) {
        // Stub: Get texture VRAM address from key
        return key; // Just return the key itself (already an address)
    }
    
    #define NNS_GFD_ALLOC_ERROR_PLTTKEY ((u32)-1)
    static inline u32 NNS_GfdGetPlttKeyAddr(u32 key) { return 0; }
    
    static inline void G2_BlendNone(void) {
        // Stub: DS 2D blend none
    }
    
    static inline void G2S_BlendNone(void) {
        // Stub: DS 2D sub-screen blend none
    }
    
    static inline void G2_SetBlendAlpha(u32 plane1, u32 plane2, int ev1, int ev2) {
        // Stub: DS main screen alpha blending
        (void)plane1; (void)plane2; (void)ev1; (void)ev2;
    }
    
    static inline void G2_ChangeBlendAlpha(int ev1, int ev2) {
        // Stub: DS change alpha blend values
        (void)ev1; (void)ev2;
    }
    
    static inline void G2S_SetBlendAlpha(u32 plane1, u32 plane2, int ev1, int ev2) {
        // Stub: DS sub-screen alpha blending
        (void)plane1; (void)plane2; (void)ev1; (void)ev2;
    }
    
    static inline void G2S_SetWndOutsidePlane(u32 plane, int visible) {
        // Stub: DS sub-screen window outside plane
        (void)plane; (void)visible;
    }
    
    static inline void G2S_SetWnd0InsidePlane(u32 plane, int visible) {
        // Stub: DS sub-screen window 0 inside plane
        (void)plane; (void)visible;
    }
    
    static inline void G2S_SetWnd0Position(int x1, int y1, int x2, int y2) {
        // Stub: DS sub-screen window 0 position
        (void)x1; (void)y1; (void)x2; (void)y2;
    }
    
    static inline void G2_SetWnd1InsidePlane(u32 plane, int visible) {
        // Stub: DS main screen window 1 inside plane
        (void)plane; (void)visible;
    }
    
    static inline void G2S_SetWnd1InsidePlane(u32 plane, int visible) {
        // Stub: DS sub-screen window 1 inside plane
        (void)plane; (void)visible;
    }
    
    static inline void G2S_SetWnd1Position(int x1, int y1, int x2, int y2) {
        // Stub: DS sub-screen window 1 position
        (void)x1; (void)y1; (void)x2; (void)y2;
    }
    
    static inline void G2_SetWndOBJInsidePlane(u32 plane, int visible) {
        // Stub: DS main screen OBJ window inside plane
        (void)plane; (void)visible;
    }
    
    static inline void G2_SetWnd0Position(int x1, int y1, int x2, int y2) {
        // Stub: DS main screen window 0 position
        (void)x1; (void)y1; (void)x2; (void)y2;
    }

    static inline void G2_SetWnd1Position(int x1, int y1, int x2, int y2) {
        // Stub: DS main screen window 1 position
        (void)x1; (void)y1; (void)x2; (void)y2;
    }
    
    static inline void G2_SetOBJMosaicSize(int width, int height) {
        // Stub: DS main screen object mosaic effect size
        (void)width; (void)height;
    }
    
    // Fixed-point math functions
    static inline fx32 FX_ModS32(fx32 a, fx32 b) {
        // Stub: DS fixed-point modulo
        return a % b;
    }
    
    // VRAM access functions
    static inline void* G2S_GetOBJCharPtr(void) {
        // Stub: DS gets pointer to sub-screen sprite character data
        static char dummy_obj_vram[256 * 1024]; // 256KB sprite VRAM
        return dummy_obj_vram;
    }
    
    // Display status functions
    static inline u32 GX_GetVCount(void) {
        // Stub: DS gets current vertical scanline (0-262)
        return 0; // SDL: Return 0 (not in VBlank)
    }
    
    // OS interrupt functions
    #define OS_IE_V_BLANK  (1 << 0)
    
    static inline void OS_SetIrqCheckFlag(u32 flags) {
        // Stub: DS IRQ check flag
        (void)flags;
    }
    
    // Wireless functions and constants
    #define WM_LINK_LEVEL_3  3
    
    static inline int DWC_GetLinkLevel(void) {
        // Stub: DS wireless link strength level (0-3)
        return 3; // Return maximum strength
    }
    
    static inline int WM_GetLinkLevel(void) {
        // Stub: DS wireless manager link strength (0-3)
        return 3; // Return maximum strength
    }
    
    // Fixed-point math functions (DS SDK fx32 types)
    static inline fx32 FX_Mul(fx32 a, fx32 b) {
        // fx32 is 20.12 fixed point (32 bits, 12 fractional bits)
        // Multiply and shift right 12 bits to maintain fixed point
        return (fx32)(((s64)a * (s64)b) >> 12);
    }
    
#else
    // DS platform uses NitroSDK types (already defined in <nitro.h>)
    // No additional type definitions needed
#endif

// Platform-agnostic constants
#define PAL_INVALID_HANDLE NULL

// Common macros used across the codebase
#ifdef PLATFORM_SDL
    #include <assert.h>
    #include <stddef.h>
    #define GF_ASSERT(condition) assert(condition)
    #define NELEMS(array) (sizeof(array) / sizeof((array)[0]))
    // XtOffset: DS X Toolkit macro for struct field offsets
    // Some usage: XtOffset(BattleMon *, ability) - with pointer type
    // We need to work with both "Type" and "Type *"
    // Use pointer-based offset calculation that works with both
    #define XtOffset(type, field) ((size_t)&(((type)0)->field))
    
    // Overlay types
    // FSOverlayID is defined in overlay_manager.h
    typedef s32 FSOverlayID;
    
    // Overlay macros
    #define FS_EXTERN_OVERLAY(name) extern const FSOverlayID FS_OVERLAY_ID_##name
    #define FS_OVERLAY_ID(name) FS_OVERLAY_ID_##name

    typedef int MIProcessor;
    #define MI_PROCESSOR_ARM9 0
    
    // File System stubs
    typedef struct FSOverlayInfo {
        FSOverlayID id;
        void *address;
        u32 size;
        u32 bssSize;
        void (*sinit_init)(void);
        void (*sinit_init_end)(void);
        FSOverlayID file_id;
        u32 compressed:24;
        u32 flag:8;
    } FSOverlayInfo;
    
    static inline BOOL FS_LoadOverlayInfo(FSOverlayInfo *info, MIProcessor proc, FSOverlayID id) { return TRUE; }
    static inline BOOL FS_UnloadOverlay(MIProcessor proc, FSOverlayID id) { return TRUE; }

    // Hardware constants
    #define HW_ITCM_IMAGE 0x01000000
    #define HW_ITCM_END   0x01008000
    #define HW_DTCM       0x027C0000
    #define HW_DTCM_END   0x027C4000
    
    // FS Constants
    #define FS_DMA_NOT_USE 0

    // FS Types
    typedef struct FSFile {
        u32 id;
    } FSFile;

    // FS Functions
    static inline void* FS_GetOverlayAddress(FSOverlayInfo* info) { return info->address; }
    static inline u32 FS_SetDefaultDMA(u32 dma) { return 0; }
    static inline u32 FS_GetOverlayTotalSize(FSOverlayInfo* info) { return info->size + info->bssSize; }
    static inline BOOL FS_LoadOverlay(MIProcessor proc, FSOverlayID id) { return TRUE; }
    static inline BOOL FS_LoadOverlayImage(FSOverlayInfo* info) { return TRUE; }
    static inline void FS_StartOverlay(FSOverlayInfo* info) {}
    static inline void FS_InitFile(FSFile* file) {}
    static inline BOOL FS_LoadOverlayImageAsync(FSOverlayInfo* info, FSFile* file) { return TRUE; }
    static inline void FS_WaitAsync(FSFile* file) {}
    static inline void FS_CloseFile(FSFile* file) {}

    // OS
    static inline void OS_Init(void) {}
    static inline u32 OS_GetVBlankCount(void) { return 0; }
    static inline void OS_ResetSystem(int parameter) { }
    
    // Graphics stubs
    static inline void GX_SetOBJVRamModeChar(int mode) {}
    static inline void GXS_SetOBJVRamModeChar(int mode) {}
    static inline void NNS_G2dInitOamManagerModule(void) {}
    static inline void NNS_G3dGeFlushBuffer(void) {}
    static inline void NNS_G2dSetupSoftwareSpriteCamera(void) {}

    // G3/GX Stubs
    #define GX_POLYGON_ATTR_MISC_FOG 0
    
    static inline void NNS_G3dInit(void) {}
    static inline void G3X_InitMtxStack(void) {}
    static inline void G3_SwapBuffers(int sortMode, int bufferMode) {}
    static inline void G3X_SetEdgeColorTable(const void* data) {}
    
    // OS Stubs
    static inline void OS_WaitIrq(int clear, int irqFlags) {}
    
    // NNS Gfd Stubs
    #define NNS_GFD_ALLOC_FROM_LOW 0

    static inline u32 NNS_GfdGetLnkTexVramManagerWorkSize(u32 size) { return 0; }
    static inline void NNS_GfdInitLnkTexVramManager(u32 size, u32 offset, void* buffer, u32 bufferSize, BOOL useAsDefault) {}
    static inline void NNS_GfdInitFrmTexVramManager(u32 size, BOOL useAsDefault) {}
    static inline u32 NNS_GfdGetLnkPlttVramManagerWorkSize(u32 size) { return 0; }
    static inline void NNS_GfdInitLnkPlttVramManager(u32 size, void* buffer, u32 bufferSize, BOOL useAsDefault) {}
    
    // NNS G3d Stubs
    static inline void NNS_G3dGlbLightVector(int lightID, int x, int y, int z) {}
    static inline void NNS_G3dGlbLightColor(int lightID, int color) {}
    static inline void NNS_G3dGlbMaterialColorDiffAmb(int diff, int amb, BOOL set) {}
    static inline void NNS_G3dGlbMaterialColorSpecEmi(int spec, int emi, BOOL set) {}
    static inline void NNS_G3dGlbPolygonAttr(int lightMask, int polyMode, int cullMode, int polygonID, int alpha, int misc) {}
    static inline void* NNS_G3dGetTex(void* header) { return NULL; }
    static inline void* NNS_G3dGetMdlSet(void* header) { return NULL; }
    static inline void* NNS_G3dGetMdlByIdx(void* mdlSet, int idx) { return NULL; }
    static inline void NNS_G3dRenderObjInit(void* renderObj, void* model) {}
    static inline void NNS_G3dMdlUseGlbDiff(void* model) {}
    static inline void NNS_G3dMdlUseGlbAmb(void* model) {}
    static inline void NNS_G3dMdlUseGlbSpec(void* model) {}
    static inline void NNS_G3dMdlUseGlbEmi(void* model) {}
    
    static inline void NNS_G3dMdlUseGlbLightEnableFlag(void* model) {}
    static inline void DC_FlushAll(void) {}
    static inline void G3X_SetFogColor(u32 color, int alpha) {}
    static inline void G3X_SetFogTable(const void* table) {}

    // 3D Box Test Types
    typedef struct {
        fx16 x, y, z;
        fx16 width, height, depth;
    } GXBoxTestParam;

    typedef struct {
        u8 dummy[0x40];
        fx32 boxX, boxY, boxZ, boxW, boxH, boxD;
        fx32 boxPosScale;
    } NNSG3dResMdlInfo;

    static inline NNSG3dResMdlInfo* NNS_G3dGetMdlInfo(const void* model) {
        static NNSG3dResMdlInfo d = {0};
        d.boxPosScale = FX32_ONE;
        return &d;
    }

    static inline void NNS_G3dGlbSetBaseTrans(const VecFx32* pTrans) {}
    static inline void NNS_G3dGlbSetBaseRot(const MtxFx33* pRot) {}
    static inline void NNS_G3dGlbSetBaseScale(const VecFx32* pScale) {}
    static inline void NNS_G3dGlbFlush(void) {}
    static inline int G3_BoxTest(const GXBoxTestParam* param) { return 0; }
    
    // Geometry Engine Stubs
    static inline void NNS_G3dGePushMtx(void) {}
    static inline void NNS_G3dGePopMtx(int num) {}
    static inline void NNS_G3dGeScale(fx32 x, fx32 y, fx32 z) {}
    static inline void NNS_G3dGePolygonAttr(int lightMask, int polyMode, int cullMode, int polygonID, int alpha, int misc) {}
    
    static inline void NNS_G3dGeBegin(int primitive) {}
    static inline void NNS_G3dGeEnd(void) {}
    static inline void NNS_G3dGeBoxTest(const GXBoxTestParam* param) {}
    
    static inline int G3X_GetBoxTestResult(int* result) { 
        *result = 1; // Simulate result ready
        return 0; // BOX_TEST_RESULT_READY (0)
    }

    // GX Constants
    #define GX_LIGHTMASK_0 0
    #define GX_POLYGON_ATTR_MISC_FAR_CLIPPING 0
    #define GX_POLYGON_ATTR_MISC_DISP_1DOT 0
    #define GX_BEGIN_TRIANGLES 0
    #define BOX_TEST_RESULT_READY 0
    #define FX16_MAX 32767

    #define GX_FOGSLOPE_0x1000 0

    // Wireless Manager stubs
    typedef struct {
        u16 length;
        u16 rssi;
        u8 bssid[6];
        u16 ssidLength;
        u8 ssid[32];
        u16 capaInfo;
        struct {
            u16 basic;
            u16 support;
        } rateSet;
        u16 beaconPeriod;
        u16 dtimPeriod;
        u8 channel;
        u8 cfpPeriod;
        u16 cfpMaxDuration;
    } WMBssDesc;

    // Math stubs
    typedef struct {
        u64 x;
        u64 mul;
        u64 add;
    } MATHRandContext32;

    // Memory stubs
    static inline void MI_CpuClear32(void *dest, u32 size) {
        memset(dest, 0, size);
    }

    static inline void MI_CpuFill8(void *dest, u8 data, u32 size) {
        memset(dest, data, size);
    }
    
    static inline void MI_CpuCopyFast(const void *src, void *dest, u32 size) {
        memcpy(dest, src, size);
    }
    
    // GX VRAM Constants
    
    #define GX_BG0_AS_2D 0
    #define GX_BG_CHARBASE_0x18000 0
    #define GX_BG_CHARBASE_0x14000 0
    #define GX_BG_CHARBASE_0x20000 0
    
    #define GX_BG_CHARBASE_0x00000 0
    #define GX_BG_CHARBASE_0x04000 1
    #define GX_BG_CHARBASE_0x08000 2
    #define GX_BG_CHARBASE_0x10000 4
    
    #define GX_BG_SCRBASE_0x0000 0
    #define GX_BG_SCRBASE_0x0800 1
    #define GX_BG_SCRBASE_0x1000 2
    #define GX_BG_SCRBASE_0x2000 4
    #define GX_BG_SCRBASE_0x4000 8
    
    #define GX_BG_SCRBASE_0x6000 12
    
    #define GX_BG_CHARBASE_0x0c000 3

    // Hardware Addresses (Stubbed for SDL)
    #define HW_BG_PLTT 0
    #define HW_DB_BG_PLTT 0

    // GX Polygon Attr
    typedef struct {
        u32 attr;
    } GXPolygonAttrMisc;

    // Math Types
    typedef struct {
        fx32 m[4][3];
    } MtxFx43;

    // Math Macros
    #define FX32_CONST(x) ((fx32)((x) * 4096.0f))
    #define FX_RAD_TO_IDX(x) ((u16)((x) * 65536.0f / (2.0f * 3.14159f)))
    #define FX_SinIdx(x) ((fx32)(sin((x) * (2.0f * 3.14159f) / 65536.0f) * 4096.0f))
    #define FX_CosIdx(x) ((fx32)(cos((x) * (2.0f * 3.14159f) / 65536.0f) * 4096.0f))
    #define FX_Atan2Idx(y, x) ((u16)(atan2((double)(y), (double)(x)) * 65536.0f / (2.0f * 3.14159f)))
    #define FX_Sqrt(x) ((fx32)(sqrt((double)(x) / 4096.0) * 4096.0))

    // Math Functions
    static inline void VEC_Normalize(const VecFx32 *a, VecFx32 *b) {
        // Stub
        *b = *a;
    }

    static inline void MTX_RotX33(MtxFx33 *mtx, fx32 sin, fx32 cos) {
        // Stub
        MTX_Identity33(mtx);
        (void)sin; (void)cos;
    }

    static inline void MTX_RotY33(MtxFx33 *mtx, fx32 sin, fx32 cos) {
        // Stub
        MTX_Identity33(mtx);
        (void)sin; (void)cos;
    }

    static inline void MTX_RotZ33(MtxFx33 *mtx, fx32 sin, fx32 cos) {
        // Stub
        MTX_Identity33(mtx);
        (void)sin; (void)cos;
    }

    static inline void MTX_Concat33(const MtxFx33 *a, const MtxFx33 *b, MtxFx33 *ab) {
        // Stub
        *ab = *a;
        (void)b;
    }

    // GX Screen Sizes
    #define GX_BG_SCRSIZE_TEXT_256x256 0
    #define GX_BG_SCRSIZE_TEXT_512x256 1
    #define GX_BG_SCRSIZE_TEXT_256x512 2
    #define GX_BG_SCRSIZE_TEXT_512x512 3

    #define GX_BG_SCRSIZE_AFFINE_128x128 0
    #define GX_BG_SCRSIZE_AFFINE_256x256 1
    #define GX_BG_SCRSIZE_AFFINE_512x512 2
    #define GX_BG_SCRSIZE_AFFINE_1024x1024 3

    #define GX_BG_SCRSIZE_256x16PLTT_128x128 0
    #define GX_BG_SCRSIZE_256x16PLTT_256x256 1
    #define GX_BG_SCRSIZE_256x16PLTT_512x512 2
    #define GX_BG_SCRSIZE_256x16PLTT_1024x1024 3

    // GX Screen Bases (offsets in 2KB units)
    #define GX_BG_SCRBASE_0x0000 0
    #define GX_BG_SCRBASE_0x0800 1
    #define GX_BG_SCRBASE_0x1000 2
    #define GX_BG_SCRBASE_0x1800 3
    #define GX_BG_SCRBASE_0x2000 4
    #define GX_BG_SCRBASE_0x2800 5
    #define GX_BG_SCRBASE_0x3000 6
    #define GX_BG_SCRBASE_0x3800 7
    #define GX_BG_SCRBASE_0x4000 8
    #define GX_BG_SCRBASE_0x4800 9
    #define GX_BG_SCRBASE_0x5000 10
    #define GX_BG_SCRBASE_0x5800 11
    #define GX_BG_SCRBASE_0x6000 12
    #define GX_BG_SCRBASE_0x6800 13
    #define GX_BG_SCRBASE_0x7000 14
    #define GX_BG_SCRBASE_0x7800 15
    #define GX_BG_SCRBASE_0x8000 16
    #define GX_BG_SCRBASE_0x8800 17
    #define GX_BG_SCRBASE_0x9000 18
    #define GX_BG_SCRBASE_0x9800 19
    #define GX_BG_SCRBASE_0xa000 20
    #define GX_BG_SCRBASE_0xa800 21
    #define GX_BG_SCRBASE_0xb000 22
    #define GX_BG_SCRBASE_0xb800 23
    #define GX_BG_SCRBASE_0xc000 24
    #define GX_BG_SCRBASE_0xc800 25
    #define GX_BG_SCRBASE_0xd000 26
    #define GX_BG_SCRBASE_0xd800 27
    #define GX_BG_SCRBASE_0xe000 28
    #define GX_BG_SCRBASE_0xe800 29
    #define GX_BG_SCRBASE_0xf000 30
    #define GX_BG_SCRBASE_0xf800 31

    // CTRDG Stubs
    static inline u32 CTRDG_GetAgbGameCode(void) { return 0; }
    static inline void CTRDG_Enable(BOOL enable) { (void)enable; }
    static inline void CTRDG_CpuCopy8(const void *src, void *dest, u32 size) { (void)src; (void)dest; (void)size; }
    static inline void CTRDG_Init(void) {}
    static inline BOOL CTRDG_IsAgbCartridge(void) { return FALSE; }
    static inline u16 CTRDG_GetAgbMakerCode(void) { return 0; }
    static inline void CTRDG_Read32(const void *src, u32 *dest) { *dest = 0; }
    static inline BOOL CTRDG_CpuCopy16(const void *src, void *dest, u32 size) { (void)src; (void)dest; (void)size; return FALSE; }
    static inline BOOL CTRDG_IsExisting(void) { return FALSE; }
    static inline void CTRDG_ReadAgbFlash(u32 sec, u32 offset, void *dest, u32 size) { (void)sec; (void)offset; (void)dest; (void)size; }
    static inline BOOL CTRDG_IsPulledOut(void) { return FALSE; }
    static inline void CTRDG_TerminateForPulledOut(void) {
    }

    // CRYPTO Stubs
    static inline void CRYPTO_SetAllocator(void *alloc, void *free) { (void)alloc; (void)free; }
    static inline BOOL CRYPTO_VerifySignature(const void *data, u32 size, const void *sig, const void *key) { (void)data; (void)size; (void)sig; (void)key; return TRUE; }

    // MATH/CRYPTO Stubs
    typedef struct {
        u16 table[256];
    } MATHCRC16Table;

    typedef struct {
        u8 state[256];
        u8 x, y;
    } CRYPTORC4Context;

    static inline void MATH_CRC16InitTable(MATHCRC16Table *table) {
        (void)table;
    }

    static inline u16 MATH_CalcCRC16(const MATHCRC16Table *table, const void *data, u32 dataLength) {
        (void)table; (void)data; (void)dataLength;
        return 0;
    }

    static inline void CRYPTO_RC4Init(CRYPTORC4Context *context, const void *key, u32 keyLength) {
        (void)context; (void)key; (void)keyLength;
    }

    // FX Macros
    #define FX_F32_TO_FX32(x) ((fx32)((x) * FX32_ONE))

    // Input Macros
    #define PAD_KEY_UP      PAL_BUTTON_UP
    #define PAD_KEY_DOWN    PAL_BUTTON_DOWN
    #define PAD_KEY_LEFT    PAL_BUTTON_LEFT
    #define PAD_KEY_RIGHT   PAL_BUTTON_RIGHT
    #define PAD_BUTTON_A    PAL_BUTTON_A
    #define PAD_BUTTON_B    PAL_BUTTON_B
    #define PAD_BUTTON_SELECT PAL_BUTTON_SELECT
    #define PAD_BUTTON_START  PAL_BUTTON_START
    #define PAD_BUTTON_X    PAL_BUTTON_X
    #define PAD_BUTTON_Y    PAL_BUTTON_Y
    #define PAD_BUTTON_L    PAL_BUTTON_L
    #define PAD_BUTTON_R    PAL_BUTTON_R
#endif // PLATFORM_TYPES_H

// Common macros (both platforms)
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#define PAD_BUTTON_A 0x0001
#define PAD_BUTTON_B 0x0002
#define PAD_BUTTON_SELECT 0x0004
#define PAD_BUTTON_START 0x0008
#define PAD_BUTTON_RIGHT 0x0010
#define PAD_BUTTON_LEFT 0x0020
#define PAD_BUTTON_UP 0x0040
#define PAD_BUTTON_DOWN 0x0080
#define PAD_BUTTON_R 0x0100
#define PAD_BUTTON_L 0x0200
#define PAD_BUTTON_X 0x0400
#define PAD_BUTTON_Y 0x0800

#endif // PLATFORM_TYPES_H
