#include "g3d_pipeline.h"

#ifdef PLATFORM_DS
#include "nitro/hw/common/lcd.h"
#include <nitro.h>
#else
#include "platform/platform_types.h"
#include "nns_types.h"
#include "unk_0202419C.h"
#endif

#include "constants/graphics.h"

#include "gx_layers.h"
#include "heap.h"

#define TEXTURE_VRAM_BLOCK_SIZE 0x20000
#define PALETTE_VRAM_BLOCK_SIZE 0x2000

static void SetupDefaultPipelineState(void);

G3DPipelineBuffers *G3DPipeline_InitEx(enum HeapID heapID, enum VramManagerType texVramManagerType, enum TextureVramSize texVramSize, enum VramManagerType plttVramManagerType, enum PaletteVramSize plttVramSize, G3DPipelineSetupCallback cb)
{
    G3DPipelineBuffers *buffers = Heap_Alloc(heapID, sizeof(G3DPipelineBuffers));
    buffers->heapID = heapID;

    #ifdef PLATFORM_DS
    NNS_G3dInit();
    #else
    // TODO: Port NNS_G3dInit to PAL
    #endif

    G3X_InitMtxStack();
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    #else
    // TODO: Port GX_SORTMODE_AUTO to PAL
    #endif
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
    #else
    // TODO: Port GX_BUFFERMODE_W to PAL
    #endif

    if (texVramManagerType == VRAM_MANAGER_TYPE_LINKED_LIST) {
        #ifdef PLATFORM_DS
        int bufferSize = NNS_GfdGetLnkTexVramManagerWorkSize(128 * texVramSize);
        #else
        // TODO: Port NNS_GfdGetLnkTexVramManagerWorkSize to PAL
        #endif
        buffers->textureBuffer = Heap_Alloc(buffers->heapID, bufferSize);
        #ifdef PLATFORM_DS
        NNS_GfdInitLnkTexVramManager(texVramSize * TEXTURE_VRAM_BLOCK_SIZE, 0, buffers->textureBuffer, bufferSize, TRUE);
        #else
        // TODO: Port NNS_GfdInitLnkTexVramManager to PAL
        #endif
    } else {
        #ifdef PLATFORM_DS
        NNS_GfdInitFrmTexVramManager(texVramSize, TRUE);
        #else
        // TODO: Port NNS_GfdInitFrmTexVramManager to PAL
        #endif
    }

    if (plttVramManagerType == VRAM_MANAGER_TYPE_LINKED_LIST) {
        #ifdef PLATFORM_DS
        int bufferSize = NNS_GfdGetLnkPlttVramManagerWorkSize(256 * plttVramSize);
        #else
        // TODO: Port NNS_GfdGetLnkPlttVramManagerWorkSize to PAL
        #endif
        buffers->paletteBuffer = Heap_Alloc(buffers->heapID, bufferSize);
        #ifdef PLATFORM_DS
        NNS_GfdInitLnkPlttVramManager(plttVramSize * PALETTE_VRAM_BLOCK_SIZE, buffers->paletteBuffer, bufferSize, TRUE);
        #else
        // TODO: Port NNS_GfdInitLnkPlttVramManager to PAL
        #endif
    } else {
        // BUG: This should be NNS_GfdInitFrmPlttVramManager
        #ifdef PLATFORM_DS
        NNS_GfdInitFrmTexVramManager(plttVramSize * PALETTE_VRAM_BLOCK_SIZE, TRUE);
        #else
        // TODO: Port NNS_GfdInitFrmTexVramManager to PAL
        #endif
    }

    if (cb != NULL) {
        cb();
    } else {
        SetupDefaultPipelineState();
    }

    return buffers;
}

void G3DPipelineBuffers_Free(G3DPipelineBuffers *state)
{
    Heap_Free(state->paletteBuffer);
    Heap_Free(state->textureBuffer);
    Heap_Free(state);
}

static void SetupDefaultPipelineState(void)
{
    #ifdef PLATFORM_DS
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_BG0, TRUE);
    #else
    // TODO: Port GX_PLANEMASK_BG0 to PAL
    #endif

    #ifdef PLATFORM_DS
    G2_SetBG0Priority(0);
    #else
    // TODO: Port G2_SetBG0Priority to PAL
    #endif
    #ifdef PLATFORM_DS
    G3X_SetShading(GX_SHADING_TOON);
    #else
    // TODO: Port GX_SHADING_TOON to PAL
    #endif
    G3X_AntiAlias(FALSE);
    G3X_AlphaTest(FALSE, 0);
    G3X_AlphaBlend(FALSE);
    G3X_EdgeMarking(FALSE);
    #ifdef PLATFORM_DS
    G3X_SetFog(FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0);
    #else
    // TODO: Port GX_FOGBLEND_COLOR_ALPHA to PAL
    #endif
    G3X_SetClearColor(COLOR_BLACK, 0, G3X_DEPTH_MAX, 63, FALSE);
    G3_ViewPort(0, 0, HW_LCD_WIDTH - 1, HW_LCD_HEIGHT - 1);
}
