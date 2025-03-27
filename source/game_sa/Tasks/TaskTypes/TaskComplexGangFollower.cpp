#include "StdInc.h"

#include "EventPassObject.h"

#include "TaskSimpleRunAnim.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskComplexGangLeader.h"
#include "TaskSimpleHoldEntity.h"
#include "IKChainManager_c.h"
#include "TaskComplexPassObject.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskSimpleGoToPoint.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h"
#include "TaskComplexSeekEntity.h"
#include "TaskComplexSignalAtPed.h"
#include "TaskComplexGangFollower.h"
#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskComplexWanderGang.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimplePause.h"

void CTaskComplexGangFollower::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangFollower, 0x86F938, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65EAA0);
    RH_ScopedInstall(CalculateOffsetPosition, 0x65ED40);
    RH_ScopedInstall(Destructor, 0x65EBB0);
    RH_ScopedVMTInstall(Clone, 0x65ECB0);
    RH_ScopedVMTInstall(MakeAbortable, 0x65EC30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x665E00);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x666160);
    RH_ScopedVMTInstall(ControlSubTask, 0x662A10);
    //RH_ScopedInstall(CalculateOffsetPosition, 0x65ED40, { .reversed = false }); // not hooked because i want to keep CVector return, but original function took a CVector&
}

// 0x65EAA0
CTaskComplexGangFollower::CTaskComplexGangFollower(CPedGroup* pedGrp, CPed* grpLeader, uint8 grpMembershitIdx, CVector followOffset, float targetRadius) :
    m_grp{pedGrp},
    m_leader{grpLeader},
    m_grpMemshitIdx{grpMembershitIdx},
    m_offsetPos{followOffset},
    m_initialOffsetPos{followOffset},
    m_targetRadius{targetRadius}
{
    if (m_leader) {
        CEntity::SafeRegisterRef(m_leader);
        m_leaderInitialPos = m_leader->GetPosition();
    }
}

CTaskComplexGangFollower::CTaskComplexGangFollower(const CTaskComplexGangFollower& o) :
    CTaskComplexGangFollower{
        o.m_grp,
        o.m_leader,
        o.m_grpMemshitIdx,
        o.m_offsetPos,
        o.m_targetRadius
    }
{
    m_followLeader = o.m_followLeader;
}


// 0x65EBB0
CTaskComplexGangFollower::~CTaskComplexGangFollower() {
    CEntity::SafeCleanUpRef(m_leader);
    if (m_animsReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
    }
}

// 0x65ED40
CVector CTaskComplexGangFollower::CalculateOffsetPosition() {
    if (notsa::contains({ PEDMOVE_WALK, PEDMOVE_RUN, PEDMOVE_SPRINT }, m_leader->m_nMoveState)) {
        m_offsetPos                 = m_leader->GetMatrix() * CVector{ CTaskComplexFollowLeaderInFormation::ms_offsets.movingOffsets[m_grpMemshitIdx] };
        m_usingStandingStillOffsets = false;
    } else if (!m_usingStandingStillOffsets || (m_leader->GetPosition() - m_leaderInitialPos).SquaredMagnitude() >= sq(3.f)) {
        m_leaderInitialPos          = m_leader->GetPosition();
        m_offsetPos                 = CVector{ CTaskComplexFollowLeaderInFormation::ms_offsets.movingOffsets[m_grpMemshitIdx] };
        m_usingStandingStillOffsets = true;
    }
    return m_offsetPos;
}

// 0x65EC30
bool CTaskComplexGangFollower::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (m_pSubTask && !m_pSubTask->MakeAbortable(ped, priority, event)) {
        return false;
    }
    ped->bDontAcceptIKLookAts           = false;
    ped->bMoveAnimSpeedHasBeenSetByTask = false;
    return true;
}

