#include "sprite.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "heap.h"
#include "system.h"

typedef void (*SpriteDrawFunc)(const SpriteList *, Sprite *);
typedef void (*SpriteAnimUpdateFunc)(Sprite *);

static void SpriteList_Reset(SpriteList *list);
static enum CellAnimType SpriteResourcesHeader_GetCellType(const SpriteResourcesHeader *resourceData);
static void Sprite_SetCellBank(const NNSG2dCellDataBank *cellBank, Sprite *sprite);
static void Sprite_SetCellAnimBank(const NNSG2dCellAnimBankData *cellAnimBank, Sprite *sprite);
static void Sprite_SetMultiCellBank(const NNSG2dMultiCellDataBank *multiCellBank, Sprite *sprite);
static void Sprite_SetMultiCellAnimBank(const NNSG2dMultiCellAnimBankData *multiCellAnimBank, Sprite *sprite);
static void Sprite_CreateCellAnim(Sprite *sprite, enum HeapID heapID);
static void Sprite_CreateVRamCellAnim(const SpriteResourcesHeader *resourceData, Sprite *sprite, enum HeapID heapID);
static void Sprite_CreateMultiCellAnim(Sprite *sprite, enum HeapID heapID);
static BOOL SpriteList_InitSprite(const SpriteList *list,
    const SpriteResourcesHeader *resourceData,
    Sprite *sprite,
    enum HeapID heapID);
static u32 GetPaletteIndexForProxy(const NNSG2dImagePaletteProxy *paletteProxy, u32 vramType);
static void SpriteList_DrawSprite(const SpriteList *list, Sprite *sprite);
static void SpriteList_DrawSprite_Stub(const SpriteList *list, Sprite *sprite);
static void Sprite_UpdateAnimInternal(Sprite *list);
static void Sprite_UpdateAnimInternal_Stub(Sprite *list);
static void SpriteList_Insert(SpriteList *list, Sprite *sprite);
static void SpriteList_Remove(Sprite *sprite);
static void SpriteList_InitSprites(SpriteList *list);
static Sprite *SpriteList_AllocSprite(SpriteList *list);
static BOOL SpriteList_FreeSprite(SpriteList *list, Sprite *sprite);

SpriteList *SpriteList_New(const SpriteListParams *params)
{
    GF_ASSERT(params);
    GF_ASSERT(params->renderer);

    SpriteList *list = Heap_Alloc(params->heapID, sizeof(SpriteList));
    GF_ASSERT(list);

    SpriteList_Reset(list);

    list->sprites = Heap_Alloc(params->heapID, sizeof(Sprite) * params->maxElements);
    GF_ASSERT(list->sprites);
    list->capacity = params->maxElements;

    list->freeSprites = Heap_Alloc(params->heapID, sizeof(Sprite *) * params->maxElements);
    GF_ASSERT(list->freeSprites);

    SpriteList_InitSprites(list);
    Sprite_Reset(&list->sentinelData);

    list->sentinelData.prev = &list->sentinelData;
    list->sentinelData.next = &list->sentinelData;
    list->renderer = params->renderer;
    list->rawAnimData = ReadFileToHeap(params->heapID, "data/clact_default.NANR");

    #ifdef PLATFORM_DS
    NNS_G2dGetUnpackedAnimBank(list->rawAnimData, &list->defaultAnimBank);
    #else
    // TODO: Port NNS_G2dGetUnpackedAnimBank to PAL
    #endif
    list->active = TRUE;

    return list;
}

BOOL SpriteList_Delete(SpriteList *list)
{
    if (list == NULL) {
        return FALSE;
    }

    if (list->sprites == NULL) {
        return TRUE;
    }

    SpriteList_DeleteAll(list);

    Heap_Free(list->rawAnimData);
    Heap_Free(list->freeSprites);
    Heap_Free(list->sprites);

    SpriteList_Reset(list);
    Heap_Free(list);

    list = NULL;

    return TRUE;
}

BOOL SpriteList_SetActive(SpriteList *list, u8 active)
{
    if (list == NULL) {
        return FALSE;
    }

    if (list->sprites == NULL) {
        return FALSE;
    }

    list->active = active;

    return TRUE;
}

BOOL SpriteList_DeleteAll(SpriteList *list)
{
    if (list == NULL) {
        return FALSE;
    }

    if (list->sprites == NULL) {
        return TRUE;
    }

    Sprite *sprite = list->sentinelData.next;
    while (sprite != &list->sentinelData) {
        Sprite *next = sprite->next;
        Sprite_Delete(sprite);
        sprite = next;
    }

    return TRUE;
}

void SpriteList_Update(const SpriteList *list)
{
    static const SpriteDrawFunc sDrawFuncs[] = {
        SpriteList_DrawSprite_Stub,
        SpriteList_DrawSprite
    };
    static const SpriteAnimUpdateFunc sAnimUpdateFuncs[] = {
        Sprite_UpdateAnimInternal_Stub,
        Sprite_UpdateAnimInternal
    };

    GF_ASSERT(list);

    if (list->active == FALSE) {
        return;
    }

    Sprite *sprite = list->sentinelData.next;

    while (sprite != &list->sentinelData) {
        sDrawFuncs[sprite->draw](list, sprite);
        sAnimUpdateFuncs[sprite->animate](sprite);
        sprite = sprite->next;
    }
}

static void SpriteList_Reset(SpriteList *list)
{
    list->sprites = NULL;
    list->capacity = 0;
    list->freeSprites = NULL;
    list->freeSpriteHead = 0;
    list->renderer = NULL;

    Sprite_Reset(&list->sentinelData);
    list->active = FALSE;
}

