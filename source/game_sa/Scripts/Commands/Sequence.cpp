#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>

/*!
* Various Sequence commands
*/

void SetSequenceToRepeat(int32 index, bool repeat) {
    const auto actualIndex = CTheScripts::GetActualScriptThingIndex(index, SCRIPT_THING_SEQUENCE_TASK);
    if (actualIndex < 0 || actualIndex >= MAX_NUM_SCRIPT_SEQUENCE_TASKS) {
        DEV_LOG("COMMAND_SET_SEQUENCE_TO_REPEAT: Index should be in [0, 64], it is {}", actualIndex);
        return;
    }
    CTaskSequences::GetActiveSequence().SetRepeatMode(repeat);
}

void notsa::script::commands::sequence::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER(COMMAND_SET_SEQUENCE_TO_REPEAT, SetSequenceToRepeat);
}
