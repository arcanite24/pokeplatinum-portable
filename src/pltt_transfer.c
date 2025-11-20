#include "pltt_transfer.h"

#include "nitro/gx/g3.h"
#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "constants/graphics.h"
#include "constants/heap.h"

#include "heap.h"
#include "vram_transfer.h"

#define NUM_VRAM_PALETTES     16
#define PLTT_RESOURCE_ID_NONE -1
#define PLTT_RANGE_SIZE       (PALETTE_SIZE_BYTES * NUM_VRAM_PALETTES)
#define PLTT_EXT_RANGE_SIZE   (PALETTE_SIZE_EXT_BYTES * NUM_VRAM_PALETTES)

typedef struct PlttTransferTask {
    NNSG2dPaletteData *data;
    #ifdef PLATFORM_DS
    NNS_G2D_VRAM_TYPE vramType;
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE to PAL
    #endif
    u32 numPalettes;
    u32 resourceID;
    NNSG2dImagePaletteProxy paletteProxy;
    u32 baseAddrMain;
    u32 baseAddrSub;
    u8 initialized;
} PlttTransferTask;

typedef struct PlttTransferTaskManager {
    PlttTransferTask *tasks;
    int capacity;
    int length;
    u32 offsetMain;
    u32 offsetSub;
    u32 extPlttOffsetMain;
    u32 extPlttOffsetSub;
    u32 extPlttVramSizeMain;
    u32 extPlttVramSizeSub;
    u16 vramTransferMain;
    u16 vramTransferSub;
} PlttTransferTaskManager;

static void InitTransferTask(PlttTransferTask *task);
static BOOL InitTransferTaskFromTemplate(const PlttTransferTaskTemplate *template, PlttTransferTask *task);
static void ResetTransferTask(PlttTransferTask *task);
static void LoadImagePalette(PlttTransferTask *task);
static BOOL ReserveAndTransferWholeRange(const PlttTransferTaskTemplate *unused, PlttTransferTask *task);
static BOOL ReserveAndTransferFreeSpace(const PlttTransferTaskTemplate *unused, PlttTransferTask *task);
static void ReserveTaskTransferRanges(PlttTransferTask *task);
static void ClearTaskTransferRanges(PlttTransferTask *task);
static PlttTransferTask *FindTransferTask(int resourceID);
static PlttTransferTask *FindNextFreeTask(void);
static void UpdateVramCapacities(void);
static void UpdateTransferSize(PlttTransferTask *task);
static BOOL TryGetDestOffsets(PlttTransferTask *task, u32 offsetMain, u32 offsetSub, u32 sizeMain, u32 sizeSub);
static void ReserveVramSpace(PlttTransferTask *task, u32 *offsetMain, u32 *offsetSub);

static void ReserveTransferRange(u16 *range, int count, int start);
static void ClearTransferRange(u16 *range, int count, int start);
static int FindAvailableTransferSlot(u16 palette, int size);

static void ResetBothTransferRanges(PlttTransferTaskManager *task);

static PlttTransferTaskManager *sTaskManager = NULL;

void PlttTransfer_Init(int capacity, enum HeapID heapID)
{
    if (sTaskManager == NULL) {
        sTaskManager = Heap_Alloc(heapID, sizeof(PlttTransferTaskManager));
        MI_CpuClear32(sTaskManager, sizeof(PlttTransferTaskManager));

        sTaskManager->capacity = capacity;
        sTaskManager->tasks = Heap_Alloc(heapID, sizeof(PlttTransferTask) * capacity);

        for (int i = 0; i < capacity; i++) {
            InitTransferTask(sTaskManager->tasks + i);
        }
    }
}

#ifdef PLATFORM_DS
void PlttTransfer_MarkReservedSlots(u16 reservedMask, NNS_G2D_VRAM_TYPE vramType)
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
{
    #ifdef PLATFORM_DS
    if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        sTaskManager->vramTransferMain |= reservedMask;
    #ifdef PLATFORM_DS
    } else if (vramType == NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        sTaskManager->vramTransferSub |= reservedMask;
    }
}

void PlttTransfer_Free(void)
{
    if (sTaskManager != NULL) {
        PlttTransfer_ResetAllTasks();
        Heap_Free(sTaskManager->tasks);
        Heap_Free(sTaskManager);
        sTaskManager = NULL;
    }
}

static void DummyFunc(void)
{
    return;
}

void PlttTransfer_Clear(void)
{
    sTaskManager->offsetMain = 0;
    sTaskManager->offsetSub = 0;
    sTaskManager->extPlttOffsetMain = 0;
    sTaskManager->extPlttOffsetSub = 0;

    UpdateVramCapacities();
    ResetBothTransferRanges(sTaskManager);
}

