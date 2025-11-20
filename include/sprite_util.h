#ifndef POKEPLATINUM_SPRITE_UTIL_H
#define POKEPLATINUM_SPRITE_UTIL_H

#include "platform/platform_types.h"

#ifdef PLATFORM_DS
#include <nitro/gx.h>
#else
#include "nns_types.h"
#endif

#include "sprite.h"
#include "sprite_resource.h"

#define RESOURCE_NONE -1

typedef struct ResdatTableEntry {
    u32 charResourceID;
    u32 plttResourceID;
    u32 cellResourceID;
    u32 animResourceID;
    u32 mcellResourceID;
    u32 manimResourceID;
    u32 vramTransfer;
    u32 priority;
} ResdatTableEntry;

typedef struct G2dRenderer {
    NNSG2dRendererInstance renderer;
    NNSG2dRenderSurface mainScreen;
    NNSG2dRenderSurface subScreen;
} G2dRenderer;

void SpriteResourcesHeader_Init(SpriteResourcesHeader *resourceHeader,
    int charResourceID,
    int plttResourceID,
    int cellResourceID,
    int animResourceID,
    int mcellResourceID,
    int manimResourceID,
    BOOL vramTransfer,
    int priority,
    SpriteResourceCollection *charResources,
    SpriteResourceCollection *plttResources,
    SpriteResourceCollection *cellResources,
    SpriteResourceCollection *animResources,
    SpriteResourceCollection *mcellResources,
    SpriteResourceCollection *manimResources);
void SpriteResourcesHeader_Clear(SpriteResourcesHeader *resourceHeader);

SpriteResourcesHeaderList *SpriteResourcesHeaderList_NewFromResdat(const ResdatTableEntry *resdatEntries,
    enum HeapID heapID,
    SpriteResourceCollection *charResources,
    SpriteResourceCollection *plttResources,
    SpriteResourceCollection *cellResources,
    SpriteResourceCollection *animResources,
    SpriteResourceCollection *mcellResources,
    SpriteResourceCollection *manimResources);
void SpriteResourcesHeaderList_Free(SpriteResourcesHeaderList *param0);

SpriteList *SpriteList_InitRendering(int maxElements, G2dRenderer *g2dRenderer, enum HeapID heapID);
void SetMainScreenViewRect(G2dRenderer *g2dRenderer, fx32 x, fx32 y);
void SetSubScreenViewRect(G2dRenderer *g2dRenderer, fx32 x, fx32 y);
#ifdef PLATFORM_DS
void ReserveVramForWirelessIconChars(NNS_G2D_VRAM_TYPE vramType, GXOBJVRamModeChar vramMode);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
#ifdef PLATFORM_DS
void ReserveSlotsForWirelessIconPalette(NNS_G2D_VRAM_TYPE vramType);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif

#endif // POKEPLATINUM_SPRITE_UTIL_H
