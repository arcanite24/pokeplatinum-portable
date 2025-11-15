#ifndef POKEPLATINUM_MAIN_H
#define POKEPLATINUM_MAIN_H

#ifdef PLATFORM_DS
#include <nitro/fs.h>
#else
#include "platform/platform_types.h"
// SDL stub - FS_EXTERN_OVERLAY declares extern overlay ID
#define FS_EXTERN_OVERLAY(name) extern const FSOverlayID FS_OVERLAY_ID_##name
// FS_OVERLAY_ID macro returns the overlay ID variable
#define FS_OVERLAY_ID(name) FS_OVERLAY_ID_##name
// Note: FS_OVERLAY_ID_NONE already defined in overlay_manager.h
#endif

#include "overlay_manager.h"
#include "savedata.h"

enum OSResetParameter {
    RESET_CLEAN = 0,
    RESET_ERROR,
};

typedef struct ApplicationArgs {
    int unk_00;
    BOOL error;
    SaveData *saveData;
} ApplicationArgs;

void EnqueueApplication(FSOverlayID overlayID, const ApplicationManagerTemplate *template);
void InitRNG(void);
void HandleConsoleFold(void);

#endif // POKEPLATINUM_MAIN_H
