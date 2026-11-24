#include "StdInc.h"
#include "TaskSimpleCarDrive.h"
#include "TaskUtilityLineUpPedWithCar.h"
#include "Ragdoll/IKChainManager.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimpleGangDriveBy.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskComplexSequence.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexCarDriveWander.h"
#include "Events/EventCarUpsideDown.h"
#include "Events/EventCopCarBeingStolen.h"
#include "Events/EventScriptCommand.h"
#include "CarEnterExit.h"
#include "PedGroups.h"
#include "VehicleAnimGroupData.h"
#include "ModelIndices.h"

void CTaskSimpleCarDrive::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarDrive, 0x86E904, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63C340);
    RH_ScopedInstall(Destructor, 0x63C460);

    RH_ScopedInstall(TriggerIK, 0x63C500);
    RH_ScopedInstall(UpdateBopping, 0x63C900);
    RH_ScopedInstall(StartBopping, 0x642760);
    RH_ScopedInstall(ProcessHeadBopping, 0x6428C0);
    RH_ScopedInstall(ProcessArmBopping, 0x642AE0);
    RH_ScopedInstall(ProcessBopping, 0x642E70);
    RH_ScopedInstall(StartRollingDoor, 0x642700);
    RH_ScopedInstall(FinishRollingDoorCB, 0x63C840);
    RH_ScopedVMTInstall(Clone, 0x63DC20);
    RH_ScopedVMTInstall(GetTaskType, 0x63C450);
    RH_ScopedVMTInstall(MakeAbortable, 0x63C670);
    RH_ScopedVMTInstall(ProcessPed, 0x644470);
    RH_ScopedVMTInstall(SetPedPosition, 0x63C770);
}

// 0x63C340
CTaskSimpleCarDrive::CTaskSimpleCarDrive(CVehicle* vehicle, CTaskUtilityLineUpPedWithCar* utilityTask, bool updateCurrentVehicle) : CTaskSimple() {
    m_pVehicle = vehicle;
    m_pAnimCloseDoorRolling = nullptr;
    m_pTaskUtilityLineUpPedWithCar = nullptr;
    m_nTimePassedSinceCarUpSideDown = 0;

    m_bUpdateCurrentVehicle = updateCurrentVehicle;
    m_b08 = true;

    CEntity::SafeRegisterRef(m_pVehicle);

    if (utilityTask) {
        m_pTaskUtilityLineUpPedWithCar = new CTaskUtilityLineUpPedWithCar(CVector{}, 0, utilityTask->m_nDoorOpenPosType, utilityTask->m_nDoorIdx);
    }

    m_fHeadBoppingFactor = 0.0f;
    m_fHeadBoppingOrientation = 0.0f;
    m_fRandomHeadBoppingMultiplier = 0.0f;
    m_nBoppingStartTime = -1;
}

// 0x63C460
CTaskSimpleCarDrive::~CTaskSimpleCarDrive() {
    CEntity::SafeCleanUpRef(m_pVehicle);

    if (m_pTaskUtilityLineUpPedWithCar) {
        delete m_pTaskUtilityLineUpPedWithCar;
        m_pTaskUtilityLineUpPedWithCar = nullptr;
    }

    if (m_b20 && m_pAnimCloseDoorRolling) {
        // TODO: FIX ME: Keeps triggering, annoying as fuck
        // Seemingly happens when getting of a motorbike (like cops getting off)
        //assert(m_pAnimCloseDoorRolling);
        m_pAnimCloseDoorRolling->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        if (m_pVehicle) {
            m_pVehicle->ClearGettingOutFlags(1);
        }
    }
}

