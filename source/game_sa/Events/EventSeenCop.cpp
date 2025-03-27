#include "StdInc.h"
#include "EventSeenCop.h"


void CEventSeenCop::InjectHooks()
{
    RH_ScopedVirtualClass(CEventSeenCop, 0x86CB58, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor1, 0x5FF380);
}

// 0x5FF380
CEventSeenCop* CEventSeenCop::Constructor1(CPed* cop)
{
    this->CEventSeenCop::CEventSeenCop(cop);
    return this;
}

