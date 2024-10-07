#include "StdInc.h"
#include "EventScriptedAttractor.h"


void CEventScriptedAttractor::InjectHooks()
{
    RH_ScopedVirtualClass(CEventScriptedAttractor, 0x86C9F0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5FEF40);
}

// 0x5FEF40
CEventScriptedAttractor::CEventScriptedAttractor(C2dEffect* the2dEffect, CEntity* entity, bool bAvoidLookingAtAttractor) :
    CEventAttractor(the2dEffect, entity, false)
{
}

// 0x5FEF40
CEventScriptedAttractor* CEventScriptedAttractor::Constructor(C2dEffect* the2dEffect, CEntity* entity, bool bAvoidLookingAtAttractor)
{
    this->CEventScriptedAttractor::CEventScriptedAttractor(the2dEffect, entity, bAvoidLookingAtAttractor);
    return this;
}

