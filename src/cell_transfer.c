#include "cell_transfer.h"

#ifdef PLATFORM_DS
#include <nnsys.h>
#else
#include "platform/platform_types.h"
#endif

#include "constants/heap.h"

#include "heap.h"
#include "vram_transfer.h"

#ifdef PLATFORM_DS

static BOOL RegisterTransferTaskCB(NNS_GFD_DST_TYPE type, u32 destAddr, void *buf, u32 size);

NNSG2dCellTransferState *CellTransfer_New(int capacity, enum HeapID heapID)
{
    NNSG2dCellTransferState *transferStates = Heap_Alloc(heapID, sizeof(NNSG2dCellTransferState) * capacity);
    NNS_G2dInitCellTransferStateManager(transferStates, capacity, RegisterTransferTaskCB);
    return transferStates;
}

void CellTransfer_Update(void)
{
    NNS_G2dUpdateCellTransferStateManager();
}

void CellTransfer_Free(NNSG2dCellTransferState *transferStates)
{
    Heap_Free(transferStates);
}

static BOOL RegisterTransferTaskCB(NNS_GFD_DST_TYPE type, u32 destAddr, void *buf, u32 size)
{
    return VramTransfer_Request(type, destAddr, buf, size);
}

#else // PLATFORM_SDL

// SDL: Cell transfer stubs - not needed for SDL rendering
NNSG2dCellTransferState *CellTransfer_New(int capacity, enum HeapID heapID)
{
    (void)capacity;
    (void)heapID;
    return NULL;
}

void CellTransfer_Update(void)
{
    // SDL: No-op
}

void CellTransfer_Free(NNSG2dCellTransferState *transferStates)
{
    (void)transferStates;
    // SDL: No-op
}

#endif // PLATFORM_DS
