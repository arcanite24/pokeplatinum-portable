#ifndef POKEPLATINUM_STRUCT_02073838_H
#define POKEPLATINUM_STRUCT_02073838_H

#include "platform/platform_types.h"

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
#include "nns_types.h"  /* SDL: Common NNS type stubs */
#endif

typedef struct {
    BOOL unk_00;
    NNSG3dResFileHeader *unk_04;
    NNSG3dResMdlSet *unk_08;
    NNSG3dResMdl *unk_0C;
    NNSG3dResTex *unk_10;
} UnkStruct_02073838;

#endif // POKEPLATINUM_STRUCT_02073838_H
