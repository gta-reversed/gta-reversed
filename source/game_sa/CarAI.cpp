/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "TrafficLights.h"
#include "VehicleRecording.h"
#include "CarAI.h"
#include "CarCtrl.h"
#include "CullZones.h"
#include "TaskComplexCopInCar.h"
#include "TaskComplexDriveFireTruck.h"
#include "TaskComplexMedicTreatInjuredPed.h"
#include "TaskComplexKillPedFromBoat.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleGangDriveBy.h"
#include "TaskSimpleCarSetPedOut.h"
#include "Timer.h"
#include "eWeaponType.h"

void CCarAI::InjectHooks() {
    RH_ScopedClass(CCarAI);
    RH_ScopedCategory("AI");

    RH_ScopedInstall(AddAmbulanceOccupants, 0x41C4A0);
    RH_ScopedInstall(AddFiretruckOccupants, 0x41C600);
    RH_ScopedInstall(AddPoliceCarOccupants, 0x41C070);
    RH_ScopedInstall(BackToCruisingIfNoWantedLevel, 0x41BFA0);
    RH_ScopedInstall(CarHasReasonToStop, 0x41C050);
    RH_ScopedInstall(EntitiesGoHeadOn, 0x41CD00);
    RH_ScopedInstall(FindPoliceBikeMissionForWantedLevel, 0x41CA40);
    RH_ScopedInstall(FindPoliceBoatMissionForWantedLevel, 0x41CA50);
    RH_ScopedInstall(FindPoliceCarMissionForWantedLevel, 0x41C9D0);
    RH_ScopedInstall(FindPoliceCarSpeedForWantedLevel, 0x41CAA0);
    RH_ScopedInstall(FindSwitchDistanceClose, 0x41BF50);
    RH_ScopedInstall(FindSwitchDistanceFar, 0x41BF70);
    RH_ScopedInstall(GetCarToGoToCoors, 0x41CE30);
    RH_ScopedInstall(GetCarToGoToCoorsAccurate, 0x41D0E0);
    RH_ScopedInstall(GetCarToGoToCoorsRacing, 0x41D210);
    RH_ScopedInstall(GetCarToGoToCoorsStraightLine, 0x41CFB0);
    RH_ScopedInstall(GetCarToParkAtCoors, 0x41D350);
    RH_ScopedInstall(MakeWayForCarWithSiren, 0x41D660);
    RH_ScopedInstall(MellowOutChaseSpeed, 0x41D3D0);
    RH_ScopedInstall(MellowOutChaseSpeedBoat, 0x41CB70);
    RH_ScopedInstall(TellCarToBlockOtherCar, 0x41C900);
    RH_ScopedInstall(TellCarToFollowOtherCar, 0x41C960);
    RH_ScopedInstall(TellCarToRamOtherCar, 0x41C8A0);
    RH_ScopedInstall(TellOccupantsToLeaveCar, 0x41C760);
    RH_ScopedInstall(UpdateCarAI, 0x41DA30);
}

// 0x41C4A0
void CCarAI::AddAmbulanceOccupants(CVehicle* vehicle) {
    const auto driver    = vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
    const auto passenger = vehicle->SetupPassenger(1, PED_TYPE_NONE, false, false);

    const auto SetupPed = [vehicle](CPed* ped, CPed* partner, bool isDriver) {
        ped->GetTaskManager().SetTask(new CTaskSimpleCarDrive{ vehicle }, TASK_PRIMARY_DEFAULT);
        ped->GetTaskManager().SetTask(new CTaskComplexMedicTreatInjuredPed{ vehicle, partner, isDriver }, TASK_PRIMARY_PRIMARY);
    };

    SetupPed(driver, passenger, true);
    SetupPed(passenger, driver, false);
}

// 0x41C600
void CCarAI::AddFiretruckOccupants(CVehicle* vehicle) {
    const auto driver    = vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
    const auto passenger = vehicle->SetupPassenger(0, PED_TYPE_NONE, false, false);

    const auto SetupPed  = [vehicle](CPed* ped, CPed* partner, bool isDriver) {
        ped->GetTaskManager().SetTask(new CTaskSimpleCarDrive{ vehicle }, TASK_PRIMARY_DEFAULT);
        ped->GetTaskManager().SetTask(new CTaskComplexDriveFireTruck{ vehicle, partner, isDriver }, TASK_PRIMARY_PRIMARY);
    };

    SetupPed(driver, passenger, true);
    SetupPed(passenger, driver, false);
}

// 0x41C070
void CCarAI::AddPoliceCarOccupants(CVehicle* vehicle, bool arg2) {
    if (vehicle->vehicleFlags.bOccupantsHaveBeenGenerated) {
        return;
    }
    vehicle->vehicleFlags.bOccupantsHaveBeenGenerated = true;

    switch (vehicle->m_nModelIndex) {
    case MODEL_ENFORCER:
    case MODEL_FBIRANCH:
        vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
        for (int32 i = 0; i < 3; i++) {
            vehicle->SetupPassenger(i, PED_TYPE_NONE, false, false);
        }
        break;
    case MODEL_PREDATOR:
        if (FindPlayerPed()->GetWantedLevel() > 1) {
            const auto drvr = vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
            CTaskSimpleCarSetPedOut{ vehicle, TARGET_DOOR_DRIVER, true }.ProcessPed(drvr);
            drvr->AttachPedToEntity(vehicle, CVector(0.f, 0.f, 0.f), 0, TWO_PI, WEAPON_PISTOL);
            drvr->bStayInSamePlace = true;
            drvr->GetTaskManager().SetTask(
                new CTaskComplexKillPedFromBoat{ FindPlayerPed() },
                TASK_PRIMARY_PRIMARY
            );
        }
        vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
        break;
    case MODEL_RHINO:
    case MODEL_COPBIKE:
        vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
        break;
    case MODEL_BARRACKS:
    case MODEL_COPCARLA:
    case MODEL_COPCARSF:
    case MODEL_COPCARVG:
    case MODEL_COPCARRU:
        const auto drvr = vehicle->SetUpDriver(PED_TYPE_NONE, false, false);
        const auto plyrWantedLvl = FindPlayerPed()->GetWantedLevel();
        if (plyrWantedLvl > 1) {
            // The player's in some serious gourmet shit now... Let's give the cop a partner too!
            const auto psgr = vehicle->SetupPassenger(0, PED_TYPE_NONE, false, false);

            // Give one of them a weapon
            if (plyrWantedLvl > 2) {
                if (CGeneral::RandomBool(25.0f)) {
                    drvr->GiveDelayedWeapon(WEAPON_SHOTGUN, 1000);
                } else if (CGeneral::RandomBool(25.0f)) {
                    psgr->GiveDelayedWeapon(WEAPON_SHOTGUN, 1000);
                }
            }

            // Now set the tasks for them
            const auto SetupPed = [vehicle](CPed* ped, CPed* partner, bool isDriver) {
                ped->GetIntelligence()->ClearTasks(true, true);
                ped->GetTaskManager().SetTask(
                    new CTaskComplexCopInCar{ vehicle, partner, FindPlayerPed(), isDriver },
                    TASK_PRIMARY_PRIMARY,
                    true
                );
            };
            SetupPed(drvr, psgr, true);
            SetupPed(psgr, drvr, false);
        } else if (arg2 || CGeneral::RandomBool(50.0f)) {
            vehicle->SetupPassenger(0, PED_TYPE_NONE, false, false);
        }
        break;
    }
}

// 0x41BFA0
void CCarAI::BackToCruisingIfNoWantedLevel(CVehicle* vehicle) {
    assert(vehicle->IsBoat());

    if (!vehicle->vehicleFlags.bIsLawEnforcer) {
        return;
    }

    CWanted* wanted = FindPlayerWanted();
    if (wanted->m_nWantedLevel > 0 && !wanted->BackOff() && !CCullZones::NoPolice()) {
        return;
    }

    CCarCtrl::JoinCarWithRoadSystem(vehicle);

    vehicle->m_autoPilot.m_nCarDrivingStyle = DRIVING_STYLE_STOP_FOR_CARS;
    vehicle->m_autoPilot.m_nCarMission = CCullZones::NoPolice()
        ? MISSION_NONE
        : MISSION_CRUISE;
}

