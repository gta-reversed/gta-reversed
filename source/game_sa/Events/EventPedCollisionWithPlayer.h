#pragma once

#include "EventPedCollisionWithPed.h"
#include "Event.h"
#include "Vector.h"
#include <extensions/WEnum.hpp>
#include "TaskComplexAvoidOtherPedWhileWandering.h"
#include "TaskComplexKillPedOnFoot.h"
#include "EventKnockOffBike.h"

class NOTSA_EXPORT_VTABLE CEventPedCollisionWithPlayer : public CEventPedCollisionWithPed {
public:
    CEventPedCollisionWithPlayer(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState);
    ~CEventPedCollisionWithPlayer() override = default;

    eEventType GetEventType() const override { return EVENT_PED_COLLISION_WITH_PLAYER; }
    CEventPedCollisionWithPlayer* Clone() override { return new CEventPedCollisionWithPlayer(m_pieceType, m_damageIntensity, m_victim, &m_collisionImpactVelocity, &m_collisionPos, m_movestate, m_victimMoveState); };

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPedCollisionWithPlayer* Constructor(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState);
};
VALIDATE_SIZE(CEventPedCollisionWithPlayer, 0x34);
