#include "StdInc.h"
#include "EventSexyPed.h"


CEventSexyPed::CEventSexyPed(CPed* ped) : CEventEditableResponse() {
    m_SexyPed = ped;
    CEntity::SafeRegisterRef(m_SexyPed);
}

CEventSexyPed::CEventSexyPed(CPed* sexyPed, eTaskType taskType) :
    CEventSexyPed{sexyPed}
{
    m_taskId = (int16)taskType;
}

CEventSexyPed::~CEventSexyPed() {
    CEntity::SafeCleanUpRef(m_SexyPed);
}

bool CEventSexyPed::AffectsPed(CPed* ped) {
    if (!ped->IsAlive())
        return false;

    if (!m_SexyPed)
        return false;

    if (!m_SexyPed->IsAlive())
        return false;

    if (g_ikChainMan.IsLooking(ped) && g_ikChainMan.GetLookAtEntity(ped) == m_SexyPed) {
        return false;
    }

    if (ped->GetTaskManager().IsFirstFoundTaskMatching<TASK_COMPLEX_PARTNER_DEAL, TASK_COMPLEX_BE_IN_COUPLE, TASK_COMPLEX_PARTNER_GREET>(m_SexyPed->GetTaskManager())) {
        return false;
    }

    return true;
}

