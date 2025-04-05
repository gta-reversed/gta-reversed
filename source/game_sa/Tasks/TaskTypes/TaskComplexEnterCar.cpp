#include "StdInc.h"

#include "IKChainManager_c.h"

#include "TaskSimpleBikeJacked.h"
#include "TaskComplexEnterCar.h"
#include "TaskSimpleDuck.h"
#include "TaskComplexCarSlowBeDraggedOutAndStandUp.h"
#include "TaskComplexGoToCarDoorAndStandStill.h"
#include "TaskSimpleCarSlowDragPedOut.h"
#include "TaskSimpleWaitUntilPedIsOutCar.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimpleCarCloseDoorFromOutside.h"
#include "TaskSimpleCarGetIn.h"
#include "TaskSimpleCarShuffle.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskSimpleCarSetPedInAsDriver.h"
#include "TaskSimpleCarCloseDoorFromInside.h"
#include "TaskComplexGoToCarDoorAndStandStill.h"
#include "TaskSimpleCarAlign.h"
#include "TaskSimpleCarOpenDoorFromOutside.h"
#include "TaskSimpleCarOpenLockedDoorFromOutside.h"
#include "TaskSimplePickUpBike.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexEnterBoatAsDriver.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskSimpleLeaveGroup.h"
#include "TaskSimpleStandStill.h"

#include "EventCopCarBeingStolen.h"
#include "EventLeaderQuitEnteringCarAsDriver.h"
#include "EventLeaderExitedCarAsDriver.h"
#include "EventLeaderEnteredCarAsDriver.h"

void CTaskComplexEnterCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCar, 0x86e6f0, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63A220);
    RH_ScopedInstall(Destructor, 0x63DFA0);
    
    RH_ScopedInstall(PreparePedForVehicleEnter, 0x63AC80);
    RH_ScopedInstall(PrepareVehicleForPedEnter, 0x63AC10);
    RH_ScopedInstall(CreateTaskUtilityLineUpPedWithCar, 0x63ACC0);
    RH_ScopedInstall(GetTargetPos, 0x63A300);
    RH_ScopedInstall(GetCameraAvoidVehicle, 0x63A690);
    RH_ScopedInstall(SetVehicleFlags, 0x63AB90);
    RH_ScopedInstall(CreateSubTask, 0x63E040);

    RH_ScopedVMTInstall(MakeAbortable, 0x63A730);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63E990);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643A60);
    RH_ScopedVMTInstall(ControlSubTask, 0x63A890);
    RH_ScopedVMTInstall(CreateNextSubTask_AfterSimpleCarAlign, 0x63F970);
}

// 0x63A220
CTaskComplexEnterCar::CTaskComplexEnterCar(CVehicle* targetVehicle, bool bAsDriver, bool bQuitAfterOpeningDoor, bool bQuitAfterDraggingPedOut, bool bCarryOnAfterFallingOff) :
    m_bAsDriver{ bAsDriver },
    m_bQuitAfterOpeningDoor{ bQuitAfterOpeningDoor },
    m_bQuitAfterDraggingPedOut{ bQuitAfterDraggingPedOut },
    m_bCarryOnAfterFallingOff{ bCarryOnAfterFallingOff },
    m_Car{ targetVehicle }
{
    CEntity::SafeRegisterRef(m_Car);
}

// 0x63DFA0
CTaskComplexEnterCar::~CTaskComplexEnterCar() {
    delete m_LineUpUtility;

    CEntity::SafeCleanUpRef(m_Car);
    if (m_Car) {
        m_Car->m_nNumGettingIn -= m_NumGettingInSet;
        m_Car->ClearGettingInFlags(m_DoorFlagsSet);
    }
}

// 0x63A730
bool CTaskComplexEnterCar::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (!m_Car) {
        return true;
    }
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        m_pSubTask->MakeAbortable(ped, priority, event);
        CTaskSimpleStandStill{ -1 }.ProcessPed(ped);
        if (m_TargetDoor != 0) { // TODO: Enum
            CTaskSimpleCarSetPedOut{ m_Car, (eTargetDoor)m_TargetDoor, false }.ProcessPed(ped);
            if (CCarEnterExit::CarHasDoorToClose(m_Car, (eDoors)m_TargetDoor)) {
                CTaskSimpleCarCloseDoorFromOutside{ m_Car, (uint32)m_TargetDoor, nullptr }.MakeAbortable(ped, priority, event);
            }
        }
        return true;
    }
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL) {
        return m_pSubTask->MakeAbortable(ped, priority, event);
    }
    return false;
}

