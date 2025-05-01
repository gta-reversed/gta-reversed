#include "StdInc.h"
#include "EventDontJoinPlayerGroup.h"


void CEventDontJoinPlayerGroup::InjectHooks()
{
    RH_ScopedVirtualClass(CEventDontJoinPlayerGroup, 0x86D0E0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x6090E0);
}

// 0x6090E0
CEventDontJoinPlayerGroup::CEventDontJoinPlayerGroup(CPed* player)
{
    m_player = player;
    CEntity::SafeRegisterRef(m_player);
}

CEventDontJoinPlayerGroup::~CEventDontJoinPlayerGroup()
{
    CEntity::SafeCleanUpRef(m_player);
}

// 0x6090E0
CEventDontJoinPlayerGroup* CEventDontJoinPlayerGroup::Constructor(CPed* player)
{
    this->CEventDontJoinPlayerGroup::CEventDontJoinPlayerGroup(player);
    return this;
}

