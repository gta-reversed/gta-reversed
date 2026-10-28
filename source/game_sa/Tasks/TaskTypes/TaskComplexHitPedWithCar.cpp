#include "StdInc.h"

#include "TaskComplexHitPedWithCar.h"
#include "TaskSimpleHurtPedWithCar.h"

void CTaskComplexHitPedWithCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexHitPedWithCar, 0x86f294, 11);
    RH_ScopedCategory(); // TODO: Change this to the appropriate category!

    RH_ScopedInstall(Constructor, 0x6539A0);
    RH_ScopedInstall(Destructor, 0x653A30);

    RH_ScopedGlobalInstall(ComputeEvasiveStepMoveDir, 0x653B40, { .Reversed = false });

    RH_ScopedInstall(HitHurtsPed, 0x653AE0, { .Reversed = false });
    RH_ScopedInstall(CreateSubTask, 0x6560E0, { .Reversed = false });

    RH_ScopedVMTInstall(Clone, 0x6559B0, { .Reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x653A20, { .Reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657AF0);
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
    return plugin::CallMethodAndReturn<bool, 0x653AE0, CTaskComplexHitPedWithCar*, CPed*>(this, ped);
}

// 0x6560E0
CTask* CTaskComplexHitPedWithCar::CreateSubTask(eTaskType tt) {
    return plugin::CallMethodAndReturn<CTask*, 0x6560E0, CTaskComplexHitPedWithCar*, eTaskType>(this, tt);
}

// 0x657AF0
CTask* CTaskComplexHitPedWithCar::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_EVASIVE_STEP:
    case TASK_NONE:
    case TASK_COMPLEX_FALL_AND_GET_UP:
    case TASK_SIMPLE_HIT_BEHIND: // 0x19E=414 (NOT TASK_SIMPLE_DUCK=415) — verified by disasm
    case TASK_SIMPLE_KILL_PED_WITH_CAR:
        return CreateSubTask(TASK_FINISHED); // Returns null — the task is finished
    case TASK_SIMPLE_HURT_PED_WITH_CAR:
        if (static_cast<const CTaskSimpleHurtPedWithCar*>(m_pSubTask)->GetWillKillPed()) {
            return nullptr;
        }
        return CreateSubTask(TASK_COMPLEX_FALL_AND_GET_UP);
    default:
        return nullptr;
    }
}

// 0x656300
CTask* CTaskComplexHitPedWithCar::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x656300, CTaskComplexHitPedWithCar*, CPed*>(this, ped);
}

// 0x653A90
CTask* CTaskComplexHitPedWithCar::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x653A90, CTaskComplexHitPedWithCar*, CPed*>(this, ped);
}
