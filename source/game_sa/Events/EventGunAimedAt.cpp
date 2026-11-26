#include "StdInc.h"

#include "EventGunAimedAt.h"

#include "PedType.h"

// 0x4B0700
CEventGunAimedAt::CEventGunAimedAt(CPed* ped) :
    CEventEditableResponse() {
    m_AimedBy = ped;
    CEntity::SafeRegisterRef(m_AimedBy);
}

// 0x4B07B0
CEventGunAimedAt::~CEventGunAimedAt() {
    CEntity::SafeCleanUpRef(m_AimedBy);
}

// 0x4B4EE0
bool CEventGunAimedAt::AffectsPed(CPed* ped) {
    if (ped->IsPlayer()) {
        return false;
    }

    CTask* activeTask = ped->GetTaskManager().GetActiveTask();
    if (activeTask && activeTask->GetTaskType() == TASK_COMPLEX_REACT_TO_GUN_AIMED_AT) {
        return false;
    }

    if (m_AimedBy && ped->GetIntelligence()->IsInSeeingRange(m_AimedBy->GetPosition()) && ped->IsAlive()) {
        if (CPedGroups::AreInSameGroup(ped, m_AimedBy)) {
            return false;
        }
        if (!ped->IsInVehicleThatHasADriver()) {
            if (ped->m_nPedType == PED_TYPE_COP) {
                CCrime::ReportCrime(eCrimeType::CRIME_AIM_GUN, ped, FindPlayerPed());
            }
            return true;
        }
    }
    return false;
}

// 0x4B09E0
void CEventGunAimedAt::ReportCriminalEvent(CPed* ped) {
    if (IsCriminalEvent()) {
        CPedType::PoliceDontCareAboutCrimesAgainstPedType(ped->m_nPedType);
    }
}

// 0x4B0810
bool CEventGunAimedAt::TakesPriorityOver(const CEvent& refEvent) {
    if (m_AimedBy && m_AimedBy->IsPlayer()) {
        if (refEvent.GetSourceEntity() == m_AimedBy) {
            return GetEventPriority() >= refEvent.GetEventPriority();
        }

        switch (refEvent.GetEventType()) {
        case EVENT_DAMAGE:
        case EVENT_ACQUAINTANCE_PED_HATE:
        case EVENT_ACQUAINTANCE_PED_RESPECT:
            return true;
        }
        return CEvent::TakesPriorityOver(refEvent);
    }
    return GetEventPriority() >= refEvent.GetEventPriority();
}

// 0x4B7630
CEventEditableResponse* CEventGunAimedAt::CloneEditable() const noexcept {
    return new CEventGunAimedAt(m_AimedBy);
}

void CEventGunAimedAt::InjectHooks() {
    RH_ScopedVirtualClass(CEventGunAimedAt, 0x85b330, 17);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4B0700, "1", {}, CPed*);
    RH_ScopedDestructorInstall(0x4B07B0);

    RH_ScopedVMTInstall(GetEventType, 0x4B0770);
    RH_ScopedVMTInstall(GetEventPriority, 0x4B07A0);
    RH_ScopedVMTInstall(GetLifeTime, 0x4B0780);
    RH_ScopedVMTInstall(AffectsPed, 0x4B4EE0);
    RH_ScopedVMTInstall(IsCriminalEvent, 0x4B09C0);
    RH_ScopedVMTInstall(ReportCriminalEvent, 0x4B09E0);
    RH_ScopedVMTInstall(GetSourceEntity, 0x4B0790);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B0810);
    RH_ScopedVMTInstall(CloneEditable, 0x4B7630);
}
