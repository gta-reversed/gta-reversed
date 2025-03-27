#include "StdInc.h"
#include "TaskComplexDiveFromAttachedEntityAndGetUp.h"

void CTaskComplexDiveFromAttachedEntityAndGetUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDiveFromAttachedEntityAndGetUp, 0x85a358, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x492E20);
    RH_ScopedInstall(Destructor, 0x492F10);

    RH_ScopedVMTInstall(Clone, 0x492E70);
    RH_ScopedVMTInstall(GetTaskType, 0x492EE0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6577F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x657820);
    RH_ScopedVMTInstall(ControlSubTask, 0x655710);
}

// 0x492E20
CTaskComplexDiveFromAttachedEntityAndGetUp::CTaskComplexDiveFromAttachedEntityAndGetUp(int32 unconsciousTime) :
    CTaskComplexEvasiveDiveAndGetUp{
        nullptr,
        unconsciousTime,
        {},
        true
    }
{
}

// 0x492E70
CTaskComplexDiveFromAttachedEntityAndGetUp::CTaskComplexDiveFromAttachedEntityAndGetUp(const CTaskComplexDiveFromAttachedEntityAndGetUp& o) :
    CTaskComplexDiveFromAttachedEntityAndGetUp{o.m_UnconsciousTime}
{
}

// 0x6577F0
CTask* CTaskComplexDiveFromAttachedEntityAndGetUp::CreateNextSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_ACHIEVE_HEADING) {
        ped->DettachPedFromEntity();
    }
    return CTaskComplexEvasiveDiveAndGetUp::CreateNextSubTask(ped);
}

// 0x657820
CTask* CTaskComplexDiveFromAttachedEntityAndGetUp::CreateFirstSubTask(CPed* ped) {
    if (const auto attachedTo = ped->m_pAttachedTo) {
        const auto hitSide = (eDirection)CPedGeometryAnalyser::ComputeEntityHitSide(*ped, *attachedTo);
        m_DiveDir = CPedGeometryAnalyser::ComputeEntityDir(*attachedTo, hitSide);
        if (attachedTo->IsVehicle()
         && notsa::contains({ eDirection::BACKWARD, eDirection::FORWARD }, hitSide)
         && m_DiveDir.Dot(attachedTo->GetMoveSpeed()) > 0.f // Same direction
        ) {
            CVector bb[4];
            float   bbPlanesDot[4];
            CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll(
                ped->GetPosition().z,
                *attachedTo,
                &bb,
                bbPlanesDot
            );
            const auto GetPlaneDot = [&, pedPos = ped->GetPosition()](int32 i) {
                return std::abs(bb[i].Dot(pedPos) + bbPlanesDot[i]);
            };
            m_DiveDir = GetPlaneDot(3) <= GetPlaneDot(1)
                ? bb[3]
                : bb[1];
        }
        ped->m_fTurretAngleB = 2.f * PI;
    } else {
        m_DiveDir = ped->GetForward();
    }
    return CTaskComplexEvasiveDiveAndGetUp::CreateFirstSubTask(ped);
}
