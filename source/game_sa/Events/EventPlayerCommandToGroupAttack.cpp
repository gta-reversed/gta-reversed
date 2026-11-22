#include "StdInc.h"
#include "EventPlayerCommandToGroupAttack.h"

void CEventPlayerCommandToGroupAttack::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPlayerCommandToGroupAttack, 0x86C710, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5F6340);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B2530);
}

// 0x5F6340
CEventPlayerCommandToGroupAttack* CEventPlayerCommandToGroupAttack::Constructor(CPed* target)
{
    this->CEventPlayerCommandToGroupAttack::CEventPlayerCommandToGroupAttack(target);
    return this;
}

// 0x4B2530
bool CEventPlayerCommandToGroupAttack::AffectsPedGroup(CPedGroup* pedGroup)
{
    bool affectsPedGroup = CEventPlayerCommandToGroup::AffectsPedGroup(pedGroup);
    if (affectsPedGroup) {
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
            CPed* member = pedGroup->GetMembership().GetMember(i);
            if (member && !member->bDoesntListenToPlayerGroupCommands)
                return true;
        }
        return false;
    }
    return affectsPedGroup;
}