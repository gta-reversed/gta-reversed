#include "StdInc.h"

#include "TaskSimpleFall.h"
#include "TaskComplexJump.h"
#include "TaskSimpleInAir.h"

#include "PedStuckChecker.h"
#include "TaskComplexStuckInAir.h"

// 0x67BE50
CTask* CTaskComplexStuckInAir::ControlSubTask(CPed* ped) {
    auto* const intel = ped->GetIntelligence();

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_FLEE_POINT: { // 0x67C07A
        if (intel->GetStuckChecker().GetState() != PED_STUCK_STATE_NONE) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT);
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
        }
        return m_pSubTask;
    }
    case TASK_COMPLEX_FALL_AND_GET_UP: { // 0x67BE80
        if (intel->GetStuckChecker().GetState() != PED_STUCK_STATE_NONE) {
            if (auto* const tSubSubSimpleFall = notsa::dyn_cast_if_present<CTaskSimpleFall>(m_pSubTask->GetSubTask()))  {
                if (tSubSubSimpleFall->IsFinished()) {
                    ped->bIsStanding = true;
                }
            }
        }
        return m_pSubTask;
    }
    case TASK_SIMPLE_STAND_STILL: { // 0x67BEE9
        if (intel->GetStuckChecker().GetState() == PED_STUCK_STATE_NONE) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT);
            return nullptr; // 0x67BEF3
        }
        ped->bIsInTheAir = false; // 0x67BFC2
        for (int32 i = ANIM_ID_JUMP_LAUNCH; i <= ANIM_ID_FALL_GLIDE; i++) {
            if (auto* const a = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, static_cast<AnimationId>(i))) {
                if (a->GetBlendAmount() > 0.f && a->GetBlendDelta() >= 0.f) {
                    a->SetBlendDelta(-8.f);
                }
            }
        }
        if (ped->IsPlayer()) { // 0x67BEFA [moved down here due to the return]
            auto* const player = ped->AsPlayer();
            auto* const pad = player->GetPadFromPlayer();
            if (std::abs(pad->GetPedWalkLeftRight()) > 0.f || std::abs(pad->GetPedWalkUpDown()) > 0.f) { // 0x67BF3E
                ped->m_fAimingRotation = CGeneral::GetRadianAngleBetweenPoints({ 0.f, 0.f }, { (float)(-pad->GetPedWalkLeftRight()), (float)(pad->GetPedWalkUpDown()) });
            }
            if (pad->JumpJustDown()) { // 0x67BF9B
                m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT);
                return CreateSubTask(TASK_COMPLEX_JUMP, ped);
            }
        }
        return m_pSubTask;
    }
    case TASK_COMPLEX_JUMP: {
        if (intel->GetStuckChecker().GetState() != PED_STUCK_STATE_NONE) {
            if (auto* const subSubTask = m_pSubTask->GetSubTask()) {
                if (notsa::isa<CTaskSimpleInAir>(subSubTask) || notsa::isa_and_present<CTaskSimpleInAir>(subSubTask->GetSubTask())) { // check if 2nd or 3rd sub tasks (1st is `TASK_COMPLEX_JUMP`)
                    m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT);
                    return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
                }
            }
        }
        return m_pSubTask;
    }
    }
    NOTSA_UNREACHABLE("SubTask type was {}", m_pSubTask->GetTaskType());
}

// 0x67BE20
CTask* CTaskComplexStuckInAir::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x67BE20, CTaskComplexStuckInAir*, CPed*>(this, ped);
}

// 0x67BD10
CTask* CTaskComplexStuckInAir::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x67BD10, CTaskComplexStuckInAir*, CPed*>(this, ped);
}

// 0x67BA80
CTask* CTaskComplexStuckInAir::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x67BA80, CTaskComplexStuckInAir*, eTaskType, CPed*>(this, taskType, ped);
}

void CTaskComplexStuckInAir::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexStuckInAir, 0x870608, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedGlobalInstall(CreateSubTask, 0x67BA80, { .reversed = false });
    RH_ScopedVMTInstall(Clone, 0x67C700);
    RH_ScopedVMTInstall(GetTaskType, 0x67BA60);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x67BD10, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x67BE20, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x67BE50);
}
