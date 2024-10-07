#include "StdInc.h"
#include "EventPedCollisionWithPlayer.h"


void CEventPedCollisionWithPlayer::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPedCollisionWithPlayer, 0x86C970, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5FED40);
}

// 0x5FED40
CEventPedCollisionWithPlayer::CEventPedCollisionWithPlayer(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState) :
    CEventPedCollisionWithPed(pieceType, damageIntensity, victim, collisionImpactVelocity, collisionPos, moveState, victimMoveState)
{
}

// 0x5FED40
CEventPedCollisionWithPlayer* CEventPedCollisionWithPlayer::Constructor(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState)
{
    this->CEventPedCollisionWithPlayer::CEventPedCollisionWithPlayer(pieceType, damageIntensity, victim, collisionImpactVelocity, collisionPos, moveState, victimMoveState);
    return this;
}

