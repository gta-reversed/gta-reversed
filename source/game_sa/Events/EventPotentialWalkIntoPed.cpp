#include "StdInc.h"

#include "EventPotentialWalkIntoPed.h"
#include "TaskComplexFollowPedFootsteps.h"

void CEventPotentialWalkIntoPed::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPotentialWalkIntoPed, 0x85AF60, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AE6E0);
    RH_ScopedVMTInstall(AffectsPed, 0x4AE800);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4AE950);
}

// 0x4AE6E0
CEventPotentialWalkIntoPed::CEventPotentialWalkIntoPed(CPed* ped, const CVector& targetPoint, eMoveState moveState) :
    m_targetPoint{targetPoint},
    m_ped{ped},
    m_moveState{moveState}
{
    CEntity::SafeRegisterRef(m_ped);
}

CEventPotentialWalkIntoPed::~CEventPotentialWalkIntoPed() {
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4AE800
bool CEventPotentialWalkIntoPed::AffectsPed(CPed* ped) {
    if (!ped->IsAlive() || !m_ped || m_moveState == PEDMOVE_STILL) {
        return false;
    }

    if (ped->GetTaskManager().IsFirstFoundTaskMatching<TASK_COMPLEX_PARTNER_DEAL, TASK_COMPLEX_BE_IN_COUPLE, TASK_COMPLEX_PARTNER_GREET>(m_ped->GetTaskManager())) {
        return false;
    }

    if (const auto task = ped->GetTaskManager().Find<CTaskComplexFollowPedFootsteps>()) {
        if (task->m_targetPed == m_ped) {
            return false;
        }
    }

    if (const auto task = ped->GetTaskManager().Find<CTaskComplexKillPedOnFoot>()) {
        if (task->m_target == m_ped) {
            return false;
        }
    }

    if (const auto simplest = ped->GetTaskManager().GetSimplestActiveTask()) {
        if (CTask::IsGoToTask(simplest)) {
            return true;
        }
    }

    return false;
}

// 0x4AE950
bool CEventPotentialWalkIntoPed::TakesPriorityOver(const CEvent& refEvent) {
    return CEventHandler::IsTemporaryEvent(refEvent) ? true : CEvent::TakesPriorityOver(refEvent);
}