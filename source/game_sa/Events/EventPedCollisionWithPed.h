#pragma once

#include "Event.h"
#include "Event.h"
#include "Vector.h"
#include <extensions/WEnum.hpp>

class NOTSA_EXPORT_VTABLE CEventPedCollisionWithPed : public CEvent {
public:
    int16                       m_pieceType;
    char                        field_E[2];
    float                       m_damageIntensity;
    CPed*                       m_victim;
    CVector                     m_collisionImpactVelocity;
    CVector                     m_collisionPos;
    notsa::WEnumS16<eMoveState> m_movestate;
    notsa::WEnumS16<eMoveState> m_victimMoveState;

public:
    CEventPedCollisionWithPed(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState);
    ~CEventPedCollisionWithPed() override;

    eEventType GetEventType() const override { return EVENT_PED_COLLISION_WITH_PED; };
    bool TakesPriorityOver(const CEvent& refEvent) override;
    int32 GetEventPriority() const override { return 60; };
    int32 GetLifeTime() override { return 0; };
    CEventPedCollisionWithPed* Clone() override { return new CEventPedCollisionWithPed(m_pieceType, m_damageIntensity, m_victim, &m_collisionImpactVelocity, &m_collisionPos, m_movestate, m_victimMoveState); };
    bool AffectsPed(CPed* ped) override;
    eMoveState GetVictimMoveState() const { return m_victimMoveState; }
    eMoveState GetMoveState() const { return m_movestate; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CEventPedCollisionWithPed* Constructor(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState);

};
VALIDATE_SIZE(CEventPedCollisionWithPed, 0x34);
