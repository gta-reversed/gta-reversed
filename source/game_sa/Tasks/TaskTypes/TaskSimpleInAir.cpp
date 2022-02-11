#include "StdInc.h"

#include "TaskSimpleInAir.h"

#include "TaskComplexInAirAndLand.h"
#include "TaskSimpleClimb.h"

float CTaskSimpleInAir::ms_fSlowFallThreshold = -0.05F;
uint32 CTaskSimpleInAir::ms_nMaxSlowFallFrames = 10;

void CTaskSimpleInAir::InjectHooks()
{
    RH_ScopedClass(CTaskSimpleInAir);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x678CD0);
    RH_ScopedInstall(DeleteAnimCB, 0x678E60);
    RH_ScopedInstall(ProcessPed_Reversed, 0x680600);
    RH_ScopedInstall(MakeAbortable_Reversed, 0x678DC0);
}

CTaskSimpleInAir* CTaskSimpleInAir::Constructor(bool bUsingJumpGlide, bool bUsingFallGlide, bool bUsingClimbJump)
{
    this->CTaskSimpleInAir::CTaskSimpleInAir(bUsingJumpGlide, bUsingFallGlide, bUsingClimbJump);
    return this;
}

// 0x678CD0
CTaskSimpleInAir::CTaskSimpleInAir(bool bUsingJumpGlide, bool bUsingFallGlide, bool bUsingClimbJump) : m_timer()
{
    this->m_bUsingJumpGlide = bUsingJumpGlide;
    this->m_bUsingFallGlide = bUsingFallGlide;
    this->m_bUsingClimbJump = bUsingClimbJump;

    m_pAnim = nullptr;
    m_fMinZSpeed = 0.0F;
    m_nProcessCounter = 0;
    m_pClimbEntity = nullptr;

    m_timer.Start(500);
}

// 0x678D50
CTaskSimpleInAir::~CTaskSimpleInAir()
{
    if (m_pAnim)
        m_pAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);

    if (m_pClimbEntity)
        m_pClimbEntity->CleanUpOldReference(&m_pClimbEntity);
}

// 0x680600
bool CTaskSimpleInAir::ProcessPed(CPed* ped)
{
    return ProcessPed_Reversed(ped);
}

// 0x678DC0
bool CTaskSimpleInAir::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event)
{
    return MakeAbortable_Reversed(ped, priority, event);
}

