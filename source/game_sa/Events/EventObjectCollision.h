#pragma once

#include "Event.h"

#include "Event.h"
#include "Vector.h"
#include <extensions/WEnum.hpp>

#include "TaskComplexAvoidOtherPedWhileWandering.h"
#include "TaskComplexKillPedOnFoot.h"
#include "EventKnockOffBike.h"


class NOTSA_EXPORT_VTABLE CEventObjectCollision : public CEvent {
public:
    int16    m_pieceType;
    int16    m_moveState;
    float    m_damageIntensity;
    CObject* m_object;
    CVector  m_impactNormal;
    CVector  m_impactPos;

public:
    static void InjectHooks();

    CEventObjectCollision(int16 pieceType, float damageIntensity, CObject* object, CVector* collisionImpactVelocity, CVector* collisionPos, int16 moveState);
    ~CEventObjectCollision() override;

    eEventType GetEventType() const override { return EVENT_OBJECT_COLLISION; }
    bool TakesPriorityOver(const CEvent& refEvent) override { return true; }
    int32 GetEventPriority() const override { return 57; }
    int32 GetLifeTime() override { return 0; }
    CEventObjectCollision* Clone() override { return new CEventObjectCollision(m_pieceType, m_damageIntensity, m_object, &m_impactNormal, &m_impactPos, m_moveState); }
    bool AffectsPed(CPed* ped) override;

private:
    CEventObjectCollision* Constructor(int16 pieceType, float damageIntensity, CObject* object, CVector* collisionImpactVelocity, CVector* collisionPos, int16 moveState);
};
VALIDATE_SIZE(CEventObjectCollision, 0x30);
