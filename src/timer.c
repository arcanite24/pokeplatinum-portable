#include "timer.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

static void UpdateTimer(void);
static u64 CalculateCurrentTimestamp(void);

static vu64 sTimerValue = 0;
static BOOL sShouldResetTimer = FALSE;

void Timer_Start(void)
{
    sTimerValue = 0;
    sShouldResetTimer = FALSE;

    OS_SetTimerControl(OS_TIMER_3, 0);
    OS_SetTimerCount(OS_TIMER_3, 0);
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_SetTimerControl to PAL
    #endif
    OS_SetTimerControl(OS_TIMER_3, (u16)((u16)(REG_OS_TM3CNT_H_E_MASK | REG_OS_TM3CNT_H_I_MASK | OS_TIMER_PRESCALER_64)));
    #else
    // TODO: Port OS_TIMER_PRESCALER_64 to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_SetIrqFunction to PAL
    #endif
    OS_SetIrqFunction(OS_IE_TIMER3, UpdateTimer);
    #else
    // TODO: Port OS_IE_TIMER3 to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_EnableIrqMask to PAL
    #endif
    OS_EnableIrqMask(OS_IE_TIMER3);
    #else
    // TODO: Port OS_IE_TIMER3 to PAL
    #endif
}

static void UpdateTimer(void)
{
    sTimerValue++;

    if (sShouldResetTimer) {
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port OS_SetTimerControl to PAL
        #endif
        OS_SetTimerControl(OS_TIMER_3, 0);
        #else
        // TODO: Port OS_TIMER_3 to PAL
        #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port OS_SetTimerCount to PAL
        #endif
        OS_SetTimerCount(OS_TIMER_3, 0);
        #else
        // TODO: Port OS_TIMER_3 to PAL
        #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port OS_SetTimerControl to PAL
        #endif
        OS_SetTimerControl(OS_TIMER_3, (u16)((u16)(REG_OS_TM3CNT_H_E_MASK | REG_OS_TM3CNT_H_I_MASK | OS_TIMER_PRESCALER_64)));
        #else
        // TODO: Port OS_TIMER_PRESCALER_64 to PAL
        #endif

        sShouldResetTimer = FALSE;
    }

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_SetIrqCheckFlag to PAL
    #endif
    OS_SetIrqCheckFlag(OS_IE_TIMER3);
    #else
    // TODO: Port OS_IE_TIMER3 to PAL
    #endif
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port OS_SetIrqFunction to PAL
    #endif
    OS_SetIrqFunction(OS_IE_TIMER3, UpdateTimer);
    #else
    // TODO: Port OS_IE_TIMER3 to PAL
    #endif
}

static u64 CalculateCurrentTimestamp(void)
{
    vu16 timerLow;
    vu64 timerHigh;
    #ifdef PLATFORM_DS
    OSIntrMode irqState = OS_DisableInterrupts();
    #else
    // TODO: Port OS_DisableInterrupts to PAL
    #endif

    #ifdef PLATFORM_DS
    timerLow = *(REGType16 *)((u32)REG_TM0CNT_L_ADDR + (OS_TIMER_3) * 4);
    #else
    // TODO: Port OS_TIMER_3 to PAL
    #endif
    timerHigh = sTimerValue & 0xffffffffffffULL;

    #ifdef PLATFORM_DS
    if (reg_OS_IF & (OS_IE_TIMER3) && !(timerLow & 0x8000)) {
    #else
    // TODO: Port OS_IE_TIMER3 to PAL
    #endif
        timerHigh++;
    }

    #ifdef PLATFORM_DS
    OS_RestoreInterrupts(irqState);
    #else
    // TODO: Port OS_RestoreInterrupts to PAL
    #endif

    return (timerHigh << 16) | timerLow;
}

u64 Timer_GetCurrentTimestamp(void)
{
    return CalculateCurrentTimestamp();
}

u64 Timer_TicksToMilliSeconds(u64 ticks)
{
    #ifdef PLATFORM_DS
    return OS_TicksToMilliSeconds(ticks);
    #else
    // TODO: Port OS_TicksToMilliSeconds to PAL
    #endif
}

u64 Timer_TicksToSeconds(u64 ticks)
{
    #ifdef PLATFORM_DS
    return OS_TicksToSeconds(ticks);
    #else
    // TODO: Port OS_TicksToSeconds to PAL
    #endif
}
