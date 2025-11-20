#include "particle_system.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <nitro/fx/fx_trig.h>
#include <string.h>

#include "camera.h"
#include "fx_util.h"
#include "heap.h"
#include "narc.h"
#include "spl_behavior.h"
#include "spl_manager.h"
#include "sys_task.h"
#include "sys_task_manager.h"

#define FIXED_POLYGON_ID 5
#define MIN_POLYGON_ID   6
#define MAX_POLYGON_ID   63

enum SPLBehaviorType {
    SPL_BEHAVIOR_GRAVITY = 0,
    SPL_BEHAVIOR_RANDOM,
    SPL_BEHAVIOR_MAGNET,
    SPL_BEHAVIOR_SPIN,
    SPL_BEHAVIOR_COLLISION_PLANE,
    SPL_BEHAVIOR_CONVERGENCE,
};

static void *sParticleSystems[MAX_PARTICLE_SYSTEMS];

// Current callback parameter for the emitter creation callback.
static void *sEmitterCallbackParam;

// Particle system whose resources are currently being uploaded to VRAM.
// Used so custom Texture/Palette allocators can save the resources.
// See ParticleSystem_Register*Key
static ParticleSystem *sUploadingParticleSystem;

static const VecFx32 sParticleSystemDefaultCameraPos = VEC_FX32(0, 0, 4);
static const VecFx32 sParticleSystemDefaultCameraUp = VEC_FX32(0, 1, 0);
static const VecFx32 sParticleSystemDefaultCameraTarget = VEC_FX32(0, 0, 0);

static void *ParticleSystem00_AllocMemory(u32 size);
static void *ParticleSystem01_AllocMemory(u32 size);
static void *ParticleSystem02_AllocMemory(u32 size);
static void *ParticleSystem03_AllocMemory(u32 size);
static void *ParticleSystem04_AllocMemory(u32 size);
static void *ParticleSystem05_AllocMemory(u32 size);
static void *ParticleSystem06_AllocMemory(u32 size);
static void *ParticleSystem07_AllocMemory(u32 size);
static void *ParticleSystem08_AllocMemory(u32 size);
static void *ParticleSystem09_AllocMemory(u32 size);
static void *ParticleSystem10_AllocMemory(u32 size);
static void *ParticleSystem11_AllocMemory(u32 size);
static void *ParticleSystem12_AllocMemory(u32 size);
static void *ParticleSystem13_AllocMemory(u32 size);
static void *ParticleSystem14_AllocMemory(u32 size);
static void *ParticleSystem15_AllocMemory(u32 size);
static inline void *ParticleSystem_AllocMemory(ParticleSystem *particleSystem, u32 size);
static void ParticleSystem_UploadResourcesInternal(ParticleSystem *particleSystem);
static void ParticleSystem_VBlankResourceUploadInternal(SysTask *task, void *param);

static const SPLAllocFunc sParticleSystemAllocFuncs[] = {
    ParticleSystem00_AllocMemory,
    ParticleSystem01_AllocMemory,
    ParticleSystem02_AllocMemory,
    ParticleSystem03_AllocMemory,
    ParticleSystem04_AllocMemory,
    ParticleSystem05_AllocMemory,
    ParticleSystem06_AllocMemory,
    ParticleSystem07_AllocMemory,
    ParticleSystem08_AllocMemory,
    ParticleSystem09_AllocMemory,
    ParticleSystem10_AllocMemory,
    ParticleSystem11_AllocMemory,
    ParticleSystem12_AllocMemory,
    ParticleSystem13_AllocMemory,
    ParticleSystem14_AllocMemory,
    ParticleSystem15_AllocMemory,
};

void ParticleSystem_ZeroAll(void)
{
    for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++) {
        sParticleSystems[i] = NULL;
    }
}