// 0x63C500
void CTaskSimpleCarDrive::TriggerIK(CPed* ped) const {
    if (!m_pVehicle) {
        return;
    }

    // Made an early-out of this, as this same code is used in both possible cases.
    if (g_ikChainMan.IsLooking(ped) || CGeneral::GetRandomNumberInRange(0, 100) >= 5) {
        return;
    }

    switch (m_pVehicle->m_autoPilot.m_nCarMission) {
    case MISSION_RAMPLAYER_FARAWAY:
    case MISSION_RAMPLAYER_CLOSE:
    case MISSION_BLOCKPLAYER_FARAWAY:
    case MISSION_BLOCKPLAYER_CLOSE:
    case MISSION_BLOCKPLAYER_HANDBRAKESTOP: { // Make ped look at player ped
        g_ikChainMan.LookAt("DriveCar", ped, FindPlayerPed(0), 3000, BONE_HEAD, nullptr, false, 0.25f, 500, 3, false);
        break;
    }
    case MISSION_RAMCAR_FARAWAY:
    case MISSION_RAMCAR_CLOSE: {
        if (const auto vehTargetCar = m_pVehicle->m_autoPilot.m_TargetEntity) {
            if (vehTargetCar->GetIsTypeVehicle()) {
                if (const auto driver = vehTargetCar->m_pDriver) { // Make ped look at target car or it's driver (if any)
                    g_ikChainMan.LookAt("DriveCar", ped, driver, 3000, BONE_HEAD, nullptr, false, 0.25f, 500, 3, false);
                } else {
                    g_ikChainMan.LookAt("DriveCar", ped, vehTargetCar, 3000, BONE_UNKNOWN, nullptr, false, 0.25f, 500, 3, false);
                }
            }
        }
        break;
    }
    }
}

// 0x63C900
void CTaskSimpleCarDrive::UpdateBopping() {
    const auto timeDelta = (int32)CTimer::GetTimeInMS() - m_nBoppingStartTime;
    m_fBoppingProgress = (float)(timeDelta % m_nBoppingEndTime) / (float)m_nBoppingEndTime;
    m_nBoppingCompletedTimes = timeDelta / m_nBoppingEndTime % 2;
}

// 0x642760
void CTaskSimpleCarDrive::StartBopping(CPed* ped) {
    // Bop to the same beat as the others in the car (NOTE: Only the first 3 passengers are checked)
    const auto veh = ped->m_pVehicle;
    for (const auto occupant : { veh->m_pDriver, veh->m_apPassengers[0], veh->m_apPassengers[1], veh->m_apPassengers[2] }) {
        if (!occupant || occupant == ped) {
            continue;
        }
        const auto tDrive = occupant->GetTaskManager().Find<CTaskSimpleCarDrive>();
        if (!tDrive || tDrive->m_nBoppingStartTime == -1) {
            continue;
        }
        m_nBoppingStartTime = tDrive->m_nBoppingStartTime;
        m_nBoppingEndTime   = tDrive->m_nBoppingEndTime;
        UpdateBopping();
        return;
    }

    const auto bpm = 60 + CGeneral::GetRandomNumberInRange(0, 60);

    m_nBoppingStartTime = (int32)CTimer::GetTimeInMS();
    m_nBoppingEndTime   = (int32)(1.f / ((float)bpm * (1.f / 60.f)) * 1000.f);
    m_fBoppingProgress  = 0.f;
}

