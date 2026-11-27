#include "StdInc.h"
#include "TaskInteriorSitAtDesk.h"
#include "CarEnterExit.h"
#include "Interior/InteriorManager_c.h"
#include "Interior/InteriorGroup_c.h"
#include "Interior/InteriorInfo_t.h"

// 0x676010
CTaskInteriorSitAtDesk::CTaskInteriorSitAtDesk(int32 duration, InteriorInfo_t* interiorInfo, bool doInstantly) :
    m_Duration{ duration },
    m_InteriorInfo{ interiorInfo },
    m_DoInstantly{ doInstantly } {
}

// 0x6760E0
CTaskInteriorSitAtDesk::CTaskInteriorSitAtDesk(const CTaskInteriorSitAtDesk& o) :
    CTaskInteriorSitAtDesk{ o.m_Duration, o.m_InteriorInfo, o.m_DoInstantly } {
}

// 0x676080
CTaskInteriorSitAtDesk::~CTaskInteriorSitAtDesk() {
    if (m_Anim) {
        m_Anim->SetDefaultDeleteCallback();
    }
}

// 0x676190
void CTaskInteriorSitAtDesk::FinishAnimCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = notsa::cast<CTaskInteriorSitAtDesk>(static_cast<CTask*>(data));

    assert(self && anim);
    assert(anim == self->m_Anim);

    self->m_PrevAnimId = anim->GetAnimId();
    if (self->m_PrevAnimId == ANIM_ID_OFF_SIT_2IDLE_180 || self->m_TaskAborting && self->m_PrevAnimId == ANIM_ID_OFF_SIT_IN) {
        anim->SetBlendDelta(-1000.f);
        self->m_TaskFinished = true;
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
            ? ANIM_ID_OFF_SIT_IDLE_LOOP
            : ANIM_ID_OFF_SIT_BORED_LOOP,
        blendDelta
    );
    m_AnimTimer.Start(chance > 40 ? GetRandomNumberInRange(7'000, 12'000) : GetRandomNumberInRange(2'000, 5'000));
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
            m_Anim->SetDefaultDeleteCallback();
            m_Anim = nullptr;
        }
        return true;
    }
    m_TaskAborting = true;
    return false;
}