void Sprite_Reset(Sprite *sprite)
{
    sprite->list = NULL;
    memset(sprite, 0, sizeof(Sprite));

    #ifdef PLATFORM_DS
    NNS_G2dInitImageProxy(&sprite->imageProxy);
    #else
    // TODO: Port NNS_G2dInitImageProxy to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G2dInitImagePaletteProxy(&sprite->paletteProxy);
    #else
    // TODO: Port NNS_G2dInitImagePaletteProxy to PAL
    #endif

    #ifdef PLATFORM_DS
    sprite->explicitOamMode = GX_OAM_MODE_NORMAL;
    #else
    // TODO: Port GX_OAM_MODE_NORMAL to PAL
    #endif
}

Sprite *SpriteList_AddAffine(const AffineSpriteListTemplate *template)
{
    Sprite *sprite = SpriteList_AllocSprite(template->list);
    if (sprite == NULL) {
        return NULL;
    }

    sprite->list = template->list;
    sprite->activeAnimID = 0;
    sprite->position = template->position;
    sprite->affineScale = template->affineScale;
    sprite->affineZRotation = template->affineZRotation;
    sprite->vramType = template->vramType;
    sprite->priority = template->priority;
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    sprite->affineOverwriteMode = NNS_G2D_RND_AFFINE_OVERWRITE_NONE;
    #else
    // TODO: Port NNS_G2D_RND_AFFINE_OVERWRITE_NONE to PAL
    #endif
    #else
    // TODO: Port NNS_G2D_RND_AFFINE_OVERWRITE_NONE to PAL
    #endif
    #ifdef PLATFORM_DS
    sprite->flip = NNS_G2D_RENDERERFLIP_NONE;
    #else
    // TODO: Port NNS_G2D_RENDERERFLIP_NONE to PAL
    #endif
    sprite->explicitMosaic = FALSE;
    #ifdef PLATFORM_DS
    sprite->explicitOamMode = GX_OAM_MODE_NORMAL;
    #else
    // TODO: Port GX_OAM_MODE_NORMAL to PAL
    #endif
    #ifdef PLATFORM_DS
    sprite->overwriteFlags = NNS_G2D_RND_OVERWRITE_PLTTNO_OFFS | NNS_G2D_RND_OVERWRITE_PRIORITY;
    #else
    // TODO: Port NNS_G2D_RND_OVERWRITE_PRIORITY to PAL
    #endif

    #ifdef PLATFORM_DS
    NNS_G2dSetRndCoreAffineOverwriteMode(
    #else
    // TODO: Port NNS_G2dSetRndCoreAffineOverwriteMode to PAL
    #endif
        &template->list->renderer->rendererCore,
        sprite->affineOverwriteMode);
    #ifdef PLATFORM_DS
    NNS_G2dSetRndCoreFlipMode(
    #else
    // TODO: Port NNS_G2dSetRndCoreFlipMode to PAL
    #endif
        &template->list->renderer->rendererCore,
        #ifdef PLATFORM_DS
        sprite->flip & NNS_G2D_RENDERERFLIP_H,
        #else
        // TODO: Port NNS_G2D_RENDERERFLIP_H to PAL
        #endif
        #ifdef PLATFORM_DS
        sprite->flip & NNS_G2D_RENDERERFLIP_V);
        #else
        // TODO: Port NNS_G2D_RENDERERFLIP_V to PAL
        #endif

    sprite->draw = TRUE;
    sprite->animate = FALSE;
    sprite->animSpeed = (FX32_ONE * 2);

    if (SpriteList_InitSprite(template->list, template->resourceData, sprite, template->heapID) == FALSE) {
        Sprite_Delete(sprite);
        return NULL;
    }

    sprite->explicitPaletteOffset = GetPaletteIndexForProxy(&sprite->paletteProxy, sprite->vramType);
    sprite->explicitPalette = sprite->explicitPaletteOffset;

    SpriteList_Insert(template->list, sprite);

    return sprite;
}

Sprite *SpriteList_Add(const SpriteListTemplate *template)
{
    AffineSpriteListTemplate affineTemplate;

    affineTemplate.list = template->list;
    affineTemplate.resourceData = template->resourceData;
    affineTemplate.position = template->position;
    affineTemplate.affineScale.x = FX32_ONE;
    affineTemplate.affineScale.y = FX32_ONE;
    affineTemplate.affineScale.z = FX32_ONE;
    affineTemplate.affineZRotation = 0;
    affineTemplate.priority = template->priority;
    affineTemplate.vramType = template->vramType;
    affineTemplate.heapID = template->heapID;

    return SpriteList_AddAffine(&affineTemplate);
}

void Sprite_Delete(Sprite *sprite)
{
    if (sprite->type == CELL_ANIM_TYPE_NONE) {
        return;
    }

    if (sprite->prev != NULL) {
        SpriteList_Remove(sprite);
    }

    if (sprite->type == CELL_ANIM_TYPE_VRAM_CELL) {
        VRamCellAnimationData *vramCellAnim = (VRamCellAnimationData *)&sprite->animData;

        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port NNS_G2dGetImageLocation to PAL
        #endif
        if (NNS_G2dGetImageLocation(&sprite->imageProxy, sprite->vramType) != NNS_G2D_VRAM_ADDR_NONE) {
        #else
        // TODO: Port NNS_G2D_VRAM_ADDR_NONE to PAL
        #endif
            #ifdef PLATFORM_DS
            NNS_G2dFreeCellTransferStateHandle(vramCellAnim->transferHandle);
            #else
            // TODO: Port NNS_G2dFreeCellTransferStateHandle to PAL
            #endif
        }
    }

    if (sprite->type == CELL_ANIM_TYPE_MULTI_CELL) {
        MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;

        if (multiCellAnim->nodes != NULL) {
            Heap_Free(multiCellAnim->nodes);
        }

        if (multiCellAnim->cellAnims != NULL) {
            Heap_Free(multiCellAnim->cellAnims);
        }
    }

    sprite->type = CELL_ANIM_TYPE_NONE;

    SpriteList_FreeSprite(sprite->list, sprite);
}

