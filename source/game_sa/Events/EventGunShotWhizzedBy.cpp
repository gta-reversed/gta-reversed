#include "StdInc.h"
#include "EventGunShotWhizzedBy.h"


void CEventGunShotWhizzedBy::InjectHooks()
{
    RH_ScopedVirtualClass(CEventGunShotWhizzedBy, 0x85B730, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B1FD0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B5120);
}

// 0x4B1FD0
CEventGunShotWhizzedBy::CEventGunShotWhizzedBy(CEntity* entity, const CVector& startPoint, const CVector& endPoint, bool bHasNoSound) :
    CEventGunShot(entity, startPoint, endPoint, bHasNoSound)
{
}

// 0x4B1FD0
CEventGunShotWhizzedBy* CEventGunShotWhizzedBy::Constructor(CEntity* entity, const CVector& startPoint, const CVector& endPoint, bool bHasNoSound)
{
    this->CEventGunShotWhizzedBy::CEventGunShotWhizzedBy(entity, startPoint, endPoint, bHasNoSound);
    return this;
}

// 0x4B5120
bool CEventGunShotWhizzedBy::AffectsPed(CPed* ped)
{
    if (!ped->IsPlayer() && ped->IsAlive()) {
        CVector direction = m_endPoint - m_startPoint;
        direction.Normalise();
        CVector distance = ped->GetPosition() - m_startPoint;
        float squaredMagnitude = (distance * direction).SquaredMagnitude();
        if (squaredMagnitude > 0.0f) {
            CVector2D point = (direction * squaredMagnitude) + m_startPoint;
            return 2.0f * 2.0f > (ped->GetPosition() - point).SquaredMagnitude();
        }
    }
    return false;
}