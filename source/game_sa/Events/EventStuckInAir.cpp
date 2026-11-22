#include "StdInc.h"
#include "EventStuckInAir.h"


void CEventStuckInAir::InjectHooks()
{
    RH_ScopedVirtualClass(CEventStuckInAir, 0x85B508, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B1490);
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1600);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1580);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B15B0);
}

// 0x4B1490
CEventStuckInAir::CEventStuckInAir(CPed* ped)
{
    m_ped = ped;
    CEntity::SafeRegisterRef(m_ped);
}

CEventStuckInAir::~CEventStuckInAir()
{
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4B1600
int32 CEventStuckInAir::GetEventPriority() const {
    return m_ped && m_ped->GetIntelligence()->GetEventHandler().GetCurrentEventType() != EVENT_STUCK_IN_AIR && m_ped->GetIntelligence()->GetEventGroup().GetNumEventsInQueue() <= 1
        ? 75
        : 63;
}

// 0x4B1490
CEventStuckInAir* CEventStuckInAir::Constructor(CPed* ped)
{
    this->CEventStuckInAir::CEventStuckInAir(ped);
    return this;
}

// 0x4B1580
bool CEventStuckInAir::AffectsPed(CPed* ped)
{
    if (ped->bIsStanding)
        return false;

    return ped->GetStuckChecker().m_state != PED_STUCK_STATE_NONE;
}

// 0x4B15B0
bool CEventStuckInAir::TakesPriorityOver(const CEvent& refEvent)
{
    if (refEvent.GetEventPriority() < 74 && refEvent.GetEventType() != EVENT_STUCK_IN_AIR)
        return true;

    return CEvent::TakesPriorityOver(refEvent);
}