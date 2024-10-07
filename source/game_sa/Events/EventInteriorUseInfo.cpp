#include "StdInc.h"
#include "EventInteriorUseInfo.h"


CEventInteriorUseInfo::CEventInteriorUseInfo(InteriorInfo_t* interiorInfo, Interior_c* interior, uint32 animTime, int8 loopAction) : CEvent() {
    m_InteriorInfo = interiorInfo;
    m_nLoopAction = loopAction;
    m_Interior = interior;
    m_ActionAnimTime = animTime;
}

bool CEventInteriorUseInfo::IsValid(CPed* ped) {
    if (ped)
        return ped->IsAlive();
    return CEvent::IsValid(ped);
}

bool CEventInteriorUseInfo::AffectsPed(CPed* ped) {
    return true;
}