ParticleSystem *ParticleSystem_New(SPLTexVRAMAllocFunc texAllocFunc, SPLPalVRAMAllocFunc palAllocFunc, void *heap, int heapSize, BOOL hasCamera, enum HeapID heapID)
{
    ParticleSystem *particleSystem;
    int id;

    for (id = 0; id < MAX_PARTICLE_SYSTEMS; id++) {
        if (sParticleSystems[id] == NULL) {
            break;
        }
    }

    if (id >= MAX_PARTICLE_SYSTEMS) {
        return NULL;
    }

    particleSystem = Heap_Alloc(heapID, sizeof(ParticleSystem));
    if (particleSystem == NULL) {
        GF_ASSERT(FALSE);
    }

    memset(particleSystem, 0, sizeof(ParticleSystem));

    particleSystem->texAllocFunc = texAllocFunc;
    particleSystem->palAllocFunc = palAllocFunc;

    particleSystem->cameraPos = sParticleSystemDefaultCameraPos;
    particleSystem->cameraUp = sParticleSystemDefaultCameraUp;
    particleSystem->cameraTarget = sParticleSystemDefaultCameraTarget;

    memset(heap, 0, heapSize);
    particleSystem->heapStart = heap;
    particleSystem->heap = heap;
    particleSystem->heapEnd = (void *)((u32)heap + heapSize);
    particleSystem->id = id;
    sParticleSystems[id] = particleSystem;

    if (hasCamera == TRUE) {
        particleSystem->camera = Camera_Alloc(heapID);

        VEC_Set(&particleSystem->unused1, 0, 0, 0);
        particleSystem->cameraFov = FX_DEG_TO_IDX(FX32_CONST(45.0f));

        Camera_InitWithTargetAndPosition(
            &sParticleSystemDefaultCameraTarget,
            &sParticleSystemDefaultCameraPos,
            particleSystem->cameraFov,
            CAMERA_PROJECTION_PERSPECTIVE,
            FALSE,
            particleSystem->camera);

        particleSystem->cameraProjection = CAMERA_PROJECTION_PERSPECTIVE;
        Camera_SetAsActive(particleSystem->camera);
    }

    particleSystem->manager = SPLManager_New(
        sParticleSystemAllocFuncs[id],
        MAX_EMITTERS,
        MAX_PARTICLES,
        FIXED_POLYGON_ID,
        MIN_POLYGON_ID,
        MAX_POLYGON_ID);

    ParticleSystem_SetCameraUp(particleSystem, &sParticleSystemDefaultCameraUp);

    return particleSystem;
}

void ParticleSystem_Free(ParticleSystem *particleSystem)
{
    ParticleSystem_DeleteAllEmitters(particleSystem);

    if (particleSystem->vramAutoRelease & VRAM_AUTO_RELEASE_TEXTURE_FRM) {
        #ifdef PLATFORM_DS
        NNS_GfdSetFrmTexVramState(&particleSystem->textureVRAMState);
        #else
        // TODO: Port NNS_GfdSetFrmTexVramState to PAL
        #endif
    } else if (particleSystem->vramAutoRelease & VRAM_AUTO_RELEASE_TEXTURE_LNK) {
        for (int i = 0; i < MAX_TEXTURE_KEYS; i++) {
            #ifdef PLATFORM_DS
            if (particleSystem->textureKeys[i] != NNS_GFD_ALLOC_ERROR_TEXKEY) {
            #else
            // TODO: Port NNS_GFD_ALLOC_ERROR_TEXKEY to PAL
            #endif
                #ifdef PLATFORM_DS
                NNS_GfdFreeLnkTexVram(particleSystem->textureKeys[i]);
                #else
                // TODO: Port NNS_GfdFreeLnkTexVram to PAL
                #endif
                #ifdef PLATFORM_DS
                particleSystem->textureKeys[i] = NNS_GFD_ALLOC_ERROR_TEXKEY;
                #else
                // TODO: Port NNS_GFD_ALLOC_ERROR_TEXKEY to PAL
                #endif
            }
        }
    }

    if (particleSystem->vramAutoRelease & (1 << 2)) {
        #ifdef PLATFORM_DS
        NNS_GfdSetFrmPlttVramState(&particleSystem->paletteVRAMState);
        #else
        // TODO: Port NNS_GfdSetFrmPlttVramState to PAL
        #endif
    } else if (particleSystem->vramAutoRelease & (1 << 3)) {
        for (int i = 0; i < MAX_PALETTE_KEYS; i++) {
            #ifdef PLATFORM_DS
            if (particleSystem->paletteKeys[i] != NNS_GFD_ALLOC_ERROR_PLTTKEY) {
            #else
            // TODO: Port NNS_GFD_ALLOC_ERROR_PLTTKEY to PAL
            #endif
                #ifdef PLATFORM_DS
                NNS_GfdFreeLnkPlttVram(particleSystem->paletteKeys[i]);
                #else
                // TODO: Port NNS_GfdFreeLnkPlttVram to PAL
                #endif
                #ifdef PLATFORM_DS
                particleSystem->paletteKeys[i] = NNS_GFD_ALLOC_ERROR_PLTTKEY;
                #else
                // TODO: Port NNS_GFD_ALLOC_ERROR_PLTTKEY to PAL
                #endif
            }
        }
    }

    particleSystem->vramAutoRelease = VRAM_AUTO_RELEASE_NONE;
    particleSystem->lastAddedEmitter = NULL;

    if (particleSystem->resource != NULL) {
        Heap_Free(particleSystem->resource);
        particleSystem->resource = NULL;
    }

    for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++) {
        if (sParticleSystems[i] == particleSystem) {
            sParticleSystems[i] = NULL;
            break;
        }
    }

    if (particleSystem->camera != NULL) {
        Camera_Delete(particleSystem->camera);
    }

    Heap_Free(particleSystem);
}

