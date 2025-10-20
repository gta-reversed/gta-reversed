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
    m_PedGroup{pedGrp},
    m_Leader{grpLeader},
    m_GrpMemIdx{grpMembershitIdx},
    m_OffsetPos{followOffset},
    m_InitialOffsetPos{followOffset},
    m_TargetRadius{targetRadius}
{
    if (m_Leader) {
        CEntity::SafeRegisterRef(m_Leader);
        m_LeaderInitialPos = m_Leader->GetPosition();
    }
}

CTaskComplexGangFollower::CTaskComplexGangFollower(const CTaskComplexGangFollower& o) :
    CTaskComplexGangFollower{
        o.m_PedGroup,
        o.m_Leader,
        o.m_GrpMemIdx,
        o.m_OffsetPos,
        o.m_TargetRadius
    }
{
    m_FollowLeader = o.m_FollowLeader;
}


// 0x65EBB0
CTaskComplexGangFollower::~CTaskComplexGangFollower() {
    CEntity::SafeCleanUpRef(m_Leader);
    if (m_AnimsRef) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
    }
}

// 0x65ED40
CVector CTaskComplexGangFollower::CalculateOffsetPosition() {
    if (notsa::contains({ PEDMOVE_WALK, PEDMOVE_RUN, PEDMOVE_SPRINT }, m_Leader->m_nMoveState)) {
        m_OffsetPos                 = m_Leader->GetMatrix() * CVector{ CTaskComplexFollowLeaderInFormation::ms_offsets.movingOffsets[m_GrpMemIdx] };
        m_IsUsingStandingStillOffsets = false;
    } else if (!m_IsUsingStandingStillOffsets || (m_Leader->GetPosition() - m_LeaderInitialPos).SquaredMagnitude() >= sq(3.f)) {
        m_LeaderInitialPos          = m_Leader->GetPosition();
        m_OffsetPos                 = CVector{ CTaskComplexFollowLeaderInFormation::ms_offsets.movingOffsets[m_GrpMemIdx] };
        m_IsUsingStandingStillOffsets = true;
    }
    return m_OffsetPos;
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

    if (m_LeaveGroup && sttype == TASK_COMPLEX_SIGNAL_AT_PED) {
        m_PedGroup->GetMembership().RemoveMember(ped);
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
        || (sttype == TASK_COMPLEX_SEEK_ENTITY && notsa::contains({ PEDMOVE_NONE, PEDMOVE_STILL, PEDMOVE_TURN_L, PEDMOVE_TURN_R }, m_Leader->m_nMoveState))
    ) {
        if (m_Leader && m_Leader->IsPlayer()) {
            m_Leader->Say(CTX_GLOBAL_FOLLOW_ARRIVE, 0, 0.3f);
        }
        return new CTaskSimpleStandStill{ 500 };
    }

    if (notsa::contains({ TASK_SIMPLE_STAND_STILL, TASK_COMPLEX_HANDSIGNAL_ANIM }, sttype)) {
        if (m_LeaveGroup) { // Inverted
            if (CGeneral::RandomBool(1.f / 30.f * 100.f)) {
                auto rndMember = m_PedGroup->GetMembership().GetRandom();
                if (rndMember == ped) {
                    rndMember = m_PedGroup->GetMembership().GetLeader();
                }
                if (rndMember) {
                    return new CTaskComplexTurnToFaceEntityOrCoord{ rndMember };
                }
            }
            return new CTaskSimplePause{ 50 };
        } else {
            return new CTaskComplexSignalAtPed{ m_Leader, -1, false };
        }
        return nullptr;
    }

    if (sttype == TASK_SIMPLE_CAR_DRIVE) {
        return CreateFirstSubTask(ped);
    }

    if (!m_FollowLeader) {
        return new CTaskSimpleStandStill{ 500 };
    }

    return new CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>{
        m_Leader,
        50'000,
        1'000,
        0.5f,
        5.f,
        2.f,
        false,
        false,
        {m_OffsetPos},
        PEDMOVE_SPRINT
    };
}

// 0x666160
CTask* CTaskComplexGangFollower::CreateFirstSubTask(CPed* ped) {
    if (!m_Leader) {
        ped->bMoveAnimSpeedHasBeenSetByTask = false;
        return nullptr;
    }

    if (ped->IsInVehicle()) {
        if (ped->m_pVehicle == m_Leader->m_pVehicle) {
            return new CTaskSimpleCarDrive{ ped->m_pVehicle };
        }
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, 0, 0, true, false };
    }

    if (m_FollowLeader && ped->GetIntelligence()->m_AnotherStaticCounter <= 30) {
        return new CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>{
            m_Leader,
            50'000,
            1'000,
            0.5f,
            5.f,
            2.f,
            false,
            false,
            {m_OffsetPos},
            PEDMOVE_SPRINT
        };
    }

    return new CTaskSimpleStandStill{ 500 };
}