// 0x63E990
CTask* CTaskComplexEnterCar::CreateNextSubTask(CPed* ped) {
    const auto C = [this, ped](eTaskType tt){ return CreateSubTask(tt, ped); };

    if (m_bAborting) { // 0x63E9BE - Inverted
        switch (const auto tt = m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER: 
            return C(TASK_SIMPLE_CAR_SET_PED_OUT);
        case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE:
        case TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL:
        case TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT:
        case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE:
        case TASK_SIMPLE_CAR_DRIVE_TIMED:
        case TASK_SIMPLE_STAND_STILL:
        case TASK_SIMPLE_CAR_SET_PED_OUT:
        case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
        case TASK_SIMPLE_CAR_GET_IN:
        case TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE:
        case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE:
        case TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR:
        case TASK_SIMPLE_CAR_ALIGN:
        case TASK_COMPLEX_LEAVE_CAR:
        case TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE:
        case TASK_COMPLEX_FALL_AND_GET_UP:
        case TASK_SIMPLE_BIKE_PICK_UP:
        case TASK_SIMPLE_CAR_SHUFFLE:
        case TASK_NONE:
            return C(TASK_FINISHED);
        default:
            NOTSA_UNREACHABLE("SubTaskType = {}", tt);
        }
    }

    if (!m_Car || !CCarEnterExit::IsVehicleHealthy(m_Car)) { // 0x63F8D9
        return C(TASK_FINISHED);
    }

    // These are likely some functions inlined from `CCarEnterExit`
    const auto CanUseTargetDoor = [this]() {
        // actually U32 == Component
        return (m_Car->IsDoorReadyU32(m_TargetDoor) || m_Car->IsDoorFullyOpenU32(m_TargetDoor))
            || (!m_Car->IsDoorMissingU32(m_TargetDoor) && !m_Car->IsDoorFullyOpenU32(m_TargetDoor) && !m_Car->IsDoorClosedU32(m_TargetDoor));
    };
    const auto OpenCarDoor = [this]() {
        if (m_Car->IsAutomobile()) {
            m_Car->AsAutomobile()->GetDamageManager().SetDoorOpen_Component((tComponent)m_TargetDoor);
        };
    };

    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL: {
        const auto tGoToDoor = static_cast<CTaskComplexGoToCarDoorAndStandStill*>(m_pSubTask);

        if (!tGoToDoor->HasAchievedDoor()) { // 0x63EBEA
            return C(TASK_FINISHED);
        }

        m_TargetDoor               = tGoToDoor->m_TargetDoor;
        m_TargetSeat               = tGoToDoor->m_TargetSeat;
        m_TargetDoorOppositeToFlag = ComputeTargetDoorOppositeToFlag();

        if (CCarEnterExit::IsCarDoorInUse(m_Car, m_TargetDoor, m_TargetDoorOppositeToFlag)) { // 0x63EC24
            ped->SetMoveState(PEDMOVE_STILL);
            if (ped->IsPlayer()) {
                ped->m_pPlayerData->m_fMoveBlendRatio = 0.f;
                ped->AsPlayer()->SetRealMoveAnim();
            } else {
                ped->SetMoveAnim();
            }
            return C(TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE);
        }

        if (!CanUseTargetDoor()) { // 0x63ECBC
            return C(TASK_FINISHED);
        }

        SetVehicleFlags(ped);
        PrepareVehicleForPedEnter(ped);
        PreparePedForVehicleEnter(ped);
        CreateTaskUtilityLineUpPedWithCar(ped);

        return m_Car->IsBike() || m_Car->IsSubQuad() // For bikes/quads we don't have to align, and can immidiately continue, for everything else first align, then do the rest.
            ? CreateNextSubTask_AfterSimpleCarAlign(ped)
            : C(TASK_SIMPLE_CAR_ALIGN);
    }
    case TASK_SIMPLE_CAR_ALIGN: // 0x63F2C1
        return CreateNextSubTask_AfterSimpleCarAlign(ped);
    case TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE: { // 0x63ED58
        // This does not make sense, i have no idea why R* put it here
        if (m_Car && m_CruiseSpeed >= 0.f) {
            m_Car->m_autoPilot.m_nCruiseSpeed = (uint32)m_CruiseSpeed;
        }
        return C(TASK_SIMPLE_STAND_STILL);
    }
    case TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE:
    case TASK_SIMPLE_BIKE_PICK_UP: { // 0x63EE0D
        if (m_Car->GetMoveSpeed().SquaredMagnitude2D() > sq(ped->IsPlayer() ? 0.2f : 0.1f)) { // 0x63EDAB
            ped->m_bUsesCollision = true;
            return C(ped->IsPlayer() ? TASK_FINISHED : TASK_COMPLEX_FALL_AND_GET_UP);
        }

        if (m_bQuitAfterOpeningDoor) { // 0x63EE4E
            m_bQuitAfterOpeningDoor = false;
            return C(TASK_FINISHED);
        }

        if (m_bQuitAfterDraggingPedOut || CCarEnterExit::IsCarSlowJackRequired(m_Car, m_TargetDoor)) { // 0x63EEBE
            CEntity::ChangeEntityReference(m_DraggedPed, CCarEnterExit::ComputeSlowJackedPed(m_Car, m_TargetDoor));

            if (m_bQuitAfterDraggingPedOut || m_DraggedPed && !CPedGroups::AreInSameGroup(ped, m_DraggedPed) && !m_DraggedPed->bDontDragMeOutCar) { // 0x63EEBE
                if (m_DraggedPed && m_DraggedPed->IsPlayer() && CGameLogic::SkipState == SKIP_IN_PROGRESS) { // 0x63EF63
                    if (const auto draggedPedGrp = m_DraggedPed->GetGroup()) {
                        if (draggedPedGrp->GetMembership().IsLeader(m_DraggedPed)) {
                            draggedPedGrp->GetIntelligence().AddEvent(CEventGroupEvent{ m_DraggedPed, new CEventLeaderQuitEnteringCarAsDriver{} });
                        }
                    }
                    return C(TASK_FINISHED);
                }

                if (m_DraggedPed) { // 0x63F034
                    return CreateDragPedOutSubTask(ped);
                }

                if (m_bQuitAfterDraggingPedOut) {
                    OpenCarDoor();
                    return C(TASK_FINISHED);
                }
            }
        } else if (m_DraggedPed && (m_bAsDriver ? m_Car->IsDriver(m_DraggedPed) : m_DraggedPed == m_Car->GetPassengers()[CCarEnterExit::ComputePassengerIndexFromCarDoor(m_Car, m_TargetDoor)])) { // 0x63EF18 - Check if it's  necessary to drag the ped out
            if (CCarEnterExit::CarHasDoorToClose(m_Car, (eDoors)m_TargetDoor)) {
                return C(TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE);
            }
            return C(TASK_FINISHED);
        }
        return C(TASK_SIMPLE_CAR_GET_IN);
    }
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE: { // 0x63F69A
        if (!m_Car || !CCarEnterExit::IsVehicleHealthy(m_Car) || !CCarEnterExit::IsPedHealthy(ped)) { // 0x63F69A
            return C(TASK_FINISHED);
        }
        if (!m_bAsDriver || m_TargetDoor != 8) {
            return C(m_bAsDriver ? TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER : TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER);
        }
        if (const auto drvr = m_Car->m_pDriver) {
            drvr->GetIntelligence()->GetEventGroup().Add(CEventDraggedOutCar{ m_Car, ped, true });
            drvr->GetIntelligence()->AddTaskPrimaryMaybeInGroup(
                drvr->IsAlive() && drvr->m_fHealth > 0.f
                    ? new CTaskComplexLeaveCar{ m_Car, CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Car->m_pDriver), 0, false, true }
                    : new CTaskComplexLeaveCar{ m_Car, CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Car->m_pDriver), 0 },
                true
            );
        }
        return C(m_Car->m_pHandlingData->m_bTandemSeats ? TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER : TASK_SIMPLE_CAR_SHUFFLE);
    }
    case TASK_SIMPLE_CAR_GET_IN: { // 0x63F3AF
        CCarEnterExit::MakeUndraggedPassengerPedsLeaveCar(m_Car, m_DraggedPed, ped);
        bool bDontCloseDoors{};
        if (m_bAsDriver && ped->IsPlayer()) {
            if (m_Car->IsAutomobile()) {
                if (!m_Car->IsSubPlane() && !m_Car->IsSubHeli()) {
                    const auto pad = CPad::GetPad();
                    if (pad->GetAccelerate() >= 255.f || pad->GetBrake() >= 255.f) {
                        if (CCarEnterExit::CarHasDoorToClose(m_Car, m_TargetDoor)) {
                            OpenCarDoor();
                        }
                        bDontCloseDoors = true;
                    }
                }
            }
        }

        if (CCarEnterExit::CarHasDoorToClose(m_Car, m_TargetDoor) && !bDontCloseDoors) {
            return C(TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE);
        }

        if (!m_bAsDriver) {
            return C(TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER);
        }

        if (m_TargetDoor != 8 || !m_Car->IsAutomobile() || m_Car->m_pHandlingData->m_bTandemSeats) {
            return C(TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER);
        }

        if (!m_Car->m_pDriver) {
            return C(TASK_SIMPLE_CAR_SHUFFLE);
        }

        if (const auto drvr = m_Car->m_pDriver) {
            drvr->GetIntelligence()->GetEventGroup().Add(CEventDraggedOutCar{ m_Car, ped, true });
            drvr->GetIntelligence()->AddTaskPrimaryMaybeInGroup(
                drvr->IsAlive() && drvr->m_fHealth > 0.f
                    ? new CTaskComplexLeaveCar{ m_Car, CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Car->m_pDriver), 0, false, true }
                    : new CTaskComplexLeaveCar{ m_Car, CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Car->m_pDriver), 0 },
                true
            );
        }
        return C(TASK_SIMPLE_CAR_SHUFFLE);
    }
    case TASK_SIMPLE_CAR_SHUFFLE: // 0x63F880
        return C(TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER);
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE: { // 0x63F23F
        if (!CanUseTargetDoor()) {
            return nullptr;
        }

        SetVehicleFlags(ped);
        PrepareVehicleForPedEnter(ped);
        PreparePedForVehicleEnter(ped);
        CreateTaskUtilityLineUpPedWithCar(ped);

        return m_Car->IsBike() || m_Car->IsSubQuad()
            ? CreateNextSubTask_AfterSimpleCarAlign(ped)
            : C(TASK_SIMPLE_CAR_ALIGN);
    }
    case TASK_SIMPLE_CAR_QUICK_DRAG_PED_OUT: { // 0x63F2ED
        CCarEnterExit::MakeUndraggedPassengerPedsLeaveCar(m_Car, m_DraggedPed, ped);
        return C(TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER);
    }
    case TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT: {
        if (!m_bQuitAfterDraggingPedOut) {
            return C(TASK_SIMPLE_CAR_GET_IN);
        }
        m_bQuitAfterDraggingPedOut = false;
        return C(m_DraggedPed ? TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR : TASK_FINISHED);
    }
    case TASK_COMPLEX_FALL_AND_GET_UP: {
        if (FindPlayerPed() == ped) {
            FindPlayerInfo().SetLastTargetVehicle(nullptr);
        }

        if (m_bCarryOnAfterFallingOff) {
            return CreateFirstSubTask(ped);
        }
        return C(TASK_FINISHED);
    }
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE:
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
    case TASK_SIMPLE_CAR_DRIVE_TIMED:
    case TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR:
    case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
    case TASK_SIMPLE_STAND_STILL:
    case TASK_NONE:
        return C(TASK_FINISHED);

    case TASK_SIMPLE_UNINTERRUPTABLE:
    case TASK_SIMPLE_PAUSE:
        return nullptr;

    default:
        NOTSA_UNREACHABLE("SubTaskType = {}", tt);
    }
}