bool CTaskSimpleInAir::ProcessPed_Reversed(CPed* ped)
{
    CColPoint colPoint{}; // default initialization is NOTSA
    CEntity* colEntity;

    CVector originalPosn = ped->m_matrix->GetPosition();
    float fColDistance = originalPosn.z - 4.0F;

    if (m_timer.IsOutOfTime()
        && ped->m_nPedState != PEDSTATE_ABSEIL_FROM_HELI
        && ped->m_vecMoveSpeed.z * 50.0F <= -20.0F
        )
    {
        ped->Say(ped->IsPlayer() ? 358 : 346, 0, 1.0F, false, false, false);
        m_timer.m_bStarted = false;
    }

    if (!m_pAnim)
    {
        ped->bIsInTheAir = true;
        if (m_bUsingJumpGlide)
        {
            m_pAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_JUMP_GLIDE);
            if (!m_pAnim)
                m_pAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_CLIMB_JUMP);
            if (!m_pAnim || m_pAnim->m_fBlendAmount < 1.0F && m_pAnim->m_fBlendDelta <= 0.0F)
                CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_JUMP_GLIDE, 4.0F);
        }
        else if (m_bUsingFallGlide)
        {
            if (ped->m_vecMoveSpeed.z <= 0.0F)
            {
                m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_FALL_GLIDE, 4.0F);
                m_pAnim->SetDeleteCallback(CTaskSimpleInAir::DeleteAnimCB, this);
                m_pAnim->SetBlend(1.0F, 0.5F);
            }
        }
        else
        {
            m_pAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_CLIMB_JUMP);
            if (!m_pAnim || m_pAnim->m_fBlendAmount < 1.0F && m_pAnim->m_fBlendDelta <= 0.0F)
                CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_FALL_GLIDE, 4.0F);
        }

        if (m_pAnim)
            m_pAnim->SetDeleteCallback(CTaskSimpleInAir::DeleteAnimCB, this);
    }

    if (m_pAnim
        && m_pAnim->m_nAnimId == ANIM_ID_CLIMB_JUMP
        && !ped->bIsStanding
        && m_pAnim->m_fCurrentTime < m_pAnim->m_pHierarchy->m_fTotalTime
        && (m_pAnim->m_fBlendAmount >= 1.0F || m_pAnim->m_fBlendDelta > 0.0F)
        )
    {
        ped->ApplyMoveForce(0.0F, 0.0F, CTimer::GetTimeStep() * ped->m_fMass * 0.35F * GAME_GRAVITY);
    }
    else if (ped->m_vecMoveSpeed.z > 0.0F && !ped->m_vecMoveSpeed.IsZero()
        || !m_bUsingFallGlide && (ped->m_nPedState == PEDSTATE_DIE || ped->m_nPedState == PEDSTATE_DEAD)
        )
    {
        if (!m_bUsingFallGlide)
        {
            if (m_pParentTask && m_pParentTask->m_pParentTask && m_pParentTask->m_pParentTask->GetTaskType() == TASK_COMPLEX_JUMP)
            {
                float moveSpeedForward = DotProduct(ped->GetForward(), ped->m_vecMoveSpeed);
                float maxMoveSpeedForward = 0.1F * 0.5F;
                if (moveSpeedForward < maxMoveSpeedForward && m_nProcessCounter < 1000)
                {
                    ped->ApplyMoveForce(ped->GetForward() * ((maxMoveSpeedForward - moveSpeedForward) * ped->m_fMass));
                    m_nProcessCounter += CTimer::GetTimeStepInMS();
                }
            }
        }
    }
    else
    {
        m_fMinZSpeed = std::min(ped->m_vecMoveSpeed.z, m_fMinZSpeed);

        bool bStopFalling = ped->bIsStanding;
        if (!bStopFalling)
            bStopFalling = CWorld::ProcessVerticalLine(originalPosn, fColDistance, colPoint, colEntity, true, true, false, true, false, false, 0);

        if (m_bUsingFallGlide)
        {
            if (ms_fSlowFallThreshold < ped->m_vecMoveSpeed.z && ped->m_vecMoveSpeed.z <= 0.0F)
            {
                if (++m_nProcessCounter > ms_nMaxSlowFallFrames)
                {
                    ped->bIsStanding = true;
                    bStopFalling = true;
                }
            }
            else
                m_nProcessCounter = 0;
        }

        if (!bStopFalling)
        {
            if (ped->m_nPedState != PEDSTATE_ABSEIL_FROM_HELI && ped->m_vecMoveSpeed.z < -0.1F)
            {
                if (m_pAnim && m_pAnim->m_nAnimId != ANIM_ID_FALL_FALL)
                {
                    m_pAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
                    m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_FALL_FALL, 4.0F);
                    m_pAnim->SetDeleteCallback(CTaskSimpleInAir::DeleteAnimCB, this);
                }
            }
        }
        /*
        * Run-Time Check Failure #3 - The variable 'colPoint' is being used without being initialized.
        * Until *colPoint* will not be initialized in CWorld::ProcessVerticalLine we will be drop here
        */
        else if (ped->m_matrix->GetPosition().z - colPoint.m_vecPoint.z < 1.3F
            || ped->bIsStanding
            || ped->m_pAttachedTo
            )
        {
            if (m_pAnim && m_bUsingFallGlide)
            {
                m_pAnim->m_fBlendDelta = -1000.0F;
                m_pAnim->m_nFlags |= ANIM_FLAG_FREEZE_LAST_FRAME;
                m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
                m_pAnim = nullptr;
            }
            ped->bIsInTheAir = false;
            return true;
        }
    }

    if (m_bUsingJumpGlide && !m_pClimbEntity
        && ped->IsAlive()
        && ped->IsPlayer()
        && CGame::CanSeeOutSideFromCurrArea()
        && ped->m_vecMoveSpeed.z > -0.2F
        && ped->m_vecMoveSpeed.Magnitude2D() > 0.05F)
    {
        if (!(
            m_pParentTask
            && m_pParentTask->GetTaskType() == TASK_COMPLEX_IN_AIR_AND_LAND
            && reinterpret_cast<CTaskComplexInAirAndLand*>(m_pParentTask)->m_bInvalidClimb
            ))
        {
            m_pClimbEntity = CTaskSimpleClimb::TestForClimb(ped, m_vecPosn, m_fAngle, m_nSurfaceType, false);
            if (m_pClimbEntity)
                m_pClimbEntity->RegisterReference(&m_pClimbEntity);
            else if (m_fAngle < -1000.0F && m_pParentTask)
                reinterpret_cast<CTaskComplexInAirAndLand*>(m_pParentTask)->m_bInvalidClimb = true;
        }
    }

    return false;
}

bool CTaskSimpleInAir::MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event)
{
    if (priority == ABORT_PRIORITY_IMMEDIATE
        || priority == ABORT_PRIORITY_URGENT && (
            ped->bIsDrowning
            || event && (event->GetEventType() == EVENT_SCRIPT_COMMAND && event->GetEventPriority() == 71 || event->GetEventType() == EVENT_STUCK_IN_AIR)
            )
        )
    {
        if (m_pAnim)
        {
            m_pAnim->m_fBlendDelta = -8.0F;
            m_pAnim->m_nFlags |= ANIM_FLAG_FREEZE_LAST_FRAME;
            m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_pAnim = nullptr;
        }
        return true;
    }
    else
        return false;
}

// 0x678E60
void CTaskSimpleInAir::DeleteAnimCB(CAnimBlendAssociation* anim, void* data)
{
    auto task = reinterpret_cast<CTaskSimpleInAir*>(data);
    if (task && task->m_pAnim && task->m_pAnim == anim)
        task->m_pAnim = nullptr;
}
