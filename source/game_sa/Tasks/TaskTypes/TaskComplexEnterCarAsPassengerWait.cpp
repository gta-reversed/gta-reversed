#include "StdInc.h"
#include "TaskComplexEnterCarAsPassengerWait.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexEnterCarAsPassengerTimed.h"
#include "TaskSimpleWaitUntilPedIsInCar.h"
#include "TaskComplexSequence.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "CarEnterExit.h"
#include "PedGroups.h"
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
CTask* CTaskComplexEnterCarAsPassengerWait::CreateSubTask(int32 taskType, CPed* ped) {
    switch ((eTaskType)taskType) {
    case TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR:
        return new CTaskSimpleWaitUntilPedIsInCar{ m_WaitForPed };
    case TASK_COMPLEX_SEQUENCE: {
        auto* seq = new CTaskComplexSequence{};
        seq->AddTask(new CTaskComplexTurnToFaceEntityOrCoord{ m_Car, 0.5f, 0.2f });
        seq->AddTask(new CTaskSimpleStandStill{ 1000, false, false, 8.0f });
        return seq;
    }
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED: {
        const int32 targetSeat = m_bForceFrontSeat ? 0 : CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(m_Car, 0);
        auto* timed = new CTaskComplexEnterCarAsPassengerTimed{ m_Car, (uint32)targetSeat, 0xFFFFFFFF, true };
        timed->SetMoveState(m_MoveState);
        return timed;
    }
    case TASK_SIMPLE_CAR_DRIVE_TIMED:
        return new CTaskSimpleCarDriveTimed{ m_Car, 0 };
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        CVector targetPos{};
        if (CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_Car, targetPos)) {
            return new CTaskComplexGoToPointAndStandStill{ m_MoveState, targetPos, 0.5f, 2.0f, false, false };
        }
        const auto radius = CModelInfo::GetModelInfo(m_Car->m_nModelIndex)->GetColModel()->GetBoundingBox().GetSize().Magnitude() / 2.0f + 0.35f;
        return new CTaskComplexGoToPointAndStandStill{ m_MoveState, ped->GetPosition(), radius, 2.0f, false, false };
    }
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
    case TASK_SIMPLE_CAR_DRIVE_TIMED:
        return nullptr;
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        if (m_WaitForPed) {
            return CreateSubTask(TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR, ped);
        }
        break;
    case TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR:
        if (m_Car) {
            return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED, ped);
        }
        break;
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED:
        if (!ped->bInVehicle && ++m_EnterCarFails < 16 && m_Car) {
            return CreateSubTask(TASK_COMPLEX_SEQUENCE, ped);
        }
        break;
    case TASK_COMPLEX_SEQUENCE:
        break;
    default:
        return nullptr;
    }
    return CreateSubTask(TASK_FINISHED, ped);
}

// 0x643F00
CTask* CTaskComplexEnterCarAsPassengerWait::CreateFirstSubTask(CPed* ped) {
    if (ped->bInVehicle && ped->m_pVehicle && ped->m_pVehicle == m_Car && m_Car->IsPassenger(ped)) {
        return CreateSubTask(TASK_SIMPLE_CAR_DRIVE_TIMED, ped);
    }
    if (!m_WaitForPed) {
        if (m_Car) {
            return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED, ped);
        }
    } else if (m_Car) {
        return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL, ped);
    }
    return CreateSubTask(TASK_FINISHED, ped);
}

// 0x640730
CTask* CTaskComplexEnterCarAsPassengerWait::ControlSubTask(CPed* ped) {
    if (!m_bPlayedSample) {
        if (!m_WaitForPed) {
            if (const auto* group = CPedGroups::GetPedsGroup(ped)) {
                if (const auto* leader = group->GetMembership().GetLeader()) {
                    if (leader->IsPlayer()) {
                        if (ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER_TIMED)) {
                            ped->Say(CTX_GLOBAL_CAR_WAIT_FOR_ME);
                            m_bPlayedSample = true;
                        }
                    }
                }
            }
        } else if (ped->IsPlayer()) {
            if (ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER_TIMED)) {
                ped->Say(CTX_GLOBAL_CAR_GET_IN);
                m_bPlayedSample = true;
            }
        }
    }
    if (m_Car) {
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
            CVector closestPt{};
            const bool found = CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_Car, closestPt);
            const CVector carPos = m_Car->GetPosition();
            CVector delta = found ? closestPt - carPos : CVector{};
            if (!found) {
                const float radius = CModelInfo::GetModelInfo(m_Car->m_nModelIndex)->GetColModel()->GetBoundingBox().GetSize().Magnitude() / 2.0f + 0.35f;
                reinterpret_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask)->GoToPoint(ped->GetPosition(), radius, 2.0f);
            } else if (delta.SquaredMagnitude() > sq(0.5f)) {
                reinterpret_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask)->GoToPoint(closestPt, 0.5f, 2.0f);
                return m_pSubTask;
            }
        }
    }
    return m_pSubTask;
}