// 0x63F970
CTask* CTaskComplexEnterCar::CreateNextSubTask_AfterSimpleCarAlign(CPed* ped) {
    const auto DoLookAtPassengers = [this, ped]() {
        if (!m_Car->vehicleFlags.bIsBus) { // 0x64013E
            const auto ProcessOneOccupant = [ped](CPed* occupant) {
                if (!occupant) {
                    return;
                }
                using namespace CGeneral;
                g_ikChainMan.LookAt(
                    "TaskEnterCar",
                    occupant,
                    ped,
                    ped->GetIntelligence()->IsFriendlyWith(*occupant) || CPedGroups::AreInSameGroup(ped, occupant)
                    ? GetRandomNumberInRange(2'000, 3'000)
                    : GetRandomNumberInRange(6'000, 8'000),
                    BONE_HEAD,
                    nullptr,
                    false,
                    0.5f,
                    350,
                    3,
                    false
                );
                };
            rng::for_each(m_Car->GetPassengers(), ProcessOneOccupant);
            ProcessOneOccupant(m_Car->m_pDriver);
        }
    };
    const auto C = [&, this](eTaskType tt, bool bDoLookAt = true){
        if (bDoLookAt) {
            DoLookAtPassengers();
        }
        return CreateSubTask(tt, ped);
    };

    if (m_Car->GetMoveSpeed().SquaredMagnitude2D() > sq(ped->IsPlayer() ? 0.2f : 0.1f)) {
        ped->m_bUsesCollision = true;
        return C(ped->IsPlayer() ? TASK_FINISHED : TASK_COMPLEX_FALL_AND_GET_UP);
    }

    if (m_bQuitAfterOpeningDoor) { // 0x63FA2E
        if (CCarEnterExit::CarHasDoorToOpen(m_Car, m_TargetDoor)) {
            return C(CCarEnterExit::CarHasOpenableDoor(m_Car, m_TargetDoor, ped) ? TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE : TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE);
        }
        m_bQuitAfterOpeningDoor = false;
        return C(TASK_FINISHED);
    }

    if (!m_bQuitAfterDraggingPedOut && CCarEnterExit::IsCarQuickJackPossible(m_Car, m_TargetDoor, ped)) { // 0x63FA8D
        DoLookAtPassengers();
        return nullptr;
    }

    if (!m_Car->IsBike() && !m_Car->IsSubQuad()) { // 0x63FAA6
        if (CCarEnterExit::CarHasDoorToOpen(m_Car, m_TargetDoor)) {
            if (!CCarEnterExit::CarHasOpenableDoor(m_Car, m_TargetDoor, ped)) { // 0x63FADA
                return C(TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE);
            }
            if (m_bQuitAfterDraggingPedOut || m_bQuitAfterOpeningDoor || m_Car->GetAnimGroupId() != ANIM_GROUP_CONVCARANIMS || m_TargetDoor != 10 || m_Car->m_pDriver) { // 0x63FAE6
                return C(TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE);
            }
            return C(TASK_SIMPLE_CAR_GET_IN);
        }

        if (!CCarEnterExit::IsCarSlowJackRequired(m_Car, m_TargetDoor)) {
            return C(m_bQuitAfterDraggingPedOut ? TASK_FINISHED : TASK_SIMPLE_CAR_GET_IN);
        }

        if (const auto slowJackedPed = CCarEnterExit::ComputeSlowJackedPed(m_Car, m_TargetDoor)) { // 0x63FB5E
            CEntity::ChangeEntityReference(m_DraggedPed, slowJackedPed);
        }

        if (!m_DraggedPed || !m_DraggedPed->IsPlayer() || CGameLogic::SkipState != SKIP_IN_PROGRESS) { // 0x63FB68
            if (m_bQuitAfterDraggingPedOut) {
                if (!m_DraggedPed) {
                    return C(TASK_FINISHED);
                }
            } else { // 0x63FC2E
                if (!m_DraggedPed || CPedGroups::AreInSameGroup(m_DraggedPed, ped) || m_DraggedPed->bDontDragMeOutCar) { // 0x63FC41
                    if (!m_DraggedPed || !m_bAsDriver || !m_Car->IsDriver(m_DraggedPed)) { // 0x63FC63 - Inverted
                        if (!m_DraggedPed || m_bAsDriver || m_DraggedPed != m_Car->GetPassengers()[CCarEnterExit::ComputePassengerIndexFromCarDoor(m_Car, m_TargetDoor)]) {
                            return C(TASK_SIMPLE_CAR_GET_IN);
                        }
                    }
                    return C(CCarEnterExit::CarHasDoorToClose(m_Car, m_TargetDoor) ? TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE : TASK_FINISHED);
                }
            }
            DoLookAtPassengers();
            return CreateDragPedOutSubTask(ped);
        }

        if (const auto pedGrp = m_DraggedPed->GetGroup()) { // 0x63FB95
            if (pedGrp->GetMembership().IsLeader(m_DraggedPed)) {
                pedGrp->GetIntelligence().AddEvent(CEventGroupEvent{ m_DraggedPed, new CEventLeaderQuitEnteringCarAsDriver{} });
            }
        }

        return C(TASK_FINISHED);
    }

    if (!CCarEnterExit::IsCarSlowJackRequired(m_Car, m_TargetDoor)) {
        if (m_bQuitAfterDraggingPedOut) { // 0x6400AE
            return C(TASK_FINISHED);
        }

        if (m_Car->GetStatus() == STATUS_ABANDONED && m_Car->IsBike()) { // 0x6400C8 - Inverted
            if (const auto& r = m_Car->GetRight(); r.z >= 0.5f || r.z <= -0.5f || m_Car->GetUp().z <= 0.f) { // 0x6400F3 - Check if the bike is on it's side
                return C(TASK_SIMPLE_BIKE_PICK_UP);
            }
            m_Car->AsBike()->bikeFlags.bGettingPickedUp = true;
        }

        return C(TASK_SIMPLE_CAR_GET_IN);
    }

    CEntity::ChangeEntityReference(m_DraggedPed, CCarEnterExit::ComputeSlowJackedPed(m_Car, m_TargetDoor));


    if (m_DraggedPed && m_DraggedPed->IsPlayer() && CGameLogic::SkipState == SKIP_IN_PROGRESS) { // 0x63FE9B - Inverted
        if (const auto draggedPedGrp = m_DraggedPed->GetGroup()) {
            if (draggedPedGrp->GetMembership().IsLeader(m_DraggedPed)) {
                draggedPedGrp->GetIntelligence().AddEvent(CEventGroupEvent{ m_DraggedPed, new CEventLeaderQuitEnteringCarAsDriver{} });
            }
        }
        return C(TASK_FINISHED, false);
    }

    assert(m_DraggedPed);

    const auto AddJackedTaskToPed = [this, ped](CPed* jacked, int32 door) {
        jacked->GetIntelligence()->AddTaskPrimaryMaybeInGroup(new CTaskSimpleBikeJacked{
            m_Car,
            (uint32)door,
            (uint32)m_DraggedPedDownTime,
            ped,
            true
        }, true);
    };

    AddJackedTaskToPed(m_DraggedPed, m_TargetDoor != 18 || m_Car->HasPassengerAtSeat(0) ? 10 : 11); // 0x63FF50
    if (m_Car->IsDriver(m_DraggedPed) && m_Car->HasPassengerAtSeat(0)) {
        AddJackedTaskToPed(m_Car->GetPassengers()[0], 11);

        if (const auto draggedPedGrp = m_DraggedPed->GetGroup()) {
            if (draggedPedGrp->GetMembership().IsLeader(m_DraggedPed)) {
                draggedPedGrp->GetIntelligence().AddEvent(CEventGroupEvent{ m_DraggedPed, new CEventLeaderExitedCarAsDriver{} });
            }
        }
    }

    return C(m_bQuitAfterDraggingPedOut || m_TargetDoor != 18 ? TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT : TASK_SIMPLE_CAR_GET_IN);
}

