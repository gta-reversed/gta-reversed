#pragma once

#include "Event.h"

#include "Event.h"
#include "Vector.h"
#include <extensions/WEnum.hpp>

#include "TaskComplexAvoidOtherPedWhileWandering.h"
#include "TaskComplexKillPedOnFoot.h"
#include "EventKnockOffBike.h"


class NOTSA_EXPORT_VTABLE CEventBuildingCollision : public CEvent {
public:
    int16      m_pieceType;
    int16      m_moveState;
    float      m_damageIntensity;
    CBuilding* m_building;
    CVector    m_impactNormal;
    CVector    m_impactPos;

public:
    CEventBuildingCollision(int16 pieceType, float damageIntensity, CBuilding* building, CVector* collisionImpactVelocity, CVector* collisionPos, int16 moveState);
    ~CEventBuildingCollision() override;

    eEventType GetEventType() const override { return EVENT_BUILDING_COLLISION; }
    bool TakesPriorityOver(const CEvent& refEvent) override { return refEvent.GetEventType() != GetEventType(); }
    bool CanBeInterruptedBySameEvent() override { return true; }
    int32 GetEventPriority() const override { return 59; }
    int32 GetLifeTime() override { return 0; }
    CEventBuildingCollision* Clone() override { return new CEventBuildingCollision(m_pieceType, m_damageIntensity, m_building, &m_impactNormal, &m_impactPos, m_moveState); }
    bool AffectsPed(CPed* ped) override;

    static bool CanTreatBuildingAsObject(CBuilding* building);
    bool IsHeadOnCollision(CPed* ped);
private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventBuildingCollision* Constructor(int16 pieceType, float damageIntensity, CBuilding* building, CVector* collisionImpactVelocity, CVector* collisionPos, int16 moveState);

};
VALIDATE_SIZE(CEventBuildingCollision, 0x30);
