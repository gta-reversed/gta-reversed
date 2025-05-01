#include "StdInc.h"
#include "EventPlayerCommandToGroup.h"


void CEventPlayerCommandToGroup::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPlayerCommandToGroup, 0x85B800, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B23D0);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B24D0);
}

// 0x4B23D0
CEventPlayerCommandToGroup::CEventPlayerCommandToGroup(ePlayerGroupCommand command, CPed* target)
{
    m_command = command;
    m_target = target;
    CEntity::SafeRegisterRef(m_target);
}

CEventPlayerCommandToGroup::~CEventPlayerCommandToGroup()
{
    CEntity::SafeCleanUpRef(m_target);
}

// 0x4B23D0
CEventPlayerCommandToGroup* CEventPlayerCommandToGroup::Constructor(ePlayerGroupCommand command, CPed* target)
{
    this->CEventPlayerCommandToGroup::CEventPlayerCommandToGroup(command, target);
    return this;
}

// 0x4B24D0
bool CEventPlayerCommandToGroup::AffectsPedGroup(CPedGroup* pedGroup)
{
    CPed* leader = pedGroup->GetMembership().GetLeader();
    if (leader && leader->IsPlayer()) {
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
            CPed* member = pedGroup->GetMembership().GetMember(i);
            if (member && member->m_nAreaCode != leader->m_nAreaCode) 
                return false;
        }
    }
    return true;
}