BOOL PlttTransfer_RequestWholeRange(const PlttTransferTaskTemplate *template)
{
    PlttTransferTask *task = FindNextFreeTask();
    if (task == NULL) {
        GF_ASSERT(FALSE);
        return FALSE;
    }

    if (InitTransferTaskFromTemplate(template, task) == FALSE) {
        return FALSE;
    }

    if (ReserveAndTransferWholeRange(template, task) == FALSE) {
        // unreachable
        PlttTransfer_ResetTask(template->resourceID);
        return FALSE;
    }

    ReserveTaskTransferRanges(task);
    return TRUE;
}

BOOL PlttTransfer_RequestFreeSpace(const PlttTransferTaskTemplate *template)
{
    PlttTransferTask *task = FindNextFreeTask();
    if (task == NULL) {
        GF_ASSERT(FALSE);
        return FALSE;
    }

    if (InitTransferTaskFromTemplate(template, task) == FALSE) {
        return FALSE;
    }

    if (ReserveAndTransferFreeSpace(template, task) == FALSE) {
        PlttTransfer_ResetTask(template->resourceID);
        return FALSE;
    }

    return TRUE;
}

void PlttTransfer_ReplacePlttData(int resourceID, NNSG2dPaletteData *data)
{
    GF_ASSERT(data);

    PlttTransferTask *task = FindTransferTask(resourceID);
    GF_ASSERT(task);
    task->data = data;

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        #ifdef PLATFORM_DS
        VramTransfer_Request(NNS_GFD_DST_2D_OBJ_PLTT_MAIN, task->baseAddrMain, data->pRawData, task->numPalettes * PALETTE_SIZE_BYTES);
        #else
        // TODO: Port NNS_GFD_DST_2D_OBJ_PLTT_MAIN to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        #ifdef PLATFORM_DS
        VramTransfer_Request(NNS_GFD_DST_2D_OBJ_PLTT_SUB, task->baseAddrSub, data->pRawData, task->numPalettes * PALETTE_SIZE_BYTES);
        #else
        // TODO: Port NNS_GFD_DST_2D_OBJ_PLTT_SUB to PAL
        #endif
    }
}

BOOL PlttTransfer_HasTask(int resourceID)
{
    return FindTransferTask(resourceID) != NULL;
}

void PlttTransfer_ResetTask(int resourceID)
{
    PlttTransferTask *task = FindTransferTask(resourceID);
    GF_ASSERT(task);

    if (task->initialized == TRUE) {
        ClearTaskTransferRanges(task);
        ResetTransferTask(task);
    }
}

void PlttTransfer_ResetAllTasks(void)
{
    for (int i = 0; i < sTaskManager->capacity; i++) {
        if (sTaskManager->tasks[i].initialized == TRUE) {
            ClearTaskTransferRanges(&sTaskManager->tasks[i]);
            ResetTransferTask(&sTaskManager->tasks[i]);
        }
    }
}

NNSG2dImagePaletteProxy *PlttTransfer_GetPaletteProxy(int resourceID)
{
    PlttTransferTask *task = FindTransferTask(resourceID);
    if (task == NULL) {
        GF_ASSERT(task);
        return NULL;
    }

    if (task->initialized == TRUE) {
        return &task->paletteProxy;
    }

    return NULL;
}

NNSG2dImagePaletteProxy *PlttTransfer_ToggleExtPalette(int resourceID, NNSG2dImageProxy *imageProxy)
{
    PlttTransferTask *task = FindTransferTask(resourceID);
    if (task == NULL) {
        GF_ASSERT(task);
        return NULL;
    }

    if (task->initialized != TRUE) {
        return NULL;
    }

    if (task->data->bExtendedPlt) {
        #ifdef PLATFORM_DS
        NNS_G2dSetImageExtPaletteFlag(imageProxy, TRUE);
        #else
        // TODO: Port NNS_G2dSetImageExtPaletteFlag to PAL
        #endif
    }

    return &task->paletteProxy;
}

#ifdef PLATFORM_DS
u32 PlttTransfer_GetPlttOffset(const NNSG2dImagePaletteProxy *paletteProxy, NNS_G2D_VRAM_TYPE vramType)
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
{
    u32 size;
    if (paletteProxy->bExtendedPlt) {
        size = PALETTE_SIZE_EXT_BYTES;
    #ifdef PLATFORM_DS
    } else if (paletteProxy->fmt == GX_TEXFMT_PLTT256) {
    #else
    // TODO: Port GX_TEXFMT_PLTT256 to PAL
    #endif
        size = 0;
    } else {
        size = PALETTE_SIZE_BYTES;
    }

    #ifdef PLATFORM_DS
    return size != 0 ? NNS_G2dGetImagePaletteLocation(paletteProxy, vramType) / size : 0;
    #else
    // TODO: Port NNS_G2dGetImagePaletteLocation to PAL
    #endif
}

