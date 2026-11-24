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
#include "TaskComplexCarSlowBeDraggedOutAndStandUp.h"
#include "Events/EventCarUpsideDown.h"
#include "Events/EventCopCarBeingStolen.h"
#include "Events/EventScriptCommand.h"
#include "CarEnterExit.h"
#include "PedGroups.h"
#include "VehicleAnimGroupData.h"
#include "ModelIndices.h"

// 0x63C340
CTaskSimpleCarDrive::CTaskSimpleCarDrive(CVehicle* vehicle, CTaskUtilityLineUpPedWithCar* utilityTask, bool updateCurrentVehicle) :
    m_Vehicle{
        vehicle
},
    m_TaskUtilityLineUpPedWithCar{ utilityTask ? new CTaskUtilityLineUpPedWithCar{ CVector{}, 0, utilityTask->m_nDoorOpenPosType, utilityTask->m_nDoorIdx } : nullptr },
    m_UpdateCurrentVehicle{ updateCurrentVehicle } {
    CEntity::SafeRegisterRef(m_Vehicle);
}

// NOTSA
CTaskSimpleCarDrive::CTaskSimpleCarDrive(const CTaskSimpleCarDrive& o) :
    CTaskSimpleCarDrive{ o.m_Vehicle, nullptr, o.m_UpdateCurrentVehicle } {
}

// 0x63C460
CTaskSimpleCarDrive::~CTaskSimpleCarDrive() {
    CEntity::SafeCleanUpRef(m_Vehicle);

    if (m_TaskUtilityLineUpPedWithCar) {
        delete m_TaskUtilityLineUpPedWithCar;
        m_TaskUtilityLineUpPedWithCar = nullptr;
    }

    if (m_IsRollingDoor && m_AnimCloseDoorRolling) {
        // TODO: FIX ME: Keeps triggering, annoying as fuck
        // Seemingly happens when getting of a motorbike (like cops getting off)
        //assert(m_AnimCloseDoorRolling);
        m_AnimCloseDoorRolling->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        if (m_Vehicle) {
            m_Vehicle->ClearGettingOutFlags(1);
        }
    }
}

