#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>
#include "CommandParser/Parser.hpp"
/*!
* Generic CLEO commands
*/

/*
template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_GOSUB_IF_FALSE>() { // 0x0AA0 (label)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_RETURN_IF_FALSE>() { // 0x0AA1
    CollectParameters(0);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_CLEO_CALL>() { // 0x0AB1 (label , int32 numParams, arguments params)
    CollectParameters(3);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_CLEO_RETURN>() { // 0x0AB2 (int32 numRet, arguments retParams)
    CollectParameters(2);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_SET_CLEO_SHARED_VAR>() { // 0x0AB3 (int32 varId, any value)
    CollectParameters(2);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_GET_CLEO_SHARED_VAR>() { // 0x0AB4 (int32 varId)
    CollectParameters(1);
    return OR_CONTINUE;
}
*/

void notsa::script::commands::cleo::generic::RegisterHandlers() {
}
