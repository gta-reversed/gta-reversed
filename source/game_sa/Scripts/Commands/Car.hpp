#pragma once

/*!
* Various utility commands
*/

template<>
OpcodeResult CRunningScript::ProcessCommand<COMMAND_SET_CAR_PROOFS>() { // 0x2AC
    CollectParameters(6);
    auto* vehicle = GetVehiclePool()->GetAt(ScriptParams[0].iParam >> 8);
    assert(vehicle);
    vehicle->physicalFlags.bBulletProof = ScriptParams[1].uParam;
    vehicle->physicalFlags.bFireProof = ScriptParams[2].uParam;
    vehicle->physicalFlags.bExplosionProof = ScriptParams[3].uParam;
    vehicle->physicalFlags.bCollisionProof = ScriptParams[4].uParam;
    vehicle->physicalFlags.bMeleeProof = ScriptParams[5].uParam;
    return OR_CONTINUE;
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
REGISTER_PARSED_COMMAND(COMMAND_SWITCH_CAR_GENERATOR, SwitchCarGenerator)

float GetCarSpeed(CVehicle& veh) {
    return veh.m_vecMoveSpeed.Magnitude() * 50.f;
}
REGISTER_PARSED_COMMAND(COMMAND_GET_CAR_SPEED, GetCarSpeed)
