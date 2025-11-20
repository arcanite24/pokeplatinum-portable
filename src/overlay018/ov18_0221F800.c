#include "overlay018/ov18_0221F800.h"

#include <dwc.h>
#include <nitro.h>
#include <string.h>

#include "heap.h"
#include "unk_02033200.h"

void ov18_0221F800(int heapID)
{
    OSIntrMode v0;
    void *v1;

    WirelessDriver_Init();

    #ifdef PLATFORM_DS
    OS_InitTick();
    #else
    // TODO: Port OS_InitTick to PAL
    #endif
    #ifdef PLATFORM_DS
    OS_InitAlarm();
    #else
    // TODO: Port OS_InitAlarm to PAL
    #endif

    #ifdef PLATFORM_DS
    v0 = OS_DisableInterrupts();
    #else
    // TODO: Port OS_DisableInterrupts to PAL
    #endif
    DWC_SetAuthServer(DWC_CONNECTINET_AUTH_RELEASE);
    v1 = Heap_Alloc(heapID, DWC_UTILITY_WORK_SIZE);

    (void)DWC_StartUtility(v1, DWC_LANGUAGE_ENGLISH, DWC_UTILITY_TOP_MENU_COMMON);

    Heap_Free(v1);

    #ifdef PLATFORM_DS
    OS_RestoreInterrupts(v0);
    #else
    // TODO: Port OS_RestoreInterrupts to PAL
    #endif
    #ifdef PLATFORM_DS
    OS_EnableIrq();
    #else
    // TODO: Port OS_EnableIrq to PAL
    #endif
}
