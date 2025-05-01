#include "StdInc.h"

#include "TaskSimpleGoToPointFine.h"
#include "TaskSimpleDuck.h"

void CTaskSimpleGoToPointFine::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGoToPointFine, 0x86F964, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x65EEB0);
    RH_ScopedVMTInstall(Clone, 0x662040);
    RH_ScopedVMTInstall(MakeAbortable, 0x663500);
    RH_ScopedVMTInstall(ProcessPed, 0x663540);
    RH_ScopedInstall(SetBlendedMoveAnim, 0x65EF80);
    RH_ScopedInstall(Finish, 0x65EF00);
    RH_ScopedInstall(SetTargetPos, 0x65F330);
    RH_ScopedInstall(BaseRatio, 0x65EF30);
}

// 0x65EEB0
CTaskSimpleGoToPointFine::CTaskSimpleGoToPointFine(float moveRatio, CVector targetPoint, float fRadius, CEntity* entity) : CTaskSimpleGoTo(PEDMOVE_WALK, targetPoint, fRadius) {
    SetMoveRatio(moveRatio);
}

// 0x65EEB0
CTaskSimpleGoToPointFine* CTaskSimpleGoToPointFine::Constructor(float moveRatio, CVector targetPoint, float fRadius, CEntity* entity) {
    this->CTaskSimpleGoToPointFine::CTaskSimpleGoToPointFine(moveRatio, targetPoint, fRadius, entity);
    return this;
}

// 0x662040
CTask* CTaskSimpleGoToPointFine::Clone() const {
    return new CTaskSimpleGoToPointFine(m_fMoveRatio, m_vecTargetPoint, m_fRadius, nullptr);
}

// 0x663500
bool CTaskSimpleGoToPointFine::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    QuitIK(ped);
    Finish(ped);
    return true;
}

// 0x65EF00
void CTaskSimpleGoToPointFine::Finish(CPed* ped) {
    ped->SetMoveState(PEDMOVE_STILL);
    ped->SetMoveAnim();
    ped->SetMoveState(static_cast<eMoveState>(m_moveState));
    ped->SetMoveAnim();
}

// 0x663540
bool CTaskSimpleGoToPointFine::ProcessPed(CPed* ped) {
    CVector2D vecDistance = m_vecTargetPoint - ped->GetPosition();
    if (m_fRadius * m_fRadius >= vecDistance.SquaredMagnitude() || HasCircledTarget(ped)) {
        QuitIK(ped);
        Finish(ped);
        return true;
    }
    SetBlendedMoveAnim(ped);
    const float fAngleInRadians = CGeneral::GetRadianAngleBetweenPoints(vecDistance.x, vecDistance.y, 0.0f, 0.0f);
    ped->m_fAimingRotation = CGeneral::LimitRadianAngle(fAngleInRadians);
    SetUpIK(ped);
    return false;
}

