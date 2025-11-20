#ifndef DWC_STUBS_H
#define DWC_STUBS_H

#include "platform/platform_types.h"

#ifdef PLATFORM_SDL

// NitroDWC (Nintendo DS Wi-Fi Connection) stubs for SDL build
// These are placeholder types since we don't have network functionality

typedef struct {
    u32 dummy; // Placeholder
} DWCInetControl;

typedef enum {
    DWC_ERROR_NONE = 0,
    DWC_ERROR_NETWORK = 1,
    DWC_ERROR_FATAL = 2
} DWCErrorType;

#endif // PLATFORM_SDL

#endif // DWC_STUBS_H