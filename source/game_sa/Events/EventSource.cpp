#include "StdInc.h"

#include "EventSource.h"

// 0x4ABAC0
eDecisionRelationship CEventSource::ComputeEventSourceType(const CEvent& event, const CPed& ped) {
    auto* const source = event.GetSourceEntity();
    if (!source || !source->GetIsTypePed()) {
        return DECISION_RELATIONSHIP_NEUTRAL;
    }
    const auto& sourcePed = *source->AsPed();
    if (ped.GetIntelligence()->IsThreatenedBy(sourcePed)) {
        return DECISION_RELATIONSHIP_THREAT;
    }
    if (ped.GetIntelligence()->IsFriendlyWith(sourcePed)) {
        return DECISION_RELATIONSHIP_FRIEND;
    }
    if (sourcePed.IsPlayer()) {
        return DECISION_RELATIONSHIP_PLAYER;
    }
    return DECISION_RELATIONSHIP_NEUTRAL;
}

void CEventSource::InjectHooks() {
    RH_ScopedClass(CEventSource);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(ComputeEventSourceType, 0x4ABAC0);
}