void ParticleSystem_FreeAll(void)
{
    for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++) {
        if (sParticleSystems[i] != NULL) {
            ParticleSystem_Free(sParticleSystems[i]);
        }
    }
}

static inline void *ParticleSystem_AllocMemory(ParticleSystem *particleSystem, u32 size)
{
    void *allocated = particleSystem->heap;

    u32 newHeapPos = (u32)particleSystem->heap + size;
    u32 offset = newHeapPos % 4;
    if (offset > 0) {
        newHeapPos += 4 - offset;
    }

    particleSystem->heap = (void *)newHeapPos;
    if (particleSystem->heap >= particleSystem->heapEnd) {
        GF_ASSERT(FALSE);
    }

    return allocated;
}

static void *ParticleSystem00_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[0], size);
}

static void *ParticleSystem01_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[1], size);
}

static void *ParticleSystem02_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[2], size);
}

static void *ParticleSystem03_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[3], size);
}

static void *ParticleSystem04_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[4], size);
}

static void *ParticleSystem05_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[5], size);
}

static void *ParticleSystem06_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[6], size);
}

static void *ParticleSystem07_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[7], size);
}

static void *ParticleSystem08_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[8], size);
}

static void *ParticleSystem09_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[9], size);
}

static void *ParticleSystem10_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[10], size);
}

static void *ParticleSystem11_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[11], size);
}

static void *ParticleSystem12_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[12], size);
}

static void *ParticleSystem13_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[13], size);
}

static void *ParticleSystem14_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[14], size);
}

static void *ParticleSystem15_AllocMemory(u32 size)
{
    return ParticleSystem_AllocMemory(sParticleSystems[15], size);
}

void *ParticleSystem_LoadResourceFromNARC(enum NarcID narcID, int memberIndex, enum HeapID heapID)
{
    return NARC_AllocAndReadWholeMemberByIndexPair(narcID, memberIndex, heapID);
}

void ParticleSystem_SetResource(ParticleSystem *particleSystem, void *resource, enum VRAMAutoRelease autoRelease, BOOL uploadImmediately)
{
    GF_ASSERT(particleSystem->manager != NULL);
    GF_ASSERT(particleSystem->resource == NULL);

    particleSystem->vramAutoRelease = autoRelease;

    if (autoRelease & VRAM_AUTO_RELEASE_TEXTURE_FRM) {
        #ifdef PLATFORM_DS
        NNS_GfdGetFrmTexVramState(&particleSystem->textureVRAMState);
        #else
        // TODO: Port NNS_GfdGetFrmTexVramState to PAL
        #endif
    } else if (autoRelease & VRAM_AUTO_RELEASE_TEXTURE_LNK) {
        for (int i = 0; i < MAX_TEXTURE_KEYS; i++) {
            #ifdef PLATFORM_DS
            particleSystem->textureKeys[i] = NNS_GFD_ALLOC_ERROR_TEXKEY;
            #else
            // TODO: Port NNS_GFD_ALLOC_ERROR_TEXKEY to PAL
            #endif
        }
    }

    if (autoRelease & VRAM_AUTO_RELEASE_PALETTE_FRM) {
        #ifdef PLATFORM_DS
        NNS_GfdGetFrmPlttVramState(&particleSystem->paletteVRAMState);
        #else
        // TODO: Port NNS_GfdGetFrmPlttVramState to PAL
        #endif
    } else if (autoRelease & VRAM_AUTO_RELEASE_PALETTE_LNK) {
        for (int i = 0; i < MAX_PALETTE_KEYS; i++) {
            #ifdef PLATFORM_DS
            particleSystem->paletteKeys[i] = NNS_GFD_ALLOC_ERROR_PLTTKEY;
            #else
            // TODO: Port NNS_GFD_ALLOC_ERROR_PLTTKEY to PAL
            #endif
        }
    }

    particleSystem->resource = resource;

    if (uploadImmediately == TRUE) {
        ParticleSystem_UploadResourcesInternal(particleSystem);
    } else {
        SysTask_ExecuteAfterVBlank(ParticleSystem_VBlankResourceUploadInternal, particleSystem, 5);
    }
}

