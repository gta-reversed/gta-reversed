#include "StdInc.h"

#include "Remote.h"

void CRemote::InjectHooks() {
    RH_ScopedClass(CRemote);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(TakeRemoteControlledCarFromPlayer, 0x45AE80);
    RH_ScopedInstall(GivePlayerRemoteControlledCar, 0x45AB10);
}

// 0x45AE80
void CRemote::TakeRemoteControlledCarFromPlayer(bool bCreateRemoteVehicleExplosion) {
    auto&       playerInfo = FindPlayerInfo();
    auto* const vehicle    = playerInfo.m_pRemoteVehicle;
    assert(vehicle);

    if (vehicle->IsMissionVehicle()) {
        vehicle->SetVehicleCreatedBy(RANDOM_VEHICLE);
        CTheScripts::MissionCleanUp.RemoveEntityFromList(*vehicle);
    }
    vehicle->vehicleFlags.bIsLocked               = false;

    playerInfo.m_nTimeOfRemoteVehicleExplosion    = CTimer::GetTimeInMS();
    playerInfo.m_bAfterRemoteVehicleExplosion     = true;
    playerInfo.m_bCreateRemoteVehicleExplosion    = bCreateRemoteVehicleExplosion;
    playerInfo.m_bFadeAfterRemoteVehicleExplosion = true;
}

// 0x45AB10
void CRemote::GivePlayerRemoteControlledCar(CVector pos, float rotation, int16 modelId) {
    auto* const vehicle = [modelId]() -> CVehicle* {
        if (CModelInfo::IsHeliModel(modelId)) {
            return new CHeli(modelId, MISSION_VEHICLE);
        }
        if (CModelInfo::IsPlaneModel(modelId)) {
            return new CPlane(modelId, MISSION_VEHICLE);
        }
        return new CAutomobile(modelId, MISSION_VEHICLE, true);
    }();

    const float groundZ    = CWorld::FindGroundZFor3DCoord({ pos.x, pos.y, pos.z + 2.0f });
    const float distToBase = vehicle->GetDistanceFromCentreOfMassToBaseOfModel();
    vehicle->GetMatrix().SetRotateZOnly(rotation);
    vehicle->SetPosn(pos.x, pos.y, groundZ + distToBase);

    vehicle->SetStatus(STATUS_REMOTE_CONTROLLED);
    vehicle->vehicleFlags.bIsLocked = true;
    CCarCtrl::JoinCarWithRoadSystem(vehicle);

    auto& autoPilot                 = vehicle->m_autoPilot;
    autoPilot.m_nCarMission         = MISSION_NONE;
    autoPilot.m_nTempAction         = TEMPACT_NONE;
    autoPilot.m_nCarDrivingStyle    = DRIVING_STYLE_STOP_FOR_CARS;
    autoPilot.m_speed               = 9.0f;
    autoPilot.m_nCruiseSpeed        = 9;
    autoPilot.m_nCurrentLane        = 0;
    autoPilot.m_nNextLane           = 0;

    vehicle->vehicleFlags.bEngineOn = !vehicle->vehicleFlags.bEngineBroken;
    CWorld::Add(vehicle);

    if (auto* const playerVehicle = FindPlayerVehicle()) {
        playerVehicle->SetStatus(STATUS_FORCED_STOP);
    }

    auto& playerInfo            = FindPlayerInfo();
    playerInfo.m_pRemoteVehicle = vehicle;
    CEntity::RegisterReference(playerInfo.m_pRemoteVehicle);

    TheCamera.TakeControl(vehicle, MODE_CAM_ON_A_STRING, eSwitchType::INTERPOLATION, 1);
    TheCamera.SetZoomValueCamStringScript(1);
}
