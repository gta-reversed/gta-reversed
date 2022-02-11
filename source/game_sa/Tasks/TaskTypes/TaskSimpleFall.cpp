#include "StdInc.h"

#include "TaskSimpleFall.h"

uint32 &CTaskSimpleFall::m_nMaxPlayerDownTime = *reinterpret_cast<uint32*>(0x8D2EF4);

void CTaskSimpleFall::InjectHooks()
{
    RH_ScopedClass(CTaskSimpleFall);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x6782C0);
    RH_ScopedInstall(StartAnim, 0x67CA40);
    RH_ScopedInstall(ProcessFall, 0x6784C0);
    RH_ScopedInstall(FinishFallAnimCB, 0x6786B0);
    RH_ScopedInstall(ProcessPed_Reversed, 0x67FAF0);
    RH_ScopedInstall(MakeAbortable_Reversed, 0x678370);
}

CTaskSimpleFall* CTaskSimpleFall::Constructor(AnimationId nAnimId, AssocGroupId nAnimGroup, int32 nDownTime)
{
    this->CTaskSimpleFall::CTaskSimpleFall(nAnimId, nAnimGroup, nDownTime);
    return this;
}

// 0x6782C0
CTaskSimpleFall::CTaskSimpleFall(AnimationId nAnimId, AssocGroupId nAnimGroup, int32 nDownTime)
{
    m_nAnimId = nAnimId;
    m_nAnimGroup = nAnimGroup;
    m_nTotalDownTime = nDownTime;
    m_nCurrentDownTime = nDownTime;

    m_bIsFinished = false;
    m_pAnim = nullptr;
}

// 0x678310
CTaskSimpleFall::~CTaskSimpleFall()
{
    if (m_pAnim)
        m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
}

// 0x67FAF0
bool CTaskSimpleFall::ProcessPed(CPed* ped)
{
    return ProcessPed_Reversed(ped);
}

// 0x678370
bool CTaskSimpleFall::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event)
{
    return MakeAbortable_Reversed(ped, priority, event);
}

bool CTaskSimpleFall::ProcessPed_Reversed(CPed* ped)
{
    ped->m_pedIK.bSlopePitch = true;
    ped->bFallenDown = true;

    if (m_bIsFinished && (ped->bIsStanding || ped->bIsDrowning))
    {
        uint32 nTimeStep = CTimer::GetTimeStepInMS();
        if (m_nCurrentDownTime <= nTimeStep)
        {
            m_nCurrentDownTime = 0;
            return true;
        }

        m_nCurrentDownTime -= nTimeStep;

        if (m_nCurrentDownTime > m_nMaxPlayerDownTime
            && ped->IsPlayer()
            && !ped->bIsBeingArrested
            && ped->m_nPedState != PEDSTATE_ARRESTED
            && ped->AsPlayer()->GetPadFromPlayer()
            && !ped->AsPlayer()->GetPadFromPlayer()->DisablePlayerControls
            )
        {
            m_nCurrentDownTime = m_nMaxPlayerDownTime - nTimeStep;
            return false;
        }
    }
    else
    {
        if (!m_pAnim && !m_bIsFinished)
            StartAnim(ped);

        ProcessFall(ped);
    }

    return false;
}

bool CTaskSimpleFall::MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event)
{
    auto pFallAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_FALL_FRONT);
    if (!pFallAnim)
        pFallAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_FALL_BACK);

    if (priority == ABORT_PRIORITY_IMMEDIATE)
    {
        auto pThisAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, m_nAnimId);

        if (pThisAnim)
            pThisAnim->m_fBlendDelta = -1000.0F;

        if (pFallAnim)
            pFallAnim->m_fBlendDelta = -1000.0F;

        m_bIsFinished = true;
        m_nCurrentDownTime = 0;
        return true;
    }

    if (priority == ABORT_PRIORITY_URGENT)
    {
        if (event)
        {
            auto eventType = event->GetEventType();
            if (eventType == EVENT_DAMAGE && ped->m_fHealth < 1.0F
                || eventType == EVENT_IN_WATER
                || event->GetEventPriority() == 71
                || eventType == EVENT_STUCK_IN_AIR
                )
            {
                float fBlendDelta = -2.0F;
                if (eventType == EVENT_IN_WATER || eventType == EVENT_STUCK_IN_AIR)
                    fBlendDelta = -8.0F;

                if (pFallAnim)
                    pFallAnim->m_fBlendDelta = fBlendDelta;

                if (m_pAnim)
                {
                    m_pAnim->m_fBlendDelta = fBlendDelta;
                    m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
                }

                m_pAnim = nullptr;
                m_bIsFinished = true;
                m_nCurrentDownTime = 0;

                return true;
            }
        }
    }

    return false;
}

