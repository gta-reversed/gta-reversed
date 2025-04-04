#include "StdInc.h"
#include "ReplayBlockType.h"
#include "Replay.h"

tReplayVehicleBlock tReplayVehicleBlock::MakeVehicleUpdateData(CVehicle& veh, int32 poolIdx) {
    tReplayVehicleBlock ret{};
    ret.type = REPLAY_PACKET_VEHICLE;
    ret.poolRef  = (uint8)poolIdx;
    ret.health   = (uint8)(std::min(veh.m_fHealth, 1000.0f) / 4.0f);
    ret.gasPedal = (uint8)(veh.m_fGasPedal * 100.0f);
    ret.matrix   = CCompressedMatrixNotAligned::Compress(veh.GetMatrix());
    ret.modelId  = veh.m_nModelIndex;
    ret.panels   = (veh.IsAutomobile()) ? veh.AsAutomobile()->m_damageManager.m_nPanelsStatus : 0;
    ret.vecMoveSpeed = {
        (int8)(std::clamp(veh.GetMoveSpeed().x, -4.0f, 4.0f) * 8000.0f),
        (int8)(std::clamp(veh.GetMoveSpeed().y, -4.0f, 4.0f) * 8000.0f),
        (int8)(std::clamp(veh.GetMoveSpeed().z, -4.0f, 4.0f) * 8000.0f)
    };
    ret.primaryColor   = veh.m_nPrimaryColor;
    ret.secondaryColor = veh.m_nSecondaryColor;
    ret.physicalFlags  = (uint8)veh.m_nPhysicalFlags;

    // todo refactor
    auto v9 = (ret.physicalFlags ^ (2 * ((uint32)veh.m_nFlags >> 7))) & 2 ^ ret.physicalFlags;
    auto v10 = v9 ^ (v9 ^ (uint8)(4 * ((uint32)veh.m_nFlags >> 30))) & 4;
    ret.physicalFlags = v10 ^ (v10 ^ (8 * (veh.m_nFlags < 0))) & 8;

    if (&veh == FindPlayerVehicle() && gbFirstPersonRunThisFrame) {
        ret.physicalFlags &= ~2u;
    }

    ret.steerAngle_or_doomVerticalRot = (uint8)[&] {
        if (veh.m_nModelIndex == MODEL_RHINO) {
            return veh.AsAutomobile()->m_fDoomVerticalRotation * HEADING_COMPRESS_VALUE;
        } else {
            return veh.m_fSteerAngle * 50.0f;
        }
    }();

    if (veh.IsAutomobile()) {
        auto automobile = veh.AsAutomobile();

        for (auto i = 0; i < 4; i++) { // for each wheel
            ret.wheelsSuspensionCompression[i] = (uint8)(automobile->m_fWheelsSuspensionCompression[i] * 50.0f);
            ret.wheelRotation[i] = (uint8)(automobile->m_wheelRotation[i] * HEADING_COMPRESS_VALUE);
        }
        ret.angleDoorLF = (uint8)(automobile->m_doors[DOOR_LEFT_FRONT].m_fAngle * 20.222929f);
        ret.angleDoorRF = (uint8)(automobile->m_doors[DOOR_RIGHT_FRONT].m_fAngle * 20.222929f);

        ret.doorStatus = 0u;
        for (auto&& [i, status] : rngv::enumerate(automobile->m_damageManager.GetAllDoorsStatus())) {
            if (status == eDoorState::DOOR_SLAM_SHUT) {
                ret.doorStatus |= (uint8)std::pow(2, i);
            }
        }
    }
    ret.physicalFlags ^= (ret.physicalFlags ^ (veh.m_nPhysicalFlags >> 29)) & 1;
    ret.vehicleSubType = veh.m_nVehicleSubType;
    return ret;
}