static void ParticleSystem_UploadResourcesInternal(ParticleSystem *particleSystem)
{
    SPLManager_LoadResources(particleSystem->manager, particleSystem->resource);

    sUploadingParticleSystem = particleSystem;

    if (particleSystem->texAllocFunc == NULL) {
        SPLManager_UploadTextures(particleSystem->manager);
    } else {
        SPLManager_UploadTexturesEx(particleSystem->manager, particleSystem->texAllocFunc);
    }

    if (particleSystem->palAllocFunc == NULL) {
        SPLManager_UploadPalettes(particleSystem->manager);
    } else {
        SPLManager_UploadPalettesEx(particleSystem->manager, particleSystem->palAllocFunc);
    }

    sUploadingParticleSystem = NULL;
}

static void ParticleSystem_VBlankResourceUploadInternal(SysTask *task, void *param)
{
    ParticleSystem *particleSystem = param;

    ParticleSystem_UploadResourcesInternal(particleSystem);
    SysTask_Done(task);
}

void ParticleSystem_RegisterTextureKey(NNSGfdTexKey key)
{
    #ifdef PLATFORM_DS
    GF_ASSERT(key != NNS_GFD_ALLOC_ERROR_TEXKEY);
    #else
    // TODO: Port NNS_GFD_ALLOC_ERROR_TEXKEY to PAL
    #endif
    GF_ASSERT(sUploadingParticleSystem != NULL);

    ParticleSystem *particleSystem = sUploadingParticleSystem;

    for (int i = 0; i < MAX_TEXTURE_KEYS; i++) {
        #ifdef PLATFORM_DS
        if (particleSystem->textureKeys[i] == NNS_GFD_ALLOC_ERROR_TEXKEY) {
        #else
        // TODO: Port NNS_GFD_ALLOC_ERROR_TEXKEY to PAL
        #endif
            particleSystem->textureKeys[i] = key;
            return;
        }
    }

    GF_ASSERT(FALSE);
}

void ParticleSystem_RegisterPaletteKey(NNSGfdPlttKey key)
{
    #ifdef PLATFORM_DS
    GF_ASSERT(key != NNS_GFD_ALLOC_ERROR_PLTTKEY);
    #else
    // TODO: Port NNS_GFD_ALLOC_ERROR_PLTTKEY to PAL
    #endif
    GF_ASSERT(sUploadingParticleSystem != NULL);

    ParticleSystem *particleSystem = sUploadingParticleSystem;

    for (int i = 0; i < MAX_PALETTE_KEYS; i++) {
        #ifdef PLATFORM_DS
        if (particleSystem->paletteKeys[i] == NNS_GFD_ALLOC_ERROR_PLTTKEY) {
        #else
        // TODO: Port NNS_GFD_ALLOC_ERROR_PLTTKEY to PAL
        #endif
            particleSystem->paletteKeys[i] = key;
            return;
        }
    }

    GF_ASSERT(FALSE);
}

void ParticleSystem_Draw(ParticleSystem *particleSystem)
{
    if (particleSystem->camera != NULL) {
        Camera_ComputeProjectionMatrix(particleSystem->cameraProjection, particleSystem->camera);
        Camera_SetAsActive(particleSystem->camera);
        Camera_ComputeViewMatrix();
    }

    #ifdef PLATFORM_DS
    NNS_G3dGlbFlush();
    #else
    // TODO: Port NNS_G3dGlbFlush to PAL
    #endif

    #ifdef PLATFORM_DS
    const MtxFx43 *viewMatrix = NNS_G3dGlbGetCameraMtx();
    #else
    // TODO: Port NNS_G3dGlbGetCameraMtx to PAL
    #endif
    SPLManager_Draw(particleSystem->manager, viewMatrix);

    if (particleSystem->camera != NULL) {
        Camera_ClearActive();
    }

    #ifdef PLATFORM_DS
    NNS_G3dGlbFlush();
    #else
    // TODO: Port NNS_G3dGlbFlush to PAL
    #endif
}