// 0x677920
bool CTaskInteriorSitAtDesk::ProcessPed(CPed* ped) {
    constexpr auto OFFICE_ANIMS = (int32)(eInteriorGroupType::OFFICE);

    const auto animId           = m_Anim
        ? m_Anim->GetAnimId()
        : ANIM_ID_UNDEFINED;

    ped->SetMoveState(PEDMOVE_STILL);

    if (m_TaskFinished && !RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_OFF_SIT_2IDLE_180)) { // 0x677964
        ped->m_fAimingRotation = ped->m_fCurrentRotation = CGeneral::LimitRadianAngle(ped->m_fCurrentRotation + PI);
        ped->SetHeading(ped->m_fCurrentRotation);
        return true;
    }

    if (m_TaskAborting) { // 0x6779CB
        if (!InteriorManager_c::AreAnimsLoaded(OFFICE_ANIMS)) {
            return true;
        }
        switch (animId) {
        case ANIM_ID_OFF_SIT_IN: // 0x6779E3
            m_Anim->SetBlendDelta(-8.f);
            break;
        case ANIM_ID_OFF_SIT_2IDLE_180: // 0x677A54
            m_Anim->SetSpeed(2.f);
            break;
        case ANIM_ID_OFF_SIT_IDLE_LOOP:
        case ANIM_ID_OFF_SIT_TYPE_LOOP:
        case ANIM_ID_OFF_SIT_BORED_LOOP:
        case ANIM_ID_OFF_SIT_CRASH:
        case ANIM_ID_OFF_SIT_DRINK:
        case ANIM_ID_OFF_SIT_READ:
        case ANIM_ID_OFF_SIT_WATCH: // 0x6779FF
            if (!m_UpdatePedPos) {
                StartAnim(ped, ANIM_ID_OFF_SIT_2IDLE_180, 1000.f);
                m_UpdatePedPos = true;
                return false;
            }
            break;
        default:
            break;
        }
    }

    if (!m_Anim) { // 0x677A69
        if (!InteriorManager_c::AreAnimsLoaded(OFFICE_ANIMS)) {
            return false;
        }
        switch (m_PrevAnimId) {
        case ANIM_ID_UNDEFINED: // 0x677A85
            if (!m_DoInstantly) {
                StartAnim(ped, ANIM_ID_OFF_SIT_IN, 4.f);
                return false;
            }
            [[fallthrough]];
        case ANIM_ID_OFF_SIT_IN: // 0x677AD9
            m_TaskTimer.Start(m_Duration);
            StartRandomLoopAnim(ped, 1000.f);
            m_UpdatePedPos = true;
            return false;
        case ANIM_ID_OFF_SIT_CRASH:
        case ANIM_ID_OFF_SIT_DRINK:
        case ANIM_ID_OFF_SIT_READ:
        case ANIM_ID_OFF_SIT_WATCH: // 0x677B21
            StartRandomLoopAnim(ped, 4.f);
            return false;
        default:
            return false;
        }
    }

    if (m_UpdatePedPos) { // 0x677B45
        const CVector pedPos = ped->GetPosition();
        const CVector newPos = [&] {
            switch (animId) {
            case ANIM_ID_OFF_SIT_IDLE_LOOP:
            case ANIM_ID_OFF_SIT_TYPE_LOOP:
            case ANIM_ID_OFF_SIT_BORED_LOOP: // 0x677B7C
                return ped->GetMatrix().TransformPoint(CCarEnterExit::ms_vecPedDeskAnimOffset);
            case ANIM_ID_OFF_SIT_2IDLE_180: // 0x677B8F
                return ped->GetMatrix().TransformPoint(-CCarEnterExit::ms_vecPedDeskAnimOffset);
            default:
                return pedPos;
            }
        }();
        ped->SetPosn({ newPos.x, newPos.y, pedPos.z });
        m_UpdatePedPos = false;
    }

    if (m_TaskTimer.IsOutOfTime()) { // 0x677BF9
        if (m_Anim->GetAnimId() != ANIM_ID_OFF_SIT_2IDLE_180) {
            StartAnim(ped, ANIM_ID_OFF_SIT_2IDLE_180, 1000.f);
            m_UpdatePedPos = true;
        }
    } else if (m_AnimTimer.IsOutOfTime()) { // 0x677C5C
        m_AnimTimer.Stop();
        if (CGeneral::GetRandomNumber() < RAND_MAX / 2) {
            StartRandomOneOffAnim(ped);
        } else {
            StartRandomLoopAnim(ped, 4.f);
        }
    }

    if (m_Anim->GetAnimId() == ANIM_ID_OFF_SIT_IN) { // 0x677C90
        const CVector pedToInfo        = m_InteriorInfo->Pos - ped->GetPosition();
        const float   dist             = pedToInfo.Magnitude();
        // The original divides by `dist` unchecked: NaN shift when the ped stands exactly on the desk point (0x677D17)
        const CVector shift            = notsa::bugfixes::GenericUB && dist == 0.f
            ? CVector{}
            : pedToInfo * (1.f / dist) * std::min(dist, 0.02f);
        ped->m_vecAnimMovingShiftLocal = {
            shift.Dot(ped->GetRight()),
            shift.Dot(ped->GetForward()),
        };
        // NOTE: Not `Dir.Heading()`, the original replaces y == 0 with a tiny value
        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(m_InteriorInfo->Dir.x, m_InteriorInfo->Dir.y, 0.f, 0.f));
    }

    return false;
}

// NOTSA
void CTaskInteriorSitAtDesk::StartAnim(CPed* ped, AnimationId animId, float blendDelta) {
    if (m_Anim) {
        m_Anim->SetDefaultDeleteCallback();
    }
    m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_INT_OFFICE, animId, blendDelta);
    m_Anim->SetFinishCallback(FinishAnimCB, this);
}

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