tReplayTrainBlock tReplayTrainBlock::MakeTrainUpdateData(CTrain& train, int32 poolIdx) {
    tReplayTrainBlock ret;
    *ret.As<tReplayVehicleBlock>() = tReplayVehicleBlock::MakeVehicleUpdateData(*train.AsVehicle(), poolIdx);

    ret.type = REPLAY_PACKET_TRAIN;
    ret.trainSpeed = train.m_fTrainSpeed;
    ret.currentRailDistance = train.m_fCurrentRailDistance;
    ret.length = train.m_fLength;
    ret.trackId = train.m_nTrackId;
    ret.prevCarriageRef = ret.nextCarriageRef = 0;

    if (auto carriage = train.m_pPrevCarriage) {
        ret.prevCarriageRef = GetVehiclePool()->GetIndex(carriage->AsVehicle()) + 1;
    }

    if (auto carriage = train.m_pNextCarriage) {
        ret.nextCarriageRef = GetVehiclePool()->GetIndex(carriage->AsVehicle()) + 1;
    }
    return ret;
}

tReplayBikeBlock tReplayBikeBlock::MakeBikeUpdateData(CBike& bike, int32 poolIdx) {
    tReplayBikeBlock ret{};
    *ret.As<tReplayVehicleBlock>() = tReplayVehicleBlock::MakeVehicleUpdateData(*bike.AsVehicle(), poolIdx);

    ret.type = REPLAY_PACKET_BIKE;
    ret.animLean = (uint8)(bike.GetRideAnimData()->m_fAnimLean * 50.0f);
    ret.steerAngle = (uint8)(bike.GetRideAnimData()->m_fSteerAngle * 50.0f);
    return ret;
}

tReplayBmxBlock tReplayBmxBlock::MakeBmxUpdateData(CBmx& bmx, int32 poolIdx) {
    // Same as Bike, but the type is different.
    tReplayBmxBlock ret{};
    *ret.As<tReplayBikeBlock>() = tReplayBikeBlock::MakeBikeUpdateData(*bmx.AsBike(), poolIdx);
    ret.type = REPLAY_PACKET_BMX;
    return ret;
}