// 0x41C050
void CCarAI::CarHasReasonToStop(CVehicle* vehicle) {
    vehicle->m_autoPilot.m_nTimeToStartMission = CTimer::m_snTimeInMilliseconds;
}

// 0x41CD00
bool CCarAI::EntitiesGoHeadOn(CEntity* eA, CEntity* eB) {
    const auto d = (eA->GetPosition() - eB->GetPosition()).Normalized();
    return eA->GetForwardVector().Dot(d) <= -0.8f
        && eB->GetForwardVector().Dot(d) >= 0.8f;
}

// 0x41CA40
eCarMission CCarAI::FindPoliceBikeMissionForWantedLevel() {
    return MISSION_APPROACHPLAYER_FARAWAY;
}

// 0x41CA50
eCarMission CCarAI::FindPoliceBoatMissionForWantedLevel() {
    if (FindPlayerWanted()->GetWantedLevel() < 2) {
        return FindPlayerVehicle()
            ? MISSION_BLOCKPLAYER_FARAWAY
            : MISSION_BOAT_CIRCLEPLAYER;
    }
    return FindPlayerVehicle()
        ? MISSION_ATTACKPLAYER
        : MISSION_BOAT_CIRCLEPLAYER;
}

// 0x41C9D0
eCarMission CCarAI::FindPoliceCarMissionForWantedLevel() {
    float chance = 100.f;
    switch (FindPlayerWanted()->m_nWantedLevel) {
    case 0:
    case 1:  return MISSION_BLOCKPLAYER_FARAWAY;
    case 2:  chance = 75.f; break; // rand() % 4 != 3
    case 3:  chance = 50.f; break; // rand() % 4 < 2
    case 4:
    case 5:
    case 6:  chance = 25.f; break; // rand() % 4 == 0
    default: NOTSA_UNREACHABLE();
    }
    return CGeneral::RandomBool(chance)
        ? MISSION_BLOCKPLAYER_FARAWAY
        : MISSION_BLOCKPLAYER_HANDBRAKESTOP;
}

// 0x41CAA0
int32 CCarAI::FindPoliceCarSpeedForWantedLevel(CVehicle* vehicle) {
    const auto& maxVelocity = vehicle->m_pHandlingData->m_transmissionData.m_MaxFlatVelocity;
    switch (FindPlayerWanted()->m_nWantedLevel) {
    case 0:  return CGeneral::GetRandomNumberInRange<int32>(12, 16);
    case 1:  return 25;
    case 2:  return 34;
    case 3:  return (int32)(maxVelocity * GAME_SPEED_TO_CAR_AI_SPEED * 0.90f);
    case 4:  return (int32)(maxVelocity * GAME_SPEED_TO_CAR_AI_SPEED * 1.20f);
    case 5:  return (int32)(maxVelocity * GAME_SPEED_TO_CAR_AI_SPEED * 1.25f);
    case 6:  return (int32)(maxVelocity * GAME_SPEED_TO_CAR_AI_SPEED * 1.30f);
    default: NOTSA_UNREACHABLE();
    }
}

// 0x41BF50
float CCarAI::FindSwitchDistanceClose(CVehicle* vehicle) {
    return (float)vehicle->m_autoPilot.m_nStraightLineDistance;
}

// 0x41BF70
float CCarAI::FindSwitchDistanceFar(CVehicle* vehicle) {
    return vehicle->vehicleFlags.bIsLawEnforcer
        ? 50.f
        : FindSwitchDistanceClose(vehicle) + 5.0f;
}

// 0x41CE30
float CCarAI::GetCarToGoToCoors(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed) {
    const auto ap = &veh->m_autoPilot;
    switch (ap->m_nCarMission) {
    case MISSION_PARK_PERPENDICULAR_0:
    case MISSION_PARK_PERPENDICULAR_1:
    case MISSION_PARK_PARALLEL_0:
    case MISSION_PARK_PARALLEL_1:
        break;
    case MISSION_GOTOCOORDINATES:
    case MISSION_GOTOCOORDINATES_STRAIGHTLINE: {
        if (veh->GetPosition2D().EqualTo(coors, 5.f)) {
            break;
        }
        [[fallthrough]];
    }
    default: {
        ap->m_nCarDrivingStyle = drivingStyle;
        ap->ClearTempAct();
        if (setCruiseSpeed) {
            ap->SetCruiseSpeed(20);
        }
        if (veh->GetStatus() != STATUS_GHOST) {
            veh->SetStatus(STATUS_PHYSICS);
        }
        ap->SetCarMission(
            CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, coors, false, false)
                ? MISSION_GOTOCOORDINATES_STRAIGHTLINE
                : MISSION_GOTOCOORDINATES,
            0
        );
        break;
    }
    }
    return (veh->GetPosition2D() - coors).Magnitude();
}

// 0x41D0E0
float CCarAI::GetCarToGoToCoorsAccurate(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed) {
    const auto ap = &veh->m_autoPilot;

    switch (ap->m_nCarMission) {
    case MISSION_GOTOCOORDINATES_ACCURATE:
    case MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE: {
        if (!veh->GetPosition2D().EqualTo(coors, 2.f)) {
            ap->m_vecDestinationCoors = coors;
        }
        break;
    }
    default: { // Copy paste from `GetCarToGoToCoors`
        ap->m_nCarDrivingStyle = drivingStyle;
        ap->ClearTempAct();
        if (setCruiseSpeed) {
            ap->SetCruiseSpeed(20);
        }
        if (veh->GetStatus() != STATUS_GHOST) {
            veh->SetStatus(STATUS_PHYSICS);
        }
        ap->SetCarMission(
            CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, coors, false, false)
                ? MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE
                : MISSION_GOTOCOORDINATES_ACCURATE,
            0
        );
        break;
    }
    }

    return (veh->GetPosition2D() - coors).Magnitude();
}

// 0x41D210
float CCarAI::GetCarToGoToCoorsRacing(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed) {
    const auto ap = &veh->m_autoPilot;

    switch (ap->m_nCarMission) {
    case MISSION_GOTOCOORDINATES_RACING:
    case MISSION_GOTOCOORDINATES_STRAIGHTLINE: {
        if (!veh->GetPosition2D().EqualTo(coors, 2.f)) {
            ap->m_vecDestinationCoors = coors;
        }
        break;
    }
    default: {
        ap->m_nCarDrivingStyle = drivingStyle;
        ap->ClearTempAct();
        if (setCruiseSpeed) {
            ap->SetCruiseSpeed(20);
        }
        if (veh->GetStatus() != STATUS_GHOST) {
            veh->SetStatus(STATUS_PHYSICS);
        }
        ap->m_vecDestinationCoors = coors;
        ap->SetCarMission(MISSION_GOTOCOORDINATES_RACING, 0);
        CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, coors, false, false);
        break;
    }
    }

    return (veh->GetPosition2D() - coors).Magnitude();
}

// 0x41CFB0
float CCarAI::GetCarToGoToCoorsStraightLine(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed) {
    const auto ap = &veh->m_autoPilot;

    switch (ap->m_nCarMission) {
    case MISSION_GOTOCOORDINATES_STRAIGHTLINE: {
        if (!veh->GetPosition2D().EqualTo(coors, 2.f)) {
            ap->m_vecDestinationCoors = coors;
        }
        break;
    }
    default: {
        ap->m_nCarDrivingStyle = drivingStyle;
        ap->ClearTempAct();
        if (setCruiseSpeed) {
            ap->SetCruiseSpeed(20);
        }
        if (veh->GetStatus() != STATUS_GHOST) {
            veh->SetStatus(STATUS_PHYSICS);
        }
        ap->m_vecDestinationCoors = coors;
        ap->SetCarMission(MISSION_GOTOCOORDINATES_STRAIGHTLINE, 0);
        // TODO/BUG: No `CCarCtrl::JoinCarWithRoadSystemGotoCoors` ?
        break;
    }
    }

    return (veh->GetPosition2D() - coors).Magnitude();
}

