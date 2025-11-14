#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

/**
 * @file platform_config.h
 * @brief Platform configuration and conditional compilation definitions
 * 
 * This file defines the platform abstraction layer (PAL) configuration.
 * Use PLATFORM_DS for Nintendo DS builds and PLATFORM_SDL for SDL3 builds.
 */

// Determine target platform based on build configuration
#ifdef TARGET_NDS
    #define PLATFORM_DS
#elif defined(TARGET_SDL)
    #ifndef PLATFORM_SDL
        #define PLATFORM_SDL
    #endif
#else
    // Default to DS if not specified
    #define PLATFORM_DS
#endif

// Platform-specific includes
#ifdef PLATFORM_DS
    #include <nitro.h>
    #include <nnsys.h>
#else
    #include <SDL3/SDL.h>
    #include <stdbool.h>
    #include <stdint.h>
#endif

// Platform identification macros
#ifdef PLATFORM_DS
    #define PAL_PLATFORM_NAME "Nintendo DS"
    #define PAL_PLATFORM_DS 1
    #define PAL_PLATFORM_SDL 0
#else
    #define PAL_PLATFORM_NAME "SDL3"
    #define PAL_PLATFORM_DS 0
    #define PAL_PLATFORM_SDL 1
#endif

// Screen dimensions (constant for both platforms)
#define PAL_SCREEN_WIDTH  256
#define PAL_SCREEN_HEIGHT 192

// Platform capabilities
#ifdef PLATFORM_SDL
    #define PAL_HAS_FILESYSTEM 1
    #define PAL_HAS_MODERN_AUDIO 1
    #define PAL_HAS_CONFIGURABLE_INPUT 1
#else
    #define PAL_HAS_FILESYSTEM 0
    #define PAL_HAS_MODERN_AUDIO 0
    #define PAL_HAS_CONFIGURABLE_INPUT 0
#endif

#endif // PLATFORM_CONFIG_H