// 0x63C500
void CTaskSimpleCarDrive::TriggerIK(CPed* ped) const {
    if (!m_Vehicle) {
        return;
    }

    // Made an early-out of this, as this same code is used in both possible cases.
    if (g_ikChainMan.IsLooking(ped) || CGeneral::GetRandomNumberInRange(0, 100) >= 5) {
        return;
    }

    switch (m_Vehicle->m_autoPilot.m_nCarMission) {
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
        if (const auto vehTargetCar = m_Vehicle->m_autoPilot.m_TargetEntity) {
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
    const auto timeDelta    = (int32)CTimer::GetTimeInMS() - m_BoppingStartTime;
    m_BoppingProgress       = (float)(timeDelta % m_BoppingInterval) / (float)m_BoppingInterval;
    m_BoppingCompletedTimes = timeDelta / m_BoppingInterval % 2;
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
        if (!tDrive || tDrive->m_BoppingStartTime == -1) {
            continue;
        }
        m_BoppingStartTime = tDrive->m_BoppingStartTime;
        m_BoppingInterval  = tDrive->m_BoppingInterval;
        UpdateBopping();
        return;
    }

    const auto bpm     = 60 + CGeneral::GetRandomNumberInRange(0, 60);

    m_BoppingStartTime = (int32)CTimer::GetTimeInMS();
    m_BoppingInterval  = (int32)(1.f / ((float)bpm * (1.f / 60.f)) * 1000.f);
    m_BoppingProgress  = 0.f;
}

// 0x6428C0
void CTaskSimpleCarDrive::ProcessHeadBopping(CPed* ped, bool canBop, float camDistSq) {
    float bopOrientation = 0.f;
    if (canBop) {
        if (!m_IsHeadBopping) {
            if (m_HeadBoppingFactor != 0.f || CGeneral::GetRandomNumberInRange(0, 1'000) <= 995) {
                canBop = false;
            } else {
                if (m_BoppingStartTime == -1) {
                    StartBopping(ped);
                    m_HeadBoppingStartTime = m_BoppingStartTime;
                } else {
                    m_HeadBoppingStartTime = (int32)CTimer::GetTimeInMS();
                }
                m_HeadBoppingDirection   = CGeneral::GetRandomNumberInRange(1, 3);
                m_HeadBoppingMultiplier  = CGeneral::GetRandomNumberInRange(3.f, 8.f);
                m_HeadBoppingFactor      = 0.f;
                m_HeadBoppingOrientation = 0.f;
            }
        } else {
            bopOrientation = std::sin(m_BoppingProgress * PI);
            if (CTimer::GetTimeInMS() - (uint32)m_HeadBoppingStartTime > 5'000 && CGeneral::GetRandomNumberInRange(0, 1'000) > 995) {
                canBop = false;
            }
        }
    }
    m_IsHeadBopping = canBop;

    if (m_IsHeadBopping) { // Blend in
        if (m_HeadBoppingFactor < 1.f) {
            m_HeadBoppingFactor += 0.05f;
        }
        if (m_HeadBoppingFactor > 1.f) {
            m_HeadBoppingFactor = 1.f;
        }
        m_HeadBoppingOrientation += (bopOrientation - m_HeadBoppingOrientation) * m_HeadBoppingFactor;
    } else { // Blend out
        if (m_HeadBoppingFactor > 0.f) {
            m_HeadBoppingFactor -= 0.05f;
        }
        if (m_HeadBoppingFactor < 0.f) {
            m_HeadBoppingFactor = 0.f;
        }
        m_HeadBoppingOrientation += (bopOrientation - m_HeadBoppingOrientation) * (1.f - m_HeadBoppingFactor);
    }

    if (camDistSq >= sq(8.f) || m_HeadBoppingOrientation <= 0.f) {
        return;
    }
    const auto angle = m_HeadBoppingMultiplier * m_HeadBoppingOrientation;
    const auto headQ = &ped->m_apBones[PED_NODE_HEAD]->KeyFrame->q;
    if (m_HeadBoppingDirection >= 1) {
        RtQuatRotate(headQ, &CPedIK::XaxisIK, m_BoppingCompletedTimes ? -angle : angle, rwCOMBINEPRECONCAT);
    }
    RtQuatRotate(headQ, &CPedIK::ZaxisIK, m_HeadBoppingDirection == 2 ? angle : -angle, rwCOMBINEPRECONCAT);
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
    const auto tapAnim   = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), tapAnimId);

    if (!canBop) {
        if (tapAnim) {
            tapAnim->m_BlendDelta = -4.f;
        }
        m_IsArmBopping = false;
        return;
    }

    if (m_IsArmBopping) {
        if (!tapAnim) {
            m_IsArmBopping = false;
            return;
        }
        if (camDistSq < sq(8.f)) {
            const auto angle = m_BoppingProgress >= 0.75f
                ? (m_BoppingProgress - 0.75f) * 45.f
                : 0.f;
            const auto hand  = ped->m_apBones[tapAnimId == ANIM_ID_TAP_HAND ? PED_NODE_LEFT_HAND : PED_NODE_RIGHT_HAND];
            RtQuatRotate(&hand->KeyFrame->q, &CPedIK::ZaxisIK, -angle, rwCOMBINEPRECONCAT);
            ped->bUpdateMatricesRequired = true;
        }
        if (CTimer::GetTimeInMS() - (uint32)m_ArmBoppingStartTime > 5'000 && CGeneral::GetRandomNumberInRange(0, 1'000) > 995) {
            tapAnim->m_BlendDelta = -4.f;
            m_IsArmBopping        = false;
        }
        return;
    }

    if (CGeneral::GetRandomNumberInRange(0, 1'000) <= 995) {
        return;
    }

    if (m_Vehicle->m_nVehicleSubType == VEHICLE_TYPE_AUTOMOBILE) { // The hand must be able to rest on the window
        const auto door          = CCarEnterExit::ComputeTargetDoorToExit(m_Vehicle, ped);
        const auto canLeanOnDoor = m_Vehicle->AsAutomobile()->GetDamageManager().GetDoorStatus_Component((tComponent)door) == DAMSTATE_OK
            && !CCarEnterExit::CarHasDoorToClose(m_Vehicle, door)
            && m_Vehicle->CanPedLeanOut(ped);

        const auto mi       = m_Vehicle->GetModelInfo()->AsVehicleModelInfoPtr();
        const auto handRest = mi->GetModelDummyPosition(DUMMY_HAND_REST);
        if (handRest.IsZero() || m_Vehicle->vehicleFlags.bLowVehicle) {
            return;
        }
        const auto handRestHeight         = handRest.z - mi->GetModelDummyPosition(DUMMY_SEAT_FRONT).z;
        const auto [minHeight, maxHeight] = m_Vehicle->GetAnimGroup().m_specialFlags.bUseTruckDriveAnims
            ? std::make_pair(0.4f, 0.44f)
            : std::make_pair(0.39f, 0.46f);
        if (handRestHeight < minHeight || handRestHeight > maxHeight || !canLeanOnDoor) {
            return;
        }
    }

    CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, tapAnimId, 4.f);
    if (m_BoppingStartTime == -1) {
        StartBopping(ped);
        m_ArmBoppingStartTime = m_BoppingStartTime;
    } else {
        m_ArmBoppingStartTime = (int32)CTimer::GetTimeInMS();
    }
    m_IsArmBopping = true;
}

