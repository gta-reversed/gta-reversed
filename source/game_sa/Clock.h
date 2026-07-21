/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CClock {
public:
    static uint8 daysInMonth[12];
    static inline auto& bClockHasBeenStored = StaticRef<bool>(0xB70144);
    static inline auto& ms_Stored_nGameClockSeconds = StaticRef<uint16>(0xB70148);
    static inline auto& ms_Stored_nGameClockMinutes = StaticRef<uint8>(0xB7014A);
    static inline auto& ms_Stored_nGameClockHours = StaticRef<uint8>(0xB7014B);
    static inline auto& ms_Stored_nGameClockDays = StaticRef<uint8>(0xB7014C);
    static inline auto& ms_Stored_nGameClockMonths = StaticRef<uint8>(0xB7014D);
    static inline auto& CurrentDay = StaticRef<uint8>(0xB7014E);
    static inline auto& ms_nGameClockSeconds = StaticRef<uint16>(0xB70150);
    static inline auto& ms_nGameClockMinutes = StaticRef<uint8>(0xB70152);
    static inline auto& ms_nGameClockHours = StaticRef<uint8>(0xB70153);
    static inline auto& ms_nGameClockDays = StaticRef<uint8>(0xB70154);
    static inline auto& ms_nGameClockMonth = StaticRef<uint8>(0xB70155);
    static inline auto& ms_nLastClockTick = StaticRef<uint32>(0xB70158);
    static inline auto& ms_nMillisecondsPerGameMinute = StaticRef<uint32>(0xB7015C);

    static inline bool gbFreezeTime;

public:
    static void InjectHooks();

    static void Initialise(uint32 millisecondsPerGameMinute);
    static void Update();

    static uint16 GetGameClockMinutesUntil(uint8 hours, uint8 minutes);
    static bool GetIsTimeInRange(uint8 from, uint8 to);
    static void NormaliseGameClock();
    static void OffsetClockByADay(uint32 timeDirection);
    static void SetGameClock(uint8 hours, uint8 minutes, uint8 day);
    static void StoreClock();
    static void RestoreClock();

    // inlined and may have not original names
    static uint16 GetGameClockSeconds()   { return ms_nGameClockSeconds; } // 0x55F460
    static uint8  GetGameClockMinutes()   { return ms_nGameClockMinutes; } // 0x4410C0
    static uint8  GetGameClockHours()     { return ms_nGameClockHours; }   // 0x43A690
    static uint8  GetGameClockDays()      { return ms_nGameClockDays; }    // 0x4E7EF0
    static uint8  GetGameClockMonth()     { return ms_nGameClockMonth; }   // 0x4E7EE0
    static uint8  GetGameWeekDay()        { return CurrentDay; }           // NOTSA, maybe

    static float GetMinutesToday() { return float(ms_nGameClockMinutes) + 60.0f * (float)ms_nGameClockHours + (float)ms_nGameClockSeconds / 60.0f; } // 0x55F470
    static float GetHoursToday() { return (float)(CClock::GetGameClockMinutes()) / 60.0f + (float)(CClock::GetGameClockSeconds()) / 3600.0f + (float)(CClock::GetGameClockHours()); } // notsa

    static bool ClockHoursInRange(uint8 start, uint8 end) { return ms_nGameClockHours > start && ms_nGameClockHours < end; }
};
