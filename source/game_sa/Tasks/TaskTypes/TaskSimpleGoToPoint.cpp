#include "StdInc.h"

#include "TaskSimpleGoToPoint.h"

#include "TaskSimpleStandStill.h"
#include "IKChainManager_c.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleDuck.h"

void CTaskSimpleGoToPoint::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGoToPoint, 0x86fd50, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x667CD0);

    RH_ScopedInstall(UpdatePoint, 0x645700);
    RH_ScopedVMTInstall(Clone, 0x66CC60);
    RH_ScopedVMTInstall(GetTaskType, 0x667D40);
    RH_ScopedVMTInstall(MakeAbortable, 0x667D60);
    RH_ScopedVMTInstall(ProcessPed, 0x66D710);
}

// 0x667CD0
CTaskSimpleGoToPoint::CTaskSimpleGoToPoint(eMoveState moveState, const CVector& targetPoint, float fRadius, bool bMoveTowardsTargetPoint, bool a6) :
    CTaskSimpleGoTo(moveState, targetPoint, fRadius)
{
    m_GoToPointFlags = 0;
    gotoPointFlags.m_bMoveTowardsTargetPoint = bMoveTowardsTargetPoint;
    gotoPointFlags.m_b04 = a6;
}

// 0x667D60
bool CTaskSimpleGoToPoint::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (gotoFlags.m_bIsIKChainSet) {
        if (g_ikChainMan.IsLooking(ped))
            g_ikChainMan.AbortLookAt(ped, 250);
    }
    if (priority != ABORT_PRIORITY_URGENT) {
        if (priority != ABORT_PRIORITY_IMMEDIATE) {
            gotoPointFlags.m_b02 = true;
            return false;
        }
        CTaskSimpleStandStill standStillTask(0, false, false, 8.0f);
        standStillTask.ProcessPed(ped);
    }
    return true;
}