// 0x6428C0
void CTaskSimpleCarDrive::ProcessHeadBopping(CPed* ped, bool canBop, float camDistSq) {
    float bopOrientation = 0.f;
    if (canBop) {
        if (!m_b01) {
            if (m_fHeadBoppingFactor != 0.f || CGeneral::GetRandomNumberInRange(0, 1000) <= 995) {
                canBop = false;
            } else {
                if (m_nBoppingStartTime == -1) {
                    StartBopping(ped);
                    m_nHeadBoppingStartTime = m_nBoppingStartTime;
                } else {
                    m_nHeadBoppingStartTime = (int32)CTimer::GetTimeInMS();
                }
                m_nHeadBoppingDirection        = CGeneral::GetRandomNumberInRange(1, 3);
                m_fRandomHeadBoppingMultiplier = CGeneral::GetRandomNumberInRange(3.f, 8.f);
                m_fHeadBoppingFactor           = 0.f;
                m_fHeadBoppingOrientation      = 0.f;
            }
        } else {
            bopOrientation = std::sin(m_fBoppingProgress * PI);
            if (CTimer::GetTimeInMS() - (uint32)m_nHeadBoppingStartTime > 5000 && CGeneral::GetRandomNumberInRange(0, 1000) > 995) {
                canBop = false;
            }
        }
    }
    m_b01 = canBop;

    if (m_b01) { // Blend in
        if (m_fHeadBoppingFactor < 1.f) {
            m_fHeadBoppingFactor += 0.05f;
        }
        if (m_fHeadBoppingFactor > 1.f) {
            m_fHeadBoppingFactor = 1.f;
        }
        m_fHeadBoppingOrientation += (bopOrientation - m_fHeadBoppingOrientation) * m_fHeadBoppingFactor;
    } else { // Blend out
        if (m_fHeadBoppingFactor > 0.f) {
            m_fHeadBoppingFactor -= 0.05f;
        }
        if (m_fHeadBoppingFactor < 0.f) {
            m_fHeadBoppingFactor = 0.f;
        }
        m_fHeadBoppingOrientation += (bopOrientation - m_fHeadBoppingOrientation) * (1.f - m_fHeadBoppingFactor);
    }

    if (camDistSq >= sq(8.f) || m_fHeadBoppingOrientation <= 0.f) {
        return;
    }
    const auto angle = m_fRandomHeadBoppingMultiplier * m_fHeadBoppingOrientation;
    const auto headQ = &ped->m_apBones[PED_NODE_HEAD]->KeyFrame->q;
    if (m_nHeadBoppingDirection >= 1) {
        RtQuatRotate(headQ, &CPedIK::XaxisIK, m_nBoppingCompletedTimes ? -angle : angle, rwCOMBINEPRECONCAT);
    }
    RtQuatRotate(headQ, &CPedIK::ZaxisIK, m_nHeadBoppingDirection == 2 ? angle : -angle, rwCOMBINEPRECONCAT);
    ped->bUpdateMatricesRequired = true;
}

