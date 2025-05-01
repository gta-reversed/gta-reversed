#include "StdInc.h"

#include "TaskComplexGoToBoatSteeringWheel.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexGoToPointAndStandStill.h"

void CTaskComplexGoToBoatSteeringWheel::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToBoatSteeringWheel, 0x86f018, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x649090);
    RH_ScopedInstall(Destructor, 0x649100);

    RH_ScopedVMTInstall(Clone, 0x64A350);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x64E350);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x64E390);
    RH_ScopedVMTInstall(ControlSubTask, 0x64E3B0);
}

// 0x649090
CTaskComplexGoToBoatSteeringWheel::CTaskComplexGoToBoatSteeringWheel(CVehicle* vehicle) :
    m_Vehicle{vehicle}
{
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x649100
CTaskComplexGoToBoatSteeringWheel::~CTaskComplexGoToBoatSteeringWheel() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x64E350
CTask* CTaskComplexGoToBoatSteeringWheel::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
        return CreateSubTask(TASK_FINISHED);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        m_bAchievedPoint = true;
        return CreateSubTask(TASK_FINISHED);
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x649160
void CTaskComplexGoToBoatSteeringWheel::ComputeTargetPos() {
    m_TargetPos = m_Vehicle->GetDummyPosition(m_Vehicle->IsBoat() ? DUMMY_LIGHT_FRONT_MAIN : DUMMY_SEAT_FRONT);
}

// 0x64CDA0
CTask* CTaskComplexGoToBoatSteeringWheel::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill(-1, false, false, 8.0f);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        ComputeTargetPos();
        return new CTaskComplexGoToPointAndStandStill(PEDMOVE_WALK, m_TargetPos, 0.5f, 2.0f, false, false);
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x64E390
CTask* CTaskComplexGoToBoatSteeringWheel::CreateFirstSubTask(CPed* ped) {
    m_FirstSubTaskStartTime = CTimer::GetTimeInMS();
    return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
}

// 0x64E3B0
CTask* CTaskComplexGoToBoatSteeringWheel::ControlSubTask(CPed* ped) {
    if (ped->IsPlayer()) {
        if (m_Vehicle) {
            if (CCarEnterExit::IsPlayerToQuitCarEnter(ped, m_Vehicle, m_FirstSubTaskStartTime, m_pSubTask)) {
                return CreateSubTask(TASK_SIMPLE_STAND_STILL);
            }
        }
    }

    // Possibly adjust goto point (if it has changed significantly)
    if (const auto subSubTask = m_pSubTask->GetSubTask()) {
        if (subSubTask->GetTaskType() != TASK_SIMPLE_STAND_STILL) {
            assert(m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
            const auto subTaskGoTo = static_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask);
            ComputeTargetPos();
            if ((m_TargetPos - subTaskGoTo->m_vecTargetPoint).SquaredMagnitude() > sq(0.2f)) {
                if (subTaskGoTo->m_fMoveStateRadius != 2.f || m_TargetPos != subTaskGoTo->m_vecTargetPoint) {
                    subTaskGoTo->GoToPoint(m_TargetPos, 0.5f, 2.f);
                }
            }
                
        }
    }

    return m_pSubTask;
}
