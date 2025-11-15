#ifndef POKEPLATINUM_CELL_TRANSFER_H
#define POKEPLATINUM_CELL_TRANSFER_H

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
#include "platform/platform_types.h"
#include "nns_types.h"
/* NNSG2d/NNSG3d types already defined in nns_types.h */
#endif

#include "constants/heap.h"

NNSG2dCellTransferState *CellTransfer_New(int capacity, enum HeapID heapID);
void CellTransfer_Update(void);
void CellTransfer_Free(NNSG2dCellTransferState *transferStates);

#endif // POKEPLATINUM_CELL_TRANSFER_H