// 0x665E00
CTask* CTaskComplexGangFollower::CreateNextSubTask(CPed* ped) {
    const auto sttype = m_pSubTask->GetTaskType();

    if (m_leaveGroup && sttype == TASK_COMPLEX_SIGNAL_AT_PED) {
        m_grp->GetMembership().RemoveMember(ped);
        ped->GetTaskManager().SetTask(new CTaskComplexWanderGang{
            PEDMOVE_WALK,
            CGeneral::RandomNodeHeading(),
            30'000,
            true,
            0.5f
        }, TASK_PRIMARY_DEFAULT);
        ped->bMoveAnimSpeedHasBeenSetByTask = false;
        return nullptr;
    }
    if (   ped->GetIntelligence()->m_AnotherStaticCounter > 30
        || (sttype == TASK_COMPLEX_SEEK_ENTITY && notsa::contains({ PEDMOVE_NONE, PEDMOVE_STILL, PEDMOVE_TURN_L, PEDMOVE_TURN_R }, m_leader->m_nMoveState))
    ) {
        if (m_leader && m_leader->IsPlayer()) {
            m_leader->Say(90, 0, 0.3f);
        }
        return new CTaskSimpleStandStill{ 500 };
    }

    if (notsa::contains({ TASK_SIMPLE_STAND_STILL, TASK_COMPLEX_HANDSIGNAL_ANIM }, sttype)) {
        if (m_leaveGroup) { // Inverted
            if (CGeneral::RandomBool(1.f / 30.f * 100.f)) {
                auto rndMember = m_grp->GetMembership().GetRandom();
                if (rndMember == ped) {
                    rndMember = m_grp->GetMembership().GetLeader();
                }
                if (rndMember) {
                    return new CTaskComplexTurnToFaceEntityOrCoord{ rndMember };
                }
            }
            return new CTaskSimplePause{ 50 };
        } else {
            return new CTaskComplexSignalAtPed{ m_leader, -1, false };
        }
        return nullptr;
    }

    if (sttype == TASK_SIMPLE_CAR_DRIVE) {
        return CreateFirstSubTask(ped);
    }

    if (!m_followLeader) {
        return new CTaskSimpleStandStill{ 500 };
    }

    return new CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>{
        m_leader,
        50'000,
        1'000,
        0.5f,
        5.f,
        2.f,
        false,
        false,
        {m_offsetPos},
        PEDMOVE_SPRINT
    };
}

// 0x666160
CTask* CTaskComplexGangFollower::CreateFirstSubTask(CPed* ped) {
    if (!m_leader) {
        ped->bMoveAnimSpeedHasBeenSetByTask = false;
        return nullptr;
    }

    if (ped->IsInVehicle()) {
        if (ped->m_pVehicle == m_leader->m_pVehicle) {
            return new CTaskSimpleCarDrive{ ped->m_pVehicle };
        }
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, 0, 0, true, false };
    }

    if (m_followLeader && ped->GetIntelligence()->m_AnotherStaticCounter <= 30) {
        return new CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>{
            m_leader,
            50'000,
            1'000,
            0.5f,
            5.f,
            2.f,
            false,
            false,
            {m_offsetPos},
            PEDMOVE_SPRINT
        };
    }

    return new CTaskSimpleStandStill{ 500 };
}

