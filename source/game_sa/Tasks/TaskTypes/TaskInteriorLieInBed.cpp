#include "StdInc.h"
#include "TaskInteriorLieInBed.h"

#include "Interior/InteriorInfo_t.h"
#include "Interior/Interior_c.h"
#include "Interior/InteriorManager_c.h"

void CTaskInteriorLieInBed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorLieInBed, 0x870338, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x675E20);
    RH_ScopedInstall(Destructor, 0x675E90);

    RH_ScopedVMTInstall(FinishAnimCB, 0x675FC0);

    RH_ScopedVMTInstall(Clone, 0x675EF0);
    RH_ScopedVMTInstall(GetTaskType, 0x675E80);
    RH_ScopedVMTInstall(MakeAbortable, 0x675F60);
    RH_ScopedVMTInstall(ProcessPed, 0x6772E0);
}

// 0x675E20
CTaskInteriorLieInBed::CTaskInteriorLieInBed(
    int32 duration,
    InteriorInfo_t* intInfo,
    bool rightHandSide,
    bool doInstantly
) :
    m_intInfo{intInfo},
    m_getOutAfterInterval{duration},
    m_instant{doInstantly},
    m_baseAnimId{ rightHandSide ? ANIM_ID_BED_IN_R : ANIM_ID_BED_IN_L },

#ifdef FIX_BUGS
    m_rightHandSide{rightHandSide}
#endif
{   
}

// For `0x675EF0`
CTaskInteriorLieInBed::CTaskInteriorLieInBed(const CTaskInteriorLieInBed& o) :
    CTaskInteriorLieInBed{
        m_getOutAfterInterval,
        m_intInfo,
        m_rightHandSide,
        m_instant
    }
{
}

// 0x675E90
CTaskInteriorLieInBed::~CTaskInteriorLieInBed() {
    if (m_anim) {
        m_anim->SetDefaultFinishCallback();
    }
}

// 0x675FC0
void CTaskInteriorLieInBed::FinishAnimCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskInteriorLieInBed*>(data);

    self->m_prevAnimId = (AnimationId)anim->m_nAnimId;
    if (self->m_prevAnimId == self->GetAnimIdInSeq(AnimSeqIdx::GET_OUT) // Last animation in the sequence
     || self->m_taskAborting && self->m_prevAnimId == (AnimationId)self->m_baseAnimId
    ) { 
        anim->m_fBlendDelta = -1000.f;
        self->m_lastAnimFinished = true;
    }
    self->m_anim = nullptr;
}

// 0x675F60
bool CTaskInteriorLieInBed::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_anim) {
            m_anim->m_fBlendDelta = -1000.f;
            m_anim->SetDefaultFinishCallback();
            m_anim = nullptr;
        }
        ped->GetIntelligence()->GetEventScanner().GetAcquaintanceScanner().SetOnlyScriptPedAllowed();
        return true;
    } else {
        m_taskAborting = true;
        return false;
    }
}

