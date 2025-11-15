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
    
    typedef float    f32;
    typedef double   f64;
    
    typedef int      BOOL;
    
    #ifndef TRUE
        #define TRUE  1
    #endif
    #ifndef FALSE
        #define FALSE 0
    #endif
    
    // Don't define NULL - let system headers define it
    // The DS codebase uses NULL for integer 0 in many places,
    // which triggers -Wint-conversion warnings on modern compilers.
    // We suppress these warnings in CMakeLists.txt instead.
    
    // Fixed-point types (used extensively in DS code)
    typedef s32 fx32;
    typedef s16 fx16;
    
    // Fixed-point conversion macros
    #define FX32_SHIFT 12
    #define FX32_ONE   (1 << FX32_SHIFT)
    #define FX_Whole(x) ((x) >> FX32_SHIFT)
    #define FX_Frac(x)  ((x) & (FX32_ONE - 1))
    
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

// Global game language variable (DS version in save.c)
#ifdef PLATFORM_SDL
extern u32 gGameLanguage;
#endif

#endif // PLATFORM_TYPES_H
