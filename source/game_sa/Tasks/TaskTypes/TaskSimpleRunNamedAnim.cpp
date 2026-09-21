#include "StdInc.h"

#include "TaskSimpleRunNamedAnim.h"

void CTaskSimpleRunNamedAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleRunNamedAnim, 0x86D54C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedOverloadedInstall(Constructor, "Default", 0x6674B0, CTaskSimpleRunNamedAnim*(CTaskSimpleRunNamedAnim::*)());
    RH_ScopedOverloadedInstall(Constructor, "Anim", 0x61A990, CTaskSimpleRunNamedAnim*(CTaskSimpleRunNamedAnim::*)(char const*, char const*, int32, float, int32, bool, bool, bool, bool));
    RH_ScopedInstall(Destructor, 0x61BF10);

    //RH_ScopedInstall(FinishRunAnimMovePedCB, 0x61AAA0, { .reversed = false });
    //RH_ScopedInstall(StartAnim, 0x61BB10, { .reversed = false });
    RH_ScopedVMTInstall(Clone, 0x61B770);
    RH_ScopedVMTInstall(GetTaskType, 0x61AA90);
    RH_ScopedVMTInstall(ProcessPed, 0x61BF20);
    RH_ScopedInstall(OffsetPedPosition, 0x61AB00);
}

// 0x6674B0
CTaskSimpleRunNamedAnim::CTaskSimpleRunNamedAnim() :
    CTaskSimpleAnim{ false }
{
    // Rest done in header
}

// todo: check m_vecOffsetAtEnd initialization
// 0x61A990
CTaskSimpleRunNamedAnim::CTaskSimpleRunNamedAnim(
    const char* animName,
    const char* animGroupName,
    uint32 animFlags,
    float blendDelta,
    uint32 endTime,
    bool bDontInterrupt,
    bool bRunInSequence,
    bool bOffsetPed,
    bool bHoldLastFrame
) :
    CTaskSimpleAnim(bHoldLastFrame),
    m_Time{ endTime },
    m_animFlags{ animFlags }
{
    m_bDontInterrupt = bDontInterrupt;
    m_bRunInSequence = bRunInSequence;
    m_bOffsetAtEnd = bOffsetPed;
    m_fBlendDelta = blendDelta;
    strcpy_s(m_animName, animName);
    strcpy_s(m_animGroupName, animGroupName);
    if (const auto block = CAnimManager::GetAnimationBlock(m_animGroupName)) {
        m_pAnimHierarchy = CAnimManager::GetAnimation(m_animName, block);
    }
}

// 0x61BF20
bool CTaskSimpleRunNamedAnim::ProcessPed(CPed* ped) {
    if (m_bOffsetAtEnd) {
        OffsetPedPosition(ped);
    }
    if (!m_bIsFinished) {
        if (m_pAnimHierarchy && m_pAnim) {
            if (m_bDontInterrupt && (!m_pAnim->IsPartial() || !(m_animFlags & 8))) {
                ped->GetEventGroup().RemoveInvalidEvents(true);
                ped->GetEventGroup().Reorganise();
            }
            if (m_bRunInSequence && ped->GetTaskManager().Find<CTaskSimpleRunNamedAnim>(false)) {
                if (!(m_animFlags & 8)) {
                    if (m_pAnim) {
                        m_pAnim->SetDefaultFinishCallback();
                    }
                    m_bIsFinished = true;
                    m_pAnim = nullptr;
                    return true;
                }
                ped->GetTaskManager().Find<CTaskSimpleRunNamedAnim>(false)->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
            }
            if (!m_pAnim) {
                if (const auto assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), true, m_pAnimHierarchy); assoc && assoc->IsPlaying()) {
                    return true;
                }
                m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_pAnimHierarchy, m_animFlags, m_fBlendDelta);
                m_pAnim->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
            }
            return m_bIsFinished;
        }
    } else if (!m_bOffsetAvailable) {
        const auto assoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), true, m_pAnimHierarchy);
        const auto bonePos = ped->GetBonePosition(BONE_SPINE1);
        if (m_Time > 100.0f && assoc) {
            const auto& pedPos = ped->GetPosition();
            if ((pedPos - bonePos).Magnitude() > 1.0f) {
                assoc->m_Flags |= ANIMATION_CAN_EXTRACT_VELOCITY;
                assoc->m_BlendDelta = (float)m_Time * -1.0f;
            }
        }
    }
    return true;
}

// 0x61AB00
void CTaskSimpleRunNamedAnim::OffsetPedPosition(CPed* ped) {
    ped->UpdateRpHAnim();
    ped->m_bDontUpdateHierarchy = true;
    auto& pos = ped->GetPosition();
    pos += ped->m_matrix->TransformVector(m_vecOffsetAtEnd);
    m_bOffsetAvailable = false;
}