// 0x642AE0
void CTaskSimpleCarDrive::ProcessArmBopping(CPed* ped, bool canBop, float camDistSq) {
    if (ped->IsPlayer()) {
        return;
    }

    const auto veh = ped->m_pVehicle;
    if (CWeather::Rain > 0.f) { // Close the windows
        for (const auto door : { CAR_DOOR_LF, CAR_DOOR_RF, CAR_DOOR_LR, CAR_DOOR_RR }) {
            veh->ClearWindowOpenFlag(door);
        }
        canBop = false;
    } else if (veh->m_pDriver == ped) {
        veh->SetWindowOpenFlag(CAR_DOOR_LF);
    } else if (veh->m_apPassengers[0] == ped) {
        veh->SetWindowOpenFlag(CAR_DOOR_RF);
    } else if (veh->m_apPassengers[1] == ped) {
        veh->SetWindowOpenFlag(CAR_DOOR_LR);
    } else if (veh->m_apPassengers[2] == ped) {
        veh->SetWindowOpenFlag(CAR_DOOR_RR);
    }

    const auto tapAnimId = veh->m_apPassengers[0] == ped || veh->m_apPassengers[2] == ped
        ? ANIM_ID_TAP_HANDP
        : ANIM_ID_TAP_HAND;
    const auto tapAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), tapAnimId);

    if (!canBop) {
        if (tapAnim) {
            tapAnim->m_BlendDelta = -4.f;
        }
        m_b02 = false;
        return;
    }

    if (m_b02) {
        if (!tapAnim) {
            m_b02 = false;
            return;
        }
        if (camDistSq < sq(8.f)) {
            const auto angle = m_fBoppingProgress >= 0.75f
                ? (m_fBoppingProgress - 0.75f) * 45.f
                : 0.f;
            const auto hand = ped->m_apBones[tapAnimId == ANIM_ID_TAP_HAND ? PED_NODE_LEFT_HAND : PED_NODE_RIGHT_HAND];
            RtQuatRotate(&hand->KeyFrame->q, &CPedIK::ZaxisIK, -angle, rwCOMBINEPRECONCAT);
            ped->bUpdateMatricesRequired = true;
        }
        if (CTimer::GetTimeInMS() - (uint32)m_nArmBoppingStartTime > 5000 && CGeneral::GetRandomNumberInRange(0, 1000) > 995) {
            tapAnim->m_BlendDelta = -4.f;
            m_b02 = false;
        }
        return;
    }

    if (CGeneral::GetRandomNumberInRange(0, 1000) <= 995) {
        return;
    }

    if (m_pVehicle->m_nVehicleSubType == VEHICLE_TYPE_AUTOMOBILE) { // The hand must be able to rest on the window
        const auto door          = CCarEnterExit::ComputeTargetDoorToExit(m_pVehicle, ped);
        const auto canLeanOnDoor = m_pVehicle->AsAutomobile()->GetDamageManager().GetDoorStatus_Component((tComponent)door) == DAMSTATE_OK
                                && !CCarEnterExit::CarHasDoorToClose(m_pVehicle, door)
                                && m_pVehicle->CanPedLeanOut(ped);

        const auto mi       = m_pVehicle->GetModelInfo()->AsVehicleModelInfoPtr();
        const auto handRest = mi->GetModelDummyPosition(DUMMY_HAND_REST);
        if (handRest.IsZero() || m_pVehicle->vehicleFlags.bLowVehicle) {
            return;
        }
        const auto handRestHeight         = handRest.z - mi->GetModelDummyPosition(DUMMY_SEAT_FRONT).z;
        const auto [minHeight, maxHeight] = m_pVehicle->GetAnimGroup().m_specialFlags.bUseTruckDriveAnims
            ? std::make_pair(0.4f, 0.44f)
            : std::make_pair(0.39f, 0.46f);
        if (handRestHeight < minHeight || handRestHeight > maxHeight || !canLeanOnDoor) {
            return;
        }
    }

    CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, tapAnimId, 4.f);
    if (m_nBoppingStartTime == -1) {
        StartBopping(ped);
        m_nArmBoppingStartTime = m_nBoppingStartTime;
    } else {
        m_nArmBoppingStartTime = (int32)CTimer::GetTimeInMS();
    }
    m_b02 = true;
}

// 0x642E70
void CTaskSimpleCarDrive::ProcessBopping(CPed* ped, bool a3) {
    if (ped->m_pVehicle->m_pDriver == FindPlayerPed(0)
        || ped->m_nPedType == PED_TYPE_COP
        || ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP)
    ) {
        return;
    }

    auto* vehicle = ped->m_pVehicle;
    if (vehicle->IsAutomobile() && !vehicle->IsSubQuad() && !ped->IsCreatedByMission()) {
        if (m_nBoppingStartTime != -1) { // IsBopping
            UpdateBopping();
        }

        const auto dist = DistanceBetweenPointsSquared(TheCamera.GetPosition(), ped->GetPosition());
        ProcessHeadBopping(ped, a3, dist);
        ProcessArmBopping(ped, a3, dist);
        if (m_nBoppingStartTime != -1 && !m_b01 && !m_b02) {
            m_nBoppingStartTime = -1;
        }
    }
}

// 0x63DC20
CTask* CTaskSimpleCarDrive::Clone() const {
    auto task = new CTaskSimpleCarDrive(m_pVehicle);
    task->m_bUpdateCurrentVehicle = m_bUpdateCurrentVehicle;
    return task;
}

// 0x63C670
bool CTaskSimpleCarDrive::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        m_b08 = true;
        return !m_b20;
    }
    if (ped->IsInVehicle()) {
        CTaskSimpleCarSetPedOut tSetPedOut{ m_pVehicle, (eTargetDoor)CCarEnterExit::ComputeTargetDoorToExit(ped->m_pVehicle, ped), false };
        if (ped->m_pVehicle->IsBike() && event) {
            tSetPedOut.m_bFallingOutOfCar = true;
        }
        tSetPedOut.ProcessPed(ped);
    }
    g_ikChainMan.AbortLookAtIfLooking(ped);
    return true;
}

