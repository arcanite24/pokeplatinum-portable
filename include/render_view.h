#ifndef POKEPLATINUM_RENDER_VIEW_H
#define POKEPLATINUM_RENDER_VIEW_H

#include "platform/platform_types.h"

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
#include "nns_types.h"  /* SDL: Common NNS type stubs */
#endif

void InitRenderer(NNSG2dRendererInstance *renderer, fx32 spriteZOffset);
void InitRenderSurface(NNSG2dRenderSurface *surface,
    NNSG2dViewRect *viewRect,
    NNSG2dOamRegisterFunction oamRegisterFunc,
    NNSG2dAffineRegisterFunction oamAffineRegisterFunc,
    NNSG2dRndCellCullingFunction visibilityCullingFunc,
    NNSG2dSurfaceType surfaceType,
    NNSG2dRendererInstance *renderer);
void SetRenderSurfaceViewRect(NNSG2dRenderSurface *surface, NNSG2dViewRect *viewRect);
BOOL IsObjectInView(const NNSG2dCellData *cellData, const MtxFx32 *objectCoords, const NNSG2dViewRect *viewRect);

#endif // POKEPLATINUM_RENDER_VIEW_H
