#include "overlay011/particle_helper.h"

#include "camera.h"
#include "heap.h"
#include "particle_system.h"
#include "unk_0202419C.h"

static u32 ParticleHelper_AllocTexVram(u32 param0, BOOL param1);
static u32 ParticleHelper_AllocPalVram(u32 param0, BOOL param1);

ParticleSystem *ParticleHelper_CreateParticleSystem(enum HeapID heapID)
{
    void *heap = Heap_Alloc(heapID, 0x4800);
    ParticleSystem *ps = ParticleSystem_New(ParticleHelper_AllocTexVram, ParticleHelper_AllocPalVram, heap, 0x4800, TRUE, heapID);
    Camera *camera = ParticleSystem_GetCamera(ps);

    if (camera != NULL) {
        Camera_SetClipping(FX32_ONE, FX32_CONST(900), camera);
    }

    return ps;
}

ParticleSystem *ov11_0221F888(enum HeapID heapID)
{
    void *heap = Heap_Alloc(heapID, 0x4200);
    if (heap == NULL) {
        return NULL;
    }

    ParticleSystem *ps = ParticleSystem_New(ParticleHelper_AllocTexVram, ParticleHelper_AllocPalVram, heap, 0x4200, TRUE, heapID);
    Camera *camera = ParticleSystem_GetCamera(ps);

    if (camera != NULL) {
        Camera_SetClipping(FX32_ONE, FX32_CONST(900), camera);
    }

    return ps;
}

void ParticleHelper_FreeParticleSystem(ParticleSystem *ps)
{
    void *heap = ParticleSystem_GetHeapStart(ps);

    ParticleSystem_Free(ps);
    Heap_Free(heap);
}

void ParticleHelper_DrawParticleSystems(void)
{
    G3_ResetG3X();

    if (ParticleSystem_GetActiveAmount() == 0) {
        return;
    }

    int drawn = ParticleSystem_DrawAll();
    if (drawn > 0) {
        G3_ResetG3X();
        #ifdef PLATFORM_DS
        NNS_G2dSetupSoftwareSpriteCamera();
        #else
        // TODO: Port NNS_G2dSetupSoftwareSpriteCamera to PAL
        #endif
    }

    ParticleSystem_UpdateAll();
}

static u32 ParticleHelper_AllocTexVram(u32 size, BOOL is4x4comp)
{
    #ifdef PLATFORM_DS
    NNSGfdTexKey texKey = NNS_GfdAllocTexVram(size, is4x4comp, 0);
    #else
    // TODO: Port NNS_GfdAllocTexVram to PAL
    #endif
    ParticleSystem_RegisterTextureKey(texKey);
    #ifdef PLATFORM_DS
    return NNS_GfdGetTexKeyAddr(texKey);
    #else
    // TODO: Port NNS_GfdGetTexKeyAddr to PAL
    #endif
}

static u32 ParticleHelper_AllocPalVram(u32 size, BOOL is4pal)
{
    #ifdef PLATFORM_DS
    NNSGfdPlttKey palKey = NNS_GfdAllocPlttVram(size, is4pal, 0);
    #else
    // TODO: Port NNS_GfdAllocPlttVram to PAL
    #endif
    ParticleSystem_RegisterPaletteKey(palKey);
    #ifdef PLATFORM_DS
    return NNS_GfdGetPlttKeyAddr(palKey);
    #else
    // TODO: Port NNS_GfdGetPlttKeyAddr to PAL
    #endif
}
