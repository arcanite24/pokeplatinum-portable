#include "unk_02099550.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "overlay018/ov18_0221F800.h"

#include "game_overlay.h"
#include "heap.h"
#include "main.h"
#include "overlay_manager.h"
#include "savedata.h"

FS_EXTERN_OVERLAY(overlay4);
FS_EXTERN_OVERLAY(overlay18);
FS_EXTERN_OVERLAY(overlay60);
FS_EXTERN_OVERLAY(overlay89);

void sub_02099550(void)
{
    #ifdef PLATFORM_DS
    Overlay_LoadByID(FS_OVERLAY_ID(overlay4), 2);
    #else
    // TODO: Port FS_OVERLAY_ID to PAL
    #endif
}

void sub_02099560(void)
{
    #ifdef PLATFORM_DS
    Overlay_UnloadByID(FS_OVERLAY_ID(overlay4));
    #else
    // TODO: Port FS_OVERLAY_ID to PAL
    #endif
}

void sub_02099570(void)
{
    #ifdef PLATFORM_DS
    Overlay_LoadByID(FS_OVERLAY_ID(overlay18), 2);
    #else
    // TODO: Port FS_OVERLAY_ID to PAL
    #endif
}

void sub_02099580(void)
{
    #ifdef PLATFORM_DS
    Overlay_UnloadByID(FS_OVERLAY_ID(overlay18));
    #else
    // TODO: Port FS_OVERLAY_ID to PAL
    #endif
}

static void sub_02099590(SaveData *saveData, int heapID)
{
    sub_02099550();
    sub_02099570();

    ov18_0221F800(heapID);

    sub_02099580();
    sub_02099560();

    #ifdef PLATFORM_DS
    OS_ResetSystem(0);
    #else
    // TODO: Port OS_ResetSystem to PAL
    #endif
}

void Overlay_LoadHttpOverlay(void)
{
    #ifdef PLATFORM_DS
    Overlay_LoadByID(FS_OVERLAY_ID(overlay60), 2);
    #else
    // TODO: Port FS_OVERLAY_ID to PAL
    #endif
}

void Overlay_UnloadHttpOverlay(void)
{
    #ifdef PLATFORM_DS
    Overlay_UnloadByID(FS_OVERLAY_ID(overlay60));
    #else
    // TODO: Port FS_OVERLAY_ID to PAL
    #endif
}

static int sub_020995D4(ApplicationManager *appMan, int *param1)
{
    Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_49, 0x41000);
    sub_02099590(((ApplicationArgs *)ApplicationManager_Args(appMan))->saveData, HEAP_ID_49);
    Heap_Destroy(HEAP_ID_49);

    #ifdef PLATFORM_DS
    OS_ResetSystem(0);
    #else
    // TODO: Port OS_ResetSystem to PAL
    #endif
    return 1;
}

const ApplicationManagerTemplate gRebootIntoWFCSettingsAppTemplate = {
    sub_020995D4,
    NULL,
    NULL,
    0xffffffff,
};
