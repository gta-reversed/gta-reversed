#include "StdInc.h"
#include "TaskComplexScreamInCarThenLeave.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexLeaveCarAndFlee.h"
#include "TaskComplexWander.h"

void CTaskComplexScreamInCarThenLeave::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexScreamInCarThenLeave, 0x86e8d8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63C240);
    RH_ScopedInstall(Destructor, 0x63C2C0);

    RH_ScopedInstall(CreateSubTask, 0x642510);

    RH_ScopedVMTInstall(Clone, 0x63DBB0);
    RH_ScopedVMTInstall(GetTaskType, 0x63C2B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x63C320);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6442D0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x644320);
    RH_ScopedVMTInstall(ControlSubTask, 0x644360);
}

// 0x63C240
CTaskComplexScreamInCarThenLeave::CTaskComplexScreamInCarThenLeave(CVehicle* veh, eTargetDoor targetDoor) :
    m_veh{veh},
    m_door{targetDoor}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexScreamInCarThenLeave::CTaskComplexScreamInCarThenLeave(const CTaskComplexScreamInCarThenLeave& o) :
    CTaskComplexScreamInCarThenLeave{ o.m_veh, o.m_door }
{
}

// 0x63C2C0
CTaskComplexScreamInCarThenLeave::~CTaskComplexScreamInCarThenLeave() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x642510
CTask* CTaskComplexScreamInCarThenLeave::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_LEAVE_CAR_AND_FLEE:
        return new CTaskComplexLeaveCarAndFlee{
            m_veh,
            m_veh->GetPosition(),
            TARGET_DOOR_FRONT_LEFT,
            0,
            !CTheScripts::IsPlayerOnAMission() && !CGeneral::DoCoinFlip()
        };
    case TASK_SIMPLE_CAR_DRIVE: {
        m_timer.Start(5000);
        return new CTaskSimpleCarDrive{ m_veh };
    case TASK_FINISHED: {
        ped->GetTaskManager().SetTask(CTaskComplexWander::GetWanderTaskByPedType(ped), TASK_PRIMARY_DEFAULT);
        return nullptr;
    }
    default:
        NOTSA_UNREACHABLE();
    }
    }
}

// 0x63C320
bool CTaskComplexScreamInCarThenLeave::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x6442D0
CTask* CTaskComplexScreamInCarThenLeave::CreateNextSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_LEAVE_CAR_AND_FLEE:
            return TASK_FINISHED;
        case TASK_SIMPLE_CAR_DRIVE:
            return TASK_COMPLEX_LEAVE_CAR_AND_FLEE;
        default:
            NOTSA_UNREACHABLE();
        }
    }(), ped);
}

// 0x644320
CTask* CTaskComplexScreamInCarThenLeave::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(
        CTheScripts::IsPlayerOnAMission()
            ? TASK_COMPLEX_LEAVE_CAR_AND_FLEE
            : TASK_SIMPLE_CAR_DRIVE,
        ped
    );
}

// 0x644360
CTask* CTaskComplexScreamInCarThenLeave::ControlSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_DRIVE) { // Moved this here
        if (m_veh && m_timer.IsOutOfTime()) {
            if (!m_veh->m_pDriver || m_veh->CanPedStepOutCar() || m_veh->CanPedJumpOutCar(ped)) {
                return CreateSubTask(TASK_COMPLEX_LEAVE_CAR_AND_FLEE, ped);
            }
            ped->Say(CTX_GLOBAL_PAIN_PANIC);
        } else {
            ped->Say(CTX_GLOBAL_TRAPPED);
        }
    }
    return m_pSubTask;
}