// 0x644470
bool CTaskSimpleCarDrive::ProcessPed(CPed* ped) {
    const auto radioRetuneJustStarted = AudioEngine.HasRadioRetuneJustStarted();

    if (m_bUpdateCurrentVehicle && ped->IsInVehicle() && ped->m_pVehicle != m_pVehicle) {
        CEntity::ChangeEntityReference(m_pVehicle, ped->m_pVehicle);
    }

    if (!m_pVehicle || !ped->bInVehicle) {
        ped->bInVehicle = false;
        g_ikChainMan.AbortLookAtIfLooking(ped);
        return true;
    }

    const auto pedGrp = CPedGroups::GetPedsGroup(ped);
    if (CTheScripts::UpsideDownCars.IsCarUpsideDown(m_pVehicle)) { // 0x64451c
        if (ped != FindPlayerPed() && FindPlayerPed()->IsInVehicle(m_pVehicle)) {
            ped->Say(CTX_GLOBAL_CAR_FLIPPED);
        }
        if (!pedGrp || pedGrp->GetMembership().IsLeader(ped)) {
            m_nTimePassedSinceCarUpSideDown += (int32)CTimer::GetTimeStepInMS();
            if ((uint32)m_nTimePassedSinceCarUpSideDown > 2000) {
                ped->GetEventGroup().Add(CEventCarUpsideDown{ m_pVehicle });
            }
        }
    } else {
        m_nTimePassedSinceCarUpSideDown = 0;
    }

    if (ped->IsPlayer() && m_pVehicle->IsLawEnforcementVehicle() && (!m_copCarStolenTimer.IsStarted() || m_copCarStolenTimer.IsOutOfTime())) { // 0x6445ee
        m_copCarStolenTimer.Start(2000);
        GetEventGlobalGroup()->Add(CEventCopCarBeingStolen{ ped, m_pVehicle });
    }

    if (m_pVehicle->IsBike() || m_pVehicle->IsSubQuad() || m_pVehicle->m_nModelIndex == MODEL_TRACTOR) {
        ped->bTestForShotInVehicle = true;
    }

    const auto CanDoDriveBy = [&] {
        const auto& smg = ped->GetWeaponInSlot(eWeaponSlot::SMG);
        return smg.GetType() != WEAPON_UNARMED
            && (int32)smg.GetTotalAmmo() > 0
            && m_pVehicle->CanPedLeanOut(ped);
    };
    const auto StartDriveBy = [&](bool seatRHS) {
        ped->GetEventGroup().Add(CEventScriptCommand{
            TASK_PRIMARY_PRIMARY,
            new CTaskSimpleGangDriveBy{ nullptr, nullptr, 100.f, 100, eDrivebyStyle::AI_ALL_DIRN, seatRHS }
        });
    };

    if (ped->IsPlayer() && m_pVehicle->m_pDriver == ped) { // 0x6446ad
        const auto pad = ped->AsPlayer()->GetPadFromPlayer();
        if (CCheat::IsActive(CHEAT_WEAPON_AIMING_WHILE_DRIVING) && CanDoDriveBy()) {
            StartDriveBy(false);
        }

        const auto IsPadIdle = [pad] {
            return !pad || (pad->GetAccelerate() == 0 && pad->GetSteeringLeftRight() == 0 && pad->GetBrake() == 0);
        };
        const auto pedVeh = ped->m_pVehicle;
        if (pedVeh->m_nModelIndex != MODEL_STREAKC && pedVeh->IsTrain() && !pedVeh->AsTrain()->m_aDoors[DOOR_LEFT_FRONT].IsClosed()) { // 0x644789
            if (IsPadIdle()) {
                pedVeh->ProcessOpenDoor(ped, CAR_DOOR_LF, m_pVehicle->GetAnimGroup().GetGroup(ANIM_ID_CAR_ROLLDOOR), ANIM_ID_CAR_ROLLDOOR, 1.f);
            }
        } else if (pedVeh->IsAutomobile() && pedVeh->AsAutomobile()->GetDamageManager().GetDoorStatus(DOOR_LEFT_FRONT) == DAMSTATE_OPENED) { // 0x644846
            if (!pedVeh->IsDoorMissing(DOOR_LEFT_FRONT)) {
                if (const auto rollAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_CAR_ROLLDOOR)) { // 0x64493e
                    pedVeh->ProcessOpenDoor(ped, CAR_DOOR_LF, m_pVehicle->GetAnimGroup().GetGroup(rollAnim->GetAnimId()), ANIM_ID_CAR_ROLLDOOR, rollAnim->GetCurrentTime());
                } else if (!(pedVeh->m_nGettingOutFlags & 1) && !(pedVeh->m_nGettingInFlags & 1) && IsPadIdle()) { // 0x64491a
                    pedVeh->SetGettingOutFlags(1);
                    m_b20 = true;
                    StartRollingDoor(ped);
                    return false;
                }
                ProcessBopping(ped, false);
            }
        } else { // 0x64498b
            m_pVehicle->ProcessDrivingAnims(ped, radioRetuneJustStarted);
            ProcessBopping(ped, true);
        }
    } else if (m_pVehicle->m_pDriver == ped) { // 0x64498e
        m_pVehicle->ProcessDrivingAnims(ped, radioRetuneJustStarted);
        ProcessBopping(ped, true);
    } else if (m_pVehicle->IsPassenger(ped)) { // 0x6449c4
        if (!m_b08 && !m_b10) {
            if (const auto rideData = m_pVehicle->GetRideAnimData()) {
                CAnimManager::BlendAnimation(ped->GetRpClump(), rideData->AnimGroup, ANIM_ID_BIKE_PASSENGER);
            }
            m_b10 = true;
        }
        ProcessBopping(ped, true);
        if (ped->IsPlayer() && CGameLogic::IsCoopGameGoingOn() && CanDoDriveBy() && TheCamera.GetActiveCam().m_nMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) { // 0x644a0b
            StartDriveBy(m_pVehicle->m_apPassengers[1] != ped);
        }
    }

    if (const auto held = ped->GetEntityThatThisPedIsHolding(); held && held->m_nModelIndex == ModelIndices::MI_GANG_SMOKE && !g_ikChainMan.IsLooking(ped) && CGeneral::GetRandomNumberInRange(0, 60) == 15) { // 0x644af7
        if (const auto tHold = ped->GetTaskManager().Find<CTaskSimpleHoldEntity>()) {
            tHold->PlayAnim(m_pVehicle->IsPassenger(ped) ? ANIM_ID_PASS_SMOKE_IN_CAR : ANIM_ID_SMOKE_IN_CAR, ANIM_GROUP_DEFAULT);
        }
    }

    if (RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SMOKE_IN_CAR)) {
        g_ikChainMan.AbortLookAtIfLooking(ped);
        m_nBoppingStartTime = -1;
    } else {
        TriggerIK(ped);
    }

    if (ped->IsCreatedBy(PED_GAME) && !CPedGroups::GetPedsGroup(ped)) { // 0x644bb9 - Take over the car if there's no driver
        if (!m_pVehicle->m_pDriver && m_pVehicle->m_apPassengers[0] == ped) {
            m_NoDriverTimer.StartIfNotAlready(4000);
            if (m_NoDriverTimer.IsOutOfTime()) {
                ped->GetEventGroup().Add(CEventScriptCommand{
                    TASK_PRIMARY_PRIMARY,
                    new CTaskComplexSequence{
                        new CTaskComplexLeaveCar{ m_pVehicle, 0, 0, true, false },
                        new CTaskComplexCarDriveWander{ m_pVehicle }
                    }
                });
            }
        } else {
            m_NoDriverTimer.Stop();
        }
    }

    if (((ped->m_nRandomSeed + CTimer::GetFrameCounter()) & 0x3FFF) == 0) { // 0x644d00
        if (const auto driver = m_pVehicle->m_pDriver; driver && driver != ped && driver->IsPlayer()) {
            if (m_pVehicle->GetMoveSpeed().Magnitude2D() > 0.7f) {
                ped->Say(CTX_GLOBAL_CAR_FAST);
            }
            if (m_pVehicle->GetMoveSpeed().Magnitude2D() < 0.1f) {
                ped->Say(CTX_GLOBAL_CAR_SLOW);
            }
            ped->Say(CTX_GLOBAL_CAR_SINGALONG);
        }
    }

    return false;
}

