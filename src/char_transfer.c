#include "char_transfer.h"

#ifdef PLATFORM_DS
#include <nitro/gx.h>
#include <nnsys.h>
#else
#include "platform/platform_types.h"
#endif

#include "heap.h"
#include "vram_transfer.h"

#ifdef PLATFORM_DS

#define CHAR_RESOURCE_ID_NONE -1
#define CHAR_TRANSFER_SHIFT   8

enum CharTransferTaskState {
    CTT_INIT = 0,
    CTT_IN_PROGRESS,
    CTT_WAITING_FOR_VRAM,
    CTT_VRAM_READY,
    CTT_VRAM_COPIED,
};

typedef struct CharTransferTask {
    NNSG2dCharacterData *data;
    NNS_G2D_VRAM_TYPE vramType;
    u8 done;
    u32 resourceID;
    NNSG2dImageProxy imageProxy;
    u32 baseAddrMain;
    u32 baseAddrSub;
    u8 state;
    BOOL useHardwareMappingType;
    BOOL atEnd;
    BOOL hasRange;
    u32 regionSizeMain;
    u32 regionSizeSub;
} CharTransferTask;

typedef struct CharTransferTaskManager {
    CharTransferTask *tasks;
    int capacity;
    int length;
    u32 offsetMain;
    u32 offsetSub;
    s32 vramSizeMain;
    s32 vramSizeSub;
    s32 freeSizeMain;
    s32 freeSizeSub;
    u32 numBlocksMain;
    u32 numBlocksSub;
    u32 blockSizeMain;
    u32 blockSizeSub;
    u8 *bufMain;
    u8 *bufSub;
} CharTransferTaskManager;

static void InitTransferTask(CharTransferTask *task);
static BOOL InitTransferTaskFromTemplate(const CharTransferTaskTemplate *template, CharTransferTask *task);
static void ResetTransferTask(CharTransferTask *task);
static BOOL ReserveAndTransfer(CharTransferTask *task);
static BOOL ReserveAndTransferFromHead(CharTransferTask *task);
static BOOL ReserveAndTransferFromTail(CharTransferTask *task);
#ifdef PLATFORM_DS
static void ReserveTransferRangeByOffsetAndSize(NNS_G2D_VRAM_TYPE vramType, u32 offsetMain, u32 offsetSub, u32 sizeMain, u32 sizeSub);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
static void ClearTransferTaskRange(CharTransferTask *task);
static CharTransferTask *FindNextFreeTask(void);
static CharTransferTask *FindTransferTaskByImageProxy(const NNSG2dImageProxy *search);
static CharTransferTask *FindTransferTaskByResourceID(int resourceID);
#ifdef PLATFORM_DS
static GXOBJVRamModeChar UpdateMappingTypeFromHardware(CharTransferTask *task, NNS_G2D_VRAM_TYPE vramType);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
static void SetBaseAddresses(CharTransferTask *task, u32 baseAddrMain, u32 baseAddrSub);
static void UpdateBaseAddresses(CharTransferTask *task, u32 offsetMain, u32 offsetSub);
static void UpdateVramCapacities(void);
static BOOL TryGetFreeTransferSpace(int vramType, u32 *outOffsetMain, u32 *outOffsetSub, u32 size, u32 *outSizeMain, u32 *outSizeSub);
static void LoadImageMapping(CharTransferTask *task);
#ifdef PLATFORM_DS
static void LoadImageMappingForScreen(CharTransferTask *task, NNS_G2D_VRAM_TYPE vramType);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
static void LoadImageVramTransfer(CharTransferTask *task);
static void LoadImageVramTransferForScreen(CharTransferTask *task, int vramType);

static int AlignToBlockSize(int size, int blockSize, BOOL rightAlign);
static int CalcBlockMaximum(int size, int blockSize);
static int CalcBlockOffset(int blockNum, int blockSize);
static void CalcByteAndBitIndices(int val, u32 *top, u8 *bottom);
static void FixOffsetAndSize(u32 base, u32 offset, u32 size, int *outOffset, int *outSize);
static u32 GetNumBlocks(u8 *buf);

static void InitTransferBuffers(u32 numBlocksMain, u32 numBlocksSub, enum HeapID heapID);
static void FreeBlockTransferBuffer(u8 *buf);
static void ReserveTransferRange(u32 start, u32 count, u8 *buf);
#ifdef PLATFORM_DS
static void ReserveVramSpace(u32 size, NNS_G2D_VRAM_TYPE vramType);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
static void ClearTransferRange(u32 start, u32 count, u8 *buf);
static void ClearTransferBuffer(u8 *buf);
static void ClearBothTransferBuffers(void);
static u32 FindAvailableTransferRange(u32 size, u8 *buf);
#ifdef PLATFORM_DS
static BOOL TryGetDestOffsets(u32 size, NNS_G2D_VRAM_TYPE vramType, u32 *outOffsetMain, u32 *outOffsetSub);
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif

static CharTransferTaskManager *sTaskManager = NULL;

void CharTransfer_Init(const CharTransferTemplate *template)
{
    #ifdef PLATFORM_DS
    CharTransfer_InitWithVramModes(template, GX_GetOBJVRamModeChar(), GXS_GetOBJVRamModeChar());
    #else
    // TODO: Port GXS_GetOBJVRamModeChar to PAL
    #endif
}

