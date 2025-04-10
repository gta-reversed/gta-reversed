#include <StdInc.h>

#include "CLEOCommands.hpp"
#include "CommandParser/Parser.hpp"

/*!
* Various CLEO dynlib commands
*/

/*
template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_LOAD_DYNAMIC_LIBRARY>() { // 0x0AA2 (string libraryFileName)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_FREE_DYNAMIC_LIBRARY>() { // 0x0AA3 (DynamicLibrary self)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_GET_DYNAMIC_LIBRARY_PROCEDURE>() { // 0x0AA4 (string procName, DynamicLibrary self)
    CollectParameters(2);
    return OR_CONTINUE;
}
*/

void notsa::script::commands::cleo::dynamiclibrary::RegisterHandlers() {
}
