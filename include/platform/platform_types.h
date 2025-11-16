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
    
    // Graphics hardware functions (DS specific, stub for SDL)
    static inline void G2_SetBG0Priority(int priority) {
        // Stub: DS sets background layer 0 priority
        (void)priority;
    }
    
    static inline void* G2_GetOBJCharPtr(void) {
        // Stub: DS returns pointer to OBJ (sprite) character VRAM
        // SDL doesn't have VRAM - return dummy pointer
        static char dummy_vram[128 * 1024]; // 128KB sprite VRAM
        return dummy_vram;
    }
    
    // Palette loading functions
    static inline void GX_LoadBGPltt(const void* src, u32 offset, u32 size) {
        // Stub: DS loads palette to background palette RAM
        (void)src; (void)offset; (void)size;
    }
    
    static inline void GXS_LoadBGPltt(const void* src, u32 offset, u32 size) {
        // Stub: DS loads palette to sub-screen background palette RAM
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
    
    // FS_EXTERN_OVERLAY: DS overlay system (dynamic code loading)
    // SDL doesn't use overlays - all code linked statically
    #define FS_EXTERN_OVERLAY(name) /* stub */
    #define FS_OVERLAY_ID(name) 0
#else
    // DS platform has these defined in SDK
    #ifndef GF_ASSERT
        #define GF_ASSERT(condition) ((void)0)
    #endif
    #ifndef NELEMS
        #define NELEMS(array) (sizeof(array) / sizeof((array)[0]))
    #endif
    #ifndef XtOffset
        #define XtOffset(type, field) ((size_t)&(((type)0)->field))
    #endif
#endif

// Global game version and language variables (DS version in pm_version.c)
#ifdef PLATFORM_SDL
extern const u8 gGameLanguage;
extern const u8 gGameVersion;
#endif

#endif // PLATFORM_TYPES_H