void CharTransfer_InitWithVramModes(const CharTransferTemplate *template, GXOBJVRamModeChar modeMain, GXOBJVRamModeChar modeSub)
{
    if (sTaskManager == NULL) {
        sTaskManager = Heap_Alloc(template->heapID, sizeof(CharTransferTaskManager));
        MI_CpuClear32(sTaskManager, sizeof(CharTransferTaskManager));

        sTaskManager->capacity = template->maxTasks;
        sTaskManager->tasks = Heap_Alloc(template->heapID, sizeof(CharTransferTask) * sTaskManager->capacity);
        for (int i = 0; i < template->maxTasks; i++) {
            InitTransferTask(sTaskManager->tasks + i);
        }

        sTaskManager->blockSizeMain = CharTransfer_GetBlockSize(modeMain);
        sTaskManager->blockSizeSub = CharTransfer_GetBlockSize(modeSub);

        #ifdef PLATFORM_DS
        GX_SetOBJVRamModeChar(modeMain);
        #else
        // TODO: Port GX_SetOBJVRamModeChar to PAL
        #endif
        #ifdef PLATFORM_DS
        GXS_SetOBJVRamModeChar(modeSub);
        #else
        // TODO: Port GXS_SetOBJVRamModeChar to PAL
        #endif

        int numBlocksMain = CalcBlockMaximum(template->sizeMain, sTaskManager->blockSizeMain);
        int numBlocksSub = CalcBlockMaximum(template->sizeSub, sTaskManager->blockSizeSub);

        InitTransferBuffers(numBlocksMain, numBlocksSub, template->heapID);
    }
}

void CharTransfer_Free(void)
{
    if (sTaskManager != NULL) {
        FreeBlockTransferBuffer(sTaskManager->bufMain);
        FreeBlockTransferBuffer(sTaskManager->bufSub);

        CharTransfer_ResetAllTasks();

        Heap_Free(sTaskManager->tasks);
        Heap_Free(sTaskManager);
        sTaskManager = NULL;
    }
}

void CharTransfer_ClearBuffers(void)
{
    sTaskManager->offsetMain = 0;
    sTaskManager->offsetSub = 0;

    ClearTransferBuffer(sTaskManager->bufMain);
    ClearTransferBuffer(sTaskManager->bufSub);
    UpdateVramCapacities();
}

#ifdef PLATFORM_DS
void CharTransfer_ReserveVramRange(u32 offset, u32 size, NNS_G2D_VRAM_TYPE vramType)
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
{
    int fixedOffset, fixedSize;
    #ifdef PLATFORM_DS
    if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        FixOffsetAndSize(sTaskManager->freeSizeMain, offset, size, &fixedOffset, &fixedSize);
        if (fixedSize > 0) {
            #ifdef PLATFORM_DS
            ReserveTransferRangeByOffsetAndSize(NNS_G2D_VRAM_TYPE_2DMAIN, fixedOffset, 0, fixedSize, 0);
            #else
            // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
            #endif
        }
    } else {
        FixOffsetAndSize(sTaskManager->freeSizeSub, offset, size, &fixedOffset, &fixedSize);
        if (fixedSize > 0) {
            #ifdef PLATFORM_DS
            ReserveTransferRangeByOffsetAndSize(NNS_G2D_VRAM_TYPE_2DSUB, 0, fixedOffset, 0, fixedSize);
            #else
            // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
            #endif
        }
    }
}

BOOL CharTransfer_Request(const CharTransferTaskTemplate *template)
{
    if (CharTransfer_HasTask(template->resourceID) == TRUE) {
        GF_ASSERT(FALSE);
    }

    CharTransferTask *task = FindNextFreeTask();
    if (task == NULL) {
        GF_ASSERT(FALSE);
        return FALSE;
    }

    if (InitTransferTaskFromTemplate(template, task) == FALSE) {
        return FALSE;
    }

    if (ReserveAndTransfer(task) == FALSE) {
        CharTransfer_ResetTask(task->resourceID);
        return FALSE;
    }

    sTaskManager->length++;
    return TRUE;
}

BOOL CharTransfer_RequestWithHardwareMappingType(const CharTransferTaskTemplate *template)
{
    if (CharTransfer_HasTask(template->resourceID) == TRUE) {
        GF_ASSERT(FALSE);
    }

    CharTransferTask *task = FindNextFreeTask();
    if (task == NULL) {
        GF_ASSERT(FALSE);
        return 0;
    }

    if (InitTransferTaskFromTemplate(template, task) == FALSE) {
        return FALSE;
    }

    task->useHardwareMappingType = TRUE;
    if (ReserveAndTransfer(task) == FALSE) {
        CharTransfer_ResetTask(task->resourceID);
        return FALSE;
    }

    sTaskManager->length++;
    return TRUE;
}

BOOL CharTransfer_HasTask(int resourceID)
{
    for (int i = 0; i < sTaskManager->capacity; i++) {
        if (sTaskManager->tasks[i].resourceID == resourceID) {
            return TRUE;
        }
    }

    return FALSE;
}

void CharTransfer_ReplaceCharData(int resourceID, NNSG2dCharacterData *data)
{
    GF_ASSERT(data);

    CharTransferTask *task = FindTransferTaskByResourceID(resourceID);
    GF_ASSERT(task);

    task->data = data;
    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        #ifdef PLATFORM_DS
        VramTransfer_Request(NNS_GFD_DST_2D_OBJ_CHAR_MAIN, task->baseAddrMain, data->pRawData, data->szByte);
        #else
        // TODO: Port NNS_GFD_DST_2D_OBJ_CHAR_MAIN to PAL
        #endif
    }
    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        #ifdef PLATFORM_DS
        VramTransfer_Request(NNS_GFD_DST_2D_OBJ_CHAR_SUB, task->baseAddrSub, data->pRawData, data->szByte);
        #else
        // TODO: Port NNS_GFD_DST_2D_OBJ_CHAR_SUB to PAL
        #endif
    }
}

