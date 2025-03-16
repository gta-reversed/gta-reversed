#include "StdInc.h"
#include "EventSignalAtPed.h"


CEventSignalAtPed::CEventSignalAtPed(CPed* signalAt, int32 initialPause, bool bPlayAnimAtEnd) :
    m_SignalAt{signalAt},
    m_InitialPause{initialPause},
    m_bPlayAnimAtEnd{bPlayAnimAtEnd}
{
    CEntity::SafeRegisterRef(m_SignalAt);
}

CEventSignalAtPed::~CEventSignalAtPed() {
    CEntity::SafeCleanUpRef(m_SignalAt);
}

bool CEventSignalAtPed::AffectsPed(CPed* ped) {
    return true;
}

bool CEventSignalAtPed::IsValid(CPed* ped) {
    if (ped)
        return ped->IsAlive();
    else
        return CEvent::IsValid(nullptr);
}

