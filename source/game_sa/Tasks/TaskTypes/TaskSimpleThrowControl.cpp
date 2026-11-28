#include "StdInc.h"
#include "TaskSimpleThrowControl.h"
#include "TaskSimpleThrowProjectile.h"

// 0x61F8B0
CTaskSimpleThrowControl::CTaskSimpleThrowControl(CEntity* targetEntity, const CVector* targetPos) :
    m_TargetEntity{ targetEntity },
    m_TargetPos{ targetPos ? *targetPos : CVector{} } {
}

// NOTSA
CTaskSimpleThrowControl::CTaskSimpleThrowControl(const CTaskSimpleThrowControl& o) :
    CTaskSimpleThrowControl{ o.m_TargetEntity, &o.m_TargetPos } {
}

// 0x61F9B0
bool CTaskSimpleThrowControl::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (!ped->GetIntelligence()->GetTaskThrow()) {
        return true;
    }
    return ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, priority, event);
}

// 0x61F9F0
bool CTaskSimpleThrowControl::ProcessPed(CPed* ped) {
    if (m_IsFinished) {
        return true;
    }

    ped->SetMoveState(PEDMOVE_STILL);

    auto& tm = ped->GetTaskManager();
    if (auto* const tAttack = tm.GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
        if (!notsa::isa<CTaskSimpleThrowProjectile>(tAttack)) { // 0x61FB32
            tAttack->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
            return false;
        }
    } else { // 0x61FA3F
        if (m_IsAttacking) {
            return true;
        }
        tm.SetTaskSecondary(new CTaskSimpleThrowProjectile{ m_TargetEntity, m_TargetPos }, TASK_SECONDARY_ATTACK);
        m_IsAttacking = true;
    }

    auto* const tThrow = ped->GetIntelligence()->GetTaskThrow();
    const auto  hasTargetPos = m_TargetPos != CVector{};
    if (m_TargetEntity) { // 0x61FAF8
        ped->m_fAimingRotation = (m_TargetEntity->GetPosition() - ped->GetPosition()).Heading();
    } else if (hasTargetPos) { // 0x61FB80
        ped->m_fAimingRotation = (m_TargetPos - ped->GetPosition()).Heading();
    }

    if (!m_IsAttacking) {
        return false;
    }
    if (tThrow->m_bIsFinished) { // 0x61FBE9
        return true;
    }
    if (hasTargetPos) {
        tThrow->m_TargetPos = m_TargetPos;
    }
    return false;
}

void CTaskSimpleThrowControl::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleThrowControl, 0x86D7B4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x61F8B0, "1", {}, CEntity*, const CVector*);
    RH_ScopedDestructorInstall(0x61F950);

    RH_ScopedVMTInstall(Clone, 0x6230B0);
    RH_ScopedVMTInstall(GetTaskType, 0x61F940);
    RH_ScopedVMTInstall(MakeAbortable, 0x61F9B0);
    RH_ScopedVMTInstall(ProcessPed, 0x61F9F0);
}
