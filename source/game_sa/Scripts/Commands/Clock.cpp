#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>

#include "CommandParser/Parser.hpp"
using namespace notsa::script;

/*!
* Various clock commands
*/

/// Get hours and minutes
MultiRet<uint8, uint8> GetTimeOfDay() {
    return { CClock::ms_nGameClockHours, CClock::ms_nGameClockMinutes };
}

void SetTimeOfDay(uint8 hours, uint8 minutes) {
    CClock::SetGameClock(hours, minutes, 0);
}

uint16 GetMinutesToTimeOfDay(uint8 hours, uint8 minutes) {
    return CClock::GetGameClockMinutesUntil(hours, minutes);
}

void StoreClock() {
    CClock::StoreClock();
}

void RestoreClock() {
    CClock::RestoreClock();
}

void notsa::script::commands::clock::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Clock");

    REGISTER_COMMAND_HANDLER(COMMAND_GET_TIME_OF_DAY, GetTimeOfDay);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TIME_OF_DAY, SetTimeOfDay);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_MINUTES_TO_TIME_OF_DAY, GetMinutesToTimeOfDay);
    REGISTER_COMMAND_HANDLER(COMMAND_STORE_CLOCK, StoreClock);
    REGISTER_COMMAND_HANDLER(COMMAND_RESTORE_CLOCK, RestoreClock);
}
