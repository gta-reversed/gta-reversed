#include "StdInc.h"
#include "EventAcquaintancePedHateBadlyLit.h"


void CEventAcquaintancePedHateBadlyLit::InjectHooks()
{
    RH_ScopedVirtualClass(CEventAcquaintancePedHateBadlyLit, 0x86CB10, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5FF250);
    RH_ScopedVMTInstall(AffectsPed, 0x4AFA90);
}

// 0x5FF250
CEventAcquaintancePedHateBadlyLit::CEventAcquaintancePedHateBadlyLit(CPed* ped, int32 startTimeInMs, const CVector& point) : CEventAcquaintancePed(ped)
{
    m_startTimeInMs = startTimeInMs;
    m_point = point;
    if (startTimeInMs == -1) {
        m_startTimeInMs = CTimer::GetTimeInMS();
        m_point = ped->GetPosition();
    }
}

// 0x5FF250
CEventAcquaintancePedHateBadlyLit* CEventAcquaintancePedHateBadlyLit::Constructor(CPed* ped, int32 startTimeInMs, const CVector& point)
{
    this->CEventAcquaintancePedHateBadlyLit::CEventAcquaintancePedHateBadlyLit(ped, startTimeInMs, point);
    return this;
}

// 0x4AFA90
bool CEventAcquaintancePedHateBadlyLit::AffectsPed(CPed* ped)
{
    if (CEventAcquaintancePed::AffectsPed(ped)) {
        CEvent* currentEvent = ped->GetEventHandlerHistory().GetCurrentEvent();
        if (!currentEvent || currentEvent->GetEventType() != EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT
            || currentEvent->GetSourceEntity() != GetSourceEntity())
        {
            return true;
        }
        auto theCurrentEvent = static_cast<CEventAcquaintancePedHateBadlyLit*>(currentEvent);
        if (m_startTimeInMs - theCurrentEvent->m_startTimeInMs >= 2000) {
            CVector distance = m_point - theCurrentEvent->m_point;
            return distance.SquaredMagnitude() >= 1.0f;
        }
    }
    return false;
}