// 0x67CA40
bool CTaskSimpleFall::StartAnim(CPed* ped)
{
    if (ped->m_pAttachedTo)
        return false;

    if (m_nAnimId == ANIM_ID_NO_ANIMATION_SET)
    {
        m_pAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_BIKE_FALL_OFF);
        if (!m_pAnim)
            m_pAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_BIKE_FALLR);

        if (m_pAnim)
            m_pAnim->SetFinishCallback(CTaskSimpleFall::FinishFallAnimCB, this);
    }
    else
    {
        m_pAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, m_nAnimId);
        if (m_pAnim)
        {
            m_pAnim->Start(0.0F);
            m_pAnim->SetBlend(0.0F, 8.0F);
            m_pAnim->m_nFlags |= ANIM_FLAG_FREEZE_LAST_FRAME;
            m_pAnim->m_nFlags &= ~ANIM_FLAG_UNLOCK_LAST_FRAME;
            m_pAnim->SetFinishCallback(CTaskSimpleFall::FinishFallAnimCB, this);
        }
        else
        {
            m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, m_nAnimGroup, m_nAnimId, 8.0F);
            m_pAnim->m_nFlags |= ANIM_FLAG_FREEZE_LAST_FRAME;
            m_pAnim->m_nFlags &= ~ANIM_FLAG_UNLOCK_LAST_FRAME;
            m_pAnim->SetFinishCallback(CTaskSimpleFall::FinishFallAnimCB, this);
            if (m_nAnimId == ANIM_ID_BIKE_FALLR)
                m_pAnim->SetCurrentTime(0.4F);
        }
    }

    return true;
}

// 0x6784C0
void CTaskSimpleFall::ProcessFall(CPed* ped)
{
    if (ped->IsPlayer()
        && (ped->bKnockedUpIntoAir || ped->bKnockedOffBike)
        && !ped->bIsStanding
        )
    {
        CAnimBlendAssociation* anim;
        auto pFirstAnim = RpAnimBlendClumpGetFirstAssociation(ped->m_pRwClump, ANIM_FLAG_PARTIAL);

        if (pFirstAnim && (pFirstAnim->m_nAnimId == ANIM_ID_FALL_BACK || pFirstAnim->m_nAnimId == ANIM_ID_FALL_FRONT))
            anim = pFirstAnim;
        else
            anim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_FALL_BACK);

        if (!anim)
            anim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_FALL_FRONT);

        if (anim)
        {
            if (anim->m_fBlendAmount > 0.3F
                && anim->m_fBlendDelta >= 0.0F
                && anim->m_fCurrentTime > 0.667F
                && anim->m_fCurrentTime - anim->m_fTimeStep <= 0.667F
                )
            {
                anim->Start(0.0F);
            }
        }
        else
        {
            if (pFirstAnim && pFirstAnim->m_fCurrentTime > pFirstAnim->m_pHierarchy->m_fTotalTime * 0.8F)
                CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, pFirstAnim->m_nFlags & ANIM_FLAG_800 ? ANIM_ID_FALL_FRONT : ANIM_ID_FALL_BACK, 8.0F);
        }
    }
    else if ((ped->bKnockedUpIntoAir || ped->bKnockedOffBike)
        && ped->bIsStanding
        && !ped->bWasStanding)
    {
        auto anim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_FALL_BACK);
        if (!anim)
            anim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_FALL_FRONT);

        if (anim)
        {
            ped->bKnockedUpIntoAir = false;
            anim->m_fSpeed = 3.0F;
        }
        else
        {
            auto pFirstAnim = RpAnimBlendClumpGetFirstAssociation(ped->m_pRwClump, ANIM_FLAG_PARTIAL);
            if (pFirstAnim && !(pFirstAnim->m_nFlags & ANIM_FLAG_STARTED))
                ped->bKnockedUpIntoAir = false;
        }
    }
}

// 0x6786B0
void CTaskSimpleFall::FinishFallAnimCB(CAnimBlendAssociation* anim, void* data)
{
    CTaskSimpleFall* task = reinterpret_cast<CTaskSimpleFall*>(data);
    task->m_pAnim = nullptr;
    task->m_bIsFinished = true;
}

