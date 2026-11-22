#include "StdInc.h"
#include "TaskComplexWalkRoundFire.h"
#include "TaskSimpleGoToPoint.h"

void CTaskComplexWalkRoundFire::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundFire, 0x86f3e8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x655720);
    RH_ScopedInstall(Destructor, 0x655780);

    RH_ScopedInstall(ComputeDetourTarget, 0x6581B0);

    RH_ScopedVMTInstall(Clone, 0x655D40);
    RH_ScopedVMTInstall(GetTaskType, 0x655770);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x655790);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6584E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6585C0);
}

// 0x655720
CTaskComplexWalkRoundFire::CTaskComplexWalkRoundFire(eMoveState moveState, CVector const& firePos, float fireRadius, CVector const& targetPos) :
    m_fireBoundSphere{firePos, fireRadius},
    m_moveState{moveState},
    m_targetPos{targetPos}
{
}

CTaskComplexWalkRoundFire::CTaskComplexWalkRoundFire(const CTaskComplexWalkRoundFire& o) :
    CTaskComplexWalkRoundFire{o.m_moveState, o.m_fireBoundSphere.m_vecCenter, o.m_fireBoundSphere.m_fRadius, o.m_targetPos}
{
}

bool CTaskComplexWalkRoundFire::ComputeDetourTarget(const CPed& ped, CVector& outTarget) const {
    CVector unused{};
    return CPedGeometryAnalyser::ComputeRouteRoundSphere(
        ped,
        m_fireBoundSphere,
        m_initialPedPos,
        m_targetPos,
        unused,
        outTarget
    );
}

// 0x6584E0
CTask* CTaskComplexWalkRoundFire::CreateFirstSubTask(CPed* ped) {
    m_initialPedPos = ped->GetPosition();
    return new CTaskSimpleGoToPoint{ m_moveState, GetDetourTarget(*ped) };
}

// 0x6585C0
CTask* CTaskComplexWalkRoundFire::ControlSubTask(CPed* ped) {
    static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->UpdatePoint(GetDetourTarget(*ped));
    return m_pSubTask;
}

// NOTSA
CVector CTaskComplexWalkRoundFire::GetDetourTarget(const CPed& ped) const {
    CVector detourTargetPos{};
    (void)ComputeDetourTarget(ped, detourTargetPos); // TODO: Use a macro to check if return value is true (in debug mode)
    return detourTargetPos;
}
