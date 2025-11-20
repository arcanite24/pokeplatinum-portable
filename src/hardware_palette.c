#include "hardware_palette.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
// Define dummy buffers for SDL
static u16 sMainBgPltt[256 * 16];
static u16 sSubBgPltt[256 * 16];
static u16 sMainObjPltt[256 * 16];
static u16 sSubObjPltt[256 * 16];

#define HW_BG_PLTT sMainBgPltt
#define HW_BG_PLTT_SIZE sizeof(sMainBgPltt)
#define HW_DB_BG_PLTT sSubBgPltt
#define HW_DB_BG_PLTT_SIZE sizeof(sSubBgPltt)
#define HW_OBJ_PLTT sMainObjPltt
#define HW_DB_OBJ_PLTT sSubObjPltt
#endif
#include <string.h>

u16 *GetHardwareMainBgPaletteAddress(void)
{
    return (u16 *)HW_BG_PLTT;
}

u32 GetHardwareMainBgPaletteSize(void)
{
    return HW_BG_PLTT_SIZE;
}

u16 *GetHardwareSubBgPaletteAddress(void)
{
    return (u16 *)HW_DB_BG_PLTT;
}

u32 GetHardwareSubBgPaletteSize(void)
{
    return HW_DB_BG_PLTT_SIZE;
}

u16 *GetHardwareMainObjPaletteAddress(void)
{
    return (u16 *)HW_OBJ_PLTT;
}

u16 *GetHardwareSubObjPaletteAddress(void)
{
    return (u16 *)HW_DB_OBJ_PLTT;
}
