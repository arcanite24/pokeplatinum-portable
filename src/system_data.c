#include "system_data.h"

#include "constants/savedata/save_table.h"

#include "rtc.h"
#include "savedata.h"

int SystemData_SaveSize()
{
    return sizeof(SystemData);
}

void SystemData_Clear(SystemData *systemData)
{
    MI_CpuClearFast(systemData, sizeof(SystemData));
    GameTime_Clear(&systemData->gameTime);
}

SystemData *SaveData_GetSystemData(SaveData *saveData)
{
    return SaveData_SaveTable(saveData, SAVE_TABLE_ENTRY_SYSTEM);
}

GameTime *SaveData_GetGameTime(SaveData *saveData)
{
    return &SaveData_GetSystemData(saveData)->gameTime;
}

void SystemData_Init(SystemData *systemData)
{
    OSOwnerInfo ownerInfo;

    #ifdef PLATFORM_DS
    systemData->rtcOffset = OS_GetOwnerRtcOffset();
    #else
    // TODO: Port OS_GetOwnerRtcOffset to PAL
    #endif
    #ifdef PLATFORM_DS
    OS_GetMacAddress(systemData->macAddress);
    #else
    // TODO: Port OS_GetMacAddress to PAL
    #endif

    #ifdef PLATFORM_DS
    OS_GetOwnerInfo(&ownerInfo);
    #else
    // TODO: Port OS_GetOwnerInfo to PAL
    #endif
    systemData->ownerBirthMonth = ownerInfo.birthday.month;
    systemData->ownerBirthDayOfMonth = ownerInfo.birthday.day;
}

BOOL SystemData_MatchesCurrentSystem(const SystemData *systemData)
{
    u8 currMacAddress[6];
    #ifdef PLATFORM_DS
    OS_GetMacAddress(currMacAddress);
    #else
    // TODO: Port OS_GetMacAddress to PAL
    #endif

    for (int i = 0; i < 6; i++) {
        if (systemData->macAddress[i] != currMacAddress[i]) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL SystemData_MatchesCurrentRTCOffset(const SystemData *systemData)
{
    #ifdef PLATFORM_DS
    return OS_GetOwnerRtcOffset() == systemData->rtcOffset;
    #else
    // TODO: Port OS_GetOwnerRtcOffset to PAL
    #endif
}

u8 SystemData_GetOwnerBirthMonth(const SystemData *systemData)
{
    return systemData->ownerBirthMonth;
}

u8 SystemData_GetOwnerBirthDayOfMonth(const SystemData *systemData)
{
    return systemData->ownerBirthDayOfMonth;
}

BOOL SystemData_IsMysteryGiftUnlocked(const SystemData *systemData)
{
    return systemData->isMysteryGiftUnlocked;
}

void SystemData_SetMysteryGiftUnlocked(SystemData *systemData, BOOL isUnlocked)
{
    systemData->isMysteryGiftUnlocked = isUnlocked;
}

s32 SystemData_GetDWCProfileId(const SystemData *systemData)
{
    return systemData->dwcProfileId;
}

void SystemData_SetDWCProfileId(SystemData *systemData, s32 profileId)
{
    if (systemData->dwcProfileId == 0) {
        systemData->dwcProfileId = profileId;
    }
}

void GameTime_Clear(GameTime *gameTime)
{
    gameTime->canary = TRUE;
    GetCurrentDateTime(&gameTime->date, &gameTime->time);
    gameTime->day = RTC_ConvertDateToDay(&gameTime->date);
    gameTime->startTimestamp = RTC_ConvertDateTimeToSecond(&gameTime->date, &gameTime->time);
    gameTime->firstCompletionTimestamp = 0;
    gameTime->penaltyInMinutes = 0;
}

BOOL GameTime_HasPenalty(const GameTime *gameTime)
{
    return gameTime->penaltyInMinutes != 0;
}

void GameTime_DecrementPenalty(GameTime *gameTime, int val)
{
    if (gameTime->penaltyInMinutes > 24 * 60) {
        gameTime->penaltyInMinutes = 24 * 60;
    }

    if (gameTime->penaltyInMinutes < val) {
        gameTime->penaltyInMinutes = 0;
    } else {
        gameTime->penaltyInMinutes -= val;
    }
}

void GameTime_StartPenalty(GameTime *gameTime)
{
    gameTime->penaltyInMinutes = 60 * 24;
    GetCurrentDateTime(&gameTime->date, &gameTime->time);
    gameTime->day = RTC_ConvertDateToDay(&gameTime->date);
}
