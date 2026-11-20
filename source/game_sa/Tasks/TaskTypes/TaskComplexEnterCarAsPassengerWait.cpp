#include "StdInc.h"
#include "TaskComplexEnterCarAsPassengerWait.h"
#include "TaskComplexEnterCarAsPassengerTimed.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleWaitUntilPedIsInCar.h"
#include "PedGeometryAnalyser.h"

void CTaskComplexEnterCarAsPassengerWait::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCarAsPassengerWait, 0x86E778, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63B320);
    RH_ScopedInstall(Destructor, 0x63B3C0);

    RH_ScopedInstall(CreateSubTask, 0x6408D0);
    RH_ScopedVMTInstall(Clone, 0x63D850);
    RH_ScopedVMTInstall(GetTaskType, 0x63B3B0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x643E10);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643F00);
    RH_ScopedVMTInstall(ControlSubTask, 0x640730);
}

// 0x63B320
CTaskComplexEnterCarAsPassengerWait::CTaskComplexEnterCarAsPassengerWait(CVehicle* target, CPed* waitFor, bool forceFrontSeat, eMoveState ms) :
    m_Car{target},
    m_WaitForPed{waitFor},
    m_bForceFrontSeat{forceFrontSeat},
    m_MoveState{ms}
{
    CEntity::SafeRegisterRef(m_Car);
    CEntity::SafeRegisterRef(m_WaitForPed);
}
    
// NOTSA (for 0x63D850)
CTaskComplexEnterCarAsPassengerWait::CTaskComplexEnterCarAsPassengerWait(const CTaskComplexEnterCarAsPassengerWait& o) :
    CTaskComplexEnterCarAsPassengerWait{
        o.m_Car,
        o.m_WaitForPed,
        o.m_bForceFrontSeat,
        o.m_MoveState
    }
{
}

// 0x63B3C0
CTaskComplexEnterCarAsPassengerWait::~CTaskComplexEnterCarAsPassengerWait() {
    CEntity::SafeCleanUpRef(m_Car);
    CEntity::SafeCleanUpRef(m_WaitForPed);
}

// 0x6408D0
CTask* CTaskComplexEnterCarAsPassengerWait::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR:
        return new CTaskSimpleWaitUntilPedIsInCar{ m_WaitForPed };
    case TASK_COMPLEX_SEQUENCE: // Failed to get in, look at the car for a bit, then try again
        return new CTaskComplexSequence{
            new CTaskComplexTurnToFaceEntityOrCoord{ m_Car },
            new CTaskSimpleStandStill{ 1'000, false, false, 8.f }
        };
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED: {
        const auto seat = m_bForceFrontSeat
            ? CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(m_Car, 0)
            : 0;
        const auto task = new CTaskComplexEnterCarAsPassengerTimed{ m_Car, (uint32)seat, (uint32)-1, true };
        task->SetMoveState(m_MoveState);
        return task;
    }
    case TASK_SIMPLE_CAR_DRIVE_TIMED: // Already in the car
        return new CTaskSimpleCarDriveTimed{ m_Car, 0 };
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // Go next to the car and wait for `m_WaitForPed` there
        if (CVector pt{}; CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_Car, pt)) {
            return new CTaskComplexGoToPointAndStandStill{ m_MoveState, pt, 1.5f, 2.f, false, false };
        }
        return new CTaskComplexGoToPointAndStandStill{ m_MoveState, m_Car->GetPosition(), m_Car->GetColModel()->GetBoundingSphere().m_fRadius + 1.5f, 2.f, false, false };
    }
    case TASK_FINISHED:
    default:
        return nullptr;
    }
}

// 0x643E10
CTask* CTaskComplexEnterCarAsPassengerWait::CreateNextSubTask(CPed* ped) {
    if (!m_Car) {
        return CreateSubTask(TASK_FINISHED, ped);
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SEQUENCE:
    case TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR: // Waited enough, try getting in
        return CreateSubTask(m_Car ? TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED : TASK_FINISHED, ped);
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED: { // Try again if we didn't manage to get in
        if (!ped->bInVehicle && ++m_EnterCarFails < MAX_ENTER_CAR_FAILS && m_Car) {
            return CreateSubTask(TASK_COMPLEX_SEQUENCE, ped);
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return CreateSubTask(m_WaitForPed ? TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR : TASK_FINISHED, ped);
    case TASK_SIMPLE_CAR_DRIVE_TIMED:
        return CreateSubTask(TASK_FINISHED, ped);
    default:
        return nullptr;
    }
}

// 0x643F00
CTask* CTaskComplexEnterCarAsPassengerWait::CreateFirstSubTask(CPed* ped) {
    if (ped->IsInVehicle() && ped->m_pVehicle == m_Car && m_Car->IsPassenger(ped)) {
        return CreateSubTask(TASK_SIMPLE_CAR_DRIVE_TIMED, ped);
    }
    if (!m_Car) {
        return CreateSubTask(TASK_FINISHED, ped);
    }
    return CreateSubTask(m_WaitForPed ? TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED, ped);
}

// 0x640730
CTask* CTaskComplexEnterCarAsPassengerWait::ControlSubTask(CPed* ped) {
    // 0x640736 - Say something while getting in
    if (!m_bPlayedSample) {
        const auto IsGettingIn = [ped] {
            return ped->GetTaskManager().FindActiveTaskByType(TASK_SIMPLE_CAR_GET_IN) != nullptr;
        };
        const auto IsLeaderThePlayer = [ped] {
            const auto* const group  = CPedGroups::GetPedsGroup(ped);
            const auto* const leader = group ? group->GetMembership().GetLeader() : nullptr;
            return leader && leader->IsPlayer();
        };
        const auto ctx = [&] {
            if (m_WaitForPed) {
                return m_WaitForPed->IsPlayer() && IsGettingIn()
                    ? CTX_GLOBAL_CAR_GET_IN
                    : CTX_GLOBAL_NO_SPEECH;
            }
            return IsLeaderThePlayer() && IsGettingIn()
                ? CTX_GLOBAL_CAR_WAIT_FOR_ME
                : CTX_GLOBAL_NO_SPEECH;
        }();
        if (ctx != CTX_GLOBAL_NO_SPEECH) {
            m_bPlayedSample = true;
            ped->Say(ctx);
        }
    }

    // 0x6407E1 - Keep the go-to point up-to-date with the car's position
    if (m_Car && m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
        const auto task = notsa::cast<CTaskComplexGoToPointAndStandStill>(m_pSubTask);

        // NOTE: If no surface point was found `pt` is compared uninitialized originally
        CVector    pt{};
        const auto found = CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_Car, pt);
        if ((pt - task->m_vecTargetPoint).SquaredMagnitude() > 0.025f) {
            if (found) {
                task->GoToPoint(pt, 1.5f, 2.f, false);
            } else {
                task->GoToPoint(m_Car->GetPosition(), m_Car->GetColModel()->GetBoundingSphere().m_fRadius + 1.5f, 2.f, false);
            }
        }
    }

    return m_pSubTask;
}