void CharTransfer_ResetTask(int resourceID)
{
    CharTransferTask *task;
    BOOL stillSearching = TRUE;

    do {
        task = FindTransferTaskByResourceID(resourceID);
        GF_ASSERT(task);

        if (task->state == CTT_VRAM_COPIED) {
            CharTransfer_DeleteTask(&task->imageProxy);
        } else {
            stillSearching = FALSE;
        }
    } while (stillSearching);

    if (task->state != CTT_INIT) {
        ResetTransferTask(task);
        sTaskManager->length--;
    }
}

void CharTransfer_ResetAllTasks(void)
{
    for (int i = 0; i < sTaskManager->capacity; i++) {
        if (sTaskManager->tasks[i].state != CTT_INIT) {
            ResetTransferTask(&sTaskManager->tasks[i]);
            sTaskManager->length--;
        }
    }
}

NNSG2dImageProxy *CharTransfer_GetImageProxy(int resourceID)
{
    CharTransferTask *task = FindTransferTaskByResourceID(resourceID);
    GF_ASSERT(task);

    if (task->state == CTT_INIT) {
        return NULL;
    }

    return &task->imageProxy;
}

NNSG2dImageProxy *CharTransfer_ResizeTaskRange(int resourceID, u32 size)
{
    CharTransferTask *task = FindTransferTaskByResourceID(resourceID);
    GF_ASSERT(task);

    if (task->state == CTT_INIT) {
        return NULL;
    }

    u32 offsetMain, offsetSub;
    u32 sizeMain, sizeSub;
    TryGetFreeTransferSpace(task->vramType, &offsetMain, &offsetSub, size, &sizeMain, &sizeSub);

    if (task->state == CTT_VRAM_READY) {
        return NULL;
    }

    task->state = CTT_VRAM_READY;
    UpdateBaseAddresses(task, offsetMain, offsetSub);

    task->hasRange = TRUE;
    task->regionSizeMain = sizeMain;
    task->regionSizeSub = sizeSub;

    LoadImageVramTransfer(task);
    ReserveTransferRangeByOffsetAndSize(task->vramType, offsetMain, offsetSub, sizeMain, sizeSub);
    return &task->imageProxy;
}

NNSG2dImageProxy *CharTransfer_CopyTask(const NNSG2dImageProxy *imageProxy)
{
    CharTransferTask *srcTask = FindTransferTaskByImageProxy(imageProxy);
    GF_ASSERT(srcTask);

    CharTransferTask *dstTask = FindNextFreeTask();
    GF_ASSERT(dstTask);

    if (srcTask->state != CTT_VRAM_READY) {
        return NULL;
    }

    *dstTask = *srcTask;
    dstTask->state = CTT_VRAM_COPIED;

    u32 size;
    #ifdef PLATFORM_DS
    if (dstTask->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        size = dstTask->regionSizeMain;
    } else {
        size = dstTask->regionSizeSub;
    }

    u32 offsetMain, offsetSub;
    u32 sizeMain, sizeSub;
    TryGetFreeTransferSpace(dstTask->vramType, &offsetMain, &offsetSub, size, &sizeMain, &sizeSub);
    UpdateBaseAddresses(dstTask, offsetMain, offsetSub);

    dstTask->hasRange = TRUE;
    dstTask->regionSizeMain = sizeMain;
    dstTask->regionSizeSub = sizeSub;

    LoadImageVramTransfer(dstTask);
    ReserveTransferRangeByOffsetAndSize(dstTask->vramType, offsetMain, offsetSub, sizeMain, sizeSub);

    return &dstTask->imageProxy;
}

void CharTransfer_DeleteTask(const NNSG2dImageProxy *imageProxy)
{
    int i;
    for (i = 0; i < sTaskManager->capacity; i++) {
        if ((sTaskManager->tasks[i].state == CTT_VRAM_READY || sTaskManager->tasks[i].state == CTT_VRAM_COPIED)
            && &sTaskManager->tasks[i].imageProxy == imageProxy) {
            break;
        }
    }

    if (i >= sTaskManager->capacity) {
        return;
    }

    ClearTransferTaskRange(sTaskManager->tasks + i);
    if (sTaskManager->tasks[i].state == CTT_VRAM_READY) {
        sTaskManager->tasks[i].state = CTT_WAITING_FOR_VRAM;
    } else {
        sTaskManager->tasks[i].state = CTT_INIT;
        InitTransferTask(&sTaskManager->tasks[i]);
    }
}

#ifdef PLATFORM_DS
BOOL CharTransfer_AllocRange(int size, BOOL atEnd, NNS_G2D_VRAM_TYPE vramType, CharTransferAllocation *allocation)
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
{
    u32 offsetMain, offsetSub;
    u32 sizeMain, sizeSub;
    BOOL result;

    if (atEnd == FALSE) {
        result = TryGetDestOffsets(size, vramType, &offsetMain, &offsetSub);
        if (result) {
            ReserveVramSpace(size, vramType);
            allocation->vramType = vramType;
            allocation->size = size;

            #ifdef PLATFORM_DS
            if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
            #else
            // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
            #endif
                allocation->offset = offsetMain;
            } else {
                allocation->offset = offsetSub;
            }

            allocation->atEnd = FALSE;
        }
    } else {
        result = TryGetFreeTransferSpace(vramType, &offsetMain, &offsetSub, size, &sizeMain, &sizeSub);
        if (result) {
            ReserveTransferRangeByOffsetAndSize(vramType, offsetMain, offsetSub, sizeMain, sizeSub);
            allocation->vramType = vramType;

            #ifdef PLATFORM_DS
            if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
            #else
            // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
            #endif
                allocation->size = sizeMain;
                allocation->offset = offsetMain + sTaskManager->freeSizeMain;
            } else {
                allocation->size = sizeSub;
                allocation->offset = offsetSub + sTaskManager->freeSizeSub;
            }

            allocation->atEnd = TRUE;
        }
    }

    return result;
}

