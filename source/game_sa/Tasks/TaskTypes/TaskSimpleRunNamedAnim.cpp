#include "StdInc.h"

#include "TaskSimpleRunNamedAnim.h"

// 0x6674B0
CTaskSimpleRunNamedAnim::CTaskSimpleRunNamedAnim() :
    CTaskSimpleAnim{ false }
{
    // Rest done in header
}

// 0x61A990
CTaskSimpleRunNamedAnim::CTaskSimpleRunNamedAnim(
    const char* animName,
    const char* animGroupName,
    uint32      animFlags,
    float       blendDelta,
    uint32      endTime,
    bool        bDontInterrupt,
    bool        bRunInSequence,
    bool        bOffsetPed,
    bool        bHoldLastFrame
) :
    CTaskSimpleAnim(bHoldLastFrame),
    m_Time{ endTime },
    m_AnimFlags{ animFlags } {
    m_bDontInterrupt = bDontInterrupt;
    m_bRunInSequence = bRunInSequence;
    m_bOffsetAtEnd = bOffsetPed;
    m_BlendDelta     = blendDelta;
    strcpy_s(m_AnimName, animName);
    strcpy_s(m_AnimGroupName, animGroupName);
    if (const auto block = CAnimManager::GetAnimationBlock(m_AnimGroupName)) {
        m_AnimHierarchy = CAnimManager::GetAnimation(m_AnimName, block);
    }
}

// NOTSA
CTaskSimpleRunNamedAnim::CTaskSimpleRunNamedAnim(const CTaskSimpleRunNamedAnim& o) :
    CTaskSimpleRunNamedAnim{
        o.m_AnimName,
        o.m_AnimGroupName,
        o.m_AnimFlags,
        o.m_BlendDelta,
        o.m_Time,
        o.m_bDontInterrupt,
        o.m_bRunInSequence,
        o.m_bOffsetAtEnd,
        o.m_bHoldLastFrame
    } {
}

// 0x61B770
CTask* CTaskSimpleRunNamedAnim::Clone() const {
    return new CTaskSimpleRunNamedAnim{ *this };
}

// 0x61BF20
bool CTaskSimpleRunNamedAnim::ProcessPed(CPed* ped) {
    if (m_bOffsetAvailable) {
        OffsetPedPosition(ped);
    }

    if (m_bIsFinished) {
        if (!m_bOffsetAtEnd) {
            const auto anim    = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), true, m_AnimHierarchy);
            const auto bonePos = ped->GetBonePosition(BONE_SPINE1, false);
            if (m_BlendDelta > 100.f && anim) {
                if (CVector::DistSqr(ped->GetPosition(), bonePos) > sq(1.f)) {
                    anim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
                    anim->m_BlendDelta = m_BlendDelta * -1.f;
                }
            }
        }
        return true;
    }

    if (!m_AnimHierarchy || !m_AnimHierarchy->m_pSequences) { // Not loaded yet
        return true;
    }

    if (m_bDontInterrupt) {
        if (!(m_AnimFlags & ANIMATION_IS_FINISH_AUTO_REMOVE) || (m_AnimFlags & ANIMATION_IS_LOOPED)) {
            auto& eventGroup = ped->GetEventGroup();
            eventGroup.RemoveInvalidEvents(true);
            eventGroup.Reorganise();
        }
    }

    if (m_Timer.IsStarted() && m_Timer.IsOutOfTime()) {
        if (!(m_AnimFlags & ANIMATION_IS_FINISH_AUTO_REMOVE)) {
            if (m_pAnim) {
                m_pAnim->SetDefaultFinishCallback();
            }
            m_bIsFinished = true;
            m_pAnim       = nullptr;
            return true;
        }
        MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
    }

    if (!m_pAnim) {
        if (const auto anim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), true, m_AnimHierarchy); anim && anim->m_pCallbackData) {
            return true;
        }
        StartAnim(ped);
    }

    return m_bIsFinished;
}

// 0x61BB10
void CTaskSimpleRunNamedAnim::StartAnim(CPed* ped) {
    m_Timer.Start((int32)(m_Time));

    if (!(m_AnimFlags & ANIMATION_IS_FINISH_AUTO_REMOVE)) {
        m_AnimFlags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
    }

    m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_AnimHierarchy, m_AnimFlags, m_BlendDelta);
    if (m_bOffsetAtEnd) {
        m_pAnim->SetFinishCallback(FinishRunAnimMovePedCB, this);
    } else if (!(m_AnimFlags & ANIMATION_IS_FINISH_AUTO_REMOVE) && !m_bRunInSequence) {
        m_pAnim->SetDeleteCallback(FinishRunAnimCB, this);
    } else {
        m_pAnim->SetFinishCallback(FinishRunAnimCB, this);
    }
    m_AnimId = (int16)(m_pAnim->GetAnimId());
}

// 0x61AAA0
void CTaskSimpleRunNamedAnim::FinishRunAnimMovePedCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = notsa::cast<CTaskSimpleRunNamedAnim>(static_cast<CTask*>(data));

    anim->GetNode(0)->GetCurrentTranslation(self->m_OffsetAtEnd, 0.f);
    self->m_bOffsetAvailable = true;
    anim->m_Flags |= ANIMATION_IGNORE_ROOT_TRANSLATION;

    if (!(anim->m_Flags & ANIMATION_IS_FINISH_AUTO_REMOVE) && anim->m_nCallbackType != ANIM_BLEND_CALLBACK_DELETE) {
        anim->SetDeleteCallback(FinishRunAnimCB, self);
    } else {
        self->m_bIsFinished = true;
        self->m_pAnim       = nullptr;
    }
}

// 0x61AB00
void CTaskSimpleRunNamedAnim::OffsetPedPosition(CPed* ped) {
    ped->UpdateRpHAnim();
    ped->m_bDontUpdateHierarchy = true;
    auto& pos = ped->GetPosition();
    pos += ped->m_matrix->TransformVector(m_OffsetAtEnd);
    m_bOffsetAvailable = false;
}

void CTaskSimpleRunNamedAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleRunNamedAnim, 0x86D54C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6674B0, "0", {});
    RH_ScopedConstructorInstall(0x61A990, "1", {}, const char*, const char*, uint32, float, uint32, bool, bool, bool, bool);
    RH_ScopedDestructorInstall(0x61BF10);

    RH_ScopedInstall(FinishRunAnimMovePedCB, 0x61AAA0);
    RH_ScopedInstall(StartAnim, 0x61BB10);
    RH_ScopedInstall(OffsetPedPosition, 0x61AB00);
    RH_ScopedVMTInstall(Clone, 0x61B770);
    RH_ScopedVMTInstall(GetTaskType, 0x61AA90);
    RH_ScopedVMTInstall(ProcessPed, 0x61BF20);
}
