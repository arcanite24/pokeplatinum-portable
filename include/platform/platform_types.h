#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

/**
 * @file platform_types.h
 * @brief Platform-agnostic type definitions
 * 
 * This file provides unified type definitions that work across both
 * Nintendo DS and SDL3 platforms.
 */

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
    
    // Sine/cosine lookup table stubs (proper implementation would use lookup table)
    #include <math.h>
    static inline fx32 FX_SinIdx(u16 idx) {
        // DS uses 0-0xFFFF range for 0-2π
        double angle = (idx / 65536.0) * 2.0 * M_PI;
        return (fx32)(sin(angle) * FX32_ONE);
    }
    static inline fx32 FX_CosIdx(u16 idx) {
        double angle = (idx / 65536.0) * 2.0 * M_PI;
        return (fx32)(cos(angle) * FX32_ONE);
    }
    static inline fx32 FX_Sqrt(fx32 x) {
        if (x < 0) return 0;
        double val = (double)x / FX32_ONE;
        return (fx32)(sqrt(val) * FX32_ONE);
    }
    static inline u16 FX_Atan2Idx(fx32 y, fx32 x) {
        double fy = (double)y / FX32_ONE;
        double fx = (double)x / FX32_ONE;
        double angle = atan2(fy, fx);
        if (angle < 0) angle += 2.0 * M_PI;
        return (u16)(angle / (2.0 * M_PI) * 65536.0);
    }
    
    // Fixed-point constants
    #define FX32_CONST(x) ((fx32)((x) * FX32_ONE))
    #define FX32_HALF     (FX32_ONE / 2)
    
    // Vector types (matching NitroSDK conventions)
    typedef struct VecFx16 {
        fx16 x;
        fx16 y;
        fx16 z;
    } VecFx16;
    
    typedef struct VecFx32 {
        fx32 x;
        fx32 y;
        fx32 z;
    } VecFx32;
    
    typedef struct MtxFx33 {
        fx32 _00, _01, _02;
        fx32 _10, _11, _12;
        fx32 _20, _21, _22;
    } MtxFx33;
    
    typedef struct MtxFx43 {
        fx32 _00, _01, _02;
        fx32 _10, _11, _12;
        fx32 _20, _21, _22;
        fx32 _30, _31, _32;
    } MtxFx43;
    
    typedef struct MtxFx22 {
        fx32 _00, _01;
        fx32 _10, _11;
    } MtxFx22;
    
    // Vector utility functions
    static inline void VEC_Set(VecFx32* vec, fx32 x, fx32 y, fx32 z) {
        vec->x = x;
        vec->y = y;
        vec->z = z;
    }
    static inline void VEC_Fx16Set(VecFx16* vec, fx16 x, fx16 y, fx16 z) {
        vec->x = x;
        vec->y = y;
        vec->z = z;
    }
    
    // Matrix utility functions
    static inline void MTX_Identity33(MtxFx33* mtx) {
        // Set to identity matrix (1s on diagonal, 0s elsewhere)
        mtx->_00 = FX32_ONE; mtx->_01 = 0; mtx->_02 = 0;
        mtx->_10 = 0; mtx->_11 = FX32_ONE; mtx->_12 = 0;
        mtx->_20 = 0; mtx->_21 = 0; mtx->_22 = FX32_ONE;
    }
    
    // Math functions
    static inline void VEC_Normalize(const VecFx32 *src, VecFx32 *dst) {
        // Stub: Just copy for now, or implement normalization if needed
        *dst = *src;
    }

    static inline void MTX_RotX33(MtxFx33 *mtx, fx32 sin, fx32 cos) {
        // Stub
        (void)mtx; (void)sin; (void)cos;
    }

    static inline void MTX_RotY33(MtxFx33 *mtx, fx32 sin, fx32 cos) {
        // Stub
        (void)mtx; (void)sin; (void)cos;
    }

    static inline void MTX_RotZ33(MtxFx33 *mtx, fx32 sin, fx32 cos) {
        // Stub
        (void)mtx; (void)sin; (void)cos;
    }

    static inline void MTX_Concat33(const MtxFx33 *a, const MtxFx33 *b, MtxFx33 *ab) {
        // Stub
        (void)a; (void)b; (void)ab;
    }

    #define FX_RAD_TO_IDX(rad) ((int)((rad) * 65536.0 / (2 * 3.14159))) // Approximate
    
    // NitroSDK memory copy functions (SDL stubs)
    #include <string.h>
    static inline void MI_CpuClear8(void* dest, u32 size) {
        memset(dest, 0, size);
    }
    static inline void MI_CpuClear16(void* dest, u32 size) {
        memset(dest, 0, size);
    }
    static inline void MI_CpuClear32(void* dest, u32 size) {
        memset(dest, 0, size);
    }
    static inline void MI_CpuCopy8(const void* src, void* dest, u32 size) {
        memcpy(dest, src, size);
    }
    static inline void MI_CpuCopy16(const void* src, void* dest, u32 size) {
        memcpy(dest, src, size);
    }
    static inline void MI_CpuCopy32(const void* src, void* dest, u32 size) {
        memcpy(dest, src, size);
    }
    static inline void MI_CpuFill8(void* dest, u32 data, u32 size) {
        memset(dest, (int)(data & 0xFF), size);
    }
    static inline void MI_CpuFill16(void* dest, u16 data, u32 size) {
        // Fill memory with 16-bit values
        u16* ptr = (u16*)dest;
        size /= 2;  // Size is in bytes, convert to u16 count
        for (u32 i = 0; i < size; i++) {
            ptr[i] = data;
        }
    }
    static inline void MI_CpuFill32(void* dest, u32 data, u32 size) {
        // Fill memory with 32-bit values
        u32* ptr = (u32*)dest;
        size /= 4;  // Size is in bytes, convert to u32 count
        for (u32 i = 0; i < size; i++) {
            ptr[i] = data;
        }
    }
    
    // Cache management (no-op on SDL - no separate cache)
    static inline void DC_FlushRange(const void* addr, u32 size) {
        (void)addr; (void)size;
        // SDL: No cache flush needed
    }
    static inline void DC_StoreRange(const void* addr, u32 size) {
        (void)addr; (void)size;
    }
    static inline void DC_InvalidateRange(const void* addr, u32 size) {
        (void)addr; (void)size;
    }
    
    // Fast memory operations (same as regular versions on SDL)
    static inline void MI_CpuClearFast(void* dest, u32 size) {
        memset(dest, 0, size);
    }
    static inline void MI_CpuCopyFast(const void* src, void* dest, u32 size) {
        memcpy(dest, src, size);
    }
    
    // Math coprocessor functions (DS hardware, SDL uses software impl)
    #include <math.h>
    static inline void CP_SetSqrt32(u32 value) {
        // Stub: DS has hardware sqrt coprocessor, SDL uses software
        (void)value;
    }
    static inline void CP_WaitSqrt(void) {
        // Stub: DS waits for hardware, SDL is instant
    }
    static inline u32 CP_GetSqrtResult32(void) {
        // Stub: Should use cached sqrt value from CP_SetSqrt32
        // For now, return 0 (caller should cache the result properly)
        return 0;
    }
    
    // Graphics control structs (DS specific)
    typedef struct {
        int screenSize;
        int colorMode;
        int screenBase;
        int charBase;
        int bgExtPltt;
        int priority;
    } GXBg01Control;

    typedef struct {
        int screenSize;
        int colorMode;
        int screenBase;
        int charBase;
        int priority;
    } GXBg23ControlText;

    typedef struct {
        int screenSize;
        int areaOver;
        int screenBase;
        int charBase;
        int priority;
    } GXBg23ControlAffine;

    typedef struct {
        int screenSize;
        int areaOver;
        int screenBase;
        int charBase;
        int priority;
    } GXBg23Control256x16Pltt;

    // Missing constants for BG screen sizes
    #define GX_BG_SCRSIZE_TEXT_256x256 0
    #define GX_BG_SCRSIZE_TEXT_256x512 1
    #define GX_BG_SCRSIZE_TEXT_512x256 2
    #define GX_BG_SCRSIZE_TEXT_512x512 3

    #define GX_BG_SCRSIZE_AFFINE_128x128 0
    #define GX_BG_SCRSIZE_AFFINE_256x256 1
    #define GX_BG_SCRSIZE_AFFINE_512x512 2
    #define GX_BG_SCRSIZE_AFFINE_1024x1024 3

    #define GX_BG_SCRSIZE_256x16PLTT_128x128 0
    #define GX_BG_SCRSIZE_256x16PLTT_256x256 1
    #define GX_BG_SCRSIZE_256x16PLTT_512x512 2
    #define GX_BG_SCRSIZE_256x16PLTT_1024x1024 3

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
    
    static inline u32 NNS_GfdGetPlttKeyAddr(u32 key) {
        // Stub: Get palette VRAM address from key
        return key; // Just return the key itself (already an address)
    }
    
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
    
    // GX VRAM Constants
    #define GX_VRAM_OBJ_80_EF 0
    #define GX_VRAM_TEX_0_A 0
    #define GX_VRAM_TEXPLTT_0_G 0
    #define GX_VRAM_OBJ_32_FG 0
    #define GX_VRAM_TEXPLTT_0123_E 0
    #define GX_VRAM_BG_256_AB 0
    #define GX_VRAM_OBJ_NONE 0
    #define GX_VRAM_SUB_OBJ_NONE 0
    #define GX_VRAM_TEX_NONE 0
    #define GX_VRAM_TEXPLTT_NONE 0
    
    // GX BG Constants
    #define GX_BG0_AS_2D 0
    #define GX_BG_CHARBASE_0x18000 0x18000
    #define GX_BG_CHARBASE_0x14000 0x14000

    // GX Display Stubs
    static inline void GX_SetVisiblePlane(int plane) {}
    static inline void GXS_SetVisiblePlane(int plane) {}
    static inline void GX_DispOn(void) {}
    static inline void GXS_DispOn(void) {}
    static inline void GX_SetDispSelect(int select) {}

    #define GX_DISP_SELECT_MAIN_SUB 0
    #define GX_DISP_SELECT_SUB_MAIN 1
    
#endif // PLATFORM_SDL

#ifndef SDK_COMPILER_ASSERT
#define SDK_COMPILER_ASSERT(expr) extern char sdk_compiler_assert_failed[(expr) ? 1 : -1]
#endif

#endif // PLATFORM_TYPES_H
