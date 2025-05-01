#include "StdInc.h"
#include "EventNewGangMember.h"


void CEventNewGangMember::InjectHooks()
{
    RH_ScopedVirtualClass(CEventNewGangMember, 0x86D0A0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x608F70);
}

// 0x608F70
CEventNewGangMember::CEventNewGangMember(CPed* member)
{
    m_member = member;
    CEntity::SafeRegisterRef(m_member);
}

CEventNewGangMember::~CEventNewGangMember()
{
    CEntity::SafeCleanUpRef(m_member);
}

// 0x608F70
CEventNewGangMember* CEventNewGangMember::Constructor(CPed* member)
{
    this->CEventNewGangMember::CEventNewGangMember(member);
    return this;
}

