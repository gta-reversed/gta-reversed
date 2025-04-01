#include <StdInc.h>

#include "CLEOCommands.hpp"
#include <CommandParser/Parser.hpp>

#include "Vehicle.h"
#include "CommandParser/Parser.hpp"

/*!
* Various CLEO car commands
*/

uint8 GetCarNumberOfGears(CVehicle& vehicle) {
    return vehicle.m_pHandlingData->m_transmissionData.m_nNumberOfGears;
}

uint8 GetCarCurrentGear(CVehicle& vehicle) {
    return vehicle.m_nCurrentGear;
}

bool IsCarSirenOn(CVehicle& vehicle) {
    return vehicle.vehicleFlags.bSirenOrAlarm;
}

bool IsCarEngineOn(CVehicle& vehicle) {
    return vehicle.vehicleFlags.bEngineOn;
}

void SetCarEngineOn(CVehicle& vehicle, bool state) {
    vehicle.vehicleFlags.bEngineOn = state;
}


void notsa::script::commands::cleo::vehicle::RegisterHandlers() {
#if 0
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_NUMBER_OF_GEARS, GetCarNumberOfGears);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_CURRENT_GEAR, GetCarNumberOfGears);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_SIREN_ON, IsCarSirenOn);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_ENGINE_ON, IsCarEngineOn);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEO_SET_CAR_ENGINE_ON, SetCarEngineOn);
#endif
}
