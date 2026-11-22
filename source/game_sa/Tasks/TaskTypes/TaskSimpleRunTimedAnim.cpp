#include "StdInc.h"
#include "TaskSimpleRunTimedAnim.h"

void CTaskSimpleRunTimedAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleRunTimedAnim, 0x86d570, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedOverloadedInstall(Constructor, "1", 0x61AB70, CTaskSimpleRunTimedAnim*(CTaskSimpleRunTimedAnim::*)(AssocGroupId, AnimationId, float, float, uint32, bool));
    RH_ScopedOverloadedInstall(Constructor, "2", 0x61ABE0, CTaskSimpleRunTimedAnim*(CTaskSimpleRunTimedAnim::*)(AssocGroupId, AnimationId, float, float, uint32, eTaskType, const char*, bool));

    RH_ScopedInstall(Destructor, 0x48E090);

    RH_ScopedInstall(StartAnim, 0x61AC50);

    RH_ScopedVMTInstall(Clone, 0x61B810);
    RH_ScopedVMTInstall(GetTaskType, 0x48E0E0);
    RH_ScopedVMTInstall(ProcessPed, 0x61BBE0);
}

// 0x61AB70
CTaskSimpleRunTimedAnim::CTaskSimpleRunTimedAnim(AssocGroupId groupId, AnimationId animId, float blendDelta, float unused1, uint32 durationMs, bool holdLastFrame) :
    CTaskSimpleAnim{holdLastFrame},
    m_blendDelta{blendDelta},
    m_unused1{unused1},
    m_durationMs{durationMs},
    m_animId{animId},
    m_animGrpId{groupId}
{
}

// 0x61ABE0
CTaskSimpleRunTimedAnim::CTaskSimpleRunTimedAnim(AssocGroupId groupId, AnimationId animId, float blendDelta, float unused1, uint32 durationMs, eTaskType taskType, const char* taskName, bool holdLastFrame) :
    CTaskSimpleAnim{ holdLastFrame },
    m_blendDelta{ blendDelta },
    m_unused1{ unused1 },
    m_durationMs{ durationMs },
    m_animId{ animId },
    m_animGrpId{ groupId },
    m_taskId{ taskType }
{
}

// 0x61AC50
void CTaskSimpleRunTimedAnim::StartAnim(CPed* ped) {
    m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_animGrpId, m_animId, m_blendDelta);
    m_pAnim->SetDeleteCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
}

// 0x61BBE0
bool CTaskSimpleRunTimedAnim::ProcessPed(CPed* ped) {
    if (m_bIsFinished) {
        return true;
    }

    if (m_timer.IsOutOfTime()) {
        MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    }

    if (!m_pAnim && !RpAnimBlendClumpGetAssociation(ped->GetRpClump(), (uint32)m_animId)) {
        m_timer.Start((int32)m_durationMs);
        StartAnim(ped);
    }

    return m_bIsFinished;
}
