#include "StdInc.h"
#include "TaskComplexLeaveCarAndFlee.h"
#include "TaskComplexSmartFleePoint.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskComplexWander.h"

void CTaskComplexLeaveCarAndFlee::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCarAndFlee, 0x86e8ac, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63BF90);
    RH_ScopedInstall(Destructor, 0x63C030);

    RH_ScopedInstall(CreateSubTask, 0x63C0A0);

    RH_ScopedVMTInstall(Clone, 0x63DB40);
    RH_ScopedVMTInstall(GetTaskType, 0x63C020);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x642390);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6424C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x63C090);
}

// 0x63BF90
CTaskComplexLeaveCarAndFlee::CTaskComplexLeaveCarAndFlee(CVehicle* targetVeh, CVector const& fleePoint, eTargetDoor targetDoor, int32 fleeDelayTime, bool waitForCarToSlowDown) :
    m_veh{targetVeh},
    m_door{targetDoor},
    m_fleeDelayMs{fleeDelayTime},
    m_fleePoint{fleePoint},
    m_waitForCarToSlowDown{waitForCarToSlowDown}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexLeaveCarAndFlee::CTaskComplexLeaveCarAndFlee(const CTaskComplexLeaveCarAndFlee& o) :
    CTaskComplexLeaveCarAndFlee{ o.m_veh, o.m_fleePoint, o.m_door, o.m_fleeDelayMs, false }
{
}

// 0x63C030
CTaskComplexLeaveCarAndFlee::~CTaskComplexLeaveCarAndFlee() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x63C0A0
CTask* CTaskComplexLeaveCarAndFlee::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_SMART_FLEE_POINT: // 0x63C177
        return new CTaskComplexSmartFleePoint{
            m_waitForCarToSlowDown // Inverted
                ? m_veh->GetPosition()
                : m_fleePoint,
            false,
            60.f,
            1000000
        };
    case TASK_COMPLEX_LEAVE_CAR: // 0x63C12E
        return new CTaskComplexLeaveCar{
            m_veh,
            (int32)(m_door),
            m_fleeDelayMs,
            m_waitForCarToSlowDown,
            true
        };
    case TASK_SIMPLE_GO_TO_POINT: // 0x63C0E6
        return new CTaskSimpleGoToPoint{ PEDMOVE_RUN, m_fleePoint };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x642390
CTask* CTaskComplexLeaveCarAndFlee::CreateNextSubTask(CPed* ped) {
    return CreateSubTask([ped, this] {
        switch (m_pSubTask->GetTaskType()) { // Tasks in same order as they're created
        case TASK_COMPLEX_LEAVE_CAR: {
            if (!m_veh || !m_veh->IsBoat()) {
                return TASK_COMPLEX_SMART_FLEE_POINT;
            }
            m_fleePoint = m_veh->GetPosition() - m_veh->GetForward() * m_veh->GetModelInfo()->GetColModel()->GetBoundRadius() * 2.f;
            return TASK_SIMPLE_GO_TO_POINT;
        }
        case TASK_SIMPLE_GO_TO_POINT:
            return TASK_COMPLEX_SMART_FLEE_POINT;
        case TASK_COMPLEX_SMART_FLEE_POINT: {
            ped->GetTaskManager().SetTask(CTaskComplexWander::GetWanderTaskByPedType(ped), TASK_PRIMARY_DEFAULT);
            return TASK_FINISHED;
        }
        default:
            NOTSA_UNREACHABLE();
        }
    }());
}

// 0x6424C0
CTask* CTaskComplexLeaveCarAndFlee::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(
        ped->bInVehicle
            ? TASK_COMPLEX_LEAVE_CAR
            : TASK_COMPLEX_SMART_FLEE_POINT
    );
}