// 0x642700
void CTaskSimpleCarDrive::StartRollingDoor(CPed* ped) {
    m_pAnimCloseDoorRolling = CAnimManager::AddAnimation(ped->GetRpClump(), ped->m_pVehicle->GetAnimGroup().GetGroup(ANIM_ID_CAR_ROLLDOOR), ANIM_ID_CAR_ROLLDOOR);
    m_pAnimCloseDoorRolling->SetFinishCallback(FinishRollingDoorCB, this);
}

// 0x63C840
void CTaskSimpleCarDrive::FinishRollingDoorCB(CAnimBlendAssociation* anim, void* data) {
    // NOTE: Not `notsa::cast`, subclasses (`CTaskSimpleCarDriveTimed`, ...) report their own type
    const auto self = static_cast<CTaskSimpleCarDrive*>(data);

    self->m_b20                   = false;
    self->m_pAnimCloseDoorRolling = nullptr;
    if (const auto veh = self->m_pVehicle) {
        veh->ClearGettingOutFlags(1);
        if (const auto driver = veh->m_pDriver) {
            veh->ProcessOpenDoor(driver, CAR_DOOR_LF, veh->GetAnimGroup().GetGroup(ANIM_ID_CAR_ROLLDOOR), ANIM_ID_CAR_ROLLDOOR, 1.f);
        }
    }
}

