#include "unk_0208C098.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "bg_window.h"
#include "heap.h"
#include "narc.h"
#include "screen_fade.h"
#include "system.h"

__attribute__((aligned(4))) static const u16 Unk_020F4030[] = {
    0x20,
    0x80,
    0x200,
    0x800,
    0x40,
    0x80,
    0x100,
    0x400,
    0x40,
    0x80,
    0x100,
    0x400
};

int sub_0208C098(int param0)
{
    param0 -= 0;
    return Unk_020F4030[param0];
}

u32 sub_0208C0A4(u32 param0, u32 param1)
{
    u32 v0 = (param0 * param0) + (param1 * param1);
    v0 = SVC_Sqrt(v0 << 4);

    return v0 >> 2;
}

u8 App_PixelCount(u32 cur, u32 max, u8 maxPixels)
{
    u8 pixels = cur * maxPixels / max;
    if (pixels == 0 && cur > 0) {
        pixels = 1;
    }

    return pixels;
}

u8 App_BarColor(u32 cur, u32 max)
{
    cur <<= 8;
    max <<= 8;

    if (cur > max / 2) {
        return BARCOLOR_GREEN;
    }

    if (cur > max / 5) {
        return BARCOLOR_YELLOW;
    }

    if (cur > 0) {
        return BARCOLOR_RED;
    }

    return BARCOLOR_EMPTY;
}

u8 HealthBar_Color(u16 curHP, u16 maxHP, u32 barSize)
{
    if (curHP == maxHP) {
        return BARCOLOR_MAX;
    }

    return App_BarColor(App_PixelCount(curHP, maxHP, barSize), barSize);
}

void App_StartScreenFade(u8 fadeOut, u32 heapID)
{
    if (fadeOut == FALSE) {
        StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_IN, FADE_TYPE_BRIGHTNESS_IN, COLOR_BLACK, 6, 1, heapID);
    } else {
        StartScreenFade(FADE_BOTH_SCREENS, FADE_TYPE_BRIGHTNESS_OUT, FADE_TYPE_BRIGHTNESS_OUT, COLOR_BLACK, 6, 1, heapID);
    }
}

u8 sub_0208C15C(s16 *param0, u16 param1)
{
    s16 v0 = *param0;

    #ifdef PLATFORM_DS
    if (gSystem.pressedKeysRepeatable & PAD_KEY_UP) {
    #else
    // TODO: Port PAD_KEY_UP to PAL
    #endif
        *param0 += 1;

        if (*param0 > param1) {
            *param0 = 1;
        }

        if (*param0 == v0) {
            return 0;
        }

        return 1;
    }

    #ifdef PLATFORM_DS
    if (gSystem.pressedKeysRepeatable & PAD_KEY_DOWN) {
    #else
    // TODO: Port PAD_KEY_DOWN to PAL
    #endif
        *param0 -= 1;

        if (*param0 <= 0) {
            *param0 = param1;
        }

        if (*param0 == v0) {
            return 0;
        }

        return 2;
    }

    #ifdef PLATFORM_DS
    if (gSystem.pressedKeysRepeatable & PAD_KEY_LEFT) {
    #else
    // TODO: Port PAD_KEY_LEFT to PAL
    #endif
        *param0 -= 10;

        if (*param0 <= 0) {
            *param0 = 1;
        }

        if (*param0 == v0) {
            return 0;
        }

        return 2;
    }

    #ifdef PLATFORM_DS
    if (gSystem.pressedKeysRepeatable & PAD_KEY_RIGHT) {
    #else
    // TODO: Port PAD_KEY_RIGHT to PAL
    #endif
        *param0 += 10;

        if (*param0 > param1) {
            *param0 = param1;
        }

        if (*param0 == v0) {
            return 0;
        }

        return 1;
    }

    return 0;
}

void App_LoadGraphicMember(BgConfig *bgConfig, int heapID, NARC *narc, int unused, int memberIndex, int bgLayer, enum GraphicMemberType memberType, u16 memberSize, u16 offset)
{
    u32 narcSize;
    void *dest;
    NNSG2dCharacterData *ppCharData;
    NNSG2dScreenData *ppScrData;
    NNSG2dPaletteData *ppPltData;

    narcSize = NARC_GetMemberSize(narc, memberIndex);
    dest = Heap_AllocAtEnd(heapID, narcSize);

    NARC_ReadWholeMember(narc, memberIndex, (void *)dest);

    switch (memberType) {
    case GRAPHICSMEMBER_TILES:
        #ifdef PLATFORM_DS
        NNS_G2dGetUnpackedCharacterData(dest, &ppCharData);
        #else
        // TODO: Port NNS_G2dGetUnpackedCharacterData to PAL
        #endif

        if (memberSize == 0) {
            memberSize = ppCharData->szByte;
        }

        Bg_LoadTiles(bgConfig, bgLayer, ppCharData->pRawData, memberSize, offset);
        break;
    case GRAPHICSMEMBER_TILEMAP:
        #ifdef PLATFORM_DS
        NNS_G2dGetUnpackedScreenData(dest, &ppScrData);
        #else
        // TODO: Port NNS_G2dGetUnpackedScreenData to PAL
        #endif

        if (memberSize == 0) {
            memberSize = ppScrData->szByte;
        }

        if (Bg_GetTilemapBuffer(bgConfig, bgLayer) != NULL) {
            Bg_LoadTilemapBuffer(bgConfig, bgLayer, ppScrData->rawData, memberSize);
        }

        Bg_CopyTilemapBufferRangeToVRAM(bgConfig, bgLayer, ppScrData->rawData, memberSize, offset);
        break;
    case GRAPHICSMEMBER_PALETTE:
        #ifdef PLATFORM_DS
        NNS_G2dGetUnpackedPaletteData(dest, &ppPltData);
        #else
        // TODO: Port NNS_G2dGetUnpackedPaletteData to PAL
        #endif

        if (memberSize == 0) {
            memberSize = ppPltData->szByte;
        }

        Bg_LoadPalette(bgLayer, ppPltData->pRawData, memberSize, offset);
    }

    Heap_Free(dest);
}

void *App_LoadScreenData(NARC *narc, enum NarcID unused, int memberIdx, NNSG2dScreenData **dst, enum HeapID heapID)
{
    int size = NARC_GetMemberSize(narc, memberIdx);
    void *file = Heap_Alloc(heapID, size);

    NARC_ReadWholeMember(narc, memberIdx, file);
    #ifdef PLATFORM_DS
    NNS_G2dGetUnpackedScreenData(file, dst);
    #else
    // TODO: Port NNS_G2dGetUnpackedScreenData to PAL
    #endif

    return file;
}
