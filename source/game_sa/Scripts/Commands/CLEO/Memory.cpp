#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>

#include "app_debug.h"
#include "CommandParser/Parser.hpp"
/*!
* Various CLEO memory commands
*/

void WriteMemory(uint32 address, uint32 size, uint32 value, bool virtualProtect) {
    UNUSED(virtualProtect);

    DEV_LOG("WriteMemory(addr={}, size={}, value={} ,virtual_protect={})", address, size, value, virtualProtect);
    memcpy(reinterpret_cast<void*>(address), &value, size);
}

uint32 ReadMemory(uint32 address, uint32 size, bool virtualProtect) {
    UNUSED(virtualProtect);

    DEV_LOG("ReadMemory(addr={}, size={}, virtual_protect={})", address, size, virtualProtect);
    if (size != 4 && size != 2) {
        size = 1; // just to be sure not to write anything stupid.
    }
    uint32 ret;
    memcpy(&ret, reinterpret_cast<void*>(address), size);
    return ret;
}

auto GetPedPointer(CPed& ped) { 
    return &ped; 
}

auto GetVehiclePointer(CVehicle& vehicle) { 
    return &vehicle; 
}

auto GetObjectPointer(CObject& object) { 
    return &object; 
}

auto GetThisScriptStruct(CRunningScript* S) { 
    return S; 
}

/*
template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_CALL_FUNCTION>() { // 0x0AA5 (int32 address, int32 numParams, int32 pop, arguments funcParams)
    // perhaps this can't be ported to new-form?
    CollectParameters(4);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_CALL_METHOD>() { // 0x0AA6 (int32 address, int32 struct, int32 numParams, int32 pop, arguments funcParams)
    // perhaps this can't be ported to new-form?
    CollectParameters(5);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_CALL_FUNCTION_RETURN>() { // 0x0AA7 (int32 address, int32 numParams, int32 pop, arguments funcParams)
    // perhaps this can't be ported to new-form?
    CollectParameters(4);
    return OR_CONTINUE;
}

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_CALL_METHOD_RETURN>() { // 0x0AA8 (int32 address, int32 struct, int32 numParams, int32 pop, arguments funcParams)
    // perhaps this can't be ported to new-form?
    CollectParameters(5);
    return OR_CONTINUE;
}
*/

const char* GetScriptStructNamed(CRunningScript* S) {
    NOTSA_UNREACHABLE("Not implemented!");
    S->m_CondResult = false;

    return nullptr;
}

void notsa::script::commands::cleo::memory::RegisterHandlers() {
    /*
    REGISTER_COMMAND_HANDLER(COMMAND_WRITE_MEMORY, WriteMemory);
    REGISTER_COMMAND_HANDLER(COMMAND_READ_MEMORY, ReadMemory);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_PED_POINTER, GetPedPointer);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_VEHICLE_POINTER, GetVehiclePointer);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_OBJECT_POINTER, GetObjectPointer);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_THIS_SCRIPT_STRUCT, GetThisScriptStruct);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_THIS_SCRIPT_STRUCT, GetScriptStructNamed);
    */
}
