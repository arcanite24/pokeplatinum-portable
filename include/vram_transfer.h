#ifndef POKEPLATINUM_VRAM_TRANSFER_H
#define POKEPLATINUM_VRAM_TRANSFER_H

#include "platform/platform_types.h"

#include "constants/heap.h"

void VramTransfer_New(u32 capacity, enum HeapID heapID);
void VramTransfer_Free(void);
#ifdef PLATFORM_DS
BOOL VramTransfer_Request(NNS_GFD_DST_TYPE type, u32 destAddr, void *buf, u32 size);
#else
// TODO: Port NNS_GFD_DST_TYPE to PAL
#endif
void VramTransfer_Process(void);

#endif // POKEPLATINUM_VRAM_TRANSFER_H