void Sprite_SetPosition(Sprite *sprite, const VecFx32 *position)
{
    sprite->position = *position;
}

void Sprite_SetAffineTranslation(Sprite *sprite, const VecFx32 *translation)
{
    sprite->affineTranslation = *translation;
}

void Sprite_SetAffineScale(Sprite *sprite, const VecFx32 *scale)
{
    sprite->affineScale = *scale;
}

void Sprite_SetAffineScaleEx(Sprite *sprite, const VecFx32 *scale, enum AffineOverwriteMode mode)
{
    Sprite_SetAffineScale(sprite, scale);
    Sprite_SetAffineOverwriteMode(sprite, mode);
}

void Sprite_SetAffineZRotation(Sprite *sprite, u16 angle)
{
    sprite->affineZRotation = angle;
}

void Sprite_SetAffineZRotationEx(Sprite *sprite, u16 angle, enum AffineOverwriteMode mode)
{
    Sprite_SetAffineZRotation(sprite, angle);
    Sprite_SetAffineOverwriteMode(sprite, mode);
}

void Sprite_SetDrawFlag(Sprite *sprite, BOOL draw)
{
    GF_ASSERT(sprite);
    GF_ASSERT(draw < 2); // This value is used as an index into an array of size 2 inside SpriteList_Update

    sprite->draw = draw;
}

void Sprite_SetAnimateFlag(Sprite *sprite, BOOL animate)
{
    GF_ASSERT(sprite);
    GF_ASSERT(animate < 2); // This value is used as an index into an array of size 2 inside SpriteList_Update

    sprite->animate = animate;
}

void Sprite_SetAnimSpeed(Sprite *sprite, fx32 speed)
{
    GF_ASSERT(sprite);
    sprite->animSpeed = speed;
}

void Sprite_SetAffineOverwriteMode(Sprite *sprite, enum AffineOverwriteMode mode)
{
    GF_ASSERT(sprite);
    sprite->affineOverwriteMode = mode;
}

void Sprite_SetFlipMode(Sprite *sprite, u32 mode)
{
    GF_ASSERT(sprite);
    sprite->flip = mode;
    #ifdef PLATFORM_DS
    sprite->affineOverwriteMode = NNS_G2D_RND_AFFINE_OVERWRITE_NONE;
    #else
    // TODO: Port NNS_G2D_RND_AFFINE_OVERWRITE_NONE to PAL
    #endif
}

const VecFx32 *Sprite_GetPosition(const Sprite *sprite)
{
    return &sprite->position;
}

const VecFx32 *Sprite_GetAffineScale(const Sprite *sprite)
{
    return &sprite->affineScale;
}

u16 Sprite_GetAffineZRotation(const Sprite *sprite)
{
    return sprite->affineZRotation;
}

BOOL Sprite_GetDrawFlag(const Sprite *sprite)
{
    return sprite->draw;
}

BOOL Sprite_GetAnimateFlag(const Sprite *sprite)
{
    return sprite->animate;
}

u32 Sprite_GetAnimCount(const Sprite *sprite)
{
    GF_ASSERT(sprite);

    if (sprite->type == CELL_ANIM_TYPE_CELL || sprite->type == CELL_ANIM_TYPE_VRAM_CELL) {
        CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;
        return cellAnim->animBank->numSequences;
    } else {
        MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;
        return multiCellAnim->animBank->numSequences;
    }
}

void Sprite_SetAnim(Sprite *sprite, u32 animID)
{
    GF_ASSERT(Sprite_GetAnimCount(sprite) > animID);
    sprite->activeAnimID = animID;

    if (sprite->type == CELL_ANIM_TYPE_CELL || sprite->type == CELL_ANIM_TYPE_VRAM_CELL) {
        CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;

        #ifdef PLATFORM_DS
        const NNSG2dAnimSequence *animSequence = NNS_G2dGetAnimSequenceByIdx(cellAnim->animBank, animID);
        #else
        // TODO: Port NNS_G2dGetAnimSequenceByIdx to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G2dSetCellAnimationSequence(&cellAnim->anim, animSequence);
        #else
        // TODO: Port NNS_G2dSetCellAnimationSequence to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G2dStartAnimCtrl(&cellAnim->anim.animCtrl);
        #else
        // TODO: Port NNS_G2dStartAnimCtrl to PAL
        #endif
    } else {
        MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;

        #ifdef PLATFORM_DS
        const NNSG2dAnimSequence *animSequence = NNS_G2dGetAnimSequenceByIdx(multiCellAnim->animBank, animID);
        #else
        // TODO: Port NNS_G2dGetAnimSequenceByIdx to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G2dSetAnimSequenceToMCAnimation(&multiCellAnim->anim, animSequence);
        #else
        // TODO: Port NNS_G2dSetAnimSequenceToMCAnimation to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G2dStartAnimCtrl(&multiCellAnim->anim.animCtrl);
        #else
        // TODO: Port NNS_G2dStartAnimCtrl to PAL
        #endif
    }
}

void Sprite_SetAnimNoRestart(Sprite *sprite, u32 animID)
{
    if (sprite->activeAnimID != animID) {
        Sprite_SetAnim(sprite, animID);
    }
}