void ParticleSystem_Update(ParticleSystem *particleSystem)
{
    SPLManager_Update(particleSystem->manager);
}

int ParticleSystem_GetActiveAmount(void)
{
    int count = 0;
    for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++) {
        if (sParticleSystems[i] != NULL) {
            count++;
        }
    }

    return count;
}

int ParticleSystem_DrawAll(void)
{
    int count = 0;
    for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++) {
        if (sParticleSystems[i] != NULL) {
            ParticleSystem_Draw(sParticleSystems[i]);
            count++;
        }
    }

    return count;
}

int ParticleSystem_UpdateAll(void)
{
    int count = 0;
    for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++) {
        if (sParticleSystems[i] != NULL) {
            ParticleSystem_Update(sParticleSystems[i]);
            count++;
        }
    }

    return count;
}

SPLEmitter *ParticleSystem_CreateEmitter(ParticleSystem *particleSystem, int resourceID, const VecFx32 *position)
{
    SPLEmitter *emitter = SPLManager_CreateEmitter(particleSystem->manager, resourceID, position);
    particleSystem->lastAddedEmitter = emitter;

    return emitter;
}

SPLEmitter *ParticleSystem_CreateEmitterWithCallback(ParticleSystem *particleSystem, int resourceID, SPLEmitterCallback callback, void *param)
{
    sEmitterCallbackParam = param;
    SPLEmitter *emitter = SPLManager_CreateEmitterWithCallback(particleSystem->manager, resourceID, callback);
    sEmitterCallbackParam = NULL;
    particleSystem->lastAddedEmitter = emitter;

    return emitter;
}

s32 ParticleSystem_GetActiveEmitterCount(ParticleSystem *particleSystem)
{
    return particleSystem->manager->activeEmitters.count;
}

void ParticleSystem_DeleteAllEmitters(ParticleSystem *particleSystem)
{
    SPLManager_DeleteAllEmitters(particleSystem->manager);
}

void ParticleSystem_DeleteEmitter(ParticleSystem *particleSystem, SPLEmitter *emitter)
{
    SPLManager_DeleteEmitter(particleSystem->manager, emitter);
}

void *ParticleSystem_GetHeapStart(ParticleSystem *particleSystem)
{
    return particleSystem->heapStart;
}

void ParticleSystem_GetCameraUp(ParticleSystem *particleSystem, VecFx32 *up)
{
    *up = particleSystem->cameraUp;
}

void ParticleSystem_SetCameraUp(ParticleSystem *particleSystem, const VecFx32 *up)
{
    particleSystem->cameraUp = *up;
    Camera_SetUp(up, particleSystem->camera);
}

void *ParticleSystem_GetEmitterCallbackParam(void)
{
    return sEmitterCallbackParam;
}

void ParticleSystem_GetDefaultCameraUp(VecFx32 *up)
{
    *up = sParticleSystemDefaultCameraUp;
}

Camera *ParticleSystem_GetCamera(ParticleSystem *particleSystem)
{
    return particleSystem->camera;
}

void ParticleSystem_SetCameraProjection(ParticleSystem *particleSystem, enum CameraProjection projection)
{
    particleSystem->cameraProjection = projection;
}

u8 ParticleSystem_GetCameraProjection(ParticleSystem *particleSystem)
{
    return particleSystem->cameraProjection;
}

void ParticleSystem_GetEmitterAxis(SPLEmitter *emitter, VecFx16 *axis)
{
    *axis = emitter->axis;
}

void ParticleSystem_SetEmitterEmissionCount(SPLEmitter *emitter, fx32 count)
{
    emitter->resource->header->emissionCount = count;
}

