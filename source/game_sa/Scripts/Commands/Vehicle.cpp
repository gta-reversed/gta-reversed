#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>
#include <cassert>

#include "PlayerInfo.h"
#include "World.h"
#include "CarGenerator.h"
#include "TheCarGenerators.h"
#include "CommandParser/Parser.hpp"
using namespace notsa::script;

/*!
* Various vehicle commands
*/

void ClearHeliOrientation(CHeli& heli) {
    heli.ClearHeliOrientation();
}

void RemoveRCBuggy() {
    FindPlayerInfo().BlowUpRCBuggy(false);
}

void SetCarProofs(CVehicle& veh, bool bullet, bool fire, bool explosion, bool collision, bool melee) {
    auto& flags           = veh.physicalFlags;
    flags.bBulletProof    = bullet;
    flags.bFireProof      = fire;
    flags.bExplosionProof = explosion;
    flags.bCollisionProof = collision;
    flags.bMeleeProof     = melee;
}

void SwitchCarGenerator(int32 generatorId, int32 count) {
    const auto generator = CTheCarGenerators::Get(generatorId);
    if (count) {
        generator->SwitchOn();
        if (count <= 100) {
            generator->m_nGenerateCount = count;
        }
    } else {
        generator->SwitchOff();
    }
}

void SetHasBeenOwnedForCarGenerator(int32 generatorId, bool alreadyOwned) {
    CTheCarGenerators::Get(generatorId)->bPlayerHasAlreadyOwnedCar = alreadyOwned;
}

float GetCarSpeed(CVehicle& veh) {
    return veh.m_vecMoveSpeed.Magnitude() * 50.f;
}

void SetCarDrivingStyle(CVehicle& veh, eCarDrivingStyle style) {
    veh.m_autoPilot.m_nCarDrivingStyle = style;
}

bool IsFirstCarColor(CVehicle& veh, int32 color) {
    return veh.m_nPrimaryColor == color;
}

bool IsSecondCarColor(CVehicle& veh, int32 color) {
    return veh.m_nSecondaryColor == color;
}
MultiRet<uint8, uint8> GetExtraCarColors(CVehicle& veh) {
    return {veh.m_nTertiaryColor, veh.m_nQuaternaryColor};
}

//void FixCar(CVehicle& vehicle) {
//
//}
//REGISTER_COMMAND_HANDLER(COMMAND_IMPROVE_CAR_BY_CHEATING, ImproveCarByCheating);

void PopCarBootUsingPhysics(CAutomobile& automobile) {
    automobile.PopBootUsingPhysics();
}

void SkipToNextAllowedStation(CTrain& train) {
    CTrain::SkipToNextAllowedStation(&train);
}


void SetRailTrackResistanceMult(float value) {
    CVehicle::ms_fRailTrackResistance = CVehicle::ms_fRailTrackResistanceDefault * (value > 0.0f ? value : 1.0f);
}

void DisableHeliAudio(CVehicle& vehicle, bool enable) {
    if (enable) {
        vehicle.m_vehicleAudio.EnableHelicoptor();
    } else {
        vehicle.m_vehicleAudio.DisableHelicoptor();
    }
}

bool IsCarInAirProper(CRunningScript& S, CVehicle& vehicle) {
    for (auto* const e : vehicle.GetCollidingEntities()) {
        if (e && (e->IsBuilding() || e->IsVehicle())) {
            return false;
        }
    }
    return true;
}

void notsa::script::commands::vehicle::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Vehicle");

    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_HELI_ORIENTATION, ClearHeliOrientation);
    REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_RC_BUGGY, RemoveRCBuggy);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_PROOFS, SetCarProofs);
    REGISTER_COMMAND_HANDLER(COMMAND_SWITCH_CAR_GENERATOR, SwitchCarGenerator);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_HAS_BEEN_OWNED_FOR_CAR_GENERATOR, SetHasBeenOwnedForCarGenerator);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_SPEED, GetCarSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_DRIVING_STYLE, SetCarDrivingStyle);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FIRST_CAR_COLOUR, IsFirstCarColor);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_SECOND_CAR_COLOUR, IsSecondCarColor);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_EXTRA_CAR_COLOURS, GetExtraCarColors);
    REGISTER_COMMAND_HANDLER(COMMAND_POP_CAR_BOOT_USING_PHYSICS, PopCarBootUsingPhysics);
    REGISTER_COMMAND_HANDLER(COMMAND_SKIP_TO_NEXT_ALLOWED_STATION, SkipToNextAllowedStation);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_RAILTRACK_RESISTANCE_MULT, SetRailTrackResistanceMult);
    REGISTER_COMMAND_HANDLER(COMMAND_DISABLE_HELI_AUDIO, DisableHeliAudio);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_IN_AIR_PROPER, IsCarInAirProper);

    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_TAXI);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SWITCH_TAXI_TIMER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_BOAT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_NUMBER_OF_CARS_COLLECTED_BY_GARAGE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_CAR_BEEN_TAKEN_TO_GARAGE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_RANDOM_CAR_OF_TYPE_IN_ZONE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_RESPRAY_HAPPENED);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CAR_RAM_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CAR_BLOCK_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CAR_FUNNY_SUSPENSION);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CAR_BIG_WHEELS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SWITCH_CAR_RADIO);
}
