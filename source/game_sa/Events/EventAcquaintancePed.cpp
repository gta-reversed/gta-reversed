#include "StdInc.h"
#include "EventAcquaintancePed.h"


void CEventAcquaintancePed::InjectHooks()
{
    RH_ScopedVirtualClass(CEventAcquaintancePed, 0x85B250, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AF820);
    RH_ScopedVMTInstall(AffectsPed, 0x4AFA30);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4AF970);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4AF8F0);
}

// 0x4AF820
CEventAcquaintancePed::CEventAcquaintancePed(CPed* ped, eTaskType taskType) :
    CEventEditableResponse(taskType)
{
    m_AcquaintancePed = ped;
    CEntity::SafeRegisterRef(m_AcquaintancePed);
}

CEventAcquaintancePed::~CEventAcquaintancePed()
{
    CEntity::SafeCleanUpRef(m_AcquaintancePed);
}

// 0x4AF820
CEventAcquaintancePed* CEventAcquaintancePed::Constructor(CPed* ped)
{
    this->CEventAcquaintancePed::CEventAcquaintancePed(ped);
    return this;
}

// 0x4AFA30
bool CEventAcquaintancePed::AffectsPed(CPed* ped)
{
    if (m_AcquaintancePed && !ped->m_pFire && ped->IsAlive() && m_AcquaintancePed->IsAlive())
        return !CPedGroups::AreInSameGroup(ped, m_AcquaintancePed);
    return false;
}

// 0x4AF970
bool CEventAcquaintancePed::AffectsPedGroup(CPedGroup* pedGroup)
{
    if (m_AcquaintancePed) {
        CPedGroupMembership& membership = pedGroup->GetMembership();
        if (!membership.IsMember(m_AcquaintancePed)
            && (GetEventType() != EVENT_ACQUAINTANCE_PED_RESPECT && GetEventType() != EVENT_ACQUAINTANCE_PED_LIKE || m_AcquaintancePed != pedGroup->m_pPed))
        {
            if (GetEventType() != EVENT_ACQUAINTANCE_PED_RESPECT && GetEventType() != EVENT_ACQUAINTANCE_PED_LIKE)
                return true;
            CPed* leader = membership.GetLeader();
            if (leader && leader->IsPlayer())
                return false;
            return FindPlayerPed()->GetPlayerGroup().GetMembership().IsMember(m_AcquaintancePed);
        }
    }
    return false;
}

// 0x4AF8F0
bool CEventAcquaintancePed::TakesPriorityOver(const CEvent& refEvent)
{
    if (refEvent.GetEventType() == GetEventType()) {
        const auto theRefEvent = static_cast<const CEventAcquaintancePed*>(&refEvent);
        if (m_AcquaintancePed && m_AcquaintancePed->IsPlayer())
            return theRefEvent->m_AcquaintancePed && !theRefEvent->m_AcquaintancePed->IsPlayer();
        return false;
    }
    return CEvent::TakesPriorityOver(refEvent);
}