// 0x6772E0
bool CTaskInteriorLieInBed::ProcessPed(CPed* ped) {
    const auto currAnimId = m_anim
        ? (AnimationId)m_anim->m_nAnimId
        : ANIM_ID_UNDEFINED;

    ped->SetMoveState(PEDMOVE_STILL);

    if (m_lastAnimFinished) {
        if (!RpAnimBlendClumpGetAssociation(ped->m_pRwClump, GetAnimIdInSeq(AnimSeqIdx::GET_OUT))) { // Check if last anim has really finished
            ped->GetIntelligence()->GetEventScanner().GetAcquaintanceScanner().SetOnlyScriptPedAllowed();
            return true;
        }
    }

    const auto CreateNextAnim = [&, this](AnimSeqIdx seqIdx, float blendDelta = 1000.f) {
        m_anim->SetDefaultFinishCallback();
        // Set next animation
        // TODO/NOTE: No need to delete the anim here or smth?
        m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_INT_HOUSE, GetAnimIdInSeq(seqIdx), blendDelta);
        m_anim->SetFinishCallback(FinishAnimCB, this);

        m_updatePedPos = true;
    };

    if (m_taskAborting) {
        if (!InteriorManager_c::AreAnimsLoaded(ANIM_GROUP_DEFAULT)) {
            ped->GetIntelligence()->GetEventScanner().GetAcquaintanceScanner().SetOnlyScriptPedAllowed();
            return true;
        }

#ifdef FIX_BUGS
        // It might happen that `MakeAbortable` was
        // called before the animation was created
        // I think.. Even if not, the analyzer
        // will complain of null access if this
        // isn't here :D
        if (!m_anim) {
            NOTSA_UNREACHABLE();
        }
#endif
        if (currAnimId == GetAnimIdInSeq(AnimSeqIdx::GET_IN)) {
            m_anim->m_fBlendDelta = -8.f;
        } else if (currAnimId == GetAnimIdInSeq(AnimSeqIdx::LOOP)) {
            if (!m_updatePedPos) {
                CreateNextAnim(AnimSeqIdx::GET_OUT);
                return false;
            }
        } else if (currAnimId == GetAnimIdInSeq(AnimSeqIdx::GET_OUT)) {
            m_anim->m_fBlendDelta = 3.f;
        }
    }

    if (m_anim) {
        if (m_updatePedPos) {
            const auto animOffsetOS = [&, this] {
                switch (currAnimId) {
                case ANIM_ID_BED_LOOP_L:
                case ANIM_ID_BED_OUT_L:
                    return CCarEnterExit::ms_vecPedBedLAnimOffset;
                case ANIM_ID_BED_LOOP_R:
                case ANIM_ID_BED_OUT_R:
                    return CCarEnterExit::ms_vecPedBedRAnimOffset;
                default:
                    NOTSA_UNREACHABLE();
                }
            }();
            const auto animOffsetWS = *ped->m_matrix * animOffsetOS; // Transform to world space
            ped->SetPosn({ animOffsetWS.x, animOffsetWS.y, ped->GetPosition().z });
            if (currAnimId == GetAnimIdInSeq(AnimSeqIdx::LOOP)) {
                ped->m_fAimingRotation = ped->m_fCurrentRotation = CGeneral::LimitRadianAngle(ped->m_fCurrentRotation + PI);
                ped->SetHeading(ped->m_fCurrentRotation);
            }
        }

        if (m_getOutTimer.IsOutOfTime()) {
            if (currAnimId != GetAnimIdInSeq(AnimSeqIdx::GET_OUT)) {
                CreateNextAnim(AnimSeqIdx::GET_OUT);
            }
        }

        // Update ped's anim shift and rotation
        if (currAnimId != GetAnimIdInSeq(AnimSeqIdx::GET_OUT)) {
            auto pedToIntDir           = m_intInfo->m_position - ped->GetPosition();
            const auto pedToIntMag     = pedToIntDir.NormaliseAndMag();
            const auto pedToIntShiftWS = pedToIntDir * std::min(pedToIntMag, 0.2f);

            // Now transform the point into ped's object space from world space
            // in quite a convoluted way.
            ped->m_vecAnimMovingShiftLocal = {
                pedToIntShiftWS.Dot(ped->GetRight()),
                pedToIntShiftWS.Dot(ped->GetForward()),
            };

            ped->m_fAimingRotation = m_intInfo->m_targetPoint.Heading();
        }
    } else if (InteriorManager_c::AreAnimsLoaded(ANIM_GROUP_DEFAULT)) { // Create animation
        const auto CreateNextAnimAndStartTimer = [&, this](AnimSeqIdx offset, float blendDelta = 1000.f) {
            m_getOutTimer.Start(m_getOutAfterInterval);
            CreateNextAnim(offset, blendDelta);
        };

        if (m_prevAnimId != ANIM_ID_UNDEFINED) {
            if (m_prevAnimId == GetAnimIdInSeq(AnimSeqIdx::GET_IN)) {
                CreateNextAnimAndStartTimer(AnimSeqIdx::LOOP);
            }
        } else {
            ped->GetIntelligence()->GetEventScanner().GetAcquaintanceScanner().TurnOffAllScanners();
            CreateNextAnimAndStartTimer(
                m_instant
                    ? AnimSeqIdx::LOOP
                    : AnimSeqIdx::GET_IN,
                4.f
            );
        }
    }

    return false;
}

// NOTSA
AnimationId CTaskInteriorLieInBed::GetAnimIdInSeq(AnimSeqIdx sequenceIdx) {
    using enum AnimSeqIdx;

    switch (m_baseAnimId) {
    case ANIM_ID_BED_IN_R:
        switch (sequenceIdx) {
        case GET_IN:  return ANIM_ID_BED_IN_R;
        case LOOP:    return ANIM_ID_BED_LOOP_R;
        case GET_OUT: return ANIM_ID_BED_OUT_R;
        default: NOTSA_UNREACHABLE();
        }
    case ANIM_ID_BED_IN_L:
        switch (sequenceIdx) {
        case GET_IN:  return ANIM_ID_BED_IN_L;
        case LOOP:    return ANIM_ID_BED_LOOP_L;
        case GET_OUT: return ANIM_ID_BED_OUT_L;
        default: NOTSA_UNREACHABLE();
        }
    default:
        NOTSA_UNREACHABLE();
    }
}