static const void *ParticleSystem_GetEmitterBehaviorInternal(SPLEmitter *emitter, int behaviorType)
{
    int i; // Required to match

    int behaviorCount = emitter->resource->behaviorCount;
    if (behaviorCount == 0) {
        return NULL;
    }

    for (i = 0; i < behaviorCount; i++) {
        SPLBehavior *behavior = &emitter->resource->behaviors[i];

        if (behavior == NULL) {
            continue;
        }

        switch (behaviorType) {
        case SPL_BEHAVIOR_GRAVITY:
            if (behavior->applyFunc == SPLBehavior_ApplyGravity) {
                return behavior->object;
            }
            continue;
        case SPL_BEHAVIOR_RANDOM:
            if (behavior->applyFunc == SPLBehavior_ApplyRandom) {
                return behavior->object;
            }
            continue;
        case SPL_BEHAVIOR_MAGNET:
            if (behavior->applyFunc == SPLBehavior_ApplyMagnet) {
                return behavior->object;
            }
            continue;
        case SPL_BEHAVIOR_SPIN:
            if (behavior->applyFunc == SPLBehavior_ApplySpin) {
                return behavior->object;
            }
            continue;
        case SPL_BEHAVIOR_COLLISION_PLANE:
            if (behavior->applyFunc == SPLBehavior_ApplyCollisionPlane) {
                return behavior->object;
            }
            break;
        case SPL_BEHAVIOR_CONVERGENCE:
            if (behavior->applyFunc == SPLBehavior_ApplyConvergence) {
                return behavior->object;
            }
            continue;
        default:
            return NULL;
        }
    }

    return NULL;
}

void ParticleSystem_SetEmitterGravityMagnitude(SPLEmitter *emitter, VecFx16 *magnitude)
{
    SPLGravityBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_GRAVITY);
    if (behavior == NULL) {
        return;
    }

    behavior->magnitude = *magnitude;
}

void ParticleSystem_SetEmitterMagnetTarget(SPLEmitter *emitter, VecFx32 *target)
{
    SPLMagnetBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_MAGNET);
    if (behavior == NULL) {
        return;
    }

    behavior->target = *target;
}

void ParticleSystem_GetEmitterMagnetTarget(SPLEmitter *emitter, VecFx32 *target)
{
    SPLMagnetBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_MAGNET);
    if (behavior == NULL) {
        *target = (VecFx32) { 0, 0, 0 };
        return;
    }

    *target = behavior->target;
}

void ParticleSystem_SetEmitterMagnetForce(SPLEmitter *emitter, fx16 *force)
{
    SPLMagnetBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_MAGNET);
    if (behavior == NULL) {
        return;
    }

    behavior->force = *force;
}

void ParticleSystem_GetEmitterMagnetForce(SPLEmitter *emitter, fx16 *force)
{
    SPLMagnetBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_MAGNET);
    if (behavior == NULL) {
        *force = 0;
        return;
    }

    *force = behavior->force;
}

void ParticleSystem_SetEmitterSpinAngle(SPLEmitter *emitter, u16 *angle)
{
    SPLSpinBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_SPIN);
    if (behavior == NULL) {
        return;
    }

    behavior->angle = *angle;
}

void ParticleSystem_GetEmitterSpinAngle(SPLEmitter *emitter, u16 *angle)
{
    SPLSpinBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_SPIN);
    if (behavior == NULL) {
        *angle = 0;
        return;
    }

    *angle = behavior->angle;
}

void ParticleSystem_SetEmitterSpinAxis(SPLEmitter *emitter, u16 *axis)
{
    SPLSpinBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_SPIN);
    if (behavior == NULL) {
        return;
    }

    behavior->axis = *axis;
}

void ParticleSystem_GetEmitterSpinAxis(SPLEmitter *emitter, u16 *axis)
{
    SPLSpinBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_SPIN);
    if (behavior == NULL) {
        *axis = 0;
        return;
    }

    *axis = behavior->axis;
}

void ParticleSystem_SetEmitterConvergenceTarget(SPLEmitter *emitter, VecFx32 *target)
{
    SPLConvergenceBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_CONVERGENCE);
    if (behavior == NULL) {
        return;
    }

    behavior->target = *target;
}

void ParticleSystem_GetEmitterConvergenceTarget(SPLEmitter *emitter, VecFx32 *target)
{
    SPLConvergenceBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_CONVERGENCE);
    if (behavior == NULL) {
        *target = (VecFx32) { 0, 0, 0 };
        return;
    }

    *target = behavior->target;
}

void ParticleSystem_SetEmitterConvergenceForce(SPLEmitter *emitter, fx16 *force)
{
    SPLConvergenceBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_CONVERGENCE);
    if (behavior == NULL) {
        return;
    }

    behavior->force = *force;
}

void ParticleSystem_GetEmitterConvergenceForce(SPLEmitter *emitter, fx16 *force)
{
    SPLConvergenceBehavior *behavior = ParticleSystem_GetEmitterBehaviorInternal(emitter, SPL_BEHAVIOR_CONVERGENCE);
    if (behavior == NULL) {
        *force = 0;
        return;
    }

    *force = behavior->force;
}
