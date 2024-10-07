#include "StdInc.h"

#include "EventPedCollisionWithPed.h"
#include "TaskComplexAvoidOtherPedWhileWandering.h"

void CEventPedCollisionWithPed::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPedCollisionWithPed, 0x85AC68, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AC990);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4ACAD0);
    RH_ScopedVMTInstall(AffectsPed, 0x4ACB10);
}

// 0x4AC990
CEventPedCollisionWithPed::CEventPedCollisionWithPed(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState)
{
    m_pieceType               = pieceType;
    m_damageIntensity         = damageIntensity;
    m_victim                  = victim;
    m_collisionImpactVelocity = *collisionImpactVelocity;
    m_collisionPos            = *collisionPos;
    m_movestate               = moveState;
    m_victimMoveState         = victimMoveState;
    CEntity::SafeRegisterRef(m_victim);
}

CEventPedCollisionWithPed::~CEventPedCollisionWithPed()
{
    CEntity::SafeCleanUpRef(m_victim);
}

// 0x4AC990
CEventPedCollisionWithPed* CEventPedCollisionWithPed::Constructor(int16 pieceType, float damageIntensity, CPed* victim, CVector* collisionImpactVelocity, CVector* collisionPos, eMoveState moveState, eMoveState victimMoveState)
{
    this->CEventPedCollisionWithPed::CEventPedCollisionWithPed(pieceType, damageIntensity, victim, collisionImpactVelocity, collisionPos, moveState, victimMoveState);
    return this;
}

// 0x4ACAD0
bool CEventPedCollisionWithPed::TakesPriorityOver(const CEvent& refEvent)
{
    if (CEventHandler::IsTemporaryEvent(refEvent))
        return true;
    return CEvent::TakesPriorityOver(refEvent);
}

// 0x4ACB10
bool CEventPedCollisionWithPed::AffectsPed(CPed* ped) {
    if (!ped->IsAlive() || ped->m_pAttachedTo || ped->bInVehicle) {
        return false;
    }

    if (!m_victim || m_victim->bInVehicle || ped->GetIntelligence()->IsThreatenedBy(*m_victim) || m_victim->GetIntelligence()->m_AnotherStaticCounter > 30) {
        return false;
    }

    switch (m_movestate) {
    case PEDMOVE_NONE:
    case PEDMOVE_STILL:
    case PEDMOVE_TURN_L:
    case PEDMOVE_TURN_R:
        break;
    default:
        return false;
    }

    if (const auto victimsGroup = CPedGroups::GetPedsGroup(m_victim); !victimsGroup || victimsGroup->GetMembership().IsLeader(m_victim)) {
        return false;
    }

    if (DotProduct(m_collisionImpactVelocity, ped->GetForward()) > -0.5f) { // If collision's velocity is not in [-120, 120] - So basically, if coming from behind
        return false;
    }

    if (ped->GetTaskManager().IsFirstFoundTaskMatching<TASK_COMPLEX_PARTNER_DEAL, TASK_COMPLEX_PARTNER_GREET>(m_victim->GetTaskManager())) {
        return false;
    }
                
    if (const auto task = ped->GetTaskManager().GetActiveTaskAs<CTaskComplexAvoidOtherPedWhileWandering>()) {
        if (task->m_OtherPed == m_victim) {
            return false;
        }
    }

    if (const auto task = ped->GetTaskManager().Find<CTaskComplexKillPedOnFoot>()) {
        if (task->m_target == m_victim) {
            if (ped->GetTaskManager().Find<TASK_SIMPLE_FIGHT_CTRL>()) {
                return false;
            }
        }
    }

    return true;
}