#include "StdInc.h"
#include "EventSexyPed.h"
#include <Ragdoll/IKChainManager.h>

// 0x4AEDF0
CEventSexyPed::CEventSexyPed(CPed* ped) :

    m_SexyPed{ ped } {
}

// ??
CEventSexyPed::CEventSexyPed(CPed* sexyPed, eTaskType taskType) :
    CEventSexyPed{ sexyPed } {
    m_TaskId = taskType;
}

// 0x4AEF00
bool CEventSexyPed::AffectsPed(CPed* ped) {
    if (!ped->IsAlive()) {
        return false;
    }

    if (!m_SexyPed) {
        return false;
    }

    if (!m_SexyPed->IsAlive()) {
        return false;
    }

    if (g_ikChainMan.IsLooking(ped) && g_ikChainMan.GetLookAtEntity(ped) == m_SexyPed) {
        return false;
    }

    if (ped->GetTaskManager().IsFirstFoundTaskMatching<TASK_COMPLEX_PARTNER_DEAL, TASK_COMPLEX_BE_IN_COUPLE, TASK_COMPLEX_PARTNER_GREET>(m_SexyPed->GetTaskManager())) {
        return false;
    }

    return true;
}

void CEventSexyPed::InjectHooks() {
    RH_ScopedVirtualClass(CEventSexyPed, 0x85b0b0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4AEDF0, "1", {}, CPed*);
    RH_ScopedDestructorInstall(0x4AEEA0);

    RH_ScopedVMTInstall(GetEventType, 0x4AEE60);
    RH_ScopedVMTInstall(GetEventPriority, 0x4AEE90);
    RH_ScopedVMTInstall(GetLifeTime, 0x4AEE70);
    RH_ScopedVMTInstall(AffectsPed, 0x4AEF00);
    RH_ScopedVMTInstall(GetSourceEntity, 0x4AEE80);
    RH_ScopedVMTInstall(CloneEditable, 0x4B7280);
}