// 0x662A10
CTask* CTaskComplexGangFollower::ControlSubTask(CPed* ped) {
    if (!m_Leader) {
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

    m_OffsetPos = CalculateOffsetPosition();

    const auto leaderToPed       = m_Leader->GetPosition() - ped->GetPosition();
    float      leaderToPedDist2D;
    const auto leaderToPedDir2D  = CVector2D{ leaderToPed }.Normalized(&leaderToPedDist2D);

    //> 0x662B5B
    if (sttype == TASK_COMPLEX_SEEK_ENTITY && m_FollowLeader) {
        const auto stSeekEntity = static_cast<CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>*>(m_pSubTask);
        stSeekEntity->GetSeekPosCalculator().SetOffset(m_OffsetPos);
        stSeekEntity->SetEntityMinDist2D(2.f);

        //> 0x662BD3
        const auto tGoToPoint = ped->GetTaskManager().Find<CTaskSimpleGoToPoint>();
        bool hasChangedAnimSpeed = false;
        if (tGoToPoint && !ped->GetTaskManager().Find<CTaskComplexFollowNodeRoute>()) {
            auto       goToPoint         = m_Leader->GetPosition() + m_OffsetPos;   // 0x662BFD
            const auto goToPointToPedDir = CVector2D{ goToPoint - ped->GetPosition() }; // 0x662C1E
            if (isPedMovingFast && m_Leader->GetPosition().Dot(m_Leader->GetForward()) <= goToPointToPedDir.Dot(m_Leader->GetForward()))  { // 0x662C34
                if (goToPointToPedDir.SquaredMagnitude() >= sq(tGoToPoint->m_fRadius)) {
                    if (walkAnim) {
                        const auto prevWalkAnimSpeed = walkAnim->GetSpeed();
                        ped->SetMoveAnimSpeed(walkAnim);
                        if (std::abs(prevWalkAnimSpeed - prevWalkAnimSpeed) < 0.013f) {
                            walkAnim->SetSpeed(prevWalkAnimSpeed + std::copysign(0.0125f, walkAnim->GetSpeed() - prevWalkAnimSpeed));
                            hasChangedAnimSpeed = true;
                        }
                    }
                } else { //> 0x662A10
                    goToPoint += m_Leader->GetForward() * 2.f;
                    if (walkAnim) {
                        walkAnim->SetSpeed(std::max(0.85f, walkAnim->GetSpeed() - 0.0125f));
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

    m_AnimsRef = CAnimManager::StreamAnimBlock("gangs", CTaskComplexGangLeader::ShouldLoadGangAnims(), m_AnimsRef);

    //if (m_AnimsRef) { // 0x662E2E
    //    if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) { // Loaded, but not needed => unload
    //        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
    //        m_AnimsRef = false;
    //    }
    //} else if (CTaskComplexGangLeader::ShouldLoadGangAnims()) { // Anim is not needed anymore
    //    const auto idx = CAnimManager::GetAnimationBlockIndex("gangs");
    //    if (CAnimManager::ms_aAnimBlocks[idx].IsLoaded) { // And is loaded, so just add ref
    //        CAnimManager::AddAnimBlockRef(idx);
    //    } else { // Not loaded, request it
    //        CStreaming::RequestModel(IFPToModelId(idx), STREAMING_KEEP_IN_MEMORY);
    //    }
    //}

    if (!m_PedGroup->m_bIsMissionGroup && !m_IsInPlayersGroup && m_PedGroup->GetMembership().CountMembers() > 3 && CGeneral::RandomBool(1.f / 2000.f * 100.f)) { //> 0x662EA3
        m_LeaveGroup = true;
    }

    if (m_ExhaleTimer.IsStarted() && m_ExhaleTimer.IsOutOfTime() && ped->m_pRwClump) { // 0x662EDD
        RwV3d pos{ 0.f, 0.1f, 0.f };
        if (const auto fx = g_fxMan.CreateFxSystem("exhale", pos, RwFrameGetMatrix(RpClumpGetFrame(ped->m_pRwClump)), false)) {
            fx->AttachToBone(ped, BONE_HEAD);
            fx->PlayAndKill();
        }
        m_ExhaleTimer.Stop();
    }

    if (!ped->IsVisible()) {
        return m_pSubTask;
    }

    if (!g_ikChainMan.IsLooking(ped) && CGeneral::RandomBool(5.f)) { // 0x662F60
        auto rndMember = m_PedGroup->GetMembership().GetRandom();
        if (rndMember == ped) {
            rndMember = m_PedGroup->GetMembership().GetLeader();
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
            if (smkCigAnim && smkCigAnim->GetCurrentTime() < 0.5f || smkCigAnimF && smkCigAnimF->GetCurrentTime() < 0.5f && !m_ExhaleTimer.IsStarted()) {
                m_ExhaleTimer.Start(2700);
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
        const auto passHeldEntityToPed = CTaskComplexGangLeader::TryToPassObject(ped, m_PedGroup);
        if (!passHeldEntityToPed || passHeldEntityToPed->GetEntityThatThisPedIsHolding() || !ped->GetActiveWeapon().IsTypeMelee()) {
            return m_pSubTask;
        }

        // 0x6631A6
        if (pedHeldEntity->GetModelID() == ModelIndices::MI_GANG_DRINK) {
            if (CGeneral::RandomBool(50.f)) {
                ped->Say(CTX_GLOBAL_BOOZE_RECEIVE, 1500);
            } else {
                passHeldEntityToPed->Say(CTX_GLOBAL_BOOZE_REQUEST);
            }
        } else if (pedHeldEntity->GetModelID() == ModelIndices::MI_GANG_SMOKE) {
            if (CGeneral::RandomBool(50.f)) {
                ped->Say(CTX_GLOBAL_SPLIFF_RECEIVE, 1500);
            } else {
                passHeldEntityToPed->Say(CTX_GLOBAL_SPLIFF_REQUEST);
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
                    case 2: return CTX_GLOBAL_CHAT;
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7: return CTX_GLOBAL_PCONV_GREET_MALE;
                    case 8: return CTX_GLOBAL_BOOZE_REQUEST;
                    case 9: return CTX_GLOBAL_SPLIFF_REQUEST;
                    }
                    NOTSA_UNREACHABLE();
                }());
            }
        }
    }
    return m_pSubTask;
}