// todo: IDA code, BaseRatio can be used
// 0x65EF80
void CTaskSimpleGoToPointFine::SetBlendedMoveAnim(CPed* ped) {
    auto idleAnimAssoc = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_IDLE);
    auto walkAnimAssoc = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_WALK);
    auto runAnimAssoc = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_RUN);
    auto sprintAnimAssoc = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_SPRINT);
    auto pIdleTiredAnimAssoc = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_IDLE_TIRED);

    if (ped->bIsDucking && ped->m_pIntelligence->GetTaskDuck(false)) {
        float fMoveSpeedY = m_fMoveRatio * 0.5f;
        fMoveSpeedY = std::min(fMoveSpeedY, 1.0f);
        CTaskSimpleDuck* duckTask = ped->m_pIntelligence->GetTaskDuck(false);
        duckTask->ControlDuckMove({ 0.0f, -fMoveSpeedY });
        return;
    }

    if (ped->bResetWalkAnims) {
        if (walkAnimAssoc)
            walkAnimAssoc->SetCurrentTime(0.0f);
        if (runAnimAssoc)
            runAnimAssoc->SetCurrentTime(0.0f);
        if (sprintAnimAssoc)
            sprintAnimAssoc->SetCurrentTime(0.0f);
        ped->bResetWalkAnims = false;
    }

    if (m_fMoveRatio == 0.0f) {
        if (!idleAnimAssoc) {
            CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, 4.0f);
        }

        delete walkAnimAssoc;
        delete runAnimAssoc;
        delete sprintAnimAssoc;
        ped->m_nMoveState = PEDMOVE_STILL;
        m_moveState = PEDMOVE_STILL;
        return;
    }

    if (idleAnimAssoc) {
        if (walkAnimAssoc)
            walkAnimAssoc->SetCurrentTime(0.0f);
        if (runAnimAssoc)
            runAnimAssoc->SetCurrentTime(0.0f);
        delete idleAnimAssoc;
    }

    if (pIdleTiredAnimAssoc) {
        pIdleTiredAnimAssoc->m_BlendDelta = -4.0f;
        delete pIdleTiredAnimAssoc;
    }

    eMoveState moveState = PEDMOVE_WALK;
    if (m_fMoveRatio > 1.0f) {
        if (m_fMoveRatio > 2.0f) {
            if (m_fMoveRatio > 3.0f)
                return;

            delete walkAnimAssoc;
            if (!runAnimAssoc) {
                runAnimAssoc = CAnimManager::AddAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_RUN);
                runAnimAssoc->m_BlendAmount = 0.0f;
                runAnimAssoc->m_Speed = 1.0f;
            }
            runAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
            runAnimAssoc->m_BlendDelta = 0.0f;
            runAnimAssoc->m_BlendAmount = 3.0f - m_fMoveRatio;
            if (!sprintAnimAssoc) {
                sprintAnimAssoc = CAnimManager::AddAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_SPRINT);
                sprintAnimAssoc->m_BlendAmount = 0.0f;
                sprintAnimAssoc->m_Speed = 1.0f;
            }
            sprintAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
            sprintAnimAssoc->m_BlendDelta = 0.0f;
            moveState = PEDMOVE_SPRINT;
            sprintAnimAssoc->m_BlendAmount = m_fMoveRatio - 2.0f;
        } else {
            if (!walkAnimAssoc) {
                walkAnimAssoc = CAnimManager::AddAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_WALK);
                walkAnimAssoc->m_BlendAmount = 0.0f;
                walkAnimAssoc->m_Speed = 1.0f;
            }
            walkAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
            walkAnimAssoc->m_BlendDelta = 0.0f;
            walkAnimAssoc->m_BlendAmount = 2.0f - m_fMoveRatio;
            if (!runAnimAssoc) {
                runAnimAssoc = CAnimManager::AddAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_RUN);
                runAnimAssoc->m_BlendAmount = 0.0f;
                runAnimAssoc->m_Speed = 1.0f;
            }
            runAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
            runAnimAssoc->m_BlendDelta = 0.0f;
            runAnimAssoc->m_BlendAmount = m_fMoveRatio - 1.0f;
            delete sprintAnimAssoc;
            moveState = PEDMOVE_RUN;
        }
    } else {
        if (!walkAnimAssoc) {
            walkAnimAssoc = CAnimManager::AddAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_WALK);
            walkAnimAssoc->m_BlendAmount = 0.0f;
            walkAnimAssoc->m_Speed = 1.0f;
        }
        walkAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
        walkAnimAssoc->m_BlendAmount = 1.0f;
        walkAnimAssoc->m_BlendDelta = 0.0f;
        delete runAnimAssoc;
        delete sprintAnimAssoc;
    }
    ped->SetMoveState(moveState);
    ped->m_nSwimmingMoveState = moveState;
    m_moveState = moveState;
}

// 0x65F330
void CTaskSimpleGoToPointFine::SetTargetPos(CVector posn) {
    if (m_vecTargetPoint != posn || m_fRadius != 0.5f) {
        m_fRadius = 0.5f;
        m_vecTargetPoint = posn;
        m_GoToFlags &= 0xD0 | 0x20; // todo: flags
    }
}

// 0x65EF30
float CTaskSimpleGoToPointFine::BaseRatio(eMoveState moveState) {
    switch (moveState) {
    case PEDMOVE_WALK:
        return 1.0f;
    case PEDMOVE_RUN:
        return 2.0f;
    case PEDMOVE_SPRINT:
        return 3.0f;
    default:
        return 0.0f;
    }
}
