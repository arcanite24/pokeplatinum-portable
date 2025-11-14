#ifndef PAL_TIMER_H
#define PAL_TIMER_H

/**
 * @file pal_timer.h
 * @brief Platform Abstraction Layer - Timing API
 * 
 * Provides unified timing interface for both Nintendo DS and SDL3 platforms.
 */

#include "platform_config.h"
#include "platform_types.h"

/**
 * Initialize the timer system
 * @return TRUE on success, FALSE on failure
 */
BOOL PAL_Timer_Init(void);

/**
 * Shutdown the timer system
 */
void PAL_Timer_Shutdown(void);

/**
 * Get current time in milliseconds
 * @return Time in milliseconds
 */
u64 PAL_Timer_GetTicks(void);

/**
 * Get performance counter value (high precision)
 * @return Counter value
 */
u64 PAL_Timer_GetPerformanceCounter(void);

/**
 * Get performance counter frequency
 * @return Frequency in Hz
 */
u64 PAL_Timer_GetPerformanceFrequency(void);

/**
 * Sleep/delay for specified milliseconds
 * @param ms Milliseconds to sleep
 */
void PAL_Timer_Delay(u32 ms);

#endif // PAL_TIMER_H
