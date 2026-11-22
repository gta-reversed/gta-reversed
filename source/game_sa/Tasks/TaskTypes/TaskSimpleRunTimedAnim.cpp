#include "StdInc.h"
#include "TaskSimpleRunTimedAnim.h"

void CTaskSimpleRunTimedAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleRunTimedAnim, 0x86d570, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x61AB70, "1", {}, AssocGroupId, AnimationId, float, float, uint32, bool);
    RH_ScopedConstructorInstall(0x61ABE0, "2", {}, AssocGroupId, AnimationId, float, float, uint32, eTaskType, const char*, bool);

    RH_ScopedInstall(StartAnim, 0x61AC50);
    RH_ScopedVMTInstall(Clone, 0x61B810);
    RH_ScopedVMTInstall(GetTaskType, 0x48E0E0);
    RH_ScopedVMTInstall(ProcessPed, 0x61BBE0);
}

// 0x61AB70
CTaskSimpleRunTimedAnim::CTaskSimpleRunTimedAnim(
    AssocGroupId groupId,
    AnimationId  animId,
    float        blendDelta,
    float        blendOutDelta,
    uint32       durationMs,
    bool         holdLastFrame
) :
    CTaskSimpleAnim{ holdLastFrame },
    m_BlendDelta{ blendDelta },
    m_BlendOutDelta{ blendOutDelta },
    m_RunDurationMs{ durationMs },
    m_AnimId{ animId },
    m_AnimGroupId{ groupId } {
}

// 0x61ABE0
CTaskSimpleRunTimedAnim::CTaskSimpleRunTimedAnim(
    AssocGroupId groupId,
    AnimationId  animId,
    float        blendDelta,
    float        blendOutDelta,
    uint32       durationMs,
    eTaskType    taskType,
    const char*  taskName,
    bool         holdLastFrame
) :
    CTaskSimpleRunTimedAnim{
        groupId,
        animId,
        blendDelta,
        blendOutDelta,
        durationMs,
        holdLastFrame,
    } {
    m_TaskType = taskType;
}

// For 0x61B810
CTaskSimpleRunTimedAnim::CTaskSimpleRunTimedAnim(const CTaskSimpleRunTimedAnim& o) :
    CTaskSimpleRunTimedAnim{
        o.m_AnimGroupId,
        o.m_AnimId,
        o.m_BlendDelta,
        o.m_BlendOutDelta,
        o.m_RunDurationMs,
        o.m_bHoldLastFrame
    } {
}

// 0x61AC50
void CTaskSimpleRunTimedAnim::StartAnim(CPed* ped) {
    m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_AnimGroupId, m_AnimId, m_BlendDelta);
    m_pAnim->SetDeleteCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
}

// 0x61BBE0
bool CTaskSimpleRunTimedAnim::ProcessPed(CPed* ped) {
    if (m_bIsFinished) {
        return true;
    }

    if (m_RunTimer.IsOutOfTime()) {
        MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    }

    if (!m_pAnim && !RpAnimBlendClumpGetAssociation(ped->GetRpClump(), (uint32)m_AnimId)) {
        m_RunTimer.Start((int32)m_RunDurationMs);
        StartAnim(ped);
    }

    return m_bIsFinished;
}