void CharTransfer_ClearRange(CharTransferAllocation *allocation)
{
    if (allocation->atEnd == FALSE) {
        return;
    }

    #ifdef PLATFORM_DS
    if (allocation->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        int count = CalcBlockMaximum(allocation->size, sTaskManager->blockSizeMain);
        int start = CalcBlockMaximum(allocation->offset - sTaskManager->freeSizeMain, sTaskManager->blockSizeMain);
        ClearTransferRange(start, count, sTaskManager->bufMain);
    }

    #ifdef PLATFORM_DS
    if (allocation->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        int count = CalcBlockMaximum(allocation->size, sTaskManager->blockSizeSub);
        int start = CalcBlockMaximum(allocation->offset - sTaskManager->freeSizeSub, sTaskManager->blockSizeSub);
        ClearTransferRange(start, count, sTaskManager->bufSub);
    }
}

void *CharTransfer_PopTaskManager(void)
{
    GF_ASSERT(sTaskManager);

    void *manager = sTaskManager;
    sTaskManager = NULL;
    return manager;
}

void CharTransfer_PushTaskManager(void *manager)
{
    GF_ASSERT(sTaskManager == NULL);
    sTaskManager = manager;
}

static void InitTransferTask(CharTransferTask *task)
{
    task->data = NULL;
    task->vramType = 0;
    task->done = FALSE;
    task->resourceID = CHAR_RESOURCE_ID_NONE;
    task->baseAddrMain = 0;
    task->baseAddrSub = 0;
    task->state = CTT_INIT;
    task->useHardwareMappingType = FALSE;

    #ifdef PLATFORM_DS
    NNS_G2dInitImageProxy(&task->imageProxy);
    #else
    // TODO: Port NNS_G2dInitImageProxy to PAL
    #endif
}

static BOOL InitTransferTaskFromTemplate(const CharTransferTaskTemplate *template, CharTransferTask *task)
{
    task->data = template->data;
    task->resourceID = template->resourceID;
    task->vramType = template->vramType;
    task->done = task->data->characterFmt >> CHAR_TRANSFER_SHIFT;
    task->atEnd = template->atEnd;
    task->hasRange = FALSE;
    task->regionSizeMain = 0;
    task->regionSizeSub = 0;

    return TRUE;
}

static CharTransferTask *FindTransferTaskByImageProxy(const NNSG2dImageProxy *search)
{
    int i;
    for (i = 0; i < sTaskManager->capacity; i++) {
        if (sTaskManager->tasks[i].state != CTT_INIT
            && &sTaskManager->tasks[i].imageProxy == search) {
            break;
        }
    }

    if (i >= sTaskManager->capacity) {
        return NULL;
    }

    return sTaskManager->tasks + i;
}

static BOOL ReserveAndTransfer(CharTransferTask *task)
{
    BOOL result = TRUE;

    if (task->done) {
        task->state = CTT_WAITING_FOR_VRAM;
    } else {
        task->state = CTT_IN_PROGRESS;
        if (task->atEnd == FALSE) {
            result = ReserveAndTransferFromHead(task);
        } else {
            result = ReserveAndTransferFromTail(task);
        }
    }

    return result;
}

static BOOL ReserveAndTransferFromHead(CharTransferTask *task)
{
    u32 offsetMain, offsetSub;

    BOOL result = TryGetDestOffsets(task->data->szByte, task->vramType, &offsetMain, &offsetSub);
    if (result) {
        SetBaseAddresses(task, offsetMain, offsetSub);
        ReserveVramSpace(task->data->szByte, task->vramType);
    }

    return result;
}

static BOOL ReserveAndTransferFromTail(CharTransferTask *task)
{
    u32 offsetMain, offsetSub;
    u32 sizeMain, sizeSub;

    if (TryGetFreeTransferSpace(task->vramType, &offsetMain, &offsetSub, task->data->szByte, &sizeMain, &sizeSub) == FALSE) {
        return FALSE;
    }

    UpdateBaseAddresses(task, offsetMain, offsetSub);
    task->hasRange = TRUE;
    task->regionSizeMain = sizeMain;
    task->regionSizeSub = sizeSub;

    LoadImageMapping(task);
    ReserveTransferRangeByOffsetAndSize(task->vramType, offsetMain, offsetSub, sizeMain, sizeSub);

    return TRUE;
}

static void ResetTransferTask(CharTransferTask *task)
{
    if (task->hasRange) {
        ClearTransferTaskRange(task);
    }

    InitTransferTask(task);
}

static CharTransferTask *FindTransferTaskByResourceID(int resourceID)
{
    for (int i = 0; i < sTaskManager->capacity; i++) {
        if (sTaskManager->tasks[i].resourceID == resourceID) {
            return &sTaskManager->tasks[i];
        }
    }

    return NULL;
}

#ifdef PLATFORM_DS
static GXOBJVRamModeChar UpdateMappingTypeFromHardware(CharTransferTask *task, NNS_G2D_VRAM_TYPE vramType)
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
{
    #ifdef PLATFORM_DS
    if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        #ifdef PLATFORM_DS
        task->data->mapingType = GX_GetOBJVRamModeChar();
        #else
        // TODO: Port GX_GetOBJVRamModeChar to PAL
        #endif
    } else {
        #ifdef PLATFORM_DS
        task->data->mapingType = GXS_GetOBJVRamModeChar();
        #else
        // TODO: Port GXS_GetOBJVRamModeChar to PAL
        #endif
    }

    return task->data->mapingType;
}

static void SetBaseAddresses(CharTransferTask *task, u32 baseAddrMain, u32 baseAddrSub)
{
    task->baseAddrMain = baseAddrMain;
    task->baseAddrSub = baseAddrSub;

    if (!task->done) {
        LoadImageMapping(task);
    }
}

