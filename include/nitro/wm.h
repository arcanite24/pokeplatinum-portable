#ifndef NITRO_WM_H
#define NITRO_WM_H

#include "platform/platform_types.h"

// Wireless Manager Stubs for SDL Port

typedef int WMErrCode;
typedef int WMStateCode;

#define WM_ERRCODE_SUCCESS 0
#define WM_ERRCODE_OPERATING 1
#define WM_ERRCODE_FAILED 2

#define WM_SIZE_BSSID 6
#define WM_SYSTEM_BUF_SIZE 1024

typedef struct {
    u8 bssid[WM_SIZE_BSSID];
    // Add other fields if needed
} WMBssDesc;

typedef WMBssDesc WMbssDesc; // Typo alias

typedef struct {
    u16 channel;
    u16 tgid;
    u16 beaconPeriod;
    u16 maxChannelTime;
    void* scanBuf;
    // Add other fields if needed
} WMParentParam;

typedef struct {
    u16 channel;
    u16 maxChannelTime;
    void* scanBuf;
    // Add fields
} WMScanParam;

typedef WMScanParam WMscanParam; // Typo alias

typedef void (*WMCallbackFunc)(void*);

static inline WMErrCode WM_SetIndCallback(WMCallbackFunc callback) {
    (void)callback;
    return WM_ERRCODE_SUCCESS;
}

static inline WMErrCode WM_Enable(WMCallbackFunc callback) {
    (void)callback;
    return WM_ERRCODE_SUCCESS;
}

static inline WMErrCode WM_SetMPDataToPort(WMCallbackFunc callback, u16 *data, u16 size, u16 mask, u16 port, u16 priority) {
    (void)callback; (void)data; (void)size; (void)mask; (void)port; (void)priority;
    return WM_ERRCODE_SUCCESS;
}

static inline u16 WM_GetAllowedChannel(void) {
    return 1;
}

static inline u16 WM_GetDispersionBeaconPeriod(void) {
    return 200;
}

static inline u16 WM_GetDispersionScanPeriod(void) {
    return 200;
}

static inline u16 WM_GetNextTgid(void) {
    return 0;
}

#define WM_SIZE_MP_PARENT_RECEIVE_BUFFER(size, maxChildren, unk) (size * maxChildren)
#define WM_SIZE_MP_CHILD_RECEIVE_BUFFER(size, unk) (size)

#endif // NITRO_WM_H