static void ResetTransferTask(PlttTransferTask *task)
{
    InitTransferTask(task);
}

static BOOL InitTransferTaskFromTemplate(const PlttTransferTaskTemplate *template, PlttTransferTask *task)
{
    task->data = template->data;

    if (PlttTransfer_HasTask(template->resourceID) == TRUE) {
        GF_ASSERT(FALSE);
        return FALSE;
    }

    task->resourceID = template->resourceID;
    task->vramType = template->vramType;
    task->initialized = TRUE;
    task->numPalettes = template->plttIndex;
    return TRUE;
}

static void InitTransferTask(PlttTransferTask *task)
{
    memset(task, 0, sizeof(PlttTransferTask));
    task->resourceID = PLTT_RESOURCE_ID_NONE;
    #ifdef PLATFORM_DS
    NNS_G2dInitImagePaletteProxy(&task->paletteProxy);
    #else
    // TODO: Port NNS_G2dInitImagePaletteProxy to PAL
    #endif
}

static BOOL ReserveAndTransferWholeRange(const PlttTransferTaskTemplate *unused, PlttTransferTask *task)
{
    u32 *targetOffsetMain;
    u32 *targetOffsetSub;
    u32 plttSizeMain;
    u32 plttSizeSub;

    if (task->data->bExtendedPlt) {
        targetOffsetMain = &sTaskManager->extPlttOffsetMain;
        targetOffsetSub = &sTaskManager->extPlttOffsetSub;
        plttSizeMain = sTaskManager->extPlttVramSizeMain;
        plttSizeSub = sTaskManager->extPlttVramSizeSub;
    } else {
        targetOffsetMain = &sTaskManager->offsetMain;
        targetOffsetSub = &sTaskManager->offsetSub;
        plttSizeMain = PLTT_RANGE_SIZE;
        plttSizeSub = PLTT_RANGE_SIZE;
    }

    TryGetDestOffsets(task, *targetOffsetMain, *targetOffsetSub, plttSizeMain, plttSizeSub);
    UpdateTransferSize(task);
    ReserveVramSpace(task, targetOffsetMain, targetOffsetSub);
    return TRUE;
}

static BOOL ReserveAndTransferFreeSpace(const PlttTransferTaskTemplate *unused, PlttTransferTask *task)
{
    if (task->data->bExtendedPlt) {
        GF_ASSERT(FALSE);
    }

    int offsetMain;
    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        offsetMain = FindAvailableTransferSlot(sTaskManager->vramTransferMain, task->numPalettes);
        if (offsetMain == -1) {
            return FALSE;
        }
    }

    int offsetSub;
    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        offsetSub = FindAvailableTransferSlot(sTaskManager->vramTransferSub, task->numPalettes);
        if (offsetSub == -1) {
            return FALSE;
        }
    }

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        task->baseAddrMain = offsetMain;
    }

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        task->baseAddrSub = offsetSub;
    }

    task->data->szByte = task->numPalettes * PALETTE_SIZE_BYTES;
    LoadImagePalette(task);
    ReserveTaskTransferRanges(task);
    return TRUE;
}

static PlttTransferTask *FindTransferTask(int resourceID)
{
    for (int i = 0; i < sTaskManager->capacity; i++) {
        if (sTaskManager->tasks[i].resourceID == resourceID) {
            return sTaskManager->tasks + i;
        }
    }

    return NULL;
}

static PlttTransferTask *FindNextFreeTask(void)
{
    for (int i = 0; i < sTaskManager->capacity; i++) {
        if (sTaskManager->tasks[i].initialized == FALSE) {
            return sTaskManager->tasks + i;
        }
    }

    return NULL;
}

static void UpdateVramCapacities(void)
{
    #ifdef PLATFORM_DS
    switch (GX_GetBankForOBJExtPltt()) {
    #else
    // TODO: Port GX_GetBankForOBJExtPltt to PAL
    #endif
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJEXTPLTT_0_F:
    #else
    // TODO: Port GX_VRAM_OBJEXTPLTT_0_F to PAL
    #endif
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJEXTPLTT_0_G:
    #else
    // TODO: Port GX_VRAM_OBJEXTPLTT_0_G to PAL
    #endif
        sTaskManager->extPlttVramSizeMain = PLTT_EXT_RANGE_SIZE;
        break;
    default:
        sTaskManager->extPlttVramSizeMain = 0;
        break;
    }

    #ifdef PLATFORM_DS
    switch (GX_GetBankForSubOBJExtPltt()) {
    #else
    // TODO: Port GX_GetBankForSubOBJExtPltt to PAL
    #endif
    #ifdef PLATFORM_DS
    case GX_VRAM_SUB_OBJEXTPLTT_0_I:
    #else
    // TODO: Port GX_VRAM_SUB_OBJEXTPLTT_0_I to PAL
    #endif
        sTaskManager->extPlttVramSizeSub = PLTT_EXT_RANGE_SIZE;
        break;
    default:
        sTaskManager->extPlttVramSizeSub = 0;
        break;
    }
}

