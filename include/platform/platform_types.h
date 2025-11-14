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
    
    #ifndef NULL
        #define NULL ((void*)0)
    #endif
    
    // Fixed-point types (used extensively in DS code)
    typedef s32 fx32;
    typedef s16 fx16;
    
    // Fixed-point conversion macros
    #define FX32_SHIFT 12
    #define FX32_ONE   (1 << FX32_SHIFT)
    #define FX_Whole(x) ((x) >> FX32_SHIFT)
    #define FX_Frac(x)  ((x) & (FX32_ONE - 1))
    
    // Vector types (matching NitroSDK conventions)
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
    
#else
    // DS platform uses NitroSDK types (already defined in <nitro.h>)
    // No additional type definitions needed
#endif

// Platform-agnostic constants
#define PAL_INVALID_HANDLE NULL

#endif // PLATFORM_TYPES_H
