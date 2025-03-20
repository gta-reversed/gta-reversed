#include "StdInc.h"
#include "SearchLight.h"

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