void tReplayVehicleBlock::ExtractVehicleUpdateData(CVehicle& veh, float interpolation) {
    veh.GetMatrix() = Lerp(veh.GetMatrix(), CCompressedMatrixNotAligned::Decompress(matrix), interpolation);
    veh.GetTurnSpeed() = CVector{0.0f};
    veh.m_fHealth = (float)(health * 4);
    veh.m_fGasPedal = (float)gasPedal / 100.0f;
    if (veh.IsAutomobile()) {
        // inlined ApplyPanelDamageToCar() @ 0x59CA10
        auto automobile = veh.AsAutomobile();
        const auto& damageManager = automobile->m_damageManager;

        for (auto i = 0; i < MAX_PANELS; i++) {
            const auto bitIdx = 0xF << (0x4 * i);
            const auto panel = (ePanels)i;
            const auto provided = (ePanelDamageState)(panels & bitIdx);
            const auto current = (ePanelDamageState)(damageManager.m_nPanelsStatus & bitIdx);

            if (provided != current) {
                automobile->m_damageManager.SetPanelStatus(panel, provided);
                automobile->SetPanelDamage(panel, true);
            }
        }
    }

    veh.GetMoveSpeed().x = (float)vecMoveSpeed.x / 8000.0f;
    veh.GetMoveSpeed().y = (float)vecMoveSpeed.y / 8000.0f;
    veh.GetMoveSpeed().z = (float)vecMoveSpeed.z / 8000.0f;

    auto v5 = ((uint8)veh.m_nFlags ^ (uint8)(physicalFlags << 6)) & 0x80 ^ veh.m_nFlags;
    auto v6 = v5 ^ (v5 ^ (physicalFlags << 28)) & 0x40000000;
    veh.m_nFlags = (physicalFlags << 28) ^ (v6 ^ (physicalFlags << 28)) & 0x7FFFFFFF;

    veh.m_fSteerAngle = [&] {
        if (veh.m_nModelIndex == MODEL_RHINO) {
            veh.AsAutomobile()->m_fDoomVerticalRotation = (float)steerAngle_or_doomVerticalRot / HEADING_COMPRESS_VALUE;
            return 0.0f;
        } else {
            return (float)steerAngle_or_doomVerticalRot / 50.0f;
        }
    }();

    if (veh.IsAutomobile()) {
        auto automobile = veh.AsAutomobile();

        for (auto i = 0; i < 4; i++) { // for each wheel
            automobile->m_fWheelsSuspensionCompression[i] = (float)wheelsSuspensionCompression[i] / 50.0f;
            automobile->m_wheelRotation[i] = (float)wheelRotation[i] / HEADING_COMPRESS_VALUE;

        }
        automobile->m_doors[DOOR_LEFT_FRONT].m_fAngle = (float)angleDoorLF / 20.222929f;
        automobile->m_doors[DOOR_LEFT_FRONT].m_fPrevAngle = automobile->m_doors[DOOR_LEFT_FRONT].m_fAngle;

        automobile->m_doors[DOOR_RIGHT_FRONT].m_fAngle = (float)angleDoorRF / 20.222929f;
        automobile->m_doors[DOOR_RIGHT_FRONT].m_fPrevAngle = automobile->m_doors[DOOR_RIGHT_FRONT].m_fAngle;

        auto& damageManager = automobile->m_damageManager;
        if (angleDoorLF != 0) {
            damageManager.SetDoorOpen(DOOR_LEFT_FRONT);
        }

        if (angleDoorRF != 0) {
            damageManager.SetDoorOpen(DOOR_RIGHT_FRONT);
        }

        for (auto i = 0; i < MAX_DOORS; i++) {
            const auto bitIdx = (uint32)std::pow(2, i);
            const auto door = (eDoors)i;
            if (doorStatus & bitIdx && damageManager.IsDoorPresent(door)) {
                damageManager.SetDoorStatus(door, DAMSTATE_NOTPRESENT);
                automobile->SetDoorDamage(door, true);
            }
        }

        automobile->m_nWheelsOnGround = 4;
    }

    veh.vehicleFlags.bEngineOn = veh.vehicleFlags.bEngineBroken != true;
    veh.m_nAreaCode = static_cast<eAreaCodes>(CGame::currArea); // FIXME

    CWorld::Remove(&veh);
    CWorld::Add(&veh);
    veh.m_nPhysicalFlags ^= (veh.m_nPhysicalFlags ^ (physicalFlags << 29)) & 0x20000000;
}

void tReplayBikeBlock::ExtractBikeUpdateData(CBike& bike, float interpolation) {
    As<tReplayVehicleBlock>()->ExtractVehicleUpdateData(*bike.AsVehicle(), interpolation);

    bike.GetRideAnimData()->m_fSteerAngle = (float)steerAngle / 50.0f;
    bike.GetRideAnimData()->m_fAnimLean   = (float)animLean / 50.0f;
    bike.m_bLeanMatrixCalculated = false;
    bike.CalculateLeanMatrix();
}

void tReplayTrainBlock::ExtractTrainUpdateData(CTrain& train, float interpolation) {
    As<tReplayVehicleBlock>()->ExtractVehicleUpdateData(*train.AsVehicle(), interpolation);

    train.m_fTrainSpeed = trainSpeed;
    train.m_fCurrentRailDistance = currentRailDistance;
    train.m_fLength = length;
    train.m_nTrackId = trackId;

    if (auto prevCarriage = prevCarriageRef) {
        CEntity::SafeCleanUpRef(train.m_pPrevCarriage);

        train.m_pPrevCarriage = GetVehiclePool()->GetAt(CReplay::FindPoolIndexForVehicle(prevCarriage - 1))->AsTrain();
        CEntity::SafeRegisterRef(train.m_pPrevCarriage);
    }

    if (auto nextCarriage = nextCarriageRef) {
        CEntity::SafeCleanUpRef(train.m_pNextCarriage);

        train.m_pNextCarriage = GetVehiclePool()->GetAt(CReplay::FindPoolIndexForVehicle(nextCarriage - 1))->AsTrain();
        CEntity::SafeRegisterRef(train.m_pNextCarriage);
    }
}
