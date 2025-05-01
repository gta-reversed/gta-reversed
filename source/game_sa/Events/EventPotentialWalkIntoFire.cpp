#include "StdInc.h"
#include "EventPotentialWalkIntoFire.h"


void CEventPotentialWalkIntoFire::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPotentialWalkIntoFire, 0x85B6A0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B1E20);
    RH_ScopedVMTInstall(AffectsPed, 0x4B6890);
}

// 0x4B1E20
CEventPotentialWalkIntoFire::CEventPotentialWalkIntoFire(const CVector& firePos, float fireSize, eMoveState moveState) :
    m_firePos{firePos},
    m_fireSize{fireSize},
    m_moveState{moveState}
{
    if (fireSize < 1.0f) {
        m_radius = 0.7f;
    } else {
        m_radius = fireSize >= 2.0f
            ? 1.5f
            : 1.0f;
    }
    m_radius += 0.35f;
}

// 0x4B1E20
CEventPotentialWalkIntoFire* CEventPotentialWalkIntoFire::Constructor(const CVector& firePos, float fireSize, eMoveState moveState) {
    this->CEventPotentialWalkIntoFire::CEventPotentialWalkIntoFire(firePos, fireSize, moveState);
    return this;
}

// 0x4B6890
bool CEventPotentialWalkIntoFire::AffectsPed(CPed* ped) {
    if (ped->IsAlive() && m_moveState != PEDMOVE_STILL) {
        auto goToTask = static_cast<CTaskSimpleGoTo*>(ped->GetTaskManager().GetSimplestActiveTask());
        if (goToTask && CTask::IsGoToTask(goToTask)) {
            CColSphere colSphere;
            colSphere.Set(m_radius, m_firePos, SURFACE_DEFAULT, 0, tColLighting(0xFF));
            CVector intersectPoint2;
            CVector intersectPoint1;
            return colSphere.IntersectEdge(ped->GetPosition(), goToTask->m_vecTargetPoint, intersectPoint1, intersectPoint2);
        }
    }
    return false;
}