#ifdef PLATFORM_DS
// NNS_G2D_VRAM_TYPE does not match against int here, for some reason.
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
static BOOL TryGetFreeTransferSpace(int vramType, u32 *outOffsetMain, u32 *outOffsetSub, u32 size, u32 *outSizeMain, u32 *outSizeSub)
{
    #ifdef PLATFORM_DS
    if (vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        *outSizeMain = AlignToBlockSize(size, sTaskManager->blockSizeMain, TRUE);
        u32 blockMax = CalcBlockMaximum(*outSizeMain, sTaskManager->blockSizeMain);
        *outOffsetMain = FindAvailableTransferRange(blockMax, sTaskManager->bufMain);

        if (*outOffsetMain == -1) {
            GF_ASSERT(FALSE);
            return FALSE;
        }

        *outOffsetMain = CalcBlockOffset(*outOffsetMain, sTaskManager->blockSizeMain);
    }

    #ifdef PLATFORM_DS
    if (vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        *outSizeSub = AlignToBlockSize(size, sTaskManager->blockSizeSub, TRUE);
        u32 blockMax = CalcBlockMaximum(*outSizeSub, sTaskManager->blockSizeSub);
        *outOffsetSub = FindAvailableTransferRange(blockMax, sTaskManager->bufSub);

        if (*outOffsetSub == -1) {
            GF_ASSERT(FALSE);
            return FALSE;
        }

        *outOffsetSub = CalcBlockOffset(*outOffsetSub, sTaskManager->blockSizeSub);
    }

    return TRUE;
}

static void UpdateBaseAddresses(CharTransferTask *task, u32 offsetMain, u32 offsetSub)
{
    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        task->baseAddrMain = offsetMain + sTaskManager->freeSizeMain;
    }

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        task->baseAddrSub = offsetSub + sTaskManager->freeSizeSub;
    }
}

#ifdef PLATFORM_DS
static void ReserveTransferRangeByOffsetAndSize(NNS_G2D_VRAM_TYPE vramType, u32 offsetMain, u32 offsetSub, u32 sizeMain, u32 sizeSub)
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
{
    #ifdef PLATFORM_DS
    if (vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        int count = CalcBlockMaximum(sizeMain, sTaskManager->blockSizeMain);
        int start = CalcBlockMaximum(offsetMain, sTaskManager->blockSizeMain);
        ReserveTransferRange(start, count, sTaskManager->bufMain);
    }

    #ifdef PLATFORM_DS
    if (vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        int count = CalcBlockMaximum(sizeSub, sTaskManager->blockSizeSub);
        int start = CalcBlockMaximum(offsetSub, sTaskManager->blockSizeSub);
        ReserveTransferRange(start, count, sTaskManager->bufSub);
    }
}

static void LoadImageMapping(CharTransferTask *task)
{
    #ifdef PLATFORM_DS
    NNS_G2dInitImageProxy(&task->imageProxy);
    #else
    // TODO: Port NNS_G2dInitImageProxy to PAL
    #endif

    #ifdef PLATFORM_DS
    if (task->vramType != NNS_G2D_VRAM_TYPE_MAX) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_MAX to PAL
    #endif
        LoadImageMappingForScreen(task, task->vramType);
    } else {
        #ifdef PLATFORM_DS
        LoadImageMappingForScreen(task, NNS_G2D_VRAM_TYPE_2DMAIN);
        #else
        // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
        #endif
        #ifdef PLATFORM_DS
        LoadImageMappingForScreen(task, NNS_G2D_VRAM_TYPE_2DSUB);
        #else
        // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
        #endif
    }
}

#ifdef PLATFORM_DS
static void LoadImageMappingForScreen(CharTransferTask *task, NNS_G2D_VRAM_TYPE vramType)
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
{
    int mappingType = task->useHardwareMappingType ? UpdateMappingTypeFromHardware(task, vramType) : vramType;

    u32 baseAddr;
    #ifdef PLATFORM_DS
    if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        baseAddr = task->baseAddrMain;
        #ifdef PLATFORM_DS
        GX_GetOBJVRamModeChar(); // result unused
        #else
        // TODO: Port GX_GetOBJVRamModeChar to PAL
        #endif
    } else {
        baseAddr = task->baseAddrSub;
        #ifdef PLATFORM_DS
        GXS_GetOBJVRamModeChar(); // result unused
        #else
        // TODO: Port GXS_GetOBJVRamModeChar to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    if (mappingType == GX_OBJVRAMMODE_CHAR_2D) {
    #else
    // TODO: Port GX_OBJVRAMMODE_CHAR_2D to PAL
    #endif
        #ifdef PLATFORM_DS
        NNS_G2dLoadImage2DMapping(task->data, baseAddr, vramType, &task->imageProxy);
        #else
        // TODO: Port NNS_G2dLoadImage2DMapping to PAL
        #endif
    } else {
        #ifdef PLATFORM_DS
        NNS_G2dLoadImage1DMapping(task->data, baseAddr, vramType, &task->imageProxy);
        #else
        // TODO: Port NNS_G2dLoadImage1DMapping to PAL
        #endif
    }
}

static void LoadImageVramTransfer(CharTransferTask *task)
{
    #ifdef PLATFORM_DS
    NNS_G2dInitImageProxy(&task->imageProxy);
    #else
    // TODO: Port NNS_G2dInitImageProxy to PAL
    #endif

    #ifdef PLATFORM_DS
    if (task->vramType != NNS_G2D_VRAM_TYPE_MAX) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_MAX to PAL
    #endif
        LoadImageVramTransferForScreen(task, task->vramType);
    } else {
        #ifdef PLATFORM_DS
        LoadImageVramTransferForScreen(task, NNS_G2D_VRAM_TYPE_2DMAIN);
        #else
        // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
        #endif
        #ifdef PLATFORM_DS
        LoadImageVramTransferForScreen(task, NNS_G2D_VRAM_TYPE_2DSUB);
        #else
        // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
        #endif
    }
}