// 0x643A60
CTask* CTaskComplexEnterCar::CreateFirstSubTask(CPed* ped) {
    const auto C = [this, ped](eTaskType tt){ return CreateSubTask(tt, ped); };

    m_EnterCarStartTime = CTimer::GetTimeInMS();

    if (   !m_Car || m_Car->m_pFire
        || !CCarEnterExit::IsVehicleHealthy(m_Car) || !CCarEnterExit::IsPedHealthy(ped)
        || !m_bAsDriver && !m_Car->m_nMaxPassengers                                         // Wants to enter as passenger, but there are no passenger seats
        || m_Car->IsTrain() && m_Car->AsTrain()->trainFlags.bNotOnARailRoad
    ) {
        return C(TASK_FINISHED);
    }

    if (m_bAsDriver && !m_bQuitAfterDraggingPedOut && !m_bQuitAfterOpeningDoor) {
        if (const auto pedGrp = ped->GetGroup()) {
            if (pedGrp->GetMembership().IsLeader(ped)) { // 0x643B15
                pedGrp->GetIntelligence().AddEvent(CEventGroupEvent{ ped, new CEventLeaderEnteredCarAsDriver{ m_Car } });
            }
        }
    }

    if (ped->bInVehicle) {
        return C(ped->m_pVehicle == m_Car
            ? TASK_SIMPLE_CAR_DRIVE_TIMED
            : TASK_COMPLEX_LEAVE_CAR
        );
    }

    if (m_Car->IsBoat()) {
        return C(TASK_COMPLEX_ENTER_BOAT_AS_DRIVER);
    }

    if (const auto tGoToCarDoor = static_cast<CTaskComplexGoToCarDoorAndStandStill*>(C(TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL))) {
        if (ped->physicalFlags.bSubmergedInWater && !ped->bIsStanding) {
            if (notsa::contains({ MODEL_SKIMMER, MODEL_VORTEX, MODEL_SEASPAR, MODEL_LEVIATHN }, m_Car->GetModelID())) {
                if (CCarEnterExit::GetNearestCarDoor(ped, m_Car, m_TargetDoorPos, m_TargetDoor)) {
                    tGoToCarDoor->SetTargetPt(m_TargetDoorPos);
                    tGoToCarDoor->SetTargetDoor(m_TargetDoor);
                    tGoToCarDoor->SetTryingToEnterInWater(true);
                }
            }
        }
        return tGoToCarDoor;
    }

    return nullptr;
}

