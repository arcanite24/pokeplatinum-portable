#ifndef POKEPLATINUM_POKETCH_GRAPHICS_H
#define POKEPLATINUM_POKETCH_GRAPHICS_H

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

#include "applications/poketch/poketch_animation.h"
#include "applications/poketch/poketch_system.h"

#include "bg_window.h"

#define POKETCH_WIDTH_TILES  32
#define POKETCH_HEIGHT_TILES 24

#define POKETCH_SCREEN_TILE_OFFSET_X 2
#define POKETCH_SCREEN_TILE_OFFSET_Y 2

#define POKETCH_DIGITS_NCGR_NUM_TILES 80

enum PoketchGraphicsTask {
    TASK_SETUP_BACKGROUND = 0,
    TASK_REVEAL_SCREEN_1,
    TASK_REVEAL_SCREEN_2,
    TASK_UNUSED_1,
    TASK_CONCEAL_SCREEN,
    TASK_UNUSED_2,
    TASK_UP_HALF_PRESSED,
    TASK_UP_RELEASED,
    TASK_UP_PRESSED,
    TASK_DOWN_HALF_PRESSED,
    TASK_DOWN_RELEASED,
    TASK_DOWN_PRESSED,
    TASK_LOAD_APP_COUNTER,
    TASK_UPDATE_APP_COUNTER,
    TASK_UNLOAD_APP_COUNTER,
    TASK_UNUSED_3,
    TASK_UNUSED_4,
    TASK_FREE_TILEMAP,
};

BOOL PoketchGraphics_Main(PoketchGraphics_TaskData **taskDataPtr, const PoketchGraphics_ConstTaskData *constTaskData, NNSG2dOamManagerInstance *oamMan, PoketchSystem *poketchSys);
PoketchAnimation_AnimationManager *PoketchGraphics_GetAnimationManager(void);
BgConfig *PoketchGraphics_GetBgConfig(void);
void PoketchGraphics_LoadActivePalette(u32 bgOffset, u32 objOffset);
void PoketchGraphics_LoadActiveBacklightPalette(u32, u32);
void PoketchGraphics_CopyActivePalette(u16 *dest);
void PoketchGraphics_Close(PoketchGraphics_TaskData *taskData);
void PoketchGraphics_StartTask(PoketchGraphics_TaskData *taskData, u32 taskId);
BOOL PoketchGraphics_TaskIsNotActive(PoketchGraphics_TaskData *taskData, u32 taskId);
BOOL PoketchGraphics_NoActiveTasks(PoketchGraphics_TaskData *taskData);

#endif // POKEPLATINUM_POKETCH_GRAPHICS_H
