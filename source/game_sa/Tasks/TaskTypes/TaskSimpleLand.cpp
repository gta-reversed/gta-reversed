#include "StdInc.h"

#include "TaskSimpleLand.h"

void CTaskSimpleLand::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleLand, 0x8704FC, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x678E90);
    RH_ScopedInstall(LeftFootLanded, 0x679010);
    RH_ScopedInstall(RightFootLanded, 0x678FE0);
    RH_ScopedInstall(FinishAnimCB, 0x678FA0);
    RH_ScopedVMTInstall(MakeAbortable, 0x678F40);
    RH_ScopedVMTInstall(ProcessPed, 0x67D380);
}

CTaskSimpleLand* CTaskSimpleLand::Constructor(AnimationId nAnimId) {
    this->CTaskSimpleLand::CTaskSimpleLand(nAnimId);
    return this;
}

// 0x678E90
CTaskSimpleLand::CTaskSimpleLand(AnimationId nAnimId) : CTaskSimple() {
    m_nAnimId = nAnimId;
    bIsFinished = false;
    bNoAnimation = (nAnimId == -1);
    bPedNotUpdated = true;
    m_pAnim = nullptr;
}

// 0x678EE0
CTaskSimpleLand::~CTaskSimpleLand() {
    if (m_pAnim) {
        m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x67D380
bool CTaskSimpleLand::ProcessPed(CPed* ped) {
    if (bIsFinished) {
        ped->bIsLanding = false;
        if (!bNoAnimation) {
            AnimationId aAnimIds[3] = { ANIM_ID_WALK, ANIM_ID_RUN, ANIM_ID_SPRINT };
            for (auto animId : aAnimIds) {
                m_pAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, animId);
                if (m_pAnim)
                    m_pAnim->SetCurrentTime(0.0F);
            }

            m_pAnim = nullptr;
        }

        return true;
    } else {
        if (!m_pAnim && !bNoAnimation) {
            m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, m_nAnimId, 100.0F);
            m_pAnim->SetFinishCallback(CTaskSimpleLand::FinishAnimCB, this);
        }

        if (bPedNotUpdated) {
            if (!(ped->GetUp().z == 1.0F && ped->m_vecTurnSpeed.IsZero())) {
                ped->ResetTurnSpeed();
                ped->SetHeading(ped->m_fCurrentRotation);
            }

            ped->bIsLanding = true;

            if (ped->IsPlayer()) {
                assert(m_pAnim); // NOTSA
                if (ped->m_nMoveState == PEDMOVE_SPRINT && ped->AsPlayer()->GetPadFromPlayer()->GetSprint())
                    m_pAnim->m_Speed = 2.0F; // possible bug, m_anim can be null here

                if (m_nAnimId != ANIM_ID_IDLE_TIRED)
                    m_pAnim->m_Speed *= CStats::GetFatAndMuscleModifier(STAT_MOD_2);
            }

            bPedNotUpdated = false;
            if (bNoAnimation)
                bIsFinished = true;
        }

        return false;
    }
}

// 0x678F40
bool CTaskSimpleLand::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE)
        return false;

    if (m_pAnim) {
        m_pAnim->m_BlendDelta = -1000.0F;
        m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr); // doesn't make sense, since there is only one callback function
        m_pAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        m_pAnim = nullptr;
    }

    ped->bIsLanding = false;
    return true;
}

// 0x679010
bool CTaskSimpleLand::LeftFootLanded() {
    return m_pAnim && m_pAnim->m_CurrentTime >= 0.2F && m_pAnim->m_CurrentTime - m_pAnim->m_TimeStep < 0.2F;
}

// 0x678FE0
bool CTaskSimpleLand::RightFootLanded() {
    return m_pAnim && m_pAnim->m_CurrentTime >= 0.1F && m_pAnim->m_CurrentTime - m_pAnim->m_TimeStep < 0.1F;
}

// 0x678FA0
void CTaskSimpleLand::FinishAnimCB(CAnimBlendAssociation* anim, void* data) {
    auto task = reinterpret_cast<CTaskSimpleLand*>(data);
    if (task) {
        task->bIsFinished = true;
        task->m_pAnim = nullptr;
    }

    if (anim && (anim->m_AnimId == ANIM_ID_JUMP_LAND || anim->m_AnimId == ANIM_ID_IDLE_TIRED))
        anim->m_BlendDelta = -100.0F;
}
