#pragma once

#include "EventPedCollisionWithPed.h"

#include "Event.h"
#include "Vector.h"
#include <extensions/WEnum.hpp>

#include "TaskComplexAvoidOtherPedWhileWandering.h"
#include "TaskComplexKillPedOnFoot.h"
#include "EventKnockOffBike.h"


class NOTSA_EXPORT_VTABLE CEventPlayerCollisionWithPed : public CEventPedCollisionWithPed {
public:
    CEventPlayerCollisionWithPed(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState);
    ~CEventPlayerCollisionWithPed() override = default;

    eEventType GetEventType() const override { return EVENT_PLAYER_COLLISION_WITH_PED; }
    CEventPlayerCollisionWithPed* Clone() override { return new CEventPlayerCollisionWithPed(m_pieceType, m_damageIntensity, m_victim, &m_collisionImpactVelocity, &m_collisionPos, m_movestate, m_victimMoveState); }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPlayerCollisionWithPed* Constructor(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState);
};
VALIDATE_SIZE(CEventPlayerCollisionWithPed, 0x34);
