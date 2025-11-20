#ifndef POKEPLATINUM_STRUCT_SENTENCE_H
#define POKEPLATINUM_STRUCT_SENTENCE_H

#ifndef PLATFORM_DS
#include "platform/platform_types.h"

#include "platform/platform_types.h"
#endif

typedef struct {
    u16 type;
    u16 id;
    u16 words[2];
} Sentence;

#endif // POKEPLATINUM_STRUCT_SENTENCE_H
