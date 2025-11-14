/**
 * @file pal_timer_sdl.c
 * @brief SDL3 implementation of timer abstraction layer
 */

#include "platform/pal_timer.h"

#ifdef PLATFORM_SDL

#include <SDL3/SDL.h>

static BOOL g_timer_initialized = FALSE;

BOOL PAL_Timer_Init(void) {
    if (g_timer_initialized) {
        return TRUE;
    }
    
    g_timer_initialized = TRUE;
    return TRUE;
}

void PAL_Timer_Shutdown(void) {
    g_timer_initialized = FALSE;
}

u64 PAL_Timer_GetTicks(void) {
    return SDL_GetTicks();
}

u64 PAL_Timer_GetPerformanceCounter(void) {
    return SDL_GetPerformanceCounter();
}

u64 PAL_Timer_GetPerformanceFrequency(void) {
    return SDL_GetPerformanceFrequency();
}

void PAL_Timer_Delay(u32 ms) {
    SDL_Delay(ms);
}

#endif // PLATFORM_SDL