// 0x642E70
void CTaskSimpleCarDrive::ProcessBopping(CPed* ped, bool canBop) {
    if (ped->m_pVehicle->m_pDriver == FindPlayerPed(0)
        || ped->m_nPedType == PED_TYPE_COP
        || ped->GetTaskManager().Find<CTaskComplexCarSlowBeDraggedOutAndStandUp>()) {
        return;
    }

    auto* vehicle = ped->m_pVehicle;
    if (vehicle->IsAutomobile() && !vehicle->IsSubQuad() && !ped->IsCreatedByMission()) {
        if (m_BoppingStartTime != -1) { // IsBopping
            UpdateBopping();
        }

        const auto dist = DistanceBetweenPointsSquared(TheCamera.GetPosition(), ped->GetPosition());
        ProcessHeadBopping(ped, canBop, dist);
        ProcessArmBopping(ped, canBop, dist);
        if (m_BoppingStartTime != -1 && !m_IsHeadBopping && !m_IsArmBopping) {
            m_BoppingStartTime = -1;
        }
    }
}

// 0x63DC20
CTask* CTaskSimpleCarDrive::Clone() const {
    return new CTaskSimpleCarDrive{ *this };
}

// 0x63C670
bool CTaskSimpleCarDrive::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        m_b08 = true;
        return !m_IsRollingDoor;
    }
    if (ped->IsInVehicle()) {
        CTaskSimpleCarSetPedOut tSetPedOut{ m_Vehicle, (eTargetDoor)CCarEnterExit::ComputeTargetDoorToExit(ped->m_pVehicle, ped), false };
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

    if (m_UpdateCurrentVehicle && ped->IsInVehicle() && ped->m_pVehicle != m_Vehicle) {
        CEntity::ChangeEntityReference(m_Vehicle, ped->m_pVehicle);
    }

    if (!m_Vehicle || !ped->bInVehicle) {
        ped->bInVehicle = false;
        g_ikChainMan.AbortLookAtIfLooking(ped);
        return true;
    }

    const auto pedGrp = CPedGroups::GetPedsGroup(ped);
    if (CTheScripts::UpsideDownCars.IsCarUpsideDown(m_Vehicle)) { // 0x64451c
        if (ped != FindPlayerPed() && FindPlayerPed()->IsInVehicle(m_Vehicle)) {
            ped->Say(CTX_GLOBAL_CAR_FLIPPED);
        }
        if (!pedGrp || pedGrp->GetMembership().IsLeader(ped)) {
            m_UpsideDownTime += (int32)CTimer::GetTimeStepInMS();
            if ((uint32)m_UpsideDownTime > 2'000) {
                ped->GetEventGroup().Add(CEventCarUpsideDown{ m_Vehicle });
            }
        }
    } else {
        m_UpsideDownTime = 0;
    }

    if (ped->IsPlayer() && m_Vehicle->IsLawEnforcementVehicle() && (!m_CopCarStolenTimer.IsStarted() || m_CopCarStolenTimer.IsOutOfTime())) { // 0x6445ee
        m_CopCarStolenTimer.Start(2'000);
        GetEventGlobalGroup()->Add(CEventCopCarBeingStolen{ ped, m_Vehicle });
    }

    if (m_Vehicle->IsBike() || m_Vehicle->IsSubQuad() || m_Vehicle->GetModelId() == MODEL_TRACTOR) {
        ped->bTestForShotInVehicle = true;
    }

    const auto CanDoDriveBy = [&] {
        const auto& smg = ped->GetWeaponInSlot(eWeaponSlot::SMG);
        return smg.GetType() != WEAPON_UNARMED
            && (int32)smg.GetTotalAmmo() > 0
            && m_Vehicle->CanPedLeanOut(ped);
    };
    const auto StartDriveBy = [&](bool seatRHS) {
        ped->GetEventGroup().Add(CEventScriptCommand{
            TASK_PRIMARY_PRIMARY, new CTaskSimpleGangDriveBy{ nullptr, nullptr, 100.f, 100, eDrivebyStyle::AI_ALL_DIRN, seatRHS }
        });
    };

    if (ped->IsPlayer() && m_Vehicle->m_pDriver == ped) { // 0x6446ad
        const auto pad = ped->AsPlayer()->GetPadFromPlayer();
        if (CCheat::IsActive(CHEAT_WEAPON_AIMING_WHILE_DRIVING) && CanDoDriveBy()) {
            StartDriveBy(false);
        }

        const auto IsPadIdle = [pad] {
            return !pad || (pad->GetAccelerate() == 0 && pad->GetSteeringLeftRight() == 0 && pad->GetBrake() == 0);
        };
        const auto pedVeh = ped->m_pVehicle;
        if (pedVeh->GetModelId() != MODEL_STREAKC && pedVeh->IsTrain() && !pedVeh->AsTrain()->m_aDoors[DOOR_LEFT_FRONT].IsClosed()) { // 0x644789
            if (IsPadIdle()) {
                pedVeh->ProcessOpenDoor(ped, CAR_DOOR_LF, m_Vehicle->GetAnimGroup().GetGroup(ANIM_ID_CAR_ROLLDOOR), ANIM_ID_CAR_ROLLDOOR, 1.f);
            }
        } else if (pedVeh->IsAutomobile() && pedVeh->AsAutomobile()->GetDamageManager().GetDoorStatus(DOOR_LEFT_FRONT) == DAMSTATE_OPENED) { // 0x644846
            if (!pedVeh->IsDoorMissing(DOOR_LEFT_FRONT)) {
                if (const auto rollAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_CAR_ROLLDOOR)) { // 0x64493e
                    pedVeh->ProcessOpenDoor(ped, CAR_DOOR_LF, m_Vehicle->GetAnimGroup().GetGroup(rollAnim->GetAnimId()), ANIM_ID_CAR_ROLLDOOR, rollAnim->GetCurrentTime());
                } else if (!(pedVeh->m_nGettingOutFlags & 1) && !(pedVeh->m_nGettingInFlags & 1) && IsPadIdle()) { // 0x64491a
                    pedVeh->SetGettingOutFlags(1);
                    m_IsRollingDoor = true;
                    StartRollingDoor(ped);
                    return false;
                }
                ProcessBopping(ped, false);
            }
        } else { // 0x64498b
            m_Vehicle->ProcessDrivingAnims(ped, radioRetuneJustStarted);
            ProcessBopping(ped, true);
        }
    } else if (m_Vehicle->m_pDriver == ped) { // 0x64498e
        m_Vehicle->ProcessDrivingAnims(ped, radioRetuneJustStarted);
        ProcessBopping(ped, true);
    } else if (m_Vehicle->IsPassenger(ped)) { // 0x6449c4
        if (!m_b08 && !m_IsPassengerAnimBlended) {
            if (const auto rideData = m_Vehicle->GetRideAnimData()) {
                CAnimManager::BlendAnimation(ped->GetRpClump(), rideData->AnimGroup, ANIM_ID_BIKE_PASSENGER);
            }
            m_IsPassengerAnimBlended = true;
        }
        ProcessBopping(ped, true);
        if (ped->IsPlayer() && CGameLogic::IsCoopGameGoingOn() && CanDoDriveBy() && TheCamera.GetActiveCam().m_nMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) { // 0x644a0b
            StartDriveBy(m_Vehicle->m_apPassengers[1] != ped);
        }
    }

    if (const auto held = ped->GetEntityThatThisPedIsHolding(); held && held->GetModelId() == ModelIndices::MI_GANG_SMOKE && !g_ikChainMan.IsLooking(ped) && CGeneral::GetRandomNumberInRange(0, 60) == 15) { // 0x644af7
        if (const auto tHold = ped->GetTaskManager().Find<CTaskSimpleHoldEntity>()) {
            tHold->PlayAnim(m_Vehicle->IsPassenger(ped) ? ANIM_ID_PASS_SMOKE_IN_CAR : ANIM_ID_SMOKE_IN_CAR, ANIM_GROUP_DEFAULT);
        }
    }

    if (RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SMOKE_IN_CAR)) {
        g_ikChainMan.AbortLookAtIfLooking(ped);
        m_BoppingStartTime = -1;
    } else {
        TriggerIK(ped);
    }

    if (ped->IsCreatedBy(PED_GAME) && !CPedGroups::GetPedsGroup(ped)) { // 0x644bb9 - Take over the car if there's no driver
        if (!m_Vehicle->m_pDriver && m_Vehicle->m_apPassengers[0] == ped) {
            m_NoDriverTimer.StartIfNotAlready(4'000);
            if (m_NoDriverTimer.IsOutOfTime()) {
                ped->GetEventGroup().Add(CEventScriptCommand{
                    TASK_PRIMARY_PRIMARY, new CTaskComplexSequence{ new CTaskComplexLeaveCar{ m_Vehicle, 0, 0, true, false }, new CTaskComplexCarDriveWander{ m_Vehicle } }
                });
            }
        } else {
            m_NoDriverTimer.Stop();
        }
    }

    if (((ped->m_nRandomSeed + CTimer::GetFrameCounter()) & 0x3FFF) == 0) { // 0x644d00
        if (const auto driver = m_Vehicle->m_pDriver; driver && driver != ped && driver->IsPlayer()) {
            if (m_Vehicle->GetMoveSpeed().Magnitude2D() > 0.7f) {
                ped->Say(CTX_GLOBAL_CAR_FAST);
            }
            if (m_Vehicle->GetMoveSpeed().Magnitude2D() < 0.1f) {
                ped->Say(CTX_GLOBAL_CAR_SLOW);
            }
            ped->Say(CTX_GLOBAL_CAR_SINGALONG);
        }
    }

    return false;
}

