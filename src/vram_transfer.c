#include "vram_transfer.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <nnsys.h>
#include <string.h>

#include "constants/heap.h"

#include "heap.h"

typedef struct VramTransferTaskManager {
    u32 max;
    u32 cur;
    NNSGfdVramTransferTask *tasks;
} VramTransferTaskManager;

static VramTransferTaskManager *sTransferTaskManager;

void VramTransfer_New(u32 capacity, enum HeapID heapID)
{
    GF_ASSERT(sTransferTaskManager == NULL);
    sTransferTaskManager = Heap_Alloc(heapID, sizeof(VramTransferTaskManager));
    GF_ASSERT(sTransferTaskManager);

    sTransferTaskManager->tasks = Heap_Alloc(heapID, sizeof(NNSGfdVramTransferTask) * capacity);
    sTransferTaskManager->max = capacity;
    sTransferTaskManager->cur = 0;

    #ifdef PLATFORM_DS
    NNS_GfdInitVramTransferManager(sTransferTaskManager->tasks, sTransferTaskManager->max);
    #else
    // TODO: Port NNS_GfdInitVramTransferManager to PAL
    #endif
}

void VramTransfer_Free(void)
{
    GF_ASSERT(sTransferTaskManager != NULL);

    Heap_Free(sTransferTaskManager->tasks);
    Heap_Free(sTransferTaskManager);

    sTransferTaskManager = NULL;
}

#ifdef PLATFORM_DS
BOOL VramTransfer_Request(NNS_GFD_DST_TYPE type, u32 destAddr, void *buf, u32 size)
#else
// TODO: Port NNS_GFD_DST_TYPE to PAL
#endif
{
    GF_ASSERT(sTransferTaskManager);
    sTransferTaskManager->cur++;

    if (sTransferTaskManager->cur >= sTransferTaskManager->max) {
        GF_ASSERT(FALSE);
        return FALSE;
    }

    #ifdef PLATFORM_DS
    return NNS_GfdRegisterNewVramTransferTask(type, destAddr, buf, size);
    #else
    // TODO: Port NNS_GfdRegisterNewVramTransferTask to PAL
    #endif
}

void VramTransfer_Process(void)
{
    if (sTransferTaskManager) {
        #ifdef PLATFORM_DS
        NNS_GfdDoVramTransfer();
        #else
        // TODO: Port NNS_GfdDoVramTransfer to PAL
        #endif
        sTransferTaskManager->cur = 0;
    }
}
