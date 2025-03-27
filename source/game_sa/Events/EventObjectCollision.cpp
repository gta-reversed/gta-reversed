#include "StdInc.h"
#include "EventObjectCollision.h"


void CEventObjectCollision::InjectHooks()
{
    RH_ScopedVirtualClass(CEventObjectCollision, 0x85ACA8, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4ACCF0);
    RH_ScopedVMTInstall(AffectsPed, 0x4ACE30);
}

// 0x4ACCF0
CEventObjectCollision::CEventObjectCollision(int16 pieceType, float damageIntensity, CObject* object, CVector* collisionImpactVelocity, CVector* collisionPos, int16 moveState)
{
    m_pieceType = pieceType;
    m_moveState = moveState;
    m_damageIntensity = damageIntensity;
    m_object = object;
    m_impactNormal = *collisionImpactVelocity;
    m_impactPos = *collisionPos;
    CEntity::SafeRegisterRef(m_object);
}

CEventObjectCollision::~CEventObjectCollision()
{
    CEntity::SafeCleanUpRef(m_object);
}

// 0x4ACCF0
CEventObjectCollision* CEventObjectCollision::Constructor(int16 pieceType, float damageIntensity, CObject* object, CVector* collisionImpactVelocity, CVector* collisionPos, int16 moveState)
{
    this->CEventObjectCollision::CEventObjectCollision(pieceType, damageIntensity, object, collisionImpactVelocity, collisionPos, moveState);
    return this;
}

// 0x4ACE30
bool CEventObjectCollision::AffectsPed(CPed* ped)
{
    if (!ped->m_pAttachedTo) {
        if (m_object && !m_object->physicalFlags.bDisableMoveForce) {
            if (!ped->IsPlayer() && ped->IsAlive()) 
                return true;
        }
    }
    return false;
}