// 0x66D710
bool CTaskSimpleGoToPoint::ProcessPed(CPed* ped) {
    CPlayerPed* player = ped->AsPlayer();
    ped->m_pedIK.bSlopePitch = true;
    if (HasCircledTarget(ped)) {
        if (!gotoPointFlags.m_b05) {
            gotoPointFlags.m_b03 = true;
            if (gotoFlags.m_bIsIKChainSet) {
                if (g_ikChainMan.IsLooking(ped))
                    g_ikChainMan.AbortLookAt(ped, 250);
            }
            return true;
        }
    }
    if (ped->bHasJustLeftCar) {
        ped->GetIntelligence()->GetEventScanner().ScanForEventsNow(*ped, false);
        ped->bHasJustLeftCar = false;
    }
    else if (ped->bHasJustSoughtCover) {
        ped->GetIntelligence()->GetEventScanner().ScanForEventsNow(*ped, true);
        ped->bHasJustSoughtCover = false;
    }
    else {
        CTaskSimpleDuck* pDuckTask = ped->GetIntelligence()->GetTaskDuck(false);
        if (pDuckTask) {
            float fMoveSpeedY = 1.1f;
            if (m_moveState == PEDMOVE_WALK)
                fMoveSpeedY = 0.75f;
            pDuckTask->ControlDuckMove({ 0.0f, -fMoveSpeedY });
        }
        else {
            ped->SetMoveState(static_cast<eMoveState>(m_moveState));
            if (ped->IsPlayer()) {
                if (CPad::GetPad(0)->DisablePlayerControls) {
                    player->SetPlayerMoveBlendRatio(nullptr);
                    player->SetRealMoveAnim();
                }
                else {
                    bool bSprinting = false;
                    CWeaponInfo* pWeaponInfo = CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().m_Type, eWeaponSkill::STD);
                    if (!pWeaponInfo->flags.bHeavy) {
                        auto* task = static_cast<CTaskSimpleHoldEntity*>(ped->GetIntelligence()->GetTaskHold(false));
                        if (!task || !task->m_pAnimBlendAssociation) {
                            CAnimBlendAssocGroup* animGroup = &CAnimManager::GetAssocGroups()[ped->m_nAnimGroup];
                            if (!ped->m_pPlayerData->m_bPlayerSprintDisabled && !g_surfaceInfos.CantSprintOn(ped->m_nContactSurface)) {
                                auto assoc1 = animGroup->GetAnimation(ANIM_ID_RUN);
                                auto assoc2 = animGroup->GetAnimation(ANIM_ID_SPRINT);
                                if (assoc1->m_BlendHier != assoc2->m_BlendHier && player->ControlButtonSprint(SPRINT_GROUND) >= 1.0f)
                                {
                                    ped->SetMoveState(PEDMOVE_SPRINT);
                                    bSprinting = true;
                                }
                            }
                        }
                    }
                    if (!bSprinting && ped->m_nMoveState == PEDMOVE_SPRINT)
                        ped->SetMoveState(PEDMOVE_RUN);
                    if (gotoPointFlags.m_bMoveTowardsTargetPoint)
                        player->SetPlayerMoveBlendRatio(&m_vecTargetPoint);
                    else
                        player->SetPlayerMoveBlendRatio(nullptr);
                    player->SetRealMoveAnim();
                }
            }
            else {
                ped->SetMoveAnim();
            }
        }
        bool bAimingRotationSet = false;
        const CVector& pedPos = ped->GetPosition();
        CVector2D vecDistance = m_vecTargetPoint - pedPos;
        CVector2D vecDirection(vecDistance);
        vecDirection.Normalise();
        CVector2D vecDistanceToTargetFromLastPoint = m_vecTargetPoint - m_vecLastPedPos;
        CVector2D vecDistanceToTargetFromPoint = m_vecTargetPoint - (CTimer::GetTimeStep() * ped->m_vecMoveSpeed + pedPos);
        m_vecLastPedPos = pedPos;
        if ((vecDistance.SquaredMagnitude() >= m_fRadius * m_fRadius
            || gotoPointFlags.m_b05
            || !ped->bIgnoreHeightCheckOnGotoPointTask && fabs(pedPos.z - m_vecTargetPoint.z) >= 2.0f
            || gotoPointFlags.m_b04 && DotProduct2D(vecDistance, vecDistanceToTargetFromLastPoint) >= 0.0f)
            && (gotoPointFlags.m_b04 || DotProduct2D(vecDistance, vecDistanceToTargetFromPoint) > 0.0f || gotoPointFlags.m_b05))
        {
            if (!gotoPointFlags.m_b02) {
                float fAngleInRadians = CGeneral::GetRadianAngleBetweenPoints(vecDirection.x, vecDirection.y, 0.0f, 0.0f);
                ped->m_fAimingRotation = CGeneral::LimitRadianAngle(fAngleInRadians);
                bAimingRotationSet = true;
            }
        }
        else {
            gotoPointFlags.m_b03 = true;
        }
        if (!bAimingRotationSet) {
            QuitIK(ped);
            return true;
        }
    }
    if (gotoFlags.m_bTargetPointUpdated) {
        if (gotoFlags.m_bIsIKChainSet && g_ikChainMan.IsLooking(ped))
            g_ikChainMan.AbortLookAt(ped, 250);
        gotoFlags.m_bTargetPointUpdated = false;
    }
    SetUpIK(ped);
    gotoPointFlags.m_b05 = false;
    return false;
}

// 0x645700
void CTaskSimpleGoToPoint::UpdatePoint(const CVector& targetPosition, float fRadius, bool bDontCheckRadius)
{
    if (bDontCheckRadius || m_vecTargetPoint != targetPosition || m_fRadius != fRadius)
    {
        m_vecTargetPoint = targetPosition;
        m_fRadius = fRadius;

        gotoFlags.m_targetCircledFlags = 0;
        gotoFlags.m_bTargetPointUpdated = true;
    }
}

bool CTaskSimpleGoToPoint::WasTaskSuccessful(CPed* ped) const {
    return gotoPointFlags.m_b03 && std::abs(m_vecTargetPoint.z - ped->GetPosition().z) < 3.f;
}
