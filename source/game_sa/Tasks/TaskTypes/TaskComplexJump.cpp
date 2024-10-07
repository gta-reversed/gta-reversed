#include "StdInc.h"

#include "TaskComplexJump.h"

#include "TaskSimpleJump.h"
#include "TaskSimpleHitHead.h"
#include "TaskComplexInAirAndLand.h"
#include "TaskSimpleClimb.h"

void CTaskComplexJump::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexJump, 0x870570, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x67A030);
    RH_ScopedInstall(CreateSubTask, 0x67D980);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x67FD10);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x67FC00);
    RH_ScopedVMTInstall(Clone, 0x67C5A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x67A070);
}

CTaskComplexJump* CTaskComplexJump::Constructor(eComplexJumpType jumpType) {
    this->CTaskComplexJump::CTaskComplexJump(jumpType);
    return this;
}

// 0x67A030
CTaskComplexJump::CTaskComplexJump(eComplexJumpType type) : CTaskComplex() {
    m_nType = type;
    m_bHighJump = false;
}

// 0x67C5A0
CTask* CTaskComplexJump::Clone() const {
    auto newTask = new CTaskComplexJump(m_nType);
    newTask->m_bHighJump = this->m_bHighJump;
    return newTask;
}

// 0x67FD10
CTask* CTaskComplexJump::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_JUMP, ped);
}

// 0x67FC00
CTask* CTaskComplexJump::CreateNextSubTask(CPed* ped) {
    eTaskType subTaskType = m_pSubTask->GetTaskType();

    if (subTaskType == TASK_SIMPLE_CLIMB)
        return CreateSubTask(ped->bIsInTheAir ? TASK_COMPLEX_IN_AIR_AND_LAND : TASK_FINISHED, ped);

    if (subTaskType == TASK_SIMPLE_HIT_HEAD || subTaskType == TASK_COMPLEX_IN_AIR_AND_LAND) {
        ped->bIsLanding = false;
        return CreateSubTask(TASK_FINISHED, ped);
    }

    if (subTaskType == TASK_SIMPLE_JUMP) {
        auto jumpTask = reinterpret_cast<CTaskSimpleJump*>(m_pSubTask);

        if (!jumpTask->m_bLaunchAnimStarted) {
            ped->bIsLanding = false;
            return CreateSubTask(TASK_FINISHED, ped);
        } else if (jumpTask->m_bIsJumpBlocked) {
            ped->bIsLanding = true;
            return CreateSubTask(TASK_SIMPLE_HIT_HEAD, ped);
        } else if (jumpTask->m_pClimbEntity && m_nType != -1) {
            ped->bIsInTheAir = true;
            return CreateSubTask(TASK_SIMPLE_CLIMB, ped);
        } else {
            ped->bIsInTheAir = true;
            return CreateSubTask(TASK_COMPLEX_IN_AIR_AND_LAND, ped);
        }
    }

    return nullptr;
}

// 0x67A070
bool CTaskComplexJump::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_URGENT && event) {
        if (event->GetEventType() == EVENT_DAMAGE) {
            const auto pDamageEvent = static_cast<const CEventDamage*>(event);
            if (pDamageEvent->m_weaponType == WEAPON_FALL && pDamageEvent->m_damageResponse.m_bHealthZero && pDamageEvent->m_bAddToEventGroup) {
                ped->bIsInTheAir = false;
                ped->bIsLanding = false;
                return true;
            }
        } else if (event->GetEventType() == EVENT_DEATH) {
            ped->bIsInTheAir = false;
            ped->bIsLanding = false;
            return true;
        }
    }

    if (m_pSubTask->MakeAbortable(ped, priority, event)) {
        ped->bIsInTheAir = false;
        ped->bIsLanding = false;
        return true;
    }

    return false;
}

// 0x67D980
CTask* CTaskComplexJump::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_HIT_HEAD:
        return new CTaskSimpleHitHead();
    case TASK_FINISHED:
        ped->bIsInTheAir = false;
        return nullptr;
    case TASK_SIMPLE_JUMP: {
        auto task = new CTaskSimpleJump(m_nType == COMPLEX_JUMP_TYPE_CLIMB);
        if (m_bHighJump || CPedGroups::IsInPlayersGroup(ped))
            task->m_bHighJump = true;
        return task;
    }
    case TASK_SIMPLE_CLIMB:
        if (m_pSubTask && m_pSubTask->GetTaskType() == TASK_SIMPLE_JUMP) {
            auto jumpTask = reinterpret_cast<CTaskSimpleJump*>(m_pSubTask);
            return new CTaskSimpleClimb(
                jumpTask->m_pClimbEntity,
                jumpTask->m_vecClimbPos,
                jumpTask->m_fClimbAngle,
                jumpTask->m_nClimbSurfaceType,
                jumpTask->m_vecClimbPos.z - ped->GetPosition().z < CTaskSimpleClimb::ms_fMinForStretchGrab ? CLIMB_PULLUP : CLIMB_GRAB,
                m_nType == COMPLEX_JUMP_TYPE_CLIMB
            );
        }
        else
        {
            return new CTaskComplexInAirAndLand(true, false);
        }
    case TASK_COMPLEX_IN_AIR_AND_LAND: {
        auto newTask = new CTaskComplexInAirAndLand(true, false);

        if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CLIMB && reinterpret_cast<CTaskSimpleClimb*>(m_pSubTask)->m_bInvalidClimb)
            newTask->m_bInvalidClimb = true;

        return newTask;
    }
    default:
        return nullptr;
    }
}