static void LoadImageVramTransferForScreen(CharTransferTask *task, int vramType)
{
    int mappingType = task->useHardwareMappingType ? UpdateMappingTypeFromHardware(task, vramType) : vramType;

    int baseAddr;
    #ifdef PLATFORM_DS
    if (vramType == NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        baseAddr = task->baseAddrMain;
        #ifdef PLATFORM_DS
        GX_GetOBJVRamModeChar(); // result unused
        #else
        // TODO: Port GX_GetOBJVRamModeChar to PAL
        #endif
    } else {
        baseAddr = task->baseAddrSub;
        #ifdef PLATFORM_DS
        GXS_GetOBJVRamModeChar(); // result unused
        #else
        // TODO: Port GXS_GetOBJVRamModeChar to PAL
        #endif
    }

    #ifdef PLATFORM_DS
    NNS_G2dLoadImageVramTransfer(task->data, baseAddr, vramType, &task->imageProxy);
    #else
    // TODO: Port NNS_G2dLoadImageVramTransfer to PAL
    #endif
}

static CharTransferTask *FindNextFreeTask(void)
{
    for (int i = 0; i < sTaskManager->capacity; i++) {
        if (sTaskManager->tasks[i].state == CTT_INIT) {
            return &sTaskManager->tasks[i];
        }
    }

    return NULL;
}

static void UpdateVramCapacities(void)
{
    #ifdef PLATFORM_DS
    switch (GX_GetBankForOBJ()) {
    #else
    // TODO: Port GX_GetBankForOBJ to PAL
    #endif
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJ_NONE:
    #else
    // TODO: Port GX_VRAM_OBJ_NONE to PAL
    #endif
        sTaskManager->vramSizeMain = 0;
        break;
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJ_16_F:
    #else
    // TODO: Port GX_VRAM_OBJ_16_F to PAL
    #endif
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJ_16_G:
    #else
    // TODO: Port GX_VRAM_OBJ_16_G to PAL
    #endif
        sTaskManager->vramSizeMain = 16 * 1024;
        break;
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJ_32_FG:
    #else
    // TODO: Port GX_VRAM_OBJ_32_FG to PAL
    #endif
        sTaskManager->vramSizeMain = 32 * 1024;
        break;
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJ_64_E:
    #else
    // TODO: Port GX_VRAM_OBJ_64_E to PAL
    #endif
        sTaskManager->vramSizeMain = 64 * 1024;
        break;
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJ_80_EF:
    #else
    // TODO: Port GX_VRAM_OBJ_80_EF to PAL
    #endif
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJ_80_EG:
    #else
    // TODO: Port GX_VRAM_OBJ_80_EG to PAL
    #endif
        sTaskManager->vramSizeMain = 80 * 1024;
        break;
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJ_96_EFG:
    #else
    // TODO: Port GX_VRAM_OBJ_96_EFG to PAL
    #endif
        sTaskManager->vramSizeMain = 96 * 1024;
        break;
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJ_128_A:
    #else
    // TODO: Port GX_VRAM_OBJ_128_A to PAL
    #endif
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJ_128_B:
    #else
    // TODO: Port GX_VRAM_OBJ_128_B to PAL
    #endif
        sTaskManager->vramSizeMain = 128 * 1024;
        break;
    #ifdef PLATFORM_DS
    case GX_VRAM_OBJ_256_AB:
    #else
    // TODO: Port GX_VRAM_OBJ_256_AB to PAL
    #endif
        sTaskManager->vramSizeMain = 256 * 1024;
        break;
    default:
        sTaskManager->vramSizeMain = 0;
        break;
    }

    #ifdef PLATFORM_DS
    switch (GX_GetBankForSubOBJ()) {
    #else
    // TODO: Port GX_GetBankForSubOBJ to PAL
    #endif
    #ifdef PLATFORM_DS
    case GX_VRAM_SUB_OBJ_NONE:
    #else
    // TODO: Port GX_VRAM_SUB_OBJ_NONE to PAL
    #endif
        sTaskManager->vramSizeSub = 0;
        break;
    #ifdef PLATFORM_DS
    case GX_VRAM_SUB_OBJ_16_I:
    #else
    // TODO: Port GX_VRAM_SUB_OBJ_16_I to PAL
    #endif
        sTaskManager->vramSizeSub = 16 * 1024;
        break;
    #ifdef PLATFORM_DS
    case GX_VRAM_SUB_OBJ_128_D:
    #else
    // TODO: Port GX_VRAM_SUB_OBJ_128_D to PAL
    #endif
        sTaskManager->vramSizeSub = 128 * 1024;
        break;
    default:
        sTaskManager->vramSizeSub = 0;
        break;
    }

    int toReserve = CalcBlockOffset(sTaskManager->numBlocksMain, sTaskManager->blockSizeMain);
    sTaskManager->freeSizeMain = sTaskManager->vramSizeMain - toReserve;
    toReserve = CalcBlockOffset(sTaskManager->numBlocksSub, sTaskManager->blockSizeSub);
    sTaskManager->freeSizeSub = sTaskManager->vramSizeSub - toReserve;

    GF_ASSERT(sTaskManager->freeSizeMain >= 0 && sTaskManager->freeSizeSub >= 0);
    GF_ASSERT(sTaskManager->freeSizeMain >= 0 && sTaskManager->freeSizeSub >= 0);
}