// 0x63A890
CTask* CTaskComplexEnterCar::ControlSubTask(CPed* ped) {
    const auto CheckForPlayerQuit = [&, this](bool bCheckSubTask) {
        if (!ped->IsPlayer()) {
            return false;
        }
        if (!CCarEnterExit::IsPlayerToQuitCarEnter(ped, m_Car, m_EnterCarStartTime, m_pSubTask)) {
            return false;
        }
        if (m_bAsDriver && !m_bQuitAfterOpeningDoor && !m_bQuitAfterDraggingPedOut) {
            if (!bCheckSubTask || m_pSubTask->GetTaskType() != TASK_SIMPLE_CAR_GET_IN) {
                if (CPedGroups::IsGroupLeader(ped)) { // Redundant?
                    if (const auto plyrGrp = ped->GetGroup()) {
                        if (plyrGrp->GetMembership().IsLeader(ped)) {
                            plyrGrp->GetIntelligence().AddEvent(CEventGroupEvent{ ped, new CEventLeaderQuitEnteringCarAsDriver{} });
                        }
                    }
                }
            }
        }
        return true;
    };

    if (ped->IsPlayer() && !m_bQuitAfterDraggingPedOut && m_Car) { // 0x63A8BA
        if (CheckForPlayerQuit(true)) {
            if (CPed* jacked; m_TargetDoor > 0 && (jacked = CCarEnterExit::ComputeSlowJackedPed(m_Car, m_TargetDoor)) && jacked->bDontDragMeOutCar) {
                m_bQuitAfterOpeningDoor = true;
            } else {
                m_bQuitAfterDraggingPedOut = true;
            }
        }
    }

    if (const auto tGoToCarDoor = notsa::dyn_cast_if_present<CTaskComplexGoToCarDoorAndStandStill>(m_pSubTask)) { // 0x63AB28 - Moved up here to simplify the code
        if (tGoToCarDoor->GetTargetDoor() != 0) {
            m_TargetDoorPos = tGoToCarDoor->GetTargetPt();
        }
    }

    if (!m_Car || !CCarEnterExit::IsVehicleHealthy(m_Car) || !CCarEnterExit::IsPedHealthy(ped)) { // 0x63A9E2
        m_bAborting = true;
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
        return m_pSubTask;
    }

    switch (m_pSubTask->GetTaskType()) { // 0x63AA03 - `if (m_bAsDriver || ped->IsPlayer())` is useless here as `CheckForPlayerQuit` checks for it too
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE:
    case TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL:
    case TASK_NONE:
    case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
        if (CheckForPlayerQuit(false)) {
            return nullptr;
        }
    }

    return m_pSubTask;
}

