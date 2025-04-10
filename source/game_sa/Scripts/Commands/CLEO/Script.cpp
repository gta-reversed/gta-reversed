#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>

#include "TheScripts.h"
#include "CommandParser/Parser.hpp"

/*!
* Various CLEO script related commands
*/
/*
template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_STREAM_CUSTOM_SCRIPT>() { // 0x0A92 (string scriptFileName, arguments)
    CollectParameters(2);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_TERMINATE_THIS_CUSTOM_SCRIPT>() { // 0x0A93
    CollectParameters(0);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_LOAD_AND_LAUNCH_CUSTOM_MISSION>() { // 0x0A94 (string scriptFileName, arguments)
    CollectParameters(2);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_SAVE_THIS_CUSTOM_SCRIPT>() { // 0x0A95
    CollectParameters(0);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_TERMINATE_ALL_CUSTOM_SCRIPTS_WITH_THIS_NAME>() { // 0x0ABA (string scriptFileName)
    CollectParameters(1);
    return OR_CONTINUE;
}
*/

void notsa::script::commands::cleo::script::RegisterHandlers() {
}
