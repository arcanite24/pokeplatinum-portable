#ifndef POKEPLATINUM_OV5_AREA_LIGHT_H
#define POKEPLATINUM_OV5_AREA_LIGHT_H

#include <nitro/fx/fx.h>
#ifdef PLATFORM_DS
#include <nitro/gx.h>
#else
#include "platform/platform_types.h"
/* GX types from enums.h already define blend/window masks */
typedef u16 GXRgb;
typedef struct { u8 r, g, b, a; } GXRgba;
typedef struct { void* data; } GXOamAttr;
#endif
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

#include "constants/graphics.h"

#include "overlay005/model_attributes.h"

typedef struct AreaLightTemplate {
    u32 endTime;
    u8 validLightsMask;
    GXRgb lightColors[GX_LIGHTS_COUNT];
    VecFx16 lightVectors[GX_LIGHTS_COUNT];
    GXRgb diffuseReflectColor;
    GXRgb ambientReflectColor;
    GXRgb specularReflectColor;
    GXRgb emissionColor;
} AreaLightTemplate;

typedef struct AreaLightManager {
    u32 templateCount;
    AreaLightTemplate *templates;
    int activeTemplateIndex;
    ModelAttributes *areaModelAttrs;
    BOOL applyToAreaModelAttrs;
} AreaLightManager;

AreaLightManager *AreaLightManager_New(ModelAttributes *areaModelAttrs, const u8 archiveID);
void AreaLightManager_Free(AreaLightManager **areaLightMan);
void AreaLightManager_UpdateActiveTemplate(AreaLightManager *areaLightMan);
void AreaLightTemplate_ApplyToModelAttributes(const AreaLightTemplate *template, ModelAttributes *modelAttrs);
void AreaLight_UseGlobalModelAttributes(NNSG3dResMdl *model);

#endif // POKEPLATINUM_OV5_AREA_LIGHT_H