// 0x63E040
CTask* CTaskComplexEnterCar::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT: { // 0x63E8CD
        if (ped && ped->IsPlayer() && !ped->AsPlayer()->bHasDisplayedPlayerQuitEnterCarHelpText) {
            ped->AsPlayer()->bHasDisplayedPlayerQuitEnterCarHelpText = true;

            // To stop Carl from entering a car once he has opened the door or dragged out a
            // driver or passenger, use the movement controls or press ~k~~PED_FIREWEAPON~.
            CHud::SetHelpMessage(TheText.Get("JCK_HLP"));
        }
        return new CTaskSimpleCarSlowDragPedOut{ m_Car, (eTargetDoor)m_TargetDoor, m_LineUpUtility, m_bQuitAfterDraggingPedOut };
    }
    case TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR: // 0x63E846
        return new CTaskSimpleWaitUntilPedIsOutCar{ m_DraggedPed, IsTargetDoorOnTheLeft() ? -m_Car->GetRight() : m_Car->GetRight() };
    case TASK_FINISHED: { // 0x63E674
        if (!ped) {
            return nullptr;
        }

        if (!ped->bInVehicle && !ped->m_bUsesCollision) { // 0x63E67A
            ped->m_bUsesCollision = true;
            if (m_Car && m_TargetDoor != 0) { // TODO: Enum
                CTaskSimpleCarSetPedOut::PositionPedOutOfCollision(ped, m_Car, m_TargetDoor);
            }
        }

        if (m_bAsDriver && !m_bQuitAfterOpeningDoor && !m_bQuitAfterDraggingPedOut) { // Moved this common piece out here
            if (ped->IsPlayer()) { // 0x63E6AA
                if (m_Car && m_Car->IsLawEnforcementVehicle()) {
                    if (m_DoorFlagsSet) {
                        GetEventGlobalGroup()->Add(CEventCopCarBeingStolen{ ped, m_Car });
                    }
                }
            }

            if (!ped->bInVehicle) { // 0x63E717
                if (const auto pedGrp = ped->GetGroup()) {
                    if (pedGrp->GetMembership().IsLeader(ped)) {
                        pedGrp->GetIntelligence().AddEvent(CEventGroupEvent{ ped, new CEventLeaderQuitEnteringCarAsDriver{} });
                    }
                }
            }
        }

        if (m_Car && m_CruiseSpeed >= 0.f) {
            if (ped->IsPlayer() ? !ped->bInVehicle && m_Car->m_pDriver && m_bQuitAfterOpeningDoor : ped->bInVehicle) {  // 0x63E7A4 - NOTE/BUG: Why is it checking `!bInVehicle` for the player? Typo?
                m_Car->m_autoPilot.SetCruiseSpeed((uint32)m_CruiseSpeed);
            }
        }

        return nullptr;
    }
    case TASK_SIMPLE_CAR_SET_PED_OUT: // 0x63E619
        return new CTaskSimpleCarSetPedOut{ m_Car, (eTargetDoor)m_TargetDoor, true };
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE: // 0x63E60D
        return new CTaskSimpleCarCloseDoorFromOutside{ m_Car, (uint32)m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_CAR_GET_IN: // 0x63E50F
        return new CTaskSimpleCarGetIn{ m_Car, (uint32)m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_CAR_SHUFFLE: // 0x63E56F
        return new CTaskSimpleCarShuffle{ m_Car, m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE: // 0x63E503
        return new CTaskSimpleCarWaitForDoorNotToBeInUse{m_Car, (uint32)m_TargetDoor, (uint32)m_TargetDoorOppositeToFlag };
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER: // 0x63E5D7
        return new CTaskSimpleCarSetPedInAsPassenger{ m_Car, (eTargetDoor)m_TargetDoor, false, m_LineUpUtility };
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER: // 0x63E5A1
        return new CTaskSimpleCarSetPedInAsDriver{ m_Car, m_LineUpUtility };
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE: // 0x0063E387
        return new CTaskSimpleCarCloseDoorFromInside{ m_Car, (uint32)m_TargetDoor, m_LineUpUtility };
    case TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL: // 0x63E387
        return new CTaskComplexGoToCarDoorAndStandStill{ m_Car, m_MoveState, m_bAsDriver, m_TargetSeat, 0.5f, 4.f, 50.f, 30'000 };
    case TASK_SIMPLE_CAR_ALIGN: // 0x63E3C1
        return new CTaskSimpleCarAlign{ m_Car, m_TargetDoorPos, (uint32)m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE: // 0x63E405
        return new CTaskSimpleCarOpenDoorFromOutside{ m_Car, m_TargetDoor, m_LineUpUtility, m_bQuitAfterOpeningDoor };
    case TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE: // 0x63E43B
        return new CTaskSimpleCarOpenLockedDoorFromOutside{ m_Car, m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_BIKE_PICK_UP: // 0x63E471
        return new CTaskSimplePickUpBike{ m_Car, m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_CAR_DRIVE_TIMED: // 0x63E314
        return new CTaskSimpleCarDriveTimed{ m_Car };
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ m_Car, 0, 0, true, false }; // 0x63E2E8
    case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER: // 0x63E2AE
        return new CTaskComplexEnterBoatAsDriver{ m_Car };
    case TASK_COMPLEX_FALL_AND_GET_UP: { // 0x63E15E
        if (FindPlayerPed() == ped) {
            FindPlayerInfo().SetLastTargetVehicle(m_Car);
        }
        if (ped && !ped->bInVehicle) {
            ped->m_bUsesCollision = true;
        }
        if (m_Car) {
            CTaskSimpleCarSetPedOut::PositionPedOutOfCollision(ped, m_Car, m_TargetDoor);

            assert(m_Car->m_nNumGettingIn >= m_NumGettingInSet);
            m_Car->m_nNumGettingIn -= m_NumGettingInSet;
            m_NumGettingInSet       = 0;

            m_Car->ClearGettingInFlags(m_DoorFlagsSet);
            m_DoorFlagsSet = 0;

            if (m_Car->IsAutomobile()) {
                if (!m_Car->IsDoorMissingU32(m_TargetDoor)) {
                    m_Car->AsAutomobile()->m_damageManager.SetDoorOpen((eDoors)m_TargetDoor);
                }
            }
        }
        return new CTaskComplexFallAndGetUp{ IsTargetDoorOnTheLeft() ? ANIM_ID_KO_SPIN_L : ANIM_ID_KO_SPIN_R, ANIM_GROUP_DEFAULT, 2000 };
    }
    case TASK_NONE:
        return new CTaskSimpleNone{}; // 0x63E0F8 - NOTE/TODO: Weird
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{}; // 0x63E0CE
    default:
        NOTSA_UNREACHABLE("taskType = {}", taskType);
    }
}

// 0x63AB90
void CTaskComplexEnterCar::SetVehicleFlags(CPed* ped) {
    m_DoorFlagsSet = CCarEnterExit::ComputeDoorFlag(m_Car, m_TargetDoor, true);
    m_Car->SetGettingInFlags(m_DoorFlagsSet);

    m_NumGettingInSet = true;
    m_Car->m_nNumGettingIn++;

    if (m_TargetDoorOppositeToFlag) {
        const auto oppositeDoorFlag = CCarEnterExit::ComputeDoorFlag(m_Car, m_TargetDoorOppositeToFlag, true);
        if (oppositeDoorFlag != m_DoorFlagsSet) {
            m_Car->SetGettingInFlags(oppositeDoorFlag);
            m_DoorFlagsSet |= oppositeDoorFlag;
        }
    }
}

// 0x63AB70
int32 CTaskComplexEnterCar::ComputeTargetDoorOppositeToFlag() const {
    return m_bAsDriver && m_TargetDoor == 8 ? 10 : 0; // I guess `8` is the driver side door?
}

// 0x63AC80
void CTaskComplexEnterCar::PreparePedForVehicleEnter(CPed* ped) {
    ped->m_bUsesCollision = false;
    if (const auto tDuck = ped->GetIntelligence()->GetTaskDuck()) {
        tDuck->MakeAbortable(ped);
    }
}

// 0x63AC10
void CTaskComplexEnterCar::PrepareVehicleForPedEnter(CPed* ped) {
    if (const auto carCruiseSpeed = m_Car->m_autoPilot.m_nCruiseSpeed) {
        m_CruiseSpeed = (float)carCruiseSpeed;
    }
    if (!ped->IsPlayer() || !CCarEnterExit::CarHasDoorToOpen(m_Car, (eDoors)m_TargetDoor) || CCarEnterExit::CarHasOpenableDoor(m_Car, m_TargetDoor, ped)) {
        m_Car->m_autoPilot.SetCruiseSpeed(0);
    }
}

// 0x63ACC0
void CTaskComplexEnterCar::CreateTaskUtilityLineUpPedWithCar(CPed* ped) {
    if (m_LineUpUtility && notsa::IsFixBugs()) {
        m_LineUpUtility = new (m_LineUpUtility) CTaskUtilityLineUpPedWithCar{
            CCarEnterExit::GetPositionToOpenCarDoor(m_Car, m_TargetDoor) - ped->GetPosition(),
            600,
            0,
            m_TargetDoor
        };
        return;
    } else {
        assert(!m_LineUpUtility);
    }
    m_LineUpUtility = new CTaskUtilityLineUpPedWithCar{
        CCarEnterExit::GetPositionToOpenCarDoor(m_Car, m_TargetDoor) - ped->GetPosition(),
        600,
        0,
        m_TargetDoor
    };
}

// NOTSA - Copy paste code elimination
CTask* CTaskComplexEnterCar::CreateDragPedOutSubTask(CPed* ped) {
    m_DraggedPed->GetIntelligence()->AddTaskPrimaryMaybeInGroup(new CTaskComplexCarSlowBeDraggedOutAndStandUp{ m_Car, m_TargetDoor }, true);
    m_DraggedPed->GetIntelligence()->GetEventGroup().Add(CEventDraggedOutCar{ m_Car, ped, m_Car->IsDriver(m_DraggedPed) });
    if (m_Car->IsDriver(m_DraggedPed)) {
        if (const auto draggedPedGrp = m_DraggedPed->GetGroup()) {
            if (draggedPedGrp->GetMembership().IsLeader(m_DraggedPed)) {
                draggedPedGrp->GetIntelligence().AddEvent(CEventGroupEvent{ m_DraggedPed, new CEventLeaderExitedCarAsDriver{} });
            }
        }
    }
    return CreateSubTask(TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT, ped);
}

// 0x63A300
CVector CTaskComplexEnterCar::GetTargetPos() const {
    if (m_TargetDoor != 0) { // TODO: Enum
        return m_TargetDoorPos;
    }
    if (const auto tGoTo = notsa::dyn_cast_if_present<CTaskComplexGoToCarDoorAndStandStill>(m_pSubTask)) {
        return tGoTo->GetTargetPt();
    }
    return {};
}

// 0x63A690
CVehicle* CTaskComplexEnterCar::GetCameraAvoidVehicle() {
    if (m_Car && m_pSubTask) {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_LEAVE_CAR:
        case TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE:
        case TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE:
        case TASK_SIMPLE_BIKE_PICK_UP:
        case TASK_SIMPLE_STAND_STILL:
        case TASK_SIMPLE_CAR_QUICK_DRAG_PED_OUT:
        case TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT:
        case TASK_SIMPLE_CAR_SET_PED_OUT:
        case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
        case TASK_SIMPLE_CAR_DRIVE_TIMED:
        case TASK_SIMPLE_CAR_GET_IN:
        case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE:
        case TASK_SIMPLE_CAR_SHUFFLE:
        case TASK_SIMPLE_CAR_ALIGN:
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
            return m_Car;
        }
    }
    return nullptr;
}
