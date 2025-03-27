#include "StdInc.h"
#include "EventPlayerCollisionWithPed.h"


void CEventPlayerCollisionWithPed::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPlayerCollisionWithPed, 0x86C9B0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5FEE40);
}

// 0x5FEE40
CEventPlayerCollisionWithPed::CEventPlayerCollisionWithPed(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState) :
    CEventPedCollisionWithPed(pieceType, damageIntensity, victim, collisionImpactVelocity, collisionPos, moveState, victimMoveState)
{
}

// 0x5FEE40
CEventPlayerCollisionWithPed* CEventPlayerCollisionWithPed::Constructor(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState)
{
    this->CEventPlayerCollisionWithPed::CEventPlayerCollisionWithPed(pieceType, damageIntensity, victim, collisionImpactVelocity, collisionPos, moveState, victimMoveState);
    return this;
}

