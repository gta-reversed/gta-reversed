#include "StdInc.h"
#include "TaskInteriorSitAtDesk.h"

#include "Interior/InteriorInfo_t.h"
#include "Interior/Interior_c.h"
#include "Interior/InteriorManager_c.h"

void CTaskInteriorSitAtDesk::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorSitAtDesk, 0x87035c, 9);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x676010);
    RH_ScopedInstall(Destructor, 0x676080);

    RH_ScopedInstall(FinishAnimCB, 0x676190);
    RH_ScopedInstall(StartRandomLoopAnim, 0x677780);
    RH_ScopedInstall(StartRandomOneOffAnim, 0x677880);
    RH_ScopedVMTInstall(Clone, 0x6760E0);
    RH_ScopedVMTInstall(GetTaskType, 0x676070);
    RH_ScopedVMTInstall(MakeAbortable, 0x676150);
    RH_ScopedVMTInstall(ProcessPed, 0x677920);
}

// 0x676010
CTaskInteriorSitAtDesk::CTaskInteriorSitAtDesk(int32 duration, InteriorInfo_t* interiorInfo, bool bDoInstantly) :
    m_bDoInstantly{bDoInstantly},
    m_InteriorInfo{interiorInfo},
    m_Duration{duration}
{
}

// 0x6760E0
CTaskInteriorSitAtDesk::CTaskInteriorSitAtDesk(const CTaskInteriorSitAtDesk& o) :
    CTaskInteriorSitAtDesk{o.m_Duration, o.m_InteriorInfo, o.m_bDoInstantly}
{
}

// 0x676080
CTaskInteriorSitAtDesk::~CTaskInteriorSitAtDesk() {
    if (m_Anim) {
        m_Anim->SetDefaultFinishCallback();
    }
}

// 0x676190
void CTaskInteriorSitAtDesk::FinishAnimCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = notsa::cast<CTaskInteriorSitAtDesk>(static_cast<CTask*>(data));

    assert(self && anim);
    assert(anim == self->m_Anim);

    if (anim->m_AnimId == ANIM_ID_OFF_SIT_2IDLE_180 || self->m_bTaskAborting && self->m_PrevAnimId == ANIM_ID_OFF_SIT_IN) {
        anim->SetBlendDelta(-1000.f);
        self->m_bTaskFinished = true;
    }

    self->m_Anim = nullptr;
}

// 0x677780
void CTaskInteriorSitAtDesk::StartRandomLoopAnim(CPed* ped, float blendDelta) {
    using CGeneral::GetRandomNumberInRange;

    const auto chance = GetRandomNumberInRange(100u);
    StartAnim(
        ped,
        chance > 40
            ? ANIM_ID_OFF_SIT_TYPE_LOOP
            : chance > 10
                ? ANIM_ID_OFF_SIT_BORED_LOOP
                : ANIM_ID_OFF_SIT_IDLE_LOOP,
        blendDelta
    );
    m_AnimTimer.Start(chance > 40 ? GetRandomNumberInRange(2000, 5000) : GetRandomNumberInRange(7000, 12000));
}

// 0x677880
void CTaskInteriorSitAtDesk::StartRandomOneOffAnim(CPed* ped) {
    const auto chance = CGeneral::GetRandomNumberInRange(100u);
    StartAnim(
        ped,
        chance > 60
            ? ANIM_ID_OFF_SIT_READ
            : chance > 35
                ? ANIM_ID_OFF_SIT_DRINK
                : chance > 10
                    ? ANIM_ID_OFF_SIT_WATCH
                    : ANIM_ID_OFF_SIT_CRASH,
        4.f
    );
}

// 0x676150
bool CTaskInteriorSitAtDesk::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_Anim) {
            m_Anim->SetBlendDelta(-1000.f);
            m_Anim->SetDefaultFinishCallback();
            m_Anim = nullptr;
        }
        return true;
    }
    m_bTaskAborting = true;
    return false;
}

