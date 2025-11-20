#ifndef POKEPLATINUM_ASCII_UTIL_H
#define POKEPLATINUM_ASCII_UTIL_H

#include "platform/platform_types.h"

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
// SDL stub - NNSG3dResName type
#include "platform/platform_types.h"
#define NNS_G3D_RESNAME_VALSIZE 16
typedef struct {
    union {
        u8 val[NNS_G3D_RESNAME_VALSIZE];
        char name[NNS_G3D_RESNAME_VALSIZE];
    };
} NNSG3dResName;
#endif

#include "charcode.h"

#define MAX_STRING_COPY_LEN 256

int Ascii_Length(char *str);
char *Ascii_CopyToTerminator(char *src, char *dst, char terminator);
int Ascii_ConvertToInt(char *str);
void Ascii_SetResourceName(NNSG3dResName *resource, const char *src);
BOOL IsFullWidthChar(charcode_t character);

#endif // POKEPLATINUM_ASCII_UTIL_H