// 0x41D350
float CCarAI::GetCarToParkAtCoors(CVehicle* veh, const CVector& coors) {
    veh->vehicleFlags.bCanPark = true;
    veh->m_autoPilot.SetCruiseSpeed(10);
    return (veh->GetPosition2D() - coors).Magnitude();
}

// 0x41D660
void CCarAI::MakeWayForCarWithSiren(CVehicle* carWithSiren) {
    float      carWithSirenSpeed2D;
    const auto carWithSirenMoveDir2D = CVector2D{ carWithSiren->GetMoveSpeed() }.Normalized(&carWithSirenSpeed2D);
    for (auto& v : GetVehiclePool()->GetAllValid()) {
        if (!v.IsAutomobile() && !v.IsBike()) {
            continue;
        }
        if (!notsa::contains({ STATUS_SIMPLE, STATUS_PHYSICS }, v.GetStatus())) {
            continue;
        }
        if (!v.IsCreatedBy(RANDOM_VEHICLE)) {
            continue;
        }
        if (v.vehicleFlags.bIsLawEnforcer) {
            continue;
        }
        if (v.m_pDriver && !v.m_pDriver->IsCreatedBy(PED_GAME)) {
            continue;
        }
        if (carWithSiren == &v) { // Thank god they remembered to check this!
            continue;
        }
        if (v.vehicleFlags.bIsFireTruckOnDuty || v.vehicleFlags.bIsAmbulanceOnDuty) {
            continue;
        }
        if (v.vehicleFlags.bMadDriver) {
            continue;
        }
        if (v.GetVehicleModelInfo()->m_nVehicleClass == VEHICLE_CLASS_BIG) {
            continue;
        }
        CVector dirToCarWithSiren = v.GetPosition() - carWithSiren->GetPosition();
        if (std::abs(dirToCarWithSiren.z) >= 5.f) { // TODO: Really should use a boundingbox height here instead....
            continue;
        }
        CVector2D dirToCarWithSiren2D{dirToCarWithSiren};
        const auto distToCarWithSiren = dirToCarWithSiren.Magnitude2D();
        if (distToCarWithSiren >= carWithSirenSpeed2D * 45.f + 40.f) {
            continue;
        }
        if (v.GetMoveSpeed().SquaredMagnitude2D() <= 0.05f) {
            continue;
        }
        const auto vehAP = &v.m_autoPilot;
        if (carWithSirenMoveDir2D.Dot(dirToCarWithSiren2D / distToCarWithSiren) <= 0.8f) { // 0x41D8A2
            if (dirToCarWithSiren2D.Dot(CVector2D{ v.GetMoveSpeed() }) < 0.f) { // Going in different directions?
                if (!notsa::contains({ TEMPACT_WAIT, TEMPACT_BRAKE }, vehAP->m_nTempAction)) {
                    vehAP->SetTempAction(TEMPACT_WAIT, 2'000);
                }
            }
        } else { // 0x41D8BD
            const auto goingSameDirDot = carWithSirenMoveDir2D.Dot(v.GetForward());
            if (goingSameDirDot > 0.7f || goingSameDirDot < -0.9f) { // 0x41D8DE
                if (!notsa::contains({ TEMPACT_SWIRVELEFT_STOP, TEMPACT_SWIRVERIGHT_STOP }, vehAP->m_nTempAction)) {
                    eAutoPilotTempAction a = carWithSirenMoveDir2D.Dot(dirToCarWithSiren2D) <= 0.f // 0x41D954
                        ? TEMPACT_SWIRVERIGHT_STOP
                        : TEMPACT_SWIRVELEFT_STOP;
                    if (goingSameDirDot < 0.f) {
                        a = a == TEMPACT_SWIRVELEFT_STOP
                            ? TEMPACT_SWIRVERIGHT_STOP
                            : TEMPACT_SWIRVELEFT_STOP;
                    }
                    if (a == TEMPACT_SWIRVERIGHT_STOP) {
                        a = TEMPACT_WAIT;
                    }
                    vehAP->SetTempAction(a, 2'500);
                }
                if (v.GetStatus() != STATUS_GHOST) {
                    v.SetStatus(STATUS_PHYSICS);
                }
            } else if (dirToCarWithSiren2D.Dot(v.GetMoveSpeed()) < 0.f) { // 0x41D910
                if (!notsa::contains({ TEMPACT_SWIRVELEFT_STOP, TEMPACT_SWIRVERIGHT_STOP }, vehAP->m_nTempAction)) {
                    vehAP->SetTempAction(TEMPACT_WAIT, 4'000);
                }
            }
        }
    }
}

// 0x41D3D0
void CCarAI::MellowOutChaseSpeed(CVehicle* vehicle) {
    const auto isPlayerInVeh = !!FindPlayerVehicle();
    const auto plyrToVehDist3D = (vehicle->GetPosition() - FindPlayerCoors()).Magnitude();
    const auto desiredSpeed = [&]() -> uint32 {
        switch (FindPlayerWanted()->GetWantedLevel())
        {
        case 1: { // 0x41D438
            if (isPlayerInVeh) {
                if (plyrToVehDist3D < 10.0f) {
                    return 15;
                } else if (plyrToVehDist3D < 20.0f) {
                    return 22;
                }
            } else {
                if (plyrToVehDist3D < 20.0f) {
                    return 5;
                } else if (plyrToVehDist3D < 40.0f) {
                    return 13;
                }
            }
            return 25;
        }
        case 2: {
            if (isPlayerInVeh) {
                if (plyrToVehDist3D < 10.0f) {
                    return 27;
                } else if (plyrToVehDist3D < 20.0f) {
                    return 30;
                }
            } else {
                if (plyrToVehDist3D < 20.0f) {
                    return 5;
                } else if (plyrToVehDist3D < 40.0f) {
                    return 18;
                }
            }
            return 34;
        }
        }
        return (uint32)vehicle->m_autoPilot.m_nCruiseSpeed;
    }();
    vehicle->m_autoPilot.SetCruiseSpeed(
        !isPlayerInVeh && desiredSpeed >= 10 && plyrToVehDist3D <= 30.f && FindPlayerPed()->m_vecMoveSpeed.SquaredMagnitude() < sq(0.07f)
            ? 10            // Clamp to 10
            : desiredSpeed
    );
}

// 0x41CB70
void CCarAI::MellowOutChaseSpeedBoat(CVehicle* vehicle) {
    assert(vehicle->IsBoat());
    vehicle->m_autoPilot.m_nCruiseSpeed = []{
        switch (FindPlayerWanted()->GetWantedLevel()) {
        case 0:  return 8;
        case 1:  return 10;
        case 2:  return 15;
        case 3:  return 20;
        case 4:  return 25;
        case 5:  return 30;
        case 6:  return 40;
        default: NOTSA_UNREACHABLE();
        }
    }();
}

// unused
// 0x41C900
void CCarAI::TellCarToBlockOtherCar(CVehicle* vehicle1, CVehicle* vehicle2) {
    /*
    vehicle1->m_autoPilot.m_pTargetCar = vehicle2;
    CEntity::SafeRegisterRef(vehicle1->m_autoPilot.m_pTargetCar);
    CCarCtrl::JoinCarWithRoadSystem(vehicle1);
    vehicle1->m_autoPilot.SetCarMission(MISSION_BLOCKCAR_FARAWAY);
    vehicle1->vehicleFlags.bEngineOn    = !vehicle1->vehicleFlags.bEngineBroken;

    vehicle1->m_autoPilot.SetCruiseSpeed(std::max(vehicle1->m_autoPilot.m_nCruiseSpeed, (uint8)6));
    */
    NOTSA_UNREACHABLE("Unused");
}

// 0x41C960
void CCarAI::TellCarToFollowOtherCar(CVehicle* follower, CVehicle* toFollow, float radius) {
    follower->m_autoPilot.m_TargetEntity = toFollow;
    CEntity::SafeRegisterRef(follower->m_autoPilot.m_TargetEntity);

    CCarCtrl::JoinCarWithRoadSystem(follower);

    follower->m_autoPilot.m_nCarMission     = MISSION_FOLLOWCAR_FARAWAY;
    follower->vehicleFlags.bEngineOn        = follower->vehicleFlags.bEngineBroken == 0;
    follower->m_autoPilot.m_nCruiseSpeed    = std::max(follower->m_autoPilot.m_nCruiseSpeed, (uint8)6);
    follower->m_autoPilot.m_ucCarFollowDist = (uint8)radius;
}

// unused
// 0x41C8A0
void CCarAI::TellCarToRamOtherCar(CVehicle* vehicle1, CVehicle* vehicle2) {
    /*
    vehicle1->m_autoPilot.m_pTargetCar = vehicle2;
    CEntity::SafeRegisterRef(vehicle1->m_autoPilot.m_pTargetCar);
    CCarCtrl::JoinCarWithRoadSystem(vehicle1);
    vehicle1->m_autoPilot.SetCarMission(MISSION_RAMCAR_FARAWAY);
    vehicle1->vehicleFlags.bEngineOn    = !vehicle1->vehicleFlags.bEngineBroken;

    vehicle1->m_autoPilot.SetCruiseSpeed(std::max(vehicle1->m_autoPilot.m_nCruiseSpeed, (uint8)6));
    */
    NOTSA_UNREACHABLE("Unused");
}

// 0x41C760
void CCarAI::TellOccupantsToLeaveCar(CVehicle* vehicle) {
    auto TellToLeaveCar = [vehicle](CPed* ped) {
        if (!ped) {
            return;
        }

        CTaskComplexCopInCar* task = ped->GetTaskManager().Find<CTaskComplexCopInCar>(false);
        if (task) {
            task->m_flag0x2 = true;
        } else {
            ped->GetTaskManager().SetTask(new CTaskComplexLeaveCar{ vehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false }, TASK_PRIMARY_PRIMARY, false);
        }
    };

    TellToLeaveCar(vehicle->m_pDriver);
    rng::for_each(vehicle->GetPassengers(), TellToLeaveCar);
}

// 0x41DA30
void CCarAI::UpdateCarAI(CVehicle* veh) {
    const auto ap = &veh->m_autoPilot;

    if (ap->m_vehicleRecordingId >= 0 && (!CVehicleRecording::bUseCarAI[ap->m_vehicleRecordingId] || veh->IsSubHeli())) {
        return;
    }

    if (ap->m_nCarMission == MISSION_PLANE_ATTACK_PLAYER_POLICE) { // 0x41DAA0
        const auto wntd = FindPlayerWanted();
        if (wntd->m_nWantedLevel < 3 || !FindPlayerVehicle()) {
            ap->m_vecDestinationCoors = CVector{-6'000.f, -10'000.f, 500.f};
        }
    }

    if (veh->vehicleFlags.bIsLawEnforcer) { // 0x41DAF8
        switch (ap->m_nCarMission) {
        case MISSION_BLOCKPLAYER_FARAWAY:
        case MISSION_RAMPLAYER_FARAWAY:
        case MISSION_BLOCKPLAYER_CLOSE:
        case MISSION_RAMPLAYER_CLOSE:
        case MISSION_APPROACHPLAYER_FARAWAY:
        case MISSION_APPROACHPLAYER_CLOSE:
        case MISSION_KILLPED_FARAWAY:
        case MISSION_KILLPED_CLOSE:
            ap->SetCruiseSpeed(FindPoliceCarSpeedForWantedLevel(veh));
        }
    }

    const auto& vehPos    = veh->GetPosition();
    const auto  plyrVeh   = FindPlayerVehicle();
    const auto  plyr      = FindPlayerPed();
    const auto& plyrPos   = plyr->GetPosition();
    const auto  plyrCoors = FindPlayerCoors();

    switch (veh->GetStatus()) {
    case STATUS_SIMPLE:
    case STATUS_PHYSICS:
    case STATUS_GHOST: {
        switch (ap->m_nCarMission) {
        case MISSION_RAMPLAYER_FARAWAY: { // 0x41DB72 - If close enough switch to `MISSION_RAMPLAYER_CLOSE`
            const auto vehPlyrDist2DSq = (vehPos - plyrPos).SquaredMagnitude2D();
            if (   vehPlyrDist2DSq <= sq(FindSwitchDistanceClose(veh))
                || EntitiesGoHeadOn(plyr, veh) && vehPlyrDist2DSq < sq(40.f)
            ) {
                ap->SetCarMission(MISSION_RAMPLAYER_CLOSE);
                if (veh->UsesSiren()) {
                    veh->vehicleFlags.bSirenOrAlarm = true;
                }
            }
            BackToCruisingIfNoWantedLevel(veh);
            break;
        }
        case MISSION_RAMPLAYER_CLOSE: // 0x41DC92
        case MISSION_BLOCKPLAYER_CLOSE: // 0x41E07D (This and `MISSION_RAMPLAYER_CLOSE` have very similar code, so i combined them. (The code below is based on it [0x41DC92])
        { 
            const auto vehPlyrDist2DSq = (vehPos - plyrPos).SquaredMagnitude2D();

            const auto maxSpeedSq = ap->m_nCarMission == MISSION_RAMPLAYER_CLOSE
                ? sq(0.05f)
                : sq(0.04f);

            if (sq(FindSwitchDistanceFar(veh)) < vehPlyrDist2DSq) { // 0x41DD4F | 0x41E13A
                if (!CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, FindPlayerCoors(), true, false)) {
                    ap->m_nCarMission               = MISSION_RAMPLAYER_FARAWAY;
                    veh->m_nHornCounter             = 0;
                    veh->vehicleFlags.bSirenOrAlarm = false;
                }
                if (veh->vehicleFlags.bIsLawEnforcer) {
                    MellowOutChaseSpeed(veh);
                    BackToCruisingIfNoWantedLevel(veh);
                }
                break;
            }

            if (ap->m_nCarMission == MISSION_RAMPLAYER_CLOSE) { // 0x41DDA2
                if (plyrVeh && veh->GetHasCollidedWith(plyrVeh)) { // 0x41DDA2
                    if (!notsa::contains({ TEMPACT_TURNLEFT, TEMPACT_TURNRIGHT }, ap->m_nTempAction)) {
                        ap->SetTempAction(
                            TEMPACT_REVERSE,
                            plyrVeh->GetMoveSpeed().SquaredMagnitude() >= maxSpeedSq
                                ? 50
                                : 800
                        );
                    }
                }
            }

            if (plyrVeh && plyrVeh->GetMoveSpeed().SquaredMagnitude() < maxSpeedSq) { // 0x41DE22 | 0x41E18D
                veh->m_nCopsInCarTimer += (int16)(CTimer::GetTimeStep() * (1000.f / 60.f));
            } else {
                veh->m_nCopsInCarTimer = 0;
            }

            if (   !plyrVeh // 0x41DE95 | 0x41E1F0
                || plyrVeh->IsUpsideDown()
                || plyrVeh->GetMoveSpeed().SquaredMagnitude() < maxSpeedSq && veh->m_nCopsInCarTimer > 2500
                    ) {
                        if (veh->vehicleFlags.bIsLawEnforcer) { // 0x41DED4 | 0x41E23F
                            if ((veh->GetModelID() != MODEL_RHINO || veh->m_nRandomSeed > 10'000) && vehPlyrDist2DSq <= sq(10.f)) { // 0x41DEE1 | 0x41E254
                                TellOccupantsToLeaveCar(veh);
                                ap->SetCruiseSpeed(0);
                                ap->SetCarMission(MISSION_NONE);
                                if (plyr->GetWantedLevel() <= 1) {
                                    veh->vehicleFlags.bSirenOrAlarm = false;
                                }
                            }
                        }
            }

            break;
        }
        case MISSION_GOTOCOORDINATES: { // 0x41E2BF - If close enough switch to `MISSION_GOTOCOORDS_STRAIGHT`
            if ((veh->GetPosition() - ap->m_vecDestinationCoors).SquaredMagnitude2D() <= sq(FindSwitchDistanceClose(veh))) {
                ap->SetCarMission(MISSION_GOTOCOORDINATES_STRAIGHTLINE);
            }
            break;
        }
        case MISSION_GOTOCOORDINATES_STRAIGHTLINE: { // 0x41E3B1
            const auto distToDestSq = (veh->GetPosition() - ap->m_vecDestinationCoors).SquaredMagnitude2D();

            if (distToDestSq < sq(3.f)) {
                ap->ClearTempAct();
                ap->ClearCarMission();
            }
            else if (distToDestSq >= sq((float)ap->m_nStraightLineDistance + 5.f) && CTimer::GetFrameCounter() % 8 == 0) {
                ap->ClearTempAct();
                ap->SetCarMission(
                    CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, ap->m_vecDestinationCoors, true, false)
                    ? MISSION_GOTOCOORDINATES_STRAIGHTLINE
                    : MISSION_GOTOCOORDINATES
                );
            }

            break;
        }
        case MISSION_EMERGENCYVEHICLE_STOP:
        case MISSION_PROTECTION_REAR:
        case MISSION_PROTECTION_FRONT:
            break;
        case MISSION_GOTOCOORDINATES_ACCURATE: { // 0x41E338 (Pretty much copy paste `MISSION_GOTOCOORDINATES`)
            if ((veh->GetPosition() - ap->m_vecDestinationCoors).SquaredMagnitude2D() <= sq(FindSwitchDistanceClose(veh))) {
                ap->SetCarMission(MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE);
            }
            break;
        }
        case MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE: { // 0x41E473 (Pretty much copy paste `MISSION_GOTOCOORDINATES_STRAIGHTLINE`)
            const auto distToDest2DSq = (veh->GetPosition() - ap->m_vecDestinationCoors).SquaredMagnitude2D();

            if (distToDest2DSq < sq(1.f)) { // Inverted
                ap->ClearTempAct();
                ap->ClearCarMission();
                if (veh->vehicleFlags.bParking) {
                    TellOccupantsToLeaveCar(veh);
                    veh->vehicleFlags.bParking = false;
                }
            }
            else if (distToDest2DSq >= sq((float)ap->m_nStraightLineDistance + 5.f) && CTimer::GetFrameCounter() % 8 == 0) {
                ap->ClearTempAct();
                ap->SetCarMission(
                    CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, ap->m_vecDestinationCoors, true, false)
                    ? MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE
                    : MISSION_GOTOCOORDINATES_ACCURATE
                );
            }

            break;
        }
        case MISSION_RAMCAR_FARAWAY: { // 0x41E568 (Pretty much copy-paste `MISSION_GOTOCOORDINATES_STRAIGHTLINE`)
            if (!ap->m_TargetEntity) { // Entity was destroyed
                ap->ClearCarMission();
                break;
            }

            if ((veh->GetPosition() - ap->m_TargetEntity->GetPosition()).SquaredMagnitude2D() <= sq(FindSwitchDistanceClose(veh))) {
                ap->SetCarMission(MISSION_RAMCAR_CLOSE);
            }

            break;
        }
        case MISSION_RAMCAR_CLOSE: { // 0x41E642
            if (!ap->m_TargetEntity) {
                ap->SetCarMission(MISSION_CRUISE); // Weird, above it was `NONE`
                break;
            }

            if ((veh->GetPosition() - ap->m_TargetEntity->GetPosition()).SquaredMagnitude2D() >= sq(FindSwitchDistanceFar(veh))) { // Inverted
                ap->SetCarMission(MISSION_RAMCAR_FARAWAY);
                CCarCtrl::JoinCarWithRoadSystem(veh);
            }
            else {
                if (veh->GetHasCollidedWith(ap->m_TargetEntity) && veh->GetMoveSpeed().SquaredMagnitude() <= sq(0.04f)) {
                    ap->SetTempAction(TEMPACT_REVERSE, 800);
                }
            }

            break;
        }
        case MISSION_BLOCKCAR_FARAWAY: { // 0x41E79D
            if (!ap->m_TargetEntity) {
                ap->ClearCarMission();
                break;
            }

            if ((veh->GetPosition() - ap->m_TargetEntity->GetPosition()).SquaredMagnitude2D() <= sq(FindSwitchDistanceClose(veh))) {
                ap->SetCarMission(MISSION_BLOCKCAR_CLOSE);
                if (veh->UsesSiren()) {
                    veh->vehicleFlags.bSirenOrAlarm = true;
                }
            }

            break;
        }
        case MISSION_BLOCKCAR_CLOSE: { // 0x41E88D
            if (!ap->m_TargetEntity) {
                ap->ClearCarMission();
                break;
            }

            if ((veh->GetPosition() - ap->m_TargetEntity->GetPosition()).SquaredMagnitude2D() >= sq(FindSwitchDistanceClose(veh))) {
                veh->vehicleFlags.bSirenOrAlarm = false;
                veh->m_nHornCounter             = 0;
                CCarCtrl::JoinCarWithRoadSystem(veh);
            }

            break;
        }
        case MISSION_ATTACKPLAYER: { // 0x41EE05
            if (!veh->vehicleFlags.bIsLawEnforcer) {
                break;
            }
            if (!plyrVeh) {
                ap->SetCarMission(MISSION_BOAT_CIRCLEPLAYER);
            }
            MellowOutChaseSpeedBoat(veh);
            BackToCruisingIfNoWantedLevel(veh);
            break;
        }

        case MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_1: { // 0x41EB46
            if ((veh->GetPosition() - ap->m_vecDestinationCoors).SquaredMagnitude2D() <= sq(1.5f)) {
                ap->SetCarMission(MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_2);
            }
            break;
        }
        case MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_2: { // 0x41EBB9
            const auto vehToPlayerDist2DSq = (FindPlayerCoors() - veh->GetPosition()).SquaredMagnitude2D();
            const auto wntd = FindPlayerWanted();
            if (   vehToPlayerDist2DSq <= sq(13.f) || vehToPlayerDist2DSq >= sq(70.f)
                || wntd->m_bEverybodyBackOff
                || veh->vehicleFlags.bIsLawEnforcer && (wntd->GetWantedLevel() == 0 || wntd->BackOff() || CCullZones::NoPolice())
            ) {
                TellOccupantsToLeaveCar(veh);
                ap->SetCarMission(MISSION_STOP_FOREVER);
                ap->SetCruiseSpeed(0);
            }
            break;
        }
        case MISSION_BLOCKPLAYER_FORWARDANDBACK: { // 0x41ED1C
            if (!plyrVeh || FindPlayerSpeed().Dot2D((veh->GetPosition() - FindPlayerCoors()).Normalized()) > 0.05f) {
                ap->SetCarMission(MISSION_BLOCKPLAYER_CLOSE);
            }
            BackToCruisingIfNoWantedLevel(veh);
            break;
        }
        case MISSION_ESCORT_LEFT:
        case MISSION_ESCORT_RIGHT:
        case MISSION_ESCORT_REAR:
        case MISSION_ESCORT_FRONT: { // 0x41F771
            if (ap->m_nStraightLineDistance >= 240) {
                break;
            }

            if ((veh->GetPosition() - FindPlayerCoors()).SquaredMagnitude2D() >= sq(FindSwitchDistanceFar(veh))) {
                if (!CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, FindPlayerCoors())) {
                    ap->m_nCarMission = [&]{
                        switch (ap->m_nCarMission) {
                        case MISSION_ESCORT_LEFT:  return MISSION_ESCORT_LEFT_FARAWAY;
                        case MISSION_ESCORT_RIGHT: return MISSION_ESCORT_RIGHT_FARAWAY;
                        case MISSION_ESCORT_REAR:  return MISSION_ESCORT_REAR_FARAWAY;
                        case MISSION_ESCORT_FRONT: return MISSION_ESCORT_FRONT_FARAWAY;
                        default:                   NOTSA_UNREACHABLE();
                        }
                    }();
                }
            }

            break;
        }
        case MISSION_APPROACHPLAYER_FARAWAY: { // 0x41F033
            if ((FindPlayerCoors() - veh->GetPosition()).SquaredMagnitude2D() <= sq(FindSwitchDistanceClose(veh))) {
                ap->SetCarMission(MISSION_APPROACHPLAYER_CLOSE);
                if (veh->UsesSiren()) {
                    veh->vehicleFlags.bSirenOrAlarm = true;
                }
            }
            CCarAI::BackToCruisingIfNoWantedLevel(veh);
            break;
        }
        case MISSION_APPROACHPLAYER_CLOSE: { // 0x41F12D
            const auto vehToPlyrDist2DSq = (FindPlayerCoors() - veh->GetPosition()).SquaredMagnitude2D();

            if (vehToPlyrDist2DSq <= sq(FindSwitchDistanceFar(veh))) {
                if (plyrVeh && plyrVeh->GetMoveSpeed().SquaredMagnitude() <= sq(0.05f)) {
                    veh->m_nCopsInCarTimer += (uint32)CTimer::GetTimeStepInMS();
                } else {
                    veh->m_nCopsInCarTimer = 0;
                }

                if ((!plyrVeh || plyrVeh->IsUpsideDown() || veh->m_nCopsInCarTimer >= (veh->GetModelID() == MODEL_COPBIKE ? 2500 : 20'000)) && veh->vehicleFlags.bIsLawEnforcer && vehToPlyrDist2DSq <= sq(10.f)) {
                    TellOccupantsToLeaveCar(veh);
                    ap->ClearCarMission();
                    ap->SetCruiseSpeed(0);
                    if (FindPlayerWanted()->GetWantedLevel() <= 1) {
                        veh->vehicleFlags.bSirenOrAlarm = false;
                    }
                } else if (veh->GetModelID() == MODEL_COPBIKE && veh->m_pDriver) {
                    const auto tUseSeq = CTask::DynCast<CTaskComplexSequence>(veh->m_pDriver->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY));
                    if (!tUseSeq || (!tUseSeq->Contains(TASK_COMPLEX_ENTER_CAR_AS_DRIVER)) && !tUseSeq->Contains(TASK_SIMPLE_GANG_DRIVEBY)) { 
                        veh->m_pDriver->GetEventGroup().Add(
                            CEventScriptCommand{
                                TASK_PRIMARY_PRIMARY,
                                new CTaskComplexSequence{
                                    new CTaskComplexEnterCarAsDriver{veh},
                                    new CTaskSimpleGangDriveBy{FindPlayerPed(), nullptr, 50.f, 50, eDrivebyStyle::AI_ALL_DIRN, false}
                                }
                            }
                        );
                        veh->m_pDriver->SetCurrentWeapon(WEAPON_PISTOL_SILENCED);
                    }
                }
            } else if (!CCarCtrl::JoinCarWithRoadSystemGotoCoors(veh, FindPlayerCoors())) {
                veh->vehicleFlags.bSirenOrAlarm = false;
                veh->m_nHornCounter             = 0;
                ap->SetCarMission(MISSION_APPROACHPLAYER_FARAWAY);
            }

            if (veh->vehicleFlags.bIsLawEnforcer) {
                MellowOutChaseSpeed(veh);
            }
            BackToCruisingIfNoWantedLevel(veh);
            break;
        }
        case MISSION_FOLLOWCAR_FARAWAY: { // 0x41E989
            if (!ap->m_TargetEntity) {
                ap->ClearCarMission();
                break;
            }

            if ((veh->GetPosition() - ap->m_TargetEntity->GetPosition()).SquaredMagnitude2D() <= sq(FindSwitchDistanceClose(veh))) {
                ap->SetCarMission(MISSION_FOLLOWCAR_CLOSE);
            }

            break;
        }
        case MISSION_FOLLOWCAR_CLOSE: { // 0x41EA63
            if (!ap->m_TargetEntity) {
                ap->ClearCarMission();
                break;
            }

            if ((veh->GetPosition() - ap->m_TargetEntity->GetPosition()).SquaredMagnitude2D() >= sq(FindSwitchDistanceFar(veh))) {
                ap->SetCarMission(MISSION_FOLLOWCAR_FARAWAY);
                CCarCtrl::JoinCarWithRoadSystem(veh);
            }

            break;
        }
        case MISSION_KILLPED_FARAWAY: { // 0x41EE76
            if (!ap->m_TargetEntity) {
                ap->ClearCarMission();
                break;
            }

            if ((veh->GetPosition() - ap->m_TargetEntity->GetPosition()).SquaredMagnitude2D() >= sq(FindSwitchDistanceFar(veh))) {
                ap->SetCarMission(MISSION_KILLPED_CLOSE);
            }

            break;
        }
        case MISSION_KILLPED_CLOSE: { // 0x41EF50
            if (!ap->m_TargetEntity) {
                ap->ClearCarMission();
                break;
            }

            if ((veh->GetPosition() - ap->m_TargetEntity->GetPosition()).SquaredMagnitude2D() >= sq(FindSwitchDistanceFar(veh))) {
                ap->SetCarMission(MISSION_KILLPED_FARAWAY);
                CCarCtrl::JoinCarWithRoadSystem(veh);
            }

            break;
        }
        case MISSION_DO_DRIVEBY_CLOSE: { // 0x41F5B1
            if (!ap->m_TargetEntity) {
                ap->ClearCarMission();
                break;
            }

            if ((veh->GetPosition() - ap->m_TargetEntity->GetPosition()).SquaredMagnitude2D() >= sq(FindSwitchDistanceFar(veh))) {
                ap->SetCarMission(MISSION_DO_DRIVEBY_FARAWAY);
                CCarCtrl::JoinCarWithRoadSystem(veh);
            }

            break;
        }
        case MISSION_DO_DRIVEBY_FARAWAY: { // 0x41F4D7
            if (!ap->m_TargetEntity) {
                ap->ClearCarMission();
                break;
            }

            if ((veh->GetPosition() - ap->m_TargetEntity->GetPosition()).SquaredMagnitude2D() <= sq(FindSwitchDistanceClose(veh))) {
                ap->SetCarMission(MISSION_DO_DRIVEBY_CLOSE);
            }

            break;
        }
        case MISSION_BOAT_CIRCLEPLAYER: { // 0x41EE3D
            if (veh->vehicleFlags.bIsLawEnforcer) {
                if (plyrVeh) {
                    ap->SetCarMission(MISSION_ATTACKPLAYER);
                }
                ap->SetCruiseSpeed(10);
                BackToCruisingIfNoWantedLevel(veh);
            }
            break;
        }
        case MISSION_ESCORT_LEFT_FARAWAY:
        case MISSION_ESCORT_RIGHT_FARAWAY:
        case MISSION_ESCORT_REAR_FARAWAY:
        case MISSION_ESCORT_FRONT_FARAWAY: {
            if ((veh->GetPosition() - FindPlayerCoors()).SquaredMagnitude2D() <= sq(FindSwitchDistanceClose(veh))) {
                ap->m_nCarMission = [&]{
                    switch (ap->m_nCarMission) {
                    case MISSION_ESCORT_LEFT_FARAWAY:  return MISSION_ESCORT_LEFT;
                    case MISSION_ESCORT_RIGHT_FARAWAY: return MISSION_ESCORT_RIGHT;
                    case MISSION_ESCORT_REAR_FARAWAY:  return MISSION_ESCORT_REAR;
                    case MISSION_ESCORT_FRONT_FARAWAY: return MISSION_ESCORT_FRONT;
                    default:                           NOTSA_UNREACHABLE();
                    }
                }();
            }
            break;
        }
        default: {
            if (!veh->vehicleFlags.bIsLawEnforcer) {
                break;
            }

            if (FindPlayerWanted()->GetWantedLevel() <= 0 || CCullZones::NoPolice()) {
                break;
            }

            if (std::abs(plyrCoors.x - vehPos.x) >= 10.f || std::abs(plyrCoors.y - vehPos.y) >= 10.f) { // 0x41F9B4
                ap->SetCruiseSpeed(FindPoliceCarSpeedForWantedLevel(veh));

                if (veh->GetStatus() != STATUS_GHOST) {
                    veh->SetStatus(STATUS_PHYSICS);
                }

                switch (veh->GetVehicleAppearance()) {
                case VEHICLE_APPEARANCE_BOAT: ap->SetCarMission(FindPoliceBoatMissionForWantedLevel()); break;
                case VEHICLE_APPEARANCE_BIKE: ap->SetCarMission(FindPoliceBikeMissionForWantedLevel()); break;
                default:                      ap->SetCarMission(FindPoliceCarMissionForWantedLevel());  break;
                }

                ap->ClearTempAct();
                ap->m_nCarDrivingStyle = DRIVING_STYLE_AVOID_CARS;
            } else if (ap->m_nCarMission == MISSION_CRUISE) { // 0x41F9CE
                if (veh->GetStatus() != STATUS_GHOST) {
                    veh->SetStatus(STATUS_PHYSICS);
                }

                TellOccupantsToLeaveCar(veh);

                ap->SetCruiseSpeed(0);
                ap->ClearCarMission();
                if (FindPlayerWanted()->GetWantedLevel() <= 1) {
                    veh->vehicleFlags.bSirenOrAlarm = false;
                }
            }

            break;
        }
        }
        break;
    }
    case STATUS_ABANDONED:
    case STATUS_WRECKED: {
        ap->SetCruiseSpeed(0);
        ap->SetCarMission(MISSION_NONE);
        break;
    }
    }

    // 0x41E2A5 [It's actually here, not inside the switch]
    if (veh->vehicleFlags.bIsLawEnforcer) {
        if (FindPlayerWanted()->GetWantedLevel() >= 1) {
            if (CCullZones::CurrentFlags_Player & eZoneAttributes::CAM_CLOSE_IN_FOR_PLAYER) {
                TellOccupantsToLeaveCar(veh);
                ap->SetCarMission(MISSION_NONE);
                ap->SetCruiseSpeed(0);
            }
        }
    }

    const auto vehMvSpeed2DSq = veh->GetMoveSpeed().SquaredMagnitude2D();

    if (vehMvSpeed2DSq >= sq(0.05f)) {
        ap->m_nTimeSwitchedToRealPhysics = CTimer::GetTimeInMS();
        ap->m_nTimeToStartMission        = CTimer::GetTimeInMS();
    }

    if (ap->m_nTempAction == TEMPACT_NONE) { // 0x41FB1B
        if (ap->m_nCruiseSpeed == 0 || notsa::contains({ // 0x41FB29
                MISSION_NONE,
                MISSION_STOP_FOREVER,
                MISSION_BLOCKPLAYER_HANDBRAKESTOP,
                MISSION_PROTECTION_REAR,
                MISSION_PROTECTION_FRONT,
                MISSION_ESCORT_LEFT,
                MISSION_ESCORT_RIGHT,
                MISSION_ESCORT_REAR,
                MISSION_ESCORT_FRONT,
                MISSION_FOLLOWCAR_CLOSE,
            }, ap->m_nCarMission)
        ) {
            if (CTimer::GetTimeInMS() - veh->m_nLastCollisionTime > 500) { // 0x41FBA8
                ap->m_nTimeToStartMission = CTimer::GetTimeInMS();
            }

            if (vehMvSpeed2DSq <= sq(0.012f)) { // 0x41FBB6
                const auto updateInterval = notsa::contains({ DRIVING_STYLE_STOP_FOR_CARS, DRIVING_STYLE_STOP_FOR_CARS_IGNORE_LIGHTS }, ap->m_nCarDrivingStyle)
                    ? 500u * (veh->m_nRandomSeed % 16) + 40u
                    : 1000u;
                if ((CTimer::GetTimeInMS() - ap->m_nTimeToStartMission) > updateInterval) {
                    ap->m_ucTempActionMode = CTimer::GetTimeInMS() >= ap->m_LastUpdateTimeMs + 10'000 // 0x41FC02
                        ? 0
                        : (ap->m_ucTempActionMode + 1) % 4;

                    ap->m_LastUpdateTimeMs = CTimer::GetTimeInMS();

                    if (ap->m_nCarMission != MISSION_CRUISE || veh->IsCreatedBy(MISSION_VEHICLE) || (veh->vehicleFlags.bUsedForReplay)) { // 0x41FC4E
                        switch (ap->m_ucTempActionMode) {
                        case 0:  ap->SetTempAction(TEMPACT_REVERSE, 1'500);       break;
                        case 1:  ap->SetTempAction(TEMPACT_REVERSE, 4'000);       break;
                        case 2:  ap->SetTempAction(TEMPACT_REVERSE_LEFT, 2'500);  break;
                        case 3:  ap->SetTempAction(TEMPACT_REVERSE_RIGHT, 2'500); break;
                        default: NOTSA_UNREACHABLE();
                        }
                    } else {
                        ap->SetTempAction(TEMPACT_REVERSE, 750); 
                    }

                    ap->m_nTimeToStartMission = CTimer::GetTimeInMS();

                    if (veh->IsCreatedBy(RANDOM_VEHICLE)) { // 0x41FCF2
                        switch (ap->m_nCarDrivingStyle) {
                        case DRIVING_STYLE_STOP_FOR_CARS:
                        case DRIVING_STYLE_SLOW_DOWN_FOR_CARS:
                        case DRIVING_STYLE_STOP_FOR_CARS_IGNORE_LIGHTS:
                        case DRIVING_STYLE_STOP_FOR_CARS_IGNORE_LIGHTS|DRIVING_STYLE_AVOID_CARS:
                            veh->m_autoPilot.m_nCarDrivingStyle = DRIVING_STYLE_AVOID_CARS;
                            break;
                        }
                    }

                    if (veh->GetStatus() == STATUS_SIMPLE) { // 0x41FD18
                        veh->SetStatus(STATUS_PHYSICS);
                    }

                    veh->PlayCarHorn();
                }
            }
        }
    }

    if (veh->m_nRandomSeed % 8 == 0) { // 0x41FD3C
        if (   CTimer::GetTimeInMS() - ap->m_nTimeSwitchedToRealPhysics > 30'000
            && CTimer::GetPreviousTimeInMS() - ap->m_nTimeSwitchedToRealPhysics < 30'000
            && ap->m_nCarMission == MISSION_CRUISE
            && !CTrafficLights::ShouldCarStopForBridge(veh)
            ) {
            CCarCtrl::SwitchVehicleToRealPhysics(veh);
            ap->m_nCarDrivingStyle = DRIVING_STYLE_AVOID_CARS;
            ap->SetTempAction(TEMPACT_REVERSE, 400);
        }
    }

    if (veh->vehicleFlags.bIsLawEnforcer) { // 0x41FDA1
        if (notsa::contains({ MISSION_RAMPLAYER_FARAWAY, MISSION_RAMPLAYER_CLOSE }, ap->m_nCarMission)) {
            if (plyrVeh) {
                if (plyrVeh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE) {
                    ap->SetCarMission(MISSION_BLOCKPLAYER_FARAWAY);
                }
            }
        }
    }

    if (veh->GetUp().z <= -0.7f) { // 0x41FDEA
        ap->SetTempAction(TEMPACT_REVERSE, 1'000);
    }

    if (   notsa::contains({ TEMPACT_NONE, TEMPACT_BOOST_USE_STEERING_ANGLE }, ap->m_nTempAction)
        && notsa::contains(
            {
                MISSION_RAMPLAYER_FARAWAY,
                MISSION_RAMPLAYER_CLOSE,
                MISSION_BLOCKPLAYER_FARAWAY,
                MISSION_BLOCKPLAYER_CLOSE,
                MISSION_APPROACHPLAYER_FARAWAY,
                MISSION_APPROACHPLAYER_CLOSE
            },
            ap->m_nCarMission
          )
        && plyrVeh
    ) { // 0x41FE1B
        const auto  plyrVehFwdDir2D = CVector2D{ plyrVeh->GetForwardVector() }.Normalized();
        const auto  thisVehFwdDir2D = CVector2D{ veh->GetForwardVector() }.Normalized();
        const auto& plyrVelocity3D  = FindPlayerSpeed();
        const auto  plyrSpeed3DSq   = plyrVelocity3D.SquaredMagnitude();
        const auto  plyrCoors       = FindPlayerCoors();

        const auto vehiclesFwdDot = plyrVehFwdDir2D.Dot(thisVehFwdDir2D);
        const auto vehToPlyrDir3D = FindPlayerCoors() - veh->GetPosition();

        if (plyrSpeed3DSq >= sq(0.1f) && veh->GetMoveSpeed().SquaredMagnitude() <= plyrSpeed3DSq) { // 0x41FEE8
            if (vehiclesFwdDot > 0.f) { // 0x41FF25 - Same direction
                if (plyrVelocity3D.Magnitude2D() * 0.5f < vehToPlyrDir3D.Magnitude2D()) { // 0x41FF9F - OG: plyrVelocity3D.Magnitude2D() * vehToPlyrDir.Magnitude2D() * 0.5f < vehToPlyrDir.SquaredMagnitude2D()
                    const auto vehToPlyrDist3DSq = vehToPlyrDir3D.SquaredMagnitude();
                    if (vehToPlyrDist3DSq >= sq(12.f)) { // 0x42002A
                        ap->SetTempAction(TEMPACT_WAIT, 250);
                    }

                    // Calculate steer angle to hit the player
                    if (vehToPlyrDist3DSq <= sq(20.f) && thisVehFwdDir2D.Dot(plyrVehFwdDir2D) > 0.8f && plyrSpeed3DSq >= sq(0.6f)) { // 0x420087 + 0x4200D6
                        ap->SetTempAction(TEMPACT_BOOST_USE_STEERING_ANGLE, 250);

                        const auto targetSteerAngle = CGeneral::LimitRadianAngle(
                            CGeneral::GetATanOf(plyrCoors + plyrVelocity3D * 120.f - veh->GetPosition()) // 0x42019F
                            - CGeneral::GetATanOf(thisVehFwdDir2D) // 0x4201B3
                        ); 
                        const auto maxSteerAngle = CCarCtrl::FindMaxSteerAngle(veh);
                        veh->m_fSteerAngle = std::clamp(targetSteerAngle, -maxSteerAngle, maxSteerAngle); // 0x42025C
                    }

                }
            }
        }

        if (   vehiclesFwdDot < -0.8f // 0x420262
            && plyrSpeed3DSq >= sq(0.3f) // 0x42029B
            && vehToPlyrDir3D.SquaredMagnitude() <= sq(45.f) // 0x4202E6
            && veh->GetMoveSpeed().SquaredMagnitude() >= sq(0.5f) // 0x420344
            ) {
            const auto rdot = vehToPlyrDir3D.Dot(veh->GetRightVector());
            ap->SetTempAction(
                std::abs(rdot) < 15.f
                    ? rdot >= 0.f
                        ? TEMPACT_HANDBRAKETURNRIGHT
                        : TEMPACT_HANDBRAKETURNLEFT
                    : ap->m_nTempAction, // No change
                2'000
            );
        }
    }

    //> 0x4203C1
    if (veh->vehicleFlags.bSirenOrAlarm && ((uint8)veh->m_nRandomSeed ^ (uint8)rand()) == 0xAD) {
        veh->m_nHornCounter = 45;
    }

    //> 0x4203F0 - Handle speed mult change based on time
    ap->m_SpeedMult = [&]{
        const auto targetSpeedMult = ap->m_nCarMission == MISSION_CRUISE
            ? CCarCtrl::FindSpeedMultiplierWithSpeedFromNodes(ap->field_41)
            : 1.f;
        const auto delta = std::abs(ap->m_SpeedMult - targetSpeedMult);
        const auto step  = CTimer::GetTimeStep() * 0.01f;
        if (delta < step) {
            return ap->m_SpeedMult; // No change
        }
        return targetSpeedMult >= ap->m_SpeedMult
            ? ap->m_SpeedMult + step
            : ap->m_SpeedMult - step;
    }();

    //> 0x420445 - Handle player changing water/land vehicles while being chased
    if (veh->vehicleFlags.bIsLawEnforcer && FindPlayerWanted()->GetWantedLevel() > 0) {
        if (plyrVeh) {
            switch (plyrVeh->GetVehicleAppearance()) {
            case VEHICLE_APPEARANCE_AUTOMOBILE:
            case VEHICLE_APPEARANCE_BIKE: {
                if (veh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BOAT) {
                    ap->SetTempAction(TEMPACT_WAIT, 1'000);
                }
                break;
            }
            case VEHICLE_APPEARANCE_BOAT: {
                switch (veh->GetVehicleAppearance()) {
                case VEHICLE_APPEARANCE_AUTOMOBILE:
                case VEHICLE_APPEARANCE_BIKE:
                    ap->SetTempAction(TEMPACT_WAIT, 1'000);
                }
                break;
            }
            }
        } else {
            if (plyr->GetIntelligence()->GetTaskSwim()) { // Inverted!
                switch (veh->GetVehicleAppearance()) {
                case VEHICLE_APPEARANCE_AUTOMOBILE:
                case VEHICLE_APPEARANCE_BIKE:
                    ap->SetTempAction(TEMPACT_WAIT, 1'000);
                }
            } else {
                if (veh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BOAT) {
                    ap->SetTempAction(TEMPACT_WAIT, 1'000);
                }
            }
        }
    }

    //> 0x420559 - 0x42064E - Adjust cruise speed in some cases
    if (CTimer::GetFrameCounter() % 16 == 14 && veh->vehicleFlags.bIsLawEnforcer) {
        if (notsa::contains({ STATUS_SIMPLE, STATUS_PHYSICS }, veh->GetStatus()) && ap->m_nCarMission == MISSION_CRUISE && veh->IsSubAutomobile()) {
            if (CPopCycle::m_bCurrentZoneIsGangArea) {
                ap->SetCruiseSpeed(10); // Start at 10, go down to min 1
                for (auto& p : GetPedPool()->GetAllValid()) {
                    if (!IsPedTypeGang(p.m_nPedType) && p.m_nPedType != PED_TYPE_DEALER) {
                        continue;
                    }
                    if ((veh->GetPosition() - p.GetPosition()).SquaredMagnitude() >= sq(10.f)) {
                        continue;
                    }
                    if (--ap->m_nCruiseSpeed == 1) { // Minimum 1
                        break;
                    }
                }
            }
        }
    }
}