// 0x677920
bool CTaskInteriorSitAtDesk::ProcessPed(CPed* ped) {
    const auto currAnimId = m_Anim ? m_Anim->GetAnimId() : ANIM_ID_UNDEFINED;
    ped->SetMoveState(PEDMOVE_STILL);
    if (m_bTaskFinished) {
        if (!RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_OFF_SIT_2IDLE_180)) {
            return true;
        }
        ped->m_fAimingRotation = ped->m_fCurrentRotation = CGeneral::LimitRadianAngle(ped->m_fCurrentRotation + PI);
        if (ped->m_matrix) {
            ped->m_matrix->SetRotateZOnly(ped->m_fCurrentRotation);
        } else {
            ped->m_placement.m_fHeading = ped->m_fCurrentRotation;
        }
        return true;
    }
    if (m_bTaskAborting) {
        if (!InteriorManager_c::AreAnimsLoaded(ANIM_GROUP_INT_OFFICE)) {
            return true;
        }
        assert(m_Anim);
        if (currAnimId == ANIM_ID_OFF_SIT_IN) {
            m_Anim->SetBlendDelta(-8.f);
        } else if (currAnimId == ANIM_ID_OFF_SIT_IDLE_LOOP
                || currAnimId == ANIM_ID_OFF_SIT_TYPE_LOOP
                || currAnimId == ANIM_ID_OFF_SIT_BORED_LOOP) {
            if (!m_bUpdatePedPos) {
                m_Anim->SetDefaultDeleteCallback();
                m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_INT_OFFICE, ANIM_ID_OFF_SIT_2IDLE_180, 1000.f);
                m_Anim->SetFinishCallback(FinishAnimCB, this);
                m_bUpdatePedPos = true;
                return false;
            }
        } else if (currAnimId == ANIM_ID_OFF_SIT_2IDLE_180) {
            m_Anim->SetBlendDelta(2.f);
        }
    }
    if (!m_Anim) {
        if (!InteriorManager_c::AreAnimsLoaded(ANIM_GROUP_INT_OFFICE)) {
            return false;
        }
        if (m_PrevAnimId != ANIM_ID_UNDEFINED) {
            if (m_PrevAnimId == ANIM_ID_OFF_SIT_IN) {
                m_TaskTimer.Start(m_Duration);
                StartRandomLoopAnim(ped, 4.f);
            }
        } else if (m_bDoInstantly) {
            m_InteriorInfo->IsInUse = true;
            m_TaskTimer.Start(m_Duration);
            StartRandomLoopAnim(ped, 4.f);
        } else {
            StartAnim(ped, ANIM_ID_OFF_SIT_IN, 8.f);
        }
        return false;
    }
    if (m_bUpdatePedPos) {
        const auto animOffsetOS = currAnimId == ANIM_ID_OFF_SIT_2IDLE_180
            ? -CCarEnterExit::ms_vecPedDeskAnimOffset
            : CCarEnterExit::ms_vecPedDeskAnimOffset;
        const auto animOffsetWS = ped->m_matrix->TransformPoint(animOffsetOS);
        ped->SetPosn({ animOffsetWS.x, animOffsetWS.y, ped->GetPosition().z });
        m_bUpdatePedPos = false;
    }
    if (m_TaskTimer.IsOutOfTime()) {
        if (currAnimId != ANIM_ID_OFF_SIT_2IDLE_180) {
            m_Anim->SetDefaultDeleteCallback();
            m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_INT_OFFICE, ANIM_ID_OFF_SIT_2IDLE_180, 1000.f);
            m_Anim->SetFinishCallback(FinishAnimCB, this);
            m_bUpdatePedPos = true;
        }
    } else if (m_AnimTimer.IsOutOfTime()) {
        m_AnimTimer.Stop();
        if (rand() < 0x3FFF) {
            StartRandomOneOffAnim(ped);
        } else {
            StartRandomLoopAnim(ped, 4.f);
        }
    }
    if (currAnimId == ANIM_ID_OFF_SIT_IN) {
        const auto pedToIntDir = m_InteriorInfo->Pos - ped->GetPosition();
        const auto pedToIntMag = CVector2D{ pedToIntDir.x, pedToIntDir.y }.Magnitude();
        const auto shift = std::min(pedToIntMag, 0.02f) / std::max(pedToIntMag, 0.001f);
        ped->m_vecAnimMovingShiftLocal = { pedToIntDir.x * shift, pedToIntDir.y * shift };
        ped->m_fAimingRotation = m_InteriorInfo->Dir.Heading();
    }
    return false;
}

void CTaskInteriorSitAtDesk::StartAnim(CPed* ped, AnimationId animId, float blendDelta) {
    if (m_Anim) {
        m_Anim->SetDefaultFinishCallback();
    }
    m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_INT_OFFICE, animId, blendDelta);
    m_Anim->SetFinishCallback(FinishAnimCB, this);
}
