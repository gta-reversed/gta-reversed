#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>

/*!
* Various CLEO filesystem commands
*/

/*
template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_SET_CURRENT_DIRECTORY>() { // 0x0A99 (any path)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_OPEN_FILE>() { // 0x0A9A (string filePathName, FileMode mode)
    CollectParameters(2);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_CLOSE_FILE>() { // 0x0A9B (File self)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_GET_FILE_SIZE>() { // 0x0A9C (File self)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_READ_FROM_FILE>() { // 0x0A9D (File self, int32 size)
    CollectParameters(2);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_WRITE_TO_FILE>() { // 0x0A9E (File self, int32 size, int32 source)
    CollectParameters(3);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_DOES_FILE_EXIST>() { // 0x0AAB (string path)
    CollectParameters(1);
    return OR_CONTINUE;
}
*/

void notsa::script::commands::cleo::fs::RegisterHandlers() {
}