// 0x642700
void CTaskSimpleCarDrive::StartRollingDoor(CPed* ped) {
    m_AnimCloseDoorRolling = CAnimManager::AddAnimation(ped->GetRpClump(), ped->m_pVehicle->GetAnimGroup().GetGroup(ANIM_ID_CAR_ROLLDOOR), ANIM_ID_CAR_ROLLDOOR);
    m_AnimCloseDoorRolling->SetFinishCallback(FinishRollingDoorCB, this);
}

// 0x63C840
void CTaskSimpleCarDrive::FinishRollingDoorCB(CAnimBlendAssociation* anim, void* data) {
    // NOTE: Not `notsa::cast`, subclasses (`CTaskSimpleCarDriveTimed`, ...) report their own type
    const auto self              = static_cast<CTaskSimpleCarDrive*>(data);

    self->m_IsRollingDoor        = false;
    self->m_AnimCloseDoorRolling = nullptr;
    if (const auto veh = self->m_Vehicle) {
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
        if (tDrive && tDrive != this && tDrive->m_TaskUtilityLineUpPedWithCar) {
            m_TaskUtilityLineUpPedWithCar = std::exchange(tDrive->m_TaskUtilityLineUpPedWithCar, nullptr);
        }
        m_b08 = false;
    }

    if (m_TaskUtilityLineUpPedWithCar) {
        constexpr AnimationId lineUpAnims[]{
            // 0x8D2E9C
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
        m_TaskUtilityLineUpPedWithCar->ProcessPed(ped, m_Vehicle, anim);
        delete std::exchange(m_TaskUtilityLineUpPedWithCar, nullptr);
        return true;
    }

    if (m_Vehicle && ped->IsInVehicle(m_Vehicle)) {
        ped->SetPedPositionInCar();
    }
    return true;
}

void CTaskSimpleCarDrive::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarDrive, 0x86E904, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x63C340, "", {}, CVehicle*, CTaskUtilityLineUpPedWithCar*, bool);
    RH_ScopedDestructorInstall(0x63C460);

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
