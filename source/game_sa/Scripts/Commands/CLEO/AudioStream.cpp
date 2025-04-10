#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>

/*!
* Various CLEO audio commands
*/
/*
template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_LOAD_AUDIO_STREAM>() { // 0x0AAC (string audioFileName)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_SET_AUDIO_STREAM_STATE>() { // 0x0AAD (AudioStream self, AudioStreamState state)
    CollectParameters(2);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_REMOVE_AUDIO_STREAM>() { // 0x0AAE (AudioStream self)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_GET_AUDIO_STREAM_LENGTH>() { // 0x0AAF (AudioStream self)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_GET_AUDIO_STREAM_STATE>() { // 0x0AB9 (AudioStream self)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_GET_AUDIO_STREAM_VOLUME>() { // 0x0ABB (AudioStream self)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_SET_AUDIO_STREAM_VOLUME>() { // 0x0ABC (AudioStream self, float volume)
    CollectParameters(2);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_SET_AUDIO_STREAM_LOOPED>() { // 0x0AC0 (AudioStream self, bool state)
    CollectParameters(2);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_LOAD_3D_AUDIO_STREAM>() { // 0x0AC1 (string audioFileName)
    CollectParameters(1);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_SET_PLAY_3D_AUDIO_STREAM_AT_COORDS>() { // 0x0AC2 (AudioStream self, float x, float y, float z)
    CollectParameters(4);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_SET_PLAY_3D_AUDIO_STREAM_AT_OBJECT>() { // 0x0AC3 (AudioStream self, Object handle)
    CollectParameters(2);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_SET_PLAY_3D_AUDIO_STREAM_AT_CHAR>() { // 0x0AC4 (AudioStream self, Char handle)
    CollectParameters(2);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_SET_PLAY_3D_AUDIO_STREAM_AT_CAR>() { // 0x0AC5 (AudioStream self, Car handle)
    CollectParameters(2);
    return OR_CONTINUE;
}
*/

void notsa::script::commands::cleo::audiostream::RegisterHandlers() {
}
