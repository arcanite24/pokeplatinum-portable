#ifndef POKEPLATINUM_RTC_H
#define POKEPLATINUM_RTC_H

#include "platform/platform_types.h"

#ifdef PLATFORM_DS
#include <nitro/rtc.h>
#else
#include "platform/platform_types.h"

// SDL port: RTC types
typedef struct {
    u32 year;
    u32 month;
    u32 day;
    u32 week;
} RTCDate;

typedef struct {
    u32 hour;
    u32 minute;
    u32 second;
} RTCTime;
#endif

#include "constants/rtc.h"

static inline BOOL IsLeapYear(u32 year)
{
    if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void InitRTC(void);
void UpdateRTC(void);
void GetCurrentDateTime(RTCDate *date, RTCTime *time);
void RTC_GetCurrentTime(RTCTime *time);
void GetCurrentDate(RTCDate *date);
int GetSecondsSinceMidnight(void);
s64 GetTimestamp(void);
int DayNumberForDate(const RTCDate *date);
BOOL IsNight(void);
enum TimeOfDay GetTimeOfDay(void);
enum TimeOfDay TimeOfDayForHour(int hour);
s64 TimeElapsed(s64 since, s64 until);

#endif // POKEPLATINUM_RTC_H