void Sprite_RestartAnim(Sprite *sprite)
{
    if (sprite->type == CELL_ANIM_TYPE_CELL || sprite->type == CELL_ANIM_TYPE_VRAM_CELL) {
        CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;

        #ifdef PLATFORM_DS
        NNS_G2dResetAnimCtrlState(&cellAnim->anim.animCtrl);
        #else
        // TODO: Port NNS_G2dResetAnimCtrlState to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G2dStartAnimCtrl(&cellAnim->anim.animCtrl);
        #else
        // TODO: Port NNS_G2dStartAnimCtrl to PAL
        #endif
        Sprite_SetAnimFrame(sprite, 0);
    } else {
        MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;

        #ifdef PLATFORM_DS
        NNS_G2dResetAnimCtrlState(&multiCellAnim->anim.animCtrl);
        #else
        // TODO: Port NNS_G2dResetAnimCtrlState to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G2dStartAnimCtrl(&multiCellAnim->anim.animCtrl);
        #else
        // TODO: Port NNS_G2dStartAnimCtrl to PAL
        #endif
        Sprite_SetAnimFrame(sprite, 0);
    }
}

u32 Sprite_GetActiveAnim(const Sprite *sprite)
{
    return sprite->activeAnimID;
}

void Sprite_UpdateAnim(Sprite *sprite, fx32 frames)
{
    if (sprite->type == CELL_ANIM_TYPE_CELL || sprite->type == CELL_ANIM_TYPE_VRAM_CELL) {
        CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;
        #ifdef PLATFORM_DS
        NNS_G2dTickCellAnimation(&cellAnim->anim, frames);
        #else
        // TODO: Port NNS_G2dTickCellAnimation to PAL
        #endif
    } else {
        MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;
        #ifdef PLATFORM_DS
        NNS_G2dTickMCAnimation(&multiCellAnim->anim, frames);
        #else
        // TODO: Port NNS_G2dTickMCAnimation to PAL
        #endif
    }
}

void Sprite_SetAnimFrame(Sprite *sprite, u16 frame)
{
    if (sprite->type == CELL_ANIM_TYPE_CELL || sprite->type == CELL_ANIM_TYPE_VRAM_CELL) {
        CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;
        #ifdef PLATFORM_DS
        NNS_G2dSetCellAnimationCurrentFrame(&cellAnim->anim, frame);
        #else
        // TODO: Port NNS_G2dSetCellAnimationCurrentFrame to PAL
        #endif
    } else {
        MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;
        #ifdef PLATFORM_DS
        NNS_G2dSetMCAnimationCurrentFrame(&multiCellAnim->anim, frame);
        #else
        // TODO: Port NNS_G2dSetMCAnimationCurrentFrame to PAL
        #endif
    }
}

