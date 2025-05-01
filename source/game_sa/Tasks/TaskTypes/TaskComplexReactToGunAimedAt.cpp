#include "StdInc.h"
#include "TaskComplexReactToGunAimedAt.h"
#include "PedStats.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleHandsUp.h"
#include "TaskSimplePause.h"
#include "TaskSimpleCower.h"
#include "TaskComplexSmartFleeEntity.h"
#include "TaskComplexWanderStandard.h"

void CTaskComplexReactToGunAimedAt::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexReactToGunAimedAt, 0x870d78, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6971B0);
    RH_ScopedInstall(Destructor, 0x697230);

    RH_ScopedInstall(CreateSubTask, 0x697B70);

    RH_ScopedVMTInstall(Clone, 0x6976E0);
    RH_ScopedVMTInstall(GetTaskType, 0x697220);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6983C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x698530);
    RH_ScopedVMTInstall(ControlSubTask, 0x698550);
}

// 0x6971B0
CTaskComplexReactToGunAimedAt::CTaskComplexReactToGunAimedAt(CPed* aimedBy) :
    m_aimedBy{aimedBy}
{
    CEntity::SafeRegisterRef(m_aimedBy);
}


CTaskComplexReactToGunAimedAt::CTaskComplexReactToGunAimedAt(const CTaskComplexReactToGunAimedAt& o) :
    CTaskComplexReactToGunAimedAt{ o.m_aimedBy }
{
}

// 0x697230
CTaskComplexReactToGunAimedAt::~CTaskComplexReactToGunAimedAt() {
    CEntity::SafeCleanUpRef(m_aimedBy);
}

// 0x697B70
CTask* CTaskComplexReactToGunAimedAt::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading{ (m_aimedBy->GetPosition() - ped->GetPosition()).Heading() };
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false };
    case TASK_SIMPLE_HANDS_UP:
        return new CTaskSimpleHandsUp{ (uint32)CGeneral::GetRandomNumberInRange(3000, 5000) };
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause{
            ped->bInVehicle
                ? CGeneral::GetRandomNumberInRange(1000, 2000)
                : 0
        };
    case TASK_SIMPLE_COWER:
        return new CTaskSimpleCower{};
    case TASK_COMPLEX_SMART_FLEE_ENTITY:
        return new CTaskComplexSmartFleeEntity{ m_aimedBy, false, 60.f, 1000000, 1000, 1.f };
    case TASK_COMPLEX_WANDER: {
        m_wanderBeginTimer.Start(10'000);
        return new CTaskComplexWanderStandard{
            PEDMOVE_WALK,
            (uint8)CVector2D{ped->GetPosition() - m_aimedBy->GetPosition()}.NodeHeading()
        };
    }
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x6983C0
CTask* CTaskComplexReactToGunAimedAt::CreateNextSubTask(CPed* ped) {
    return CreateSubTask([ped, this] {
        // Cases ordered in the same way the tasks are created (eg.: Pause is created first)
        const auto type = m_pSubTask->GetTaskType();
        switch (type) {
        case TASK_SIMPLE_PAUSE:
            return ped->IsInVehicle()
                ? TASK_COMPLEX_LEAVE_CAR
                : TASK_SIMPLE_ACHIEVE_HEADING;
        case TASK_COMPLEX_LEAVE_CAR:
            return TASK_SIMPLE_ACHIEVE_HEADING;
        case TASK_SIMPLE_ACHIEVE_HEADING: {
            if (ped->m_pStats->m_flags & 0x80) {
                return TASK_SIMPLE_COWER; // => TASK_COMPLEX_SMART_FLEE_ENTITY
            } else {
                ped->Say(CTX_GLOBAL_GUN_COOL);
                return TASK_SIMPLE_HANDS_UP; // => TASK_COMPLEX_WANDER
            }
        }
        case TASK_SIMPLE_COWER:
            return TASK_COMPLEX_SMART_FLEE_ENTITY;
        case TASK_SIMPLE_HANDS_UP:
            return TASK_COMPLEX_WANDER;
        case TASK_COMPLEX_SMART_FLEE_ENTITY:
        case TASK_COMPLEX_WANDER:
            return TASK_FINISHED;
        default:
            NOTSA_UNREACHABLE();
        }
    }(), ped);
}

// 0x698530
CTask* CTaskComplexReactToGunAimedAt::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
}

// 0x698550
CTask* CTaskComplexReactToGunAimedAt::ControlSubTask(CPed* ped) {
    if (!m_aimedBy) {
        return CreateSubTask(TASK_FINISHED, ped);
    }
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_WANDER) {
        if (m_wanderBeginTimer.IsOutOfTime()) {
            if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) {
                return nullptr;
            }
        }
    }
    return m_pSubTask;
}
