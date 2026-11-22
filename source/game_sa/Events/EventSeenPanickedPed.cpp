#include "StdInc.h"
#include "EventSeenPanickedPed.h"


void CEventSeenPanickedPed::InjectHooks() {
    RH_ScopedVirtualClass(CEventSeenPanickedPed, 0x85B778, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B2080);
    RH_ScopedVMTInstall(AffectsPed, 0x4B53C0);
}

// 0x4B2080
CEventSeenPanickedPed* CEventSeenPanickedPed::Constructor(CPed* ped) { this->CEventSeenPanickedPed::CEventSeenPanickedPed(ped); return this; }

// 0x4B2080
CEventSeenPanickedPed::CEventSeenPanickedPed(CPed* ped) : CEventEditableResponse() {
    m_ped = ped;
    CEntity::SafeRegisterRef(m_ped);
}

CEventSeenPanickedPed::~CEventSeenPanickedPed() {
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4B53C0
bool CEventSeenPanickedPed::AffectsPed(CPed* ped) {
    if (!ped->IsPlayer() && m_ped && m_ped != ped) {
        CEvent* currentEvent = m_ped->GetEventHandlerHistory().GetCurrentEvent();
        if (currentEvent && currentEvent->GetSourceEntity()) {
            CVector distance = ped->GetPosition() - m_ped->GetPosition();
            return distance.SquaredMagnitude() < 100.0f;
        }
    }
    return false;
}