u16 Sprite_GetAnimFrame(const Sprite *sprite)
{
    NNSG2dAnimController *controller;

    if (sprite->type == CELL_ANIM_TYPE_CELL || sprite->type == CELL_ANIM_TYPE_VRAM_CELL) {
        CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;
        #ifdef PLATFORM_DS
        controller = NNS_G2dGetCellAnimationAnimCtrl(&cellAnim->anim);
        #else
        // TODO: Port NNS_G2dGetCellAnimationAnimCtrl to PAL
        #endif
    } else {
        MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;
        #ifdef PLATFORM_DS
        controller = NNS_G2dGetMCAnimAnimCtrl(&multiCellAnim->anim);
        #else
        // TODO: Port NNS_G2dGetMCAnimAnimCtrl to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    return NNS_G2dGetAnimCtrlCurrentFrame(controller);
    #else
    // TODO: Port NNS_G2dGetAnimCtrlCurrentFrame to PAL
    #endif
}

void Sprite_SetExplicitPriority(Sprite *sprite, u8 priority)
{
    sprite->explicitPriority = priority;
}

u8 Sprite_GetExplicitPriority(const Sprite *sprite)
{
    return sprite->explicitPriority;
}

void Sprite_SetExplicitPalette(Sprite *sprite, u32 palette)
{
    GF_ASSERT(sprite);

    sprite->explicitPalette = palette;
    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    sprite->overwriteFlags |= NNS_G2D_RND_OVERWRITE_PLTTNO;
    #else
    // TODO: Port NNS_G2D_RND_OVERWRITE_PLTTNO to PAL
    #endif
    #else
    // TODO: Port NNS_G2D_RND_OVERWRITE_PLTTNO to PAL
    #endif
    #ifdef PLATFORM_DS
    sprite->overwriteFlags &= ~NNS_G2D_RND_OVERWRITE_PLTTNO_OFFS;
    #else
    // TODO: Port NNS_G2D_RND_OVERWRITE_PLTTNO_OFFS to PAL
    #endif
}

void Sprite_SetExplicitPaletteWithOffset(Sprite *sprite, u32 palette)
{
    Sprite_SetExplicitPalette(sprite, palette);
    sprite->explicitPalette += GetPaletteIndexForProxy(&sprite->paletteProxy, sprite->vramType);
}

u32 Sprite_GetExplicitPalette(const Sprite *sprite)
{
    return sprite->explicitPalette;
}

void Sprite_SetExplicitPaletteOffset(Sprite *sprite, u32 paletteOffset)
{
    GF_ASSERT(sprite);

    sprite->explicitPaletteOffset = paletteOffset;
    #ifdef PLATFORM_DS
    sprite->overwriteFlags |= NNS_G2D_RND_OVERWRITE_PLTTNO_OFFS;
    #else
    // TODO: Port NNS_G2D_RND_OVERWRITE_PLTTNO_OFFS to PAL
    #endif
    #ifdef PLATFORM_DS
    sprite->overwriteFlags &= ~NNS_G2D_RND_OVERWRITE_PLTTNO;
    #else
    // TODO: Port NNS_G2D_RND_OVERWRITE_PLTTNO to PAL
    #endif
}

void Sprite_SetExplicitPaletteOffsetAutoAdjust(Sprite *sprite, u32 paletteOffset)
{
    Sprite_SetExplicitPaletteOffset(sprite, paletteOffset);
    sprite->explicitPaletteOffset += GetPaletteIndexForProxy(&sprite->paletteProxy, sprite->vramType);
}

u32 Sprite_GetExplicitPaletteOffset(const Sprite *sprite)
{
    GF_ASSERT(sprite);
    return sprite->explicitPaletteOffset;
}

void Sprite_SetPriority(Sprite *sprite, u32 priority)
{
    SpriteList *list = sprite->list; // Required to match
    sprite->priority = priority;

    SpriteList_Remove(sprite);
    SpriteList_Insert(list, sprite);
}

u32 Sprite_GetPriority(const Sprite *sprite)
{
    return sprite->priority;
}

void Sprite_SetImageProxy(Sprite *sprite, const NNSG2dImageProxy *imageProxy)
{
    sprite->imageProxy = *imageProxy;
}

NNSG2dImageProxy *Sprite_GetImageProxy(Sprite *sprite)
{
    return &sprite->imageProxy;
}

NNSG2dImagePaletteProxy *Sprite_GetPaletteProxy(Sprite *paletteProxy)
{
    return &paletteProxy->paletteProxy;
}

void Sprite_SetMosaicFlag(Sprite *sprite, BOOL mosaic)
{
    sprite->explicitMosaic = mosaic;

    if (mosaic == TRUE) {
        #ifdef PLATFORM_DS
        sprite->overwriteFlags |= NNS_G2D_RND_OVERWRITE_MOSAIC;
        #else
        // TODO: Port NNS_G2D_RND_OVERWRITE_MOSAIC to PAL
        #endif
    } else {
        #ifdef PLATFORM_DS
        sprite->overwriteFlags ^= NNS_G2D_RND_OVERWRITE_MOSAIC;
        #else
        // TODO: Port NNS_G2D_RND_OVERWRITE_MOSAIC to PAL
        #endif
    }
}

#ifdef PLATFORM_DS
NNS_G2D_VRAM_TYPE Sprite_GetVRamType(const Sprite *sprite)
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
{
    return sprite->vramType;
}

BOOL Sprite_IsAnimated(Sprite *sprite)
{
    GF_ASSERT(sprite);

    if (sprite->type == CELL_ANIM_TYPE_CELL || sprite->type == CELL_ANIM_TYPE_VRAM_CELL) {
        CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;
        #ifdef PLATFORM_DS
        return NNS_G2dIsAnimCtrlActive(&cellAnim->anim.animCtrl);
        #else
        // TODO: Port NNS_G2dIsAnimCtrlActive to PAL
        #endif
    } else {
        MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;
        #ifdef PLATFORM_DS
        return NNS_G2dIsAnimCtrlActive(&multiCellAnim->anim.animCtrl);
        #else
        // TODO: Port NNS_G2dIsAnimCtrlActive to PAL
        #endif
    }
}

void Sprite_SetExplicitOAMMode(Sprite *sprite, GXOamMode mode)
{
    GF_ASSERT(sprite);

    sprite->explicitOamMode = mode;

    #ifdef PLATFORM_DS
    if (mode == GX_OAM_MODE_NORMAL) {
    #else
    // TODO: Port GX_OAM_MODE_NORMAL to PAL
    #endif
        #ifdef PLATFORM_DS
        sprite->overwriteFlags ^= NNS_G2D_RND_OVERWRITE_OBJMODE;
        #else
        // TODO: Port NNS_G2D_RND_OVERWRITE_OBJMODE to PAL
        #endif
    } else {
        #ifdef PLATFORM_DS
        sprite->overwriteFlags |= NNS_G2D_RND_OVERWRITE_OBJMODE;
        #else
        // TODO: Port NNS_G2D_RND_OVERWRITE_OBJMODE to PAL
        #endif
    }
}

void Utility_Clear2DMainOAM(enum HeapID heapID)
{
    GXOamAttr *oam = Heap_Alloc(heapID, sizeof(GXOamAttr) * MAX_SPRITES);

    MI_CpuFill16(oam, 0x2C0, sizeof(GXOamAttr) * MAX_SPRITES);
    DC_FlushRange(oam, sizeof(GXOamAttr) * MAX_SPRITES);
    #ifdef PLATFORM_DS
    GX_LoadOAM(oam, 0, sizeof(GXOamAttr) * MAX_SPRITES);
    #else
    // TODO: Port GX_LoadOAM to PAL
    #endif

    Heap_Free(oam);
}

void Utility_Clear2DSubOAM(enum HeapID heapID)
{
    GXOamAttr *oam = Heap_Alloc(heapID, sizeof(GXOamAttr) * MAX_SPRITES);

    MI_CpuFill16(oam, 0x2C0, sizeof(GXOamAttr) * MAX_SPRITES);
    // According to the NitroSDK docs there should be a call to DC_FlushRange here.
    #ifdef PLATFORM_DS
    GXS_LoadOAM(oam, 0, sizeof(GXOamAttr) * MAX_SPRITES);
    #else
    // TODO: Port GXS_LoadOAM to PAL
    #endif

    Heap_Free(oam);
}

u32 Sprite_GetUserAttrForAnimFrame(const Sprite *sprite, u32 animID, u32 frame)
{
    const NNSG2dAnimBankData *animBank;

    if (sprite->type == CELL_ANIM_TYPE_CELL || sprite->type == CELL_ANIM_TYPE_VRAM_CELL) {
        CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;
        animBank = cellAnim->animBank;
    } else {
        MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;
        animBank = multiCellAnim->animBank;
    }

    #ifdef PLATFORM_DS
    #ifdef PLATFORM_DS
    const NNSG2dUserExAnimAttrBank *animAttrBank = NNS_G2dGetUserExAnimAttrBank(animBank);
    #else
    // TODO: Port NNS_G2dGetUserExAnimAttrBank to PAL
    #endif
    #else
    // TODO: Port NNS_G2dGetUserExAnimAttrBank to PAL
    #endif

    // Guard clauses don't match here so the nested if's are required
    if (animAttrBank) {
        #ifdef PLATFORM_DS
        const NNSG2dUserExAnimSequenceAttr *animAttr = NNS_G2dGetUserExAnimSequenceAttr(animAttrBank, animID);
        #else
        // TODO: Port NNS_G2dGetUserExAnimSequenceAttr to PAL
        #endif

        if (animAttr) {
            #ifdef PLATFORM_DS
            const NNSG2dUserExAnimFrameAttr *frameAttr = NNS_G2dGetUserExAnimFrameAttr(animAttr, frame);
            #else
            // TODO: Port NNS_G2dGetUserExAnimFrameAttr to PAL
            #endif

            if (frameAttr) {
                #ifdef PLATFORM_DS
                return NNS_G2dGetUserExAnimFrmAttrValue(frameAttr);
                #else
                // TODO: Port NNS_G2dGetUserExAnimFrmAttrValue to PAL
                #endif
            }
        }
    }

    return 0;
}

u32 Sprite_GetUserAttrForCurrentAnimFrame(const Sprite *sprite)
{
    u32 animID = Sprite_GetActiveAnim(sprite);
    u32 frame = Sprite_GetAnimFrame(sprite);

    return Sprite_GetUserAttrForAnimFrame(sprite, animID, frame);
}

static BOOL SpriteList_InitSprite(const SpriteList *list,
    const SpriteResourcesHeader *resourceData,
    Sprite *sprite,
    enum HeapID heapID)
{
    sprite->type = SpriteResourcesHeader_GetCellType(resourceData);
    sprite->imageProxy = *resourceData->imageProxy;
    sprite->paletteProxy = *resourceData->paletteProxy;

    Sprite_SetCellBank(resourceData->cellBank, sprite);

    if (resourceData->cellAnimBank) {
        Sprite_SetCellAnimBank(resourceData->cellAnimBank, sprite);
    } else {
        Sprite_SetCellAnimBank(list->defaultAnimBank, sprite);
    }

    if (sprite->type == CELL_ANIM_TYPE_MULTI_CELL) {
        Sprite_SetMultiCellBank(resourceData->multiCellBank, sprite);
        Sprite_SetMultiCellAnimBank(resourceData->multiCellAnimBank, sprite);
        Sprite_CreateMultiCellAnim(sprite, heapID);
    } else if (sprite->type == CELL_ANIM_TYPE_VRAM_CELL) {
        Sprite_CreateVRamCellAnim(resourceData, sprite, heapID);
    } else {
        Sprite_CreateCellAnim(sprite, heapID);
    }

    sprite->explicitPriority = resourceData->priority;

    return TRUE;
}

static enum CellAnimType SpriteResourcesHeader_GetCellType(const SpriteResourcesHeader *resourceData)
{
    if (resourceData->multiCellBank != NULL) {
        return CELL_ANIM_TYPE_MULTI_CELL;
    }

    if (resourceData->isVRamTransfer == TRUE) {
        return CELL_ANIM_TYPE_VRAM_CELL;
    }

    return CELL_ANIM_TYPE_CELL;
}

static void Sprite_SetCellBank(const NNSG2dCellDataBank *cellBank, Sprite *sprite)
{
    CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;
    cellAnim->cellBank = cellBank;
}

static void Sprite_SetCellAnimBank(const NNSG2dCellAnimBankData *cellAnimBank, Sprite *sprite)
{
    CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;
    cellAnim->animBank = cellAnimBank;
}

static void Sprite_SetMultiCellBank(const NNSG2dMultiCellDataBank *multiCellBank, Sprite *sprite)
{
    MultiCellAnimationData *multiCell = (MultiCellAnimationData *)&sprite->animData;
    multiCell->cellBank = multiCellBank;
}

static void Sprite_SetMultiCellAnimBank(const NNSG2dMultiCellAnimBankData *multiCellAnimBank, Sprite *sprite)
{
    MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;
    multiCellAnim->animBank = multiCellAnimBank;
}

static void Sprite_CreateCellAnim(Sprite *sprite, enum HeapID heapID)
{
    CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;
    #ifdef PLATFORM_DS
    NNS_G2dInitCellAnimation(
    #else
    // TODO: Port NNS_G2dInitCellAnimation to PAL
    #endif
        &cellAnim->anim,
        #ifdef PLATFORM_DS
        NNS_G2dGetAnimSequenceByIdx(cellAnim->animBank, 0),
        #else
        // TODO: Port NNS_G2dGetAnimSequenceByIdx to PAL
        #endif
        cellAnim->cellBank);
}

static void Sprite_CreateVRamCellAnim(const SpriteResourcesHeader *resourceData, Sprite *sprite, enum HeapID heapID)
{
    VRamCellAnimationData *vramCellAnim = (VRamCellAnimationData *)&sprite->animData;
    #ifdef PLATFORM_DS
    vramCellAnim->transferHandle = NNS_G2dGetNewCellTransferStateHandle();
    #else
    // TODO: Port NNS_G2dGetNewCellTransferStateHandle to PAL
    #endif
    const NNSG2dCharacterData *charData = resourceData->charData;

    #ifdef PLATFORM_DS
    NNS_G2dInitCellAnimationVramTransfered(
    #else
    // TODO: Port NNS_G2dInitCellAnimationVramTransfered to PAL
    #endif
        &vramCellAnim->anim,
        #ifdef PLATFORM_DS
        NNS_G2dGetAnimSequenceByIdx(vramCellAnim->animBank, 0),
        #else
        // TODO: Port NNS_G2dGetAnimSequenceByIdx to PAL
        #endif
        vramCellAnim->cellBank,
        vramCellAnim->transferHandle,
        #ifdef PLATFORM_DS
        NNS_G2D_VRAM_ADDR_NONE,
        #else
        // TODO: Port NNS_G2D_VRAM_ADDR_NONE to PAL
        #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port NNS_G2dGetImageLocation to PAL
        #endif
        NNS_G2dGetImageLocation(&sprite->imageProxy, NNS_G2D_VRAM_TYPE_2DMAIN),
        #else
        // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
        #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port NNS_G2dGetImageLocation to PAL
        #endif
        NNS_G2dGetImageLocation(&sprite->imageProxy, NNS_G2D_VRAM_TYPE_2DSUB),
        #else
        // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
        #endif
        charData->pRawData,
        NULL,
        charData->szByte);
}

static void Sprite_CreateMultiCellAnim(Sprite *sprite, enum HeapID heapID)
{
    MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;
    #ifdef PLATFORM_DS
    const NNSG2dMultiCellAnimSequence *animSequence = NNS_G2dGetAnimSequenceByIdx(multiCellAnim->animBank, 0);
    #else
    // TODO: Port NNS_G2dGetAnimSequenceByIdx to PAL
    #endif
    #ifdef PLATFORM_DS
    u16 maxNodes = NNS_G2dGetMCBankNumNodesRequired(multiCellAnim->cellBank);
    #else
    // TODO: Port NNS_G2dGetMCBankNumNodesRequired to PAL
    #endif
    multiCellAnim->nodes = Heap_Alloc(heapID, sizeof(NNSG2dNode) * maxNodes);
    multiCellAnim->cellAnims = Heap_Alloc(heapID, sizeof(NNSG2dCellAnimation) * maxNodes);

    #ifdef PLATFORM_DS
    NNS_G2dInitMCAnimation(
    #else
    // TODO: Port NNS_G2dInitMCAnimation to PAL
    #endif
        &multiCellAnim->anim,
        multiCellAnim->nodes,
        multiCellAnim->cellAnims,
        maxNodes,
        multiCellAnim->individualAnimBank,
        multiCellAnim->individualCellBank,
        multiCellAnim->cellBank);

    #ifdef PLATFORM_DS
    NNS_G2dSetAnimSequenceToMCAnimation(
    #else
    // TODO: Port NNS_G2dSetAnimSequenceToMCAnimation to PAL
    #endif
        &multiCellAnim->anim,
        animSequence);
}

static u32 GetPaletteIndexForProxy(const NNSG2dImagePaletteProxy *paletteProxy, u32 vramType)
{
    u32 paletteSize;

    if (paletteProxy->bExtendedPlt) {
        paletteSize = 32 * 16;
    } else {
        #ifdef PLATFORM_DS
        if (paletteProxy->fmt == GX_TEXFMT_PLTT256) {
        #else
        // TODO: Port GX_TEXFMT_PLTT256 to PAL
        #endif
            paletteSize = 0;
        } else {
            paletteSize = 32;
        }
    }

    return paletteSize != 0
        #ifdef PLATFORM_DS
        ? NNS_G2dGetImagePaletteLocation(paletteProxy, vramType) / paletteSize
        #else
        // TODO: Port NNS_G2dGetImagePaletteLocation to PAL
        #endif
        : 0;
}

static void SpriteList_DrawSprite(const SpriteList *list, Sprite *sprite)
{
    VecFx32 pos = sprite->position;

    #ifdef PLATFORM_DS
    NNS_G2dSetRendererImageProxy(list->renderer, &sprite->imageProxy, &sprite->paletteProxy);
    #else
    // TODO: Port NNS_G2dSetRendererImageProxy to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G2dBeginRendering(list->renderer);
    #else
    // TODO: Port NNS_G2dBeginRendering to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G2dPushMtx();
    #else
    // TODO: Port NNS_G2dPushMtx to PAL
    #endif

    #ifdef PLATFORM_DS
    NNS_G2dSetRndCoreAffineOverwriteMode(&list->renderer->rendererCore, sprite->affineOverwriteMode);
    #else
    // TODO: Port NNS_G2dSetRndCoreAffineOverwriteMode to PAL
    #endif

    #ifdef PLATFORM_DS
    if (sprite->affineOverwriteMode == NNS_G2D_RND_AFFINE_OVERWRITE_NONE) {
    #else
    // TODO: Port NNS_G2D_RND_AFFINE_OVERWRITE_NONE to PAL
    #endif
        #ifdef PLATFORM_DS
        NNS_G2dSetRndCoreFlipMode(
        #else
        // TODO: Port NNS_G2dSetRndCoreFlipMode to PAL
        #endif
            &list->renderer->rendererCore,
            #ifdef PLATFORM_DS
            sprite->flip & NNS_G2D_RENDERERFLIP_H,
            #else
            // TODO: Port NNS_G2D_RENDERERFLIP_H to PAL
            #endif
            #ifdef PLATFORM_DS
            sprite->flip & NNS_G2D_RENDERERFLIP_V);
            #else
            // TODO: Port NNS_G2D_RENDERERFLIP_V to PAL
            #endif
    } else {
        #ifdef PLATFORM_DS
        NNS_G2dSetRndCoreFlipMode(&list->renderer->rendererCore, FALSE, FALSE);
        #else
        // TODO: Port NNS_G2dSetRndCoreFlipMode to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    NNS_G2dTranslate(pos.x, pos.y, pos.z);
    #else
    // TODO: Port NNS_G2dTranslate to PAL
    #endif

    #ifdef PLATFORM_DS
    if (sprite->affineOverwriteMode != NNS_G2D_RND_AFFINE_OVERWRITE_NONE) {
    #else
    // TODO: Port NNS_G2D_RND_AFFINE_OVERWRITE_NONE to PAL
    #endif
        #ifdef PLATFORM_DS
        NNS_G2dTranslate(sprite->affineTranslation.x, sprite->affineTranslation.y, sprite->affineTranslation.z);
        #else
        // TODO: Port NNS_G2dTranslate to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G2dScale(sprite->affineScale.x, sprite->affineScale.y, sprite->affineScale.z);
        #else
        // TODO: Port NNS_G2dScale to PAL
        #endif
        #ifdef PLATFORM_DS
        NNS_G2dRotZ(FX_SinIdx(sprite->affineZRotation), FX_CosIdx(sprite->affineZRotation));
        #else
        // TODO: Port NNS_G2dRotZ to PAL
        #endif

        // affineTranslation only serves as a pivot point for rotation and scaling
        // so we undo this translation after applying these transformations.
        #ifdef PLATFORM_DS
        NNS_G2dTranslate(-sprite->affineTranslation.x, -sprite->affineTranslation.y, -sprite->affineTranslation.z);
        #else
        // TODO: Port NNS_G2dTranslate to PAL
        #endif
    }

    // Set the overwrite parameters
    // We always want the actual overwrite flags to be equal to overwriteParam
    // so we set the flags that are not in overwriteParam to 0 with the second call
    #ifdef PLATFORM_DS
    NNS_G2dSetRendererOverwriteEnable(list->renderer, sprite->overwriteFlags);
    #else
    // TODO: Port NNS_G2dSetRendererOverwriteEnable to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G2dSetRendererOverwriteDisable(list->renderer, ~sprite->overwriteFlags);
    #else
    // TODO: Port NNS_G2dSetRendererOverwriteDisable to PAL
    #endif

    #ifdef PLATFORM_DS
    NNS_G2dSetRendererOverwritePlttNo(list->renderer, sprite->explicitPalette);
    #else
    // TODO: Port NNS_G2dSetRendererOverwritePlttNo to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G2dSetRendererOverwritePlttNoOffset(list->renderer, sprite->explicitPaletteOffset);
    #else
    // TODO: Port NNS_G2dSetRendererOverwritePlttNoOffset to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G2dSetRendererOverwriteMosaicFlag(list->renderer, sprite->explicitMosaic);
    #else
    // TODO: Port NNS_G2dSetRendererOverwriteMosaicFlag to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G2dSetRendererOverwriteOBJMode(list->renderer, sprite->explicitOamMode);
    #else
    // TODO: Port NNS_G2dSetRendererOverwriteOBJMode to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G2dSetRendererOverwritePriority(list->renderer, sprite->explicitPriority);
    #else
    // TODO: Port NNS_G2dSetRendererOverwritePriority to PAL
    #endif

    if (sprite->type == CELL_ANIM_TYPE_CELL || sprite->type == CELL_ANIM_TYPE_VRAM_CELL) {
        CellAnimationData *cellAnim = (CellAnimationData *)&sprite->animData;
        #ifdef PLATFORM_DS
        NNS_G2dDrawCellAnimation(&cellAnim->anim);
        #else
        // TODO: Port NNS_G2dDrawCellAnimation to PAL
        #endif
    } else {
        MultiCellAnimationData *multiCellAnim = (MultiCellAnimationData *)&sprite->animData;
        #ifdef PLATFORM_DS
        NNS_G2dDrawMultiCellAnimation(&multiCellAnim->anim);
        #else
        // TODO: Port NNS_G2dDrawMultiCellAnimation to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    NNS_G2dPopMtx();
    #else
    // TODO: Port NNS_G2dPopMtx to PAL
    #endif
    #ifdef PLATFORM_DS
    NNS_G2dEndRendering();
    #else
    // TODO: Port NNS_G2dEndRendering to PAL
    #endif
}

static void SpriteList_DrawSprite_Stub(const SpriteList *list, Sprite *sprite)
{
    return;
}

static void Sprite_UpdateAnimInternal(Sprite *sprite)
{
    Sprite_UpdateAnim(sprite, sprite->animSpeed);
}

static void Sprite_UpdateAnimInternal_Stub(Sprite *sprite)
{
    return;
}

static void SpriteList_Insert(SpriteList *list, Sprite *sprite)
{
    // If the list is empty, insert the sprite as the only sprite
    if (list->sentinelData.next == &list->sentinelData) {
        list->sentinelData.next = sprite;
        list->sentinelData.prev = sprite;
        sprite->prev = &list->sentinelData;
        sprite->next = &list->sentinelData;
        return;
    }

    // Check if the sprite should be inserted at the end of the list (lowest priority)
    if (list->sentinelData.prev->priority <= sprite->priority) {
        sprite->prev = list->sentinelData.prev;
        list->sentinelData.prev->next = sprite;
        sprite->next = &list->sentinelData;
        list->sentinelData.prev = sprite;
        return;
    }

    // Insert the sprite in the correct position according to its priority
    Sprite *cur = list->sentinelData.next;
    while (cur != &list->sentinelData) {
        if (cur->priority > sprite->priority) {
            cur->prev->next = sprite;
            sprite->prev = cur->prev;
            cur->prev = sprite;
            sprite->next = cur;
            break;
        }

        cur = cur->next;
    }
}

static void SpriteList_Remove(Sprite *sprite)
{
    sprite->prev->next = sprite->next;
    sprite->next->prev = sprite->prev;
}

static void SpriteList_InitSprites(SpriteList *list)
{
    for (int i = 0; i < list->capacity; i++) {
        Sprite_Reset(&list->sprites[i]);
        list->freeSprites[i] = list->sprites + i;
    }

    list->freeSpriteHead = 0;
}

static Sprite *SpriteList_AllocSprite(SpriteList *list)
{
    if (list->freeSpriteHead >= list->capacity) {
        return NULL;
    }

    Sprite *sprite = list->freeSprites[list->freeSpriteHead];
    list->freeSpriteHead++;

    return sprite;
}

static BOOL SpriteList_FreeSprite(SpriteList *list, Sprite *sprite)
{
    if (list->freeSpriteHead <= 0) {
        return FALSE;
    }

    Sprite_Reset(sprite);

    list->freeSpriteHead--;
    list->freeSprites[list->freeSpriteHead] = sprite;

    return TRUE;
}