// 0x662A10
CTask* CTaskComplexGangFollower::ControlSubTask(CPed* ped) {
    if (!m_leader) {
        if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            ped->bMoveAnimSpeedHasBeenSetByTask = false;
            return nullptr;
        }
        return m_pSubTask;
    }

    const auto sttype = m_pSubTask->GetTaskType();
    if (sttype == TASK_SIMPLE_CAR_DRIVE) {
        return m_pSubTask;
    }

    const auto walkAnim        = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_WALK);
    const auto isPedMovingFast = notsa::contains({ PEDMOVE_WALK, PEDMOVE_RUN, PEDMOVE_SPRINT }, ped->GetIntelligence()->GetMoveStateFromGoToTask());

    m_offsetPos = CalculateOffsetPosition();

    const auto leaderToPed       = m_leader->GetPosition() - ped->GetPosition();
    float      leaderToPedDist2D;
    const auto leaderToPedDir2D  = CVector2D{ leaderToPed }.Normalized(&leaderToPedDist2D);

    //> 0x662B5B
    if (sttype == TASK_COMPLEX_SEEK_ENTITY && m_followLeader) {
        const auto stSeekEntity = static_cast<CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>*>(m_pSubTask);
        stSeekEntity->GetSeekPosCalculator().SetOffset(m_offsetPos);
        stSeekEntity->SetMinEntityDist2D(2.f);

        //> 0x662BD3
        const auto tGoToPoint = ped->GetTaskManager().Find<CTaskSimpleGoToPoint>();
        bool hasChangedAnimSpeed = false;
        if (tGoToPoint && !ped->GetTaskManager().Find<CTaskComplexFollowNodeRoute>()) {
            auto       goToPoint         = m_leader->GetPosition() + m_offsetPos;   // 0x662BFD
            const auto goToPointToPedDir = CVector2D{ goToPoint - ped->GetPosition() }; // 0x662C1E
            if (isPedMovingFast && m_leader->GetPosition().Dot(m_leader->GetForward()) <= goToPointToPedDir.Dot(m_leader->GetForward()))  { // 0x662C34
                if (goToPointToPedDir.SquaredMagnitude() >= sq(tGoToPoint->m_fRadius)) {
                    if (walkAnim) {
                        const auto prevWalkAnimSpeed = walkAnim->m_fSpeed;
                        ped->SetMoveAnimSpeed(walkAnim);
                        if (std::abs(prevWalkAnimSpeed - prevWalkAnimSpeed) < 0.013f) {
                            walkAnim->m_fSpeed = prevWalkAnimSpeed + std::copysign(0.0125f, walkAnim->m_fSpeed - prevWalkAnimSpeed);
                            hasChangedAnimSpeed = true;
                        }
                    }
                } else { //> 0x662A10
                    goToPoint += m_leader->GetForward() * 2.f;
                    if (walkAnim) {
                        walkAnim->m_fSpeed = std::max(0.85f, walkAnim->m_fSpeed - 0.0125f);
                        hasChangedAnimSpeed = true;
                    }
                }
                tGoToPoint->UpdatePoint(goToPoint); // 0x662D88
            }
        }
        if (ped->GetIntelligence()->m_AnotherStaticCounter > 8 && leaderToPedDist2D <= 8.f && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) { // 0x662D9F
            return new CTaskSimpleStandStill{ 500 };
        }
        ped->bMoveAnimSpeedHasBeenSetByTask = hasChangedAnimSpeed; // 0x662E28
    }

    if (m_animsReferenced) { // 0x662E2E
        if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) { // Loaded, but not needed => unload
            CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
            m_animsReferenced = false;
        }
    } else if (CTaskComplexGangLeader::ShouldLoadGangAnims()) { // Anim is not needed anymore
        const auto idx = CAnimManager::GetAnimationBlockIndex("gangs");
        if (CAnimManager::ms_aAnimBlocks[idx].bLoaded) { // And is loaded, so just add ref
            CAnimManager::AddAnimBlockRef(idx);
        } else { // Not loaded, request it
            CStreaming::RequestModel(IFPToModelId(idx), STREAMING_KEEP_IN_MEMORY);
        }
    }

    if (!m_grp->m_bIsMissionGroup && !m_inPlayersGroup && m_grp->GetMembership().CountMembers() > 3 && CGeneral::RandomBool(1.f / 2000.f * 100.f)) { //> 0x662EA3
        m_leaveGroup = true;
    }

    if (m_exhaleTimer.IsStarted() && m_exhaleTimer.IsOutOfTime() && ped->m_pRwClump) { // 0x662EDD
        RwV3d pos{ 0.f, 0.1f, 0.f };
        if (const auto fx = g_fxMan.CreateFxSystem("exhale", &pos, RwFrameGetMatrix(RpClumpGetFrame(ped->m_pRwClump)), false)) {
            fx->AttachToBone(ped, BONE_HEAD);
            fx->PlayAndKill();
        }
        m_exhaleTimer.Stop();
    }

    if (!ped->IsVisible()) {
        return m_pSubTask;
    }

    if (!g_ikChainMan.IsLooking(ped) && CGeneral::RandomBool(5.f)) { // 0x662F60
        auto rndMember = m_grp->GetMembership().GetRandom();
        if (rndMember == ped) {
            rndMember = m_grp->GetMembership().GetLeader();
        }
        if (rndMember) {
            g_ikChainMan.LookAt(
                "TaskGangFollower",
                ped,
                rndMember,
                CGeneral::GetRandomNumberInRange(3000, 5000),
                BONE_HEAD,
                nullptr,
                true,
                0.15f,
                500,
                3,
                false
            );
        }
    }

    if (ped->IsRunningOrSprinting()) {
        return m_pSubTask;
    }

    if (const auto pedHeldEntity = ped->GetEntityThatThisPedIsHolding()) { // Inverted
        // 0x66302D

        const auto GetPedAnim = [&](AnimationId id) {
            return RpAnimBlendClumpGetAssociation(ped->m_pRwClump, id);
        };

        const auto drnkBrAnim = GetPedAnim(ANIM_ID_DRNKBR_PRTL);
        const auto drnkBrAnimF = GetPedAnim(ANIM_ID_DRNKBR_PRTL_F);

        const auto smkCigAnim = GetPedAnim(ANIM_ID_SMKCIG_PRTL);
        const auto smkCigAnimF = GetPedAnim(ANIM_ID_SMKCIG_PRTL_F);

        if (drnkBrAnim || drnkBrAnimF || smkCigAnim || smkCigAnimF) {
            if (g_ikChainMan.IsLooking(ped)) {
                g_ikChainMan.AbortLookAt(ped, 250);
            }
            ped->bDontAcceptIKLookAts = true;
            if (smkCigAnim && smkCigAnim->m_fCurrentTime < 0.5f || smkCigAnimF && smkCigAnimF->m_fCurrentTime < 0.5f && !m_exhaleTimer.IsStarted()) {
                m_exhaleTimer.Start(2700);
            }
        }

        // 0x663108
        if (ped->GetTaskManager().Has<CTaskComplexPassObject>()) {
            return m_pSubTask;
        }

        // 0x66311B
        if (CGeneral::RandomBool(499.f / 500.f * 100.f)) {
            if (CGeneral::RandomBool(1.f / 100.f * 100.f)) {
                if (const auto holdEntityTask = ped->GetTaskManager().Find<CTaskSimpleHoldEntity>()) {
                    holdEntityTask->PlayAnim(CTaskComplexGangLeader::GetRandomGangAmbientAnim(ped, pedHeldEntity), ANIM_GROUP_GANGS);
                }
            }
            return m_pSubTask;
        }

        if (drnkBrAnim || drnkBrAnimF || smkCigAnim || smkCigAnimF) {
            return m_pSubTask;
        }

        // 0x66315B
        const auto passHeldEntityToPed = CTaskComplexGangLeader::TryToPassObject(ped, m_grp);
        if (!passHeldEntityToPed || passHeldEntityToPed->GetEntityThatThisPedIsHolding() || !ped->GetActiveWeapon().IsTypeMelee()) {
            return m_pSubTask;
        }

        // 0x6631A6
        if (pedHeldEntity->GetModelID() == ModelIndices::MI_GANG_DRINK) {
            if (CGeneral::RandomBool(50.f)) {
                ped->Say(23, 1500);
            } else {
                passHeldEntityToPed->Say(24);
            }
        } else if (pedHeldEntity->GetModelID() == ModelIndices::MI_GANG_SMOKE) {
            if (CGeneral::RandomBool(50.f)) {
                ped->Say(200, 1500);
            } else {
                passHeldEntityToPed->Say(201);
            }
        }

        passHeldEntityToPed->GetEventGroup().Add(CEventPassObject{ ped, true });
        return new CTaskComplexPassObject{ passHeldEntityToPed, true };
    } else if (!ped->IsPlayingHandSignal() && !ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
        const auto rndNum = CGeneral::GetRandomNumberInRange(0, 500); // 0x663301
        if (rndNum <= 50 || rndNum >= 56) {
            const auto pedIsStandingStill = ped->m_nMoveState == PEDMOVE_STILL;
            if (rndNum == 100 && pedIsStandingStill) {
                ped->GetTaskManager().SetTaskSecondary(new CTaskComplexPlayHandSignalAnim{}, TASK_SECONDARY_PARTIAL_ANIM);
            } else {
                ped->GetTaskManager().SetTaskSecondary(
                    new CTaskSimpleRunAnim{ANIM_GROUP_GANGS, CAnimManager::GetRandomGangTalkAnim()},
                    TASK_SECONDARY_PARTIAL_ANIM
                );
            }
            if (pedIsStandingStill) { // 0x66338D and 0x663430
                ped->Say([] {
                    switch (CGeneral::GetRandomNumberInRange(0, 10)) {
                    case 0:
                    case 1:
                    case 2:
                        return 45;
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        return 166;
                    case 8:
                        return 24;
                    case 9:
                        return 201;
                    }
                    NOTSA_UNREACHABLE();
                }());
            }
        }
    }
    return m_pSubTask;
}
