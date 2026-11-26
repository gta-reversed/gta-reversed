#include "StdInc.h"
#include "SearchLight.h"

//
// Seems like this class doesn't even exist, the stuff here is actually in `CTheScripts` under different names
//

void CSearchLight::InjectHooks() {}

void CSearchLight::SetTravelToPoint() {
    assert(0);
}

void CSearchLight::SetFollowEntity() {
    assert(0);
}

void CSearchLight::SetPathBetween() {
    assert(0);
}

// Actually CTheScripts::IsPointWithinSearchLight
void CSearchLight::IsLookingAtPos() {
    assert(0);
}

void CSearchLight::GetOnEntity() {
    assert(0);
}

// 0x493900
bool CSearchLight::IsSpottedEntity(uint32 index, const CEntity& entity) {
    return plugin::CallAndReturn < bool, 0x493900, uint32, const CEntity&>(index, entity);
}