static void UpdateTransferSize(PlttTransferTask *task)
{
    task->data->szByte = task->numPalettes * PALETTE_SIZE_BYTES;
    LoadImagePalette(task);
}

static void LoadImagePalette(PlttTransferTask *task)
{
    #ifdef PLATFORM_DS
    NNS_G2dInitImagePaletteProxy(&task->paletteProxy);
    #else
    // TODO: Port NNS_G2dInitImagePaletteProxy to PAL
    #endif
    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port NNS_G2dLoadPalette to PAL
        #endif
        NNS_G2dLoadPalette(task->data, task->baseAddrMain, NNS_G2D_VRAM_TYPE_2DMAIN, &task->paletteProxy);
        #else
        // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port NNS_G2dLoadPalette to PAL
        #endif
        NNS_G2dLoadPalette(task->data, task->baseAddrSub, NNS_G2D_VRAM_TYPE_2DSUB, &task->paletteProxy);
        #else
        // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
        #endif
    }
}

static void ReserveTransferRange(u16 *range, int count, int start)
{
    for (int i = 0; i < count; i++) {
        *range |= 1 << (start + i);
    }
}

static void ClearTransferRange(u16 *range, int count, int start)
{
    for (int i = 0; i < count; i++) {
        *range &= ~(1 << (start + i));
    }
}

static int FindAvailableTransferSlot(u16 palette, int size)
{
    int slot;
    for (slot = 0; slot < NUM_VRAM_PALETTES; slot++) {
        int colorIdx = 0;
        while ((palette & (1 << (slot + colorIdx))) == 0 && colorIdx < size) {
            if (slot + colorIdx >= NUM_VRAM_PALETTES) {
                break;
            }
            colorIdx++;
        }

        if (colorIdx >= size) {
            break;
        }

        slot += colorIdx;
    }

    if (slot >= NUM_VRAM_PALETTES) {
        return -1;
    }

    return PLTT_OFFSET(slot);
}

static void ResetBothTransferRanges(PlttTransferTaskManager *manager)
{
    manager->vramTransferMain = 0;
    manager->vramTransferSub = 0;
}

static void ReserveTaskTransferRanges(PlttTransferTask *task)
{
    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        ReserveTransferRange(&sTaskManager->vramTransferMain, task->numPalettes, task->baseAddrMain / PALETTE_SIZE_BYTES);
    }

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        ReserveTransferRange(&sTaskManager->vramTransferSub, task->numPalettes, task->baseAddrSub / PALETTE_SIZE_BYTES);
    }
}

static void ClearTaskTransferRanges(PlttTransferTask *task)
{
    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        ClearTransferRange(&sTaskManager->vramTransferMain, task->numPalettes, task->baseAddrMain / PALETTE_SIZE_BYTES);
    }

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        ClearTransferRange(&sTaskManager->vramTransferSub, task->numPalettes, task->baseAddrSub / PALETTE_SIZE_BYTES);
    }
}

static BOOL TryGetDestOffsets(PlttTransferTask *task, u32 offsetMain, u32 offsetSub, u32 sizeMain, u32 sizeSub)
{
    BOOL result = TRUE;

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        if (offsetMain + (task->numPalettes * PALETTE_SIZE_BYTES) > sizeMain) {
            GF_ASSERT(FALSE);
            DummyFunc();
            result = FALSE;
        } else {
            task->baseAddrMain = offsetMain;
        }
    }

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        if (offsetSub + (task->numPalettes * PALETTE_SIZE_BYTES) > sizeSub) {
            GF_ASSERT(FALSE);
            DummyFunc();
            result = FALSE;
        } else {
            task->baseAddrSub = offsetSub;
        }
    }

    return result;
}

static void ReserveVramSpace(PlttTransferTask *task, u32 *offsetMain, u32 *offsetSub)
{
    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        *offsetMain += task->numPalettes * PALETTE_SIZE_BYTES;
    }

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        *offsetSub += task->numPalettes * PALETTE_SIZE_BYTES;
    }
}
