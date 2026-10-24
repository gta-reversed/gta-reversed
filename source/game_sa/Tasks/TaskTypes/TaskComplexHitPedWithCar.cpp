#include "StdInc.h"

#include "TaskComplexHitPedWithCar.h"

void CTaskComplexHitPedWithCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexHitPedWithCar, 0x86f294, 11);
    RH_ScopedCategory(); // TODO: Change this to the appropriate category!

    RH_ScopedInstall(Constructor, 0x6539A0, { .Reversed = false });
    RH_ScopedInstall(Destructor, 0x653A30, { .Reversed = false });

    RH_ScopedGlobalInstall(ComputeEvasiveStepMoveDir, 0x653B40, { .Reversed = false });

    RH_ScopedInstall(HitHurtsPed, 0x653AE0);
    RH_ScopedInstall(CreateSubTask, 0x6560E0, { .Reversed = false });

    RH_ScopedVMTInstall(Clone, 0x6559B0, { .Reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x653A20, { .Reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657AF0, { .Reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x656300, { .Reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x653A90, { .Reversed = false });

}

// 0x6539A0
CTaskComplexHitPedWithCar::CTaskComplexHitPedWithCar(CVehicle* veh, float impulseMagnitude) :
    m_Veh{veh},
    m_ImpulseMag{impulseMagnitude}
{
    CEntity::SafeRegisterRef(m_Veh);
}

CTaskComplexHitPedWithCar::CTaskComplexHitPedWithCar(const CTaskComplexHitPedWithCar& o) :
    CTaskComplexHitPedWithCar{o.m_Veh, o.m_ImpulseMag}
{
}

// 0x653A30
CTaskComplexHitPedWithCar::~CTaskComplexHitPedWithCar() {
    CEntity::SafeCleanUpRef(m_Veh);
}

// 0x653B40
CVector CTaskComplexHitPedWithCar::ComputeEvasiveStepMoveDir(const CPed* ped, CVehicle* veh) {
    return CPedGeometryAnalyser::ComputeEntityDir(*veh, (eDirection)CPedGeometryAnalyser::ComputeEntityHitSide(*ped, *veh)); // Same shit, difference packaging
}

// 0x653AE0
bool CTaskComplexHitPedWithCar::HitHurtsPed(CPed* ped) {
    // Min impulse magnitude for the ped to get hurt
    const auto minImpulseToHurt = ped->IsPlayer()
        ? 10.0f // 0x86F124
        : 6.0f; // 0x86F11C

    if (m_ImpulseMag > minImpulseToHurt) {
        return true;
    }

    // Ped was slammed into the ground (downward) hard enough, and the hit was still reasonably strong
    return ped->m_vecLastCollisionImpactVelocity.z < -0.8f // 0x858C9C
        && m_ImpulseMag > 3.0f;                            // 0x858B3C
}

// 0x6560E0
CTask* CTaskComplexHitPedWithCar::CreateSubTask(eTaskType tt) {
    return plugin::CallMethodAndReturn<CTask*, 0x6560E0, CTaskComplexHitPedWithCar*, eTaskType>(this, tt);
}

// 0x657AF0
CTask* CTaskComplexHitPedWithCar::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x657AF0, CTaskComplexHitPedWithCar*, CPed*>(this, ped);
}

// 0x656300
CTask* CTaskComplexHitPedWithCar::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x656300, CTaskComplexHitPedWithCar*, CPed*>(this, ped);
}

// 0x653A90
CTask* CTaskComplexHitPedWithCar::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x653A90, CTaskComplexHitPedWithCar*, CPed*>(this, ped);
}
