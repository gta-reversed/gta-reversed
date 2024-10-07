#pragma once

#include "EventBuildingCollision.h"



class CEventPotentialWalkIntoBuilding : public CEventBuildingCollision {
public:
    CEventPotentialWalkIntoBuilding();
    ~CEventPotentialWalkIntoBuilding() override = default;

    eEventType GetEventType() const override { return EVENT_POTENTIAL_WALK_INTO_BUILDING; }
    int32 GetEventPriority() const override { return 30; }
    CEventBuildingCollision* Clone() override { return new CEventPotentialWalkIntoBuilding(); }
    bool AffectsPed(CPed* ped) override;
};

