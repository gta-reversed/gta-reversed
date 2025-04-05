#include <StdInc.h>
#include <Stats.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>

namespace {
// INCREMENT_INT_STAT (1571)
// INCREMENT_FLOAT_STAT (1572)
template<typename T>
void IncrementStat(eStats stat, T by) {
    CStats::IncrementStat(stat, (float)(by));
    CStats::DisplayScriptStatUpdateMessage(eStatUpdateState::STAT_UPDATE_INCREASE, stat, (float)(by));
}

// DECREMENT_INT_STAT (1573)
// DECREMENT_FLOAT_STAT (1574)
template<typename T>
void DecrementStat(eStats stat, T by) {
    CStats::DecrementStat(stat, (float)(by));
    CStats::DisplayScriptStatUpdateMessage(eStatUpdateState::STAT_UPDATE_DECREASE, stat, (float)(by));
}

// REGISTER_INT_STAT (1575)
// REGISTER_FLOAT_STAT (1576)
template<typename T>
void RegisterStat(eStats stat, T value) {
    CStats::SetNewRecordStat(stat, (float)(value));
    CStats::DisplayScriptStatUpdateMessage(eStatUpdateState::STAT_UPDATE_INCREASE, stat, (float)(value));
}

// SET_INT_STAT (1577)
// SET_FLOAT_STAT (1578)
template<typename T>
void SetStat(eStats stat, T value) {
    CStats::SetStatValue(stat, (float)(value));
    CStats::DisplayScriptStatUpdateMessage(eStatUpdateState::STAT_UPDATE_INCREASE, stat, (float)(value));
}

// GET_INT_STAT (1618)
// GET_FLOAT_STAT (1619)
template<typename T>
T GetStat(eStats stat) {
    return (T)(CStats::GetStatValue(stat));
}
};

void notsa::script::commands::stat::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER(COMMAND_INCREMENT_INT_STAT, IncrementStat<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_INCREMENT_FLOAT_STAT, IncrementStat<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_DECREMENT_INT_STAT, DecrementStat<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_DECREMENT_FLOAT_STAT, DecrementStat<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_REGISTER_INT_STAT, RegisterStat<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_REGISTER_FLOAT_STAT, RegisterStat<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_SET_INT_STAT, SetStat<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_FLOAT_STAT, SetStat<float>);

    REGISTER_COMMAND_HANDLER(COMMAND_GET_INT_STAT, GetStat<int32>);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_FLOAT_STAT, GetStat<float>);

    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_STAT_CHANGE_AMOUNT);
}
