#ifndef POKEPLATINUM_STRUCT_BOX_CUSTOMIZATION_H
#define POKEPLATINUM_STRUCT_BOX_CUSTOMIZATION_H

#include "platform/platform_types.h"

#include "strbuf.h"

typedef struct {
    u8 boxID;
    u8 wallpaper;
    u8 padding_02[2];
    Strbuf *name;
} BoxCustomization;

#endif // POKEPLATINUM_STRUCT_BOX_CUSTOMIZATION_H