// 0x63C770
bool CTaskSimpleCarDrive::SetPedPosition(CPed* ped) {
    if (m_b08) {
        const auto tDrive = notsa::cast_if_present<CTaskSimpleCarDrive>(ped->GetTaskManager().FindTaskByType(TASK_PRIMARY_DEFAULT, TASK_SIMPLE_CAR_DRIVE));
        if (tDrive && tDrive != this && tDrive->m_pTaskUtilityLineUpPedWithCar) {
            m_pTaskUtilityLineUpPedWithCar = std::exchange(tDrive->m_pTaskUtilityLineUpPedWithCar, nullptr);
        }
        m_b08 = false;
    }

    if (m_pTaskUtilityLineUpPedWithCar) {
        constexpr AnimationId lineUpAnims[]{ // 0x8D2E9C
            ANIM_ID_CAR_GETIN_LHS_0,
            ANIM_ID_CAR_GETIN_RHS_0,
            ANIM_ID_CAR_GETIN_LHS_1,
            ANIM_ID_CAR_GETIN_RHS_1,
            ANIM_ID_CAR_GETIN_BIKE_FRONT,
            ANIM_ID_CAR_CLOSEDOOR_LHS_0,
            ANIM_ID_CAR_CLOSEDOOR_RHS_0,
            ANIM_ID_CAR_CLOSEDOOR_LHS_1,
            ANIM_ID_CAR_CLOSEDOOR_RHS_1,
            ANIM_ID_CAR_SHUFFLE_RHS_0,
            ANIM_ID_CAR_SHUFFLE_RHS_1,
            ANIM_ID_CAR_ROLLDOOR,
        };
        CAnimBlendAssociation* anim{};
        for (const auto animId : lineUpAnims) {
            if ((anim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), animId))) {
                break;
            }
        }
        m_pTaskUtilityLineUpPedWithCar->ProcessPed(ped, m_pVehicle, anim);
        delete std::exchange(m_pTaskUtilityLineUpPedWithCar, nullptr);
        return true;
    }

    if (m_pVehicle && ped->IsInVehicle(m_pVehicle)) {
        ped->SetPedPositionInCar();
    }
    return true;
}