static void ClearBothTransferBuffers(void)
{
    ClearTransferBuffer(sTaskManager->bufMain);
    ClearTransferBuffer(sTaskManager->bufSub);
}

static void InitTransferBuffers(u32 numBlocksMain, u32 numBlocksSub, enum HeapID heapID)
{
    sTaskManager->numBlocksMain = numBlocksMain;
    sTaskManager->numBlocksSub = numBlocksSub;

    if (sTaskManager->bufMain != NULL) {
        Heap_Free(sTaskManager->bufMain);
    }

    if (sTaskManager->bufSub != NULL) {
        Heap_Free(sTaskManager->bufSub);
    }

    if (sTaskManager->numBlocksMain != 0) {
        sTaskManager->bufMain = Heap_Alloc(heapID, numBlocksMain / 8);
    }

    if (sTaskManager->numBlocksSub != 0) {
        sTaskManager->bufSub = Heap_Alloc(heapID, numBlocksSub / 8);
    }

    ClearBothTransferBuffers();
}

static void FreeBlockTransferBuffer(u8 *buf)
{
    if (buf != NULL) {
        if (buf == sTaskManager->bufMain) {
            sTaskManager->numBlocksMain = 0;
            Heap_Free(buf);
        } else {
            sTaskManager->numBlocksSub = 0;
            Heap_Free(buf);
        }
        buf = NULL;
    }
}

static u32 GetNumBlocks(u8 *buf)
{
    if (buf == sTaskManager->bufMain) {
        return sTaskManager->numBlocksMain;
    }

    return sTaskManager->numBlocksSub;
}

static void ClearTransferBuffer(u8 *buf)
{
    if (buf != NULL) {
        memset(buf, 0, GetNumBlocks(buf) / 8);
    }
}

static void ReserveTransferRange(u32 start, u32 count, u8 *buf)
{
    if (buf == NULL) {
        return;
    }

    u32 max = GetNumBlocks(buf);
    u32 end = start + count;
    for (int i = start; i < end; i++) {
        if (i >= max) {
            break;
        }

        u32 byte;
        u8 bit;
        CalcByteAndBitIndices(i, &byte, &bit);
        GF_ASSERT((buf[byte] & (1 << bit)) == 0);

        u8 mask = (1 << bit);
        buf[byte] |= mask;
    }
}

static u32 FindAvailableTransferRange(u32 size, u8 *buf)
{
    int i, j;
    u32 max;

    if (buf != NULL) {
        max = GetNumBlocks(buf);

        for (i = 0; i < max; i++) {
            u32 byte;
            u8 bit, mask;
            CalcByteAndBitIndices(i, &byte, &bit);

            mask = (1 << bit);
            j = 0;

            while ((buf[byte] & mask) == 0 && j <= size) {
                int count = i + j;
                CalcByteAndBitIndices(count, &byte, &bit);

                mask = (1 << bit);
                if (count >= max) {
                    return -1;
                }

                j++;
            }

            if (j > size) {
                return i;
            }

            i += j;
        }
    }

    return -1;
}

static void ClearTransferRange(u32 start, u32 count, u8 *buf)
{
    if (buf == NULL) {
        return;
    }

    u32 end = start + count;
    for (int i = start; i < end; i++) {
        u32 byte;
        u8 bit;
        CalcByteAndBitIndices(i, &byte, &bit);

        GF_ASSERT(buf[byte] & (1 << bit));

        u8 mask = (1 << bit) ^ 0xFF;
        buf[byte] &= mask;
    }
}

static void ClearTransferTaskRange(CharTransferTask *task)
{
    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port NNS_G2dGetImageLocation to PAL
        #endif
        u32 location = NNS_G2dGetImageLocation(&task->imageProxy, NNS_G2D_VRAM_TYPE_2DMAIN);
        #else
        // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
        #endif
        u32 start = CalcBlockMaximum(location - sTaskManager->freeSizeMain, sTaskManager->blockSizeMain);
        u32 count = CalcBlockMaximum(task->regionSizeMain, sTaskManager->blockSizeMain);
        ClearTransferRange(start, count, sTaskManager->bufMain);
    }

    #ifdef PLATFORM_DS
    if (task->vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        #ifdef PLATFORM_DS
        #ifdef PLATFORM_DS
        #else
        // TODO: Port NNS_G2dGetImageLocation to PAL
        #endif
        u32 location = NNS_G2dGetImageLocation(&task->imageProxy, NNS_G2D_VRAM_TYPE_2DSUB);
        #else
        // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
        #endif
        u32 start = CalcBlockMaximum(location - sTaskManager->freeSizeSub, sTaskManager->blockSizeSub);
        u32 count = CalcBlockMaximum(task->regionSizeSub, sTaskManager->blockSizeSub);
        ClearTransferRange(start, count, sTaskManager->bufSub);
    }

    task->hasRange = FALSE;
}

int CharTransfer_GetBlockSize(GXOBJVRamModeChar vramMode)
{
    switch (vramMode) {
    #ifdef PLATFORM_DS
    case GX_OBJVRAMMODE_CHAR_1D_32K:
    #else
    // TODO: Port GX_OBJVRAMMODE_CHAR_1D_32K to PAL
    #endif
        return 1;
    #ifdef PLATFORM_DS
    case GX_OBJVRAMMODE_CHAR_1D_64K:
    #else
    // TODO: Port GX_OBJVRAMMODE_CHAR_1D_64K to PAL
    #endif
        return 2;
    #ifdef PLATFORM_DS
    case GX_OBJVRAMMODE_CHAR_1D_128K:
    #else
    // TODO: Port GX_OBJVRAMMODE_CHAR_1D_128K to PAL
    #endif
        return 4;
    #ifdef PLATFORM_DS
    case GX_OBJVRAMMODE_CHAR_1D_256K:
    #else
    // TODO: Port GX_OBJVRAMMODE_CHAR_1D_256K to PAL
    #endif
        return 8;
    default:
        return 1;
    }
}

