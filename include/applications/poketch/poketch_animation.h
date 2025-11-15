#ifndef POKEPLATINUM_POKETCH_ANIMATION_H
#define POKEPLATINUM_POKETCH_ANIMATION_H

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
#include "platform/platform_types.h"

/* Common NNSG2d types */
typedef struct { void* data; } NNSG2dImageProxy;
typedef struct { void* data; } NNSG2dImagePaletteProxy;
typedef struct { void* data; } NNSG2dCharacterData;
typedef struct { void* data; } NNSG2dPaletteData;
typedef struct { s16 x, y; } NNSG2dSVec2;
typedef struct { s32 x, y; } NNSG2dLVec2;
typedef struct { fx32 x, y; } NNSG2dFVec2;
typedef struct { void* data; } NNSG2dImageAttr;

/* Common NNSG3d types */
typedef struct { u8 val[16]; char name[16]; } NNSG3dResName;
typedef struct { void* data; } NNSG3dResMdl;
typedef struct { void* data; } NNSG3dResTex;
typedef struct { void* data; } NNSG3dRenderObj;

/* GX color types */
typedef u16 GXRgb;
typedef struct { u8 r, g, b; } GXRgba;
#endif

#include "constants/heap.h"
#include "constants/narc.h"

typedef struct PoketchAnimation_AnimationManager PoketchAnimation_AnimationManager;
typedef struct PoketchAnimation_AnimatedSpriteData PoketchAnimation_AnimatedSpriteData;

typedef struct PoketchAnimation_AnimationData {
    NNSG2dFVec2 translation;
    u16 animIdx;
    u8 flip;
    u8 oamPriority;
    u8 priority;
    u8 hasAffineTransform;
} PoketchAnimation_AnimationData;

typedef struct PoketchAnimation_SpriteData {
    void *compressedSprite;
    void *compressedAnim;
    NNSG2dCellDataBank *cell;
    NNSG2dAnimBankData *anim;
    u32 heapID;
} PoketchAnimation_SpriteData;

PoketchAnimation_AnimationManager *PoketchAnimation_SetupAnimationManager(NNSG2dOamManagerInstance *oamMan, u32 heapID);
void PoketchAnimation_FreeAnimationManager(PoketchAnimation_AnimationManager *animMan);
void PoketchAnimation_UpdateAnimations(PoketchAnimation_AnimationManager *animMan);
PoketchAnimation_AnimatedSpriteData *PoketchAnimation_SetupNewAnimatedSprite(PoketchAnimation_AnimationManager *animMan, const PoketchAnimation_AnimationData *animData, const PoketchAnimation_SpriteData *spriteData);
void PoketchAnimation_RemoveAnimatedSprite(PoketchAnimation_AnimationManager *animMan, PoketchAnimation_AnimatedSpriteData *animatedSprite);
void PoketchAnimation_UpdateAnimationIdx(PoketchAnimation_AnimatedSpriteData *animatedSprite, u32 animIdx);
BOOL PoketchAnimation_AnimationInactive(PoketchAnimation_AnimatedSpriteData *animatedSprite);
void PoketchAnimation_TranslateSprite(PoketchAnimation_AnimatedSpriteData *animatedSprite, fx32 x, fx32 y);
void PoketchAnimation_SetSpritePosition(PoketchAnimation_AnimatedSpriteData *animatedSprite, fx32 x, fx32 y);
void PoketchAnimation_GetSpritePosition(const PoketchAnimation_AnimatedSpriteData *animatedSprite, fx32 *x, fx32 *y);
void PoketchAnimation_HideSprite(PoketchAnimation_AnimatedSpriteData *animatedSprite, BOOL isHidden);
void PoketchAnimation_SetSpritePrority(PoketchAnimation_AnimationManager *animMan, PoketchAnimation_AnimatedSpriteData *animatedSprite, u32 priority);
void PoketchAnimation_SetCParam(PoketchAnimation_AnimatedSpriteData *animatedSprite, u32 value);
void PoketchAnimation_SetSpriteCharNo(PoketchAnimation_AnimatedSpriteData *animatedSprite, u32 value);
void PoketchAnimation_SetMosaic(PoketchAnimation_AnimatedSpriteData *animatedSprite, BOOL isMosaic);
void PoketchAnimation_SetSpriteRotation(PoketchAnimation_AnimatedSpriteData *animatedSprite, u16 rotation);
BOOL PoketchAnimation_LoadSpriteFromNARC(PoketchAnimation_SpriteData *spriteData, enum NarcID narcID, u32 spriteID, u32 animID, enum HeapID heapID);
void PoketchAnimation_FreeSpriteData(PoketchAnimation_SpriteData *spriteData);

#endif // POKEPLATINUM_POKETCH_ANIMATION_H
