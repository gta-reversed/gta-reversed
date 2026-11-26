#include "StdInc.h"
#include "TaskSimpleOnEscalator.h"

// 0x4B8670
CTaskSimpleOnEscalator::CTaskSimpleOnEscalator() :
    CTaskSimpleStandStill{ 0, true }
{
}

// 0x4B86C0
CTaskSimpleOnEscalator::CTaskSimpleOnEscalator(const CTaskSimpleOnEscalator&) :
    CTaskSimpleOnEscalator{}
{
}

// 0x62F520
bool CTaskSimpleOnEscalator::ProcessPed(CPed* ped) {
    CTaskSimpleStandStill::ProcessPed(ped);

    auto* const step = ped->m_pContactEntity;
    if (!step || (step->GetModelIndex() != ModelIndices::MI_ESCALATORSTEP && step->GetModelIndex() != ModelIndices::MI_ESCALATORSTEP8)) {
        return true;
    }

    // NOTE: Not `CVector::Heading()`, the original handles a zero move speed differently
    const auto& moveSpeed  = step->AsPhysical()->GetMoveSpeed();
    ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(moveSpeed.x, moveSpeed.y, 0.0f, 0.0f));
    return false;
}

void CTaskSimpleOnEscalator::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleOnEscalator, 0x85b8dc, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x4B8670, "1", {});
    RH_ScopedDestructorInstall(0x4B8750);

    RH_ScopedVMTInstall(Clone, 0x4B86C0);
    RH_ScopedVMTInstall(GetTaskType, 0x4B8720);
    RH_ScopedVMTInstall(ProcessPed, 0x62F520);
}