static int AlignToBlockSize(int size, int blockSize, BOOL rightAlign)
{
    blockSize *= 32;

    if (size % blockSize) {
        size -= (size % blockSize);
        if (rightAlign == TRUE) {
            size += blockSize;
        }
    }

    return size;
}

static int CalcBlockMaximum(int size, int blockSize)
{
    return size / 32 / blockSize;
}

static int CalcBlockOffset(int blockNum, int blockSize)
{
    return blockNum * blockSize * 32;
}

#ifdef PLATFORM_DS
static BOOL TryGetDestOffsets(u32 size, NNS_G2D_VRAM_TYPE vramType, u32 *outOffsetMain, u32 *outOffsetSub)
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
{
    BOOL result = TRUE;

    #ifdef PLATFORM_DS
    if (vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        if (sTaskManager->offsetMain + size > sTaskManager->freeSizeMain) {
            GF_ASSERT(FALSE);
            result = FALSE;
        } else {
            *outOffsetMain = sTaskManager->offsetMain;
        }
    }

    #ifdef PLATFORM_DS
    if (vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        if (sTaskManager->offsetSub + size > sTaskManager->freeSizeSub) {
            GF_ASSERT(FALSE);
            result = FALSE;
        } else {
            *outOffsetSub = sTaskManager->offsetSub;
        }
    }

    return result;
}

#ifdef PLATFORM_DS
static void ReserveVramSpace(u32 size, NNS_G2D_VRAM_TYPE vramType)
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
{
    #ifdef PLATFORM_DS
    if (vramType & NNS_G2D_VRAM_TYPE_2DMAIN) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DMAIN to PAL
    #endif
        sTaskManager->offsetMain += size;
        sTaskManager->offsetMain = AlignToBlockSize(sTaskManager->offsetMain, sTaskManager->blockSizeMain, TRUE);
    }

    #ifdef PLATFORM_DS
    if (vramType & NNS_G2D_VRAM_TYPE_2DSUB) {
    #else
    // TODO: Port NNS_G2D_VRAM_TYPE_2DSUB to PAL
    #endif
        sTaskManager->offsetSub += size;
        sTaskManager->offsetSub = AlignToBlockSize(sTaskManager->offsetSub, sTaskManager->blockSizeSub, TRUE);
    }
}

static void CalcByteAndBitIndices(int val, u32 *top, u8 *bottom)
{
    *top = val >> 3;
    *bottom = val & 7;
}

static void FixOffsetAndSize(u32 base, u32 offset, u32 size, int *outOffset, int *outSize)
{
    *outOffset = offset - base;
    if (*outOffset < 0) {
        *outSize = size + *outOffset;
        *outOffset = 0;
    } else {
        *outSize = size;
    }
}

#else // PLATFORM_SDL

// SDL: Character transfer stubs - VRAM management not needed for SDL rendering

void CharTransfer_Init(const CharTransferTemplate *template) {
    (void)template;
}

void CharTransfer_InitWithVramModes(const CharTransferTemplate *template, GXOBJVRamModeChar modeMain, GXOBJVRamModeChar modeSub) {
    (void)template; (void)modeMain; (void)modeSub;
}

void CharTransfer_Free(void) {
}

void CharTransfer_ClearBuffers(void) {
}

void CharTransfer_ReserveVramRange(u32 offset, u32 size, NNS_G2D_VRAM_TYPE vramType) {
    (void)offset; (void)size; (void)vramType;
}

BOOL CharTransfer_Request(const CharTransferTaskTemplate *template) {
    (void)template;
    return FALSE;
}

BOOL CharTransfer_RequestWithHardwareMappingType(const CharTransferTaskTemplate *template) {
    (void)template;
    return FALSE;
}

BOOL CharTransfer_HasTask(int resourceID) {
    (void)resourceID;
    return FALSE;
}

void CharTransfer_ReplaceCharData(int resourceID, NNSG2dCharacterData *data) {
    (void)resourceID; (void)data;
}

void CharTransfer_ResetTask(int resourceID) {
    (void)resourceID;
}

void CharTransfer_ResetAllTasks(void) {
}

NNSG2dImageProxy *CharTransfer_GetImageProxy(int resourceID) {
    (void)resourceID;
    return NULL;
}

NNSG2dImageProxy *CharTransfer_ResizeTaskRange(int resourceID, u32 size) {
    (void)resourceID; (void)size;
    return NULL;
}

NNSG2dImageProxy *CharTransfer_CopyTask(const NNSG2dImageProxy *imageProxy) {
    (void)imageProxy;
    return NULL;
}

void CharTransfer_DeleteTask(const NNSG2dImageProxy *imageProxy) {
    (void)imageProxy;
}

#ifdef PLATFORM_DS
BOOL CharTransfer_AllocRange(int size, BOOL atEnd, NNS_G2D_VRAM_TYPE vramType, CharTransferAllocation *allocation) {
#else
// TODO: Port NNS_G2D_VRAM_TYPE to PAL
#endif
    (void)size; (void)atEnd; (void)vramType; (void)allocation;
    return FALSE;
}

void CharTransfer_ClearRange(CharTransferAllocation *allocation) {
    (void)allocation;
}

void *CharTransfer_PopTaskManager(void) {
    return NULL;
}

void CharTransfer_PushTaskManager(void *manager) {
    (void)manager;
}

int CharTransfer_GetBlockSize(GXOBJVRamModeChar vramMode) {
    (void)vramMode;
    return 0;
}

#endif // PLATFORM_DS
