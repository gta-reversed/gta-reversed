#include "StdInc.h"
#include "Ragdoll/IKChainManager.h"
#include "ModelIndices.h"
#include "EventPassObject.h"
#include "EventGroupEvent.h"
#include "EventLeanOnVehicle.h"
#include "TaskComplexBeInGroup.h"
#include "TaskComplexPassObject.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleRunAnim.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexWanderGang.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"

/// NOTSA - All th gang talk anims
constexpr AnimationId s_gangTalkAnims[]{
    ANIM_ID_PRTIAL_GNGTLKA,
    ANIM_ID_PRTIAL_GNGTLKB,
    ANIM_ID_PRTIAL_GNGTLKC,
    ANIM_ID_PRTIAL_GNGTLKD,

    ANIM_ID_PRTIAL_GNGTLKE,
    ANIM_ID_PRTIAL_GNGTLKF,
    ANIM_ID_PRTIAL_GNGTLKG,
    ANIM_ID_PRTIAL_GNGTLKH,
};

// 0x65DED0
CTaskComplexGangLeader::CTaskComplexGangLeader(CPedGroup* gang) :
    m_Gang{ gang } {
}

// NOTSA
CTaskComplexGangLeader::CTaskComplexGangLeader(const CTaskComplexGangLeader& o) :
    CTaskComplexGangLeader{ o.m_Gang } {
}

// 0x65DF30
CTaskComplexGangLeader::~CTaskComplexGangLeader() {
    if (m_AnimsReferenced) {
        UnrefAnimBlock();
    }
}

// 0x65E730
AnimationId CTaskComplexGangLeader::GetRandomGangAmbientAnim(CPed* ped, CEntity* entity) {
    const auto UseFemaleAnims = [ped] {
        return ped->GetModelIndex() == MODEL_BALLAS2 || ped->GetModelIndex() == MODEL_FAM1;
    };

    if (entity) {
        if (entity->GetModelIndex() == ModelIndices::MI_GANG_DRINK) { // 0x65E74B
            if (CGeneral::DoCoinFlip()) {
                return CGeneral::RandomChoice(s_gangTalkAnims | rng::views::take(4));
            }
            return UseFemaleAnims() ? ANIM_ID_DRNKBR_PRTL_F : ANIM_ID_DRNKBR_PRTL;
        }

        if (entity->GetModelIndex() == ModelIndices::MI_GANG_SMOKE) { // 0x65E791
            if (CGeneral::DoCoinFlip()) {
                return CGeneral::RandomChoice(s_gangTalkAnims);
            }
            return UseFemaleAnims() ? ANIM_ID_SMKCIG_PRTL_F : ANIM_ID_SMKCIG_PRTL; // 0x65E710
        }
    }

    return CGeneral::RandomChoice(s_gangTalkAnims); // 0x65E7BD
}

// 0x65E7F0
bool CTaskComplexGangLeader::ShouldLoadGangAnims() {
    if (CStreaming::IsVeryBusy()) {
        return false;
    }

    const auto* const veh = FindPlayerPed()->GetVehicleIfInOne();
    return !veh || veh->GetMoveSpeed().SquaredMagnitude() <= sq(0.2f);
}

// 0x65E860
void CTaskComplexGangLeader::DoGangAbuseSpeech(CPed* talker, CPed* sayTo) {
    if (!talker->IsGangster()) {
        return;
    }

    if (!sayTo->IsGangster() && !sayTo->IsPlayer()) {
        return;
    }

    if (const auto phrase = [&] {
        switch (sayTo->m_nPedType) {
        case PED_TYPE_GANG1:   return CTX_GLOBAL_ABUSE_GANG_BALLAS;
        case PED_TYPE_GANG2:
        case PED_TYPE_PLAYER1:
        case PED_TYPE_PLAYER2: return CTX_GLOBAL_ABUSE_GANG_FAMILIES;
        case PED_TYPE_GANG4:   return CTX_GLOBAL_ABUSE_RIFA;
        case PED_TYPE_GANG5:   return CTX_GLOBAL_ABUSE_DA_NANG;
        case PED_TYPE_GANG6:   return CTX_GLOBAL_ABUSE_MAFIA;
        case PED_TYPE_GANG7:   return CTX_GLOBAL_ABUSE_TRIAD;
        case PED_TYPE_GANG8:   return CTX_GLOBAL_ABUSE_GANG_VLA;
        default:               return CTX_GLOBAL_NO_SPEECH;
        }
    }()) {
        talker->Say(phrase);
    }
}

// 0x65EA50
CPed* CTaskComplexGangLeader::TryToPassObject(CPed* ped, CPedGroup* group) {
    const auto [closestPed, distSq] = group->GetMembership().GetMemberClosestTo(ped);
    if (closestPed && distSq < sq(2.f) && !ped->IsPlayer()) {
        return closestPed;
    }
    return nullptr;
}

// 0x65DFA0
bool CTaskComplexGangLeader::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (m_pSubTask && !m_pSubTask->MakeAbortable(ped, priority, event)) {
        return false;
    }
    ped->bDontAcceptIKLookAts = false;
    return true;
}

// 0x65DFF0
CTask* CTaskComplexGangLeader::CreateNextSubTask(CPed* ped) {
    if (m_pSubTask) {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_STAND_STILL:
        case TASK_COMPLEX_HANDSIGNAL_ANIM: { // 0x65E044
            if (auto* const mem = m_Gang->GetMembership().GetRandom()) {
                return new CTaskComplexTurnToFaceEntityOrCoord{ mem };
            }
        }
        }
    }

    if (CGeneral::GetRandomNumberInRange(0, 100) > 95 || m_Gang->GetMembership().CountMembers() < 3) { // 0x65E0E6
        m_WanderTimer.Start(CGeneral::GetRandomNumberInRange(15'000, 30'000));
        return new CTaskComplexWanderGang{ PEDMOVE_WALK, CGeneral::RandomNodeHeading(), 5'000, true, 0.05f };
    }

    return new CTaskSimpleStandStill{ 5'000 }; // 0x65E10E
}

// 0x65E1F0
CTask* CTaskComplexGangLeader::CreateFirstSubTask(CPed* ped) {
    return CreateNextSubTask(ped);
}

// 0x662370
CTask* CTaskComplexGangLeader::ControlSubTask(CPed* ped) {
    ped->bDontAcceptIKLookAts = false;

    // Make sure anims are loaded (if they can/need to be)
    if (m_AnimsReferenced) { // 0x66239B
        if (!ShouldLoadGangAnims()) {
            UnrefAnimBlock();
        }
    } else if (ShouldLoadGangAnims()) {
        const auto blk = CAnimManager::GetAnimationBlockIndex("gangs");
        if (CAnimManager::GetAnimBlocks()[blk].IsLoaded) {
            CAnimManager::AddAnimBlockRef(blk);
            m_AnimsReferenced = true;
        } else {
            CStreaming::RequestModel(IFPToModelId(blk), STREAMING_KEEP_IN_MEMORY);
        }
    }

    // If we're wandering and the wander time is out of time...
    // NOTE: The original doesn't check `m_pSubTask` for null here
    if (auto* const tWander = notsa::dyn_cast_if_present<CTaskComplexWander>(m_pSubTask)) { // 0x66241A
        if (m_WanderTimer.IsOutOfTime() && tWander->GetDistSqOfClosestPathNodeToPed(ped) < 2.f) {
            m_Gang->GetIntelligence().SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType::RANDOM);
            // Above call causes this task to be flushed (deleted), and changes our vfptr to `CTaskComplex`'s.
            // If we return non-null here, `CTaskManager::ParentsControlChildren` will be called, and calls our
            // `ControlSubTask` causing an assert (as CTaskComplex defines it as `pure` (`= 0`))
            // This is an OG bug, and we can't even wrap it into `FIX_BUGS` because it literally aborts the process,
            // makes no sense keeping it.
            return nullptr; //return new CTaskSimpleStandStill{ 500 };
        }
    }

    if (m_ExhaleTimer.IsOutOfTime()) { // 0x6624C1
        if (ped->GetRpClump()) {
            if (auto* const matrix = RwFrameGetMatrix(RpClumpGetFrame(ped->GetRpClump()))) {
                if (auto* const fx = g_fxMan.CreateFxSystem("exhale", CVector{ 0.f, 0.1f, 0.f }, matrix)) {
                    fx->AttachToBone(ped, eBoneTag::BONE_HEAD);
                    fx->PlayAndKill();
                }
                m_ExhaleTimer.Stop();
            }
        }
    }

    ScanForStuff(ped);

    if (!ped->IsVisible()) {
        return m_pSubTask;
    }

    // If ped isn't already looking at someone, look at a random member
    if (!g_ikChainMan.IsLooking(ped) && CGeneral::GetRandomNumberInRange(0, 100) > 95) { // 0x6625A9
        const auto lookTime = CGeneral::GetRandomNumberInRange(3'000, 5'000);
        if (auto* const mem = m_Gang->GetMembership().GetMember(CGeneral::GetRandomNumberInRange(0, TOTAL_PED_GROUP_MEMBERS)); mem && mem != ped) {
            g_ikChainMan.LookAt(
                "TaskGangLeader",
                ped,
                mem,
                lookTime,
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

    if (!m_AnimsReferenced || ped->IsRunningOrSprinting()) { // 0x66261A
        return m_pSubTask;
    }

    auto* const pedHeldEntity = ped->GetEntityThatThisPedIsHolding();

    if (!pedHeldEntity) { // 0x6628F6
        // If they're already playing an anim, early out
        if (ped->IsPlayingHandSignal() || ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
            return m_pSubTask;
        }

        // Otherwise maybe start a new partial anim
        const auto rnd = CGeneral::GetRandomNumberInRange(0, 500);
        if (rnd > 50 && rnd < 56) { // 0x662935
            ped->GetTaskManager().SetTaskSecondary(new CTaskSimpleRunAnim{ ANIM_GROUP_GANGS, CGeneral::RandomChoice(s_gangTalkAnims) }, TASK_SECONDARY_PARTIAL_ANIM);
        } else if (rnd == 100) { // 0x662991
            ped->GetTaskManager().SetTaskSecondary(new CTaskComplexPlayHandSignalAnim{}, TASK_SECONDARY_PARTIAL_ANIM);
        }
        return m_pSubTask;
    }

    // Enum to be in the same order as anims below
    enum {
        DRNKBR_PRTL,
        SMKCIG_PRTL,
        DRNKBR_PRTL_F,
        SMKCIG_PRTL_F,
    };
    const auto GetAnim = [ped](AnimationId id) { return RpAnimBlendClumpGetAssociation(ped->GetRpClump(), id); };
    const CAnimBlendAssociation* anims[]{
        GetAnim(ANIM_ID_DRNKBR_PRTL),
        GetAnim(ANIM_ID_SMKCIG_PRTL),
        GetAnim(ANIM_ID_DRNKBR_PRTL_F),
        GetAnim(ANIM_ID_SMKCIG_PRTL_F),
    };
    const bool anyOfTheAnimsPlaying = rng::any_of(anims, notsa::NotIsNull{});

    // If any of the anims are playing, stop looking, start exhale timer of smkcig anims
    if (anyOfTheAnimsPlaying) { // 0x6626A4
        if (g_ikChainMan.IsLooking(ped)) {
            g_ikChainMan.AbortLookAt(ped);
        }

        ped->bDontAcceptIKLookAts = true;

        // Start exhale timer (for smkcig anims)
        if (!m_ExhaleTimer.IsStarted()) {
            if (rng::any_of(
                    std::array{ SMKCIG_PRTL, SMKCIG_PRTL_F },
                    [&](auto idx) {
                        const auto* const anim = anims[idx];
                        return anim && anim->GetCurrentTime() < 0.5f;
                    }
                )) {
                m_ExhaleTimer.Start(2'700);
            }
        }
    }

    if (pedHeldEntity->GetModelIndex() == ModelIndices::MI_GANG_DRINK) { // 0x662718
        ped->Say(CTX_GLOBAL_BOOZE_RECEIVE, 0, 0.2f);
    } else if (pedHeldEntity->GetModelIndex() == ModelIndices::MI_GANG_SMOKE) {
        ped->Say(CTX_GLOBAL_SPLIFF_RECEIVE, 0, 0.2f);
    }

    // Now, pass on the entity held in hand (if not already)

    if (ped->GetTaskManager().Find<CTaskComplexPassObject>()) { // 0x662760
        return m_pSubTask;
    }

    if (CGeneral::GetRandomNumberInRange(0, 500) != 200) {    // 0x662789
        if (CGeneral::GetRandomNumberInRange(0, 100) == 50) { // 0x6628A7
            if (auto* const tHold = ped->GetTaskManager().Find<CTaskSimpleHoldEntity>()) {
                tHold->PlayAnim(GetRandomGangAmbientAnim(ped, pedHeldEntity), ANIM_GROUP_GANGS);
            }
        }
        return m_pSubTask;
    }

    if (anyOfTheAnimsPlaying) { // 0x662794
        return m_pSubTask;
    }

    if (auto* const passObjTo = TryToPassObject(ped, m_Gang)) { // 0x6627C1
        if (!passObjTo->GetEntityThatThisPedIsHolding() && passObjTo->IsCurrentlyUnarmed()) {
            // Very similar to code above, but not quite the same!
            if (pedHeldEntity->GetModelIndex() == ModelIndices::MI_GANG_DRINK) {
                passObjTo->Say(CTX_GLOBAL_BOOZE_REQUEST);
            } else if (pedHeldEntity->GetModelIndex() == ModelIndices::MI_GANG_SMOKE) {
                passObjTo->Say(CTX_GLOBAL_SPLIFF_REQUEST);
            }
            passObjTo->GetEventGroup().Add(CEventPassObject{ ped });
            return new CTaskComplexPassObject{ passObjTo, true };
        }
    }

    return m_pSubTask;
}

// 0x65E200
void CTaskComplexGangLeader::ScanForStuff(CPed* ped) {
    if (m_ScanTimer.IsStarted() && !m_ScanTimer.IsOutOfTime()) {
        return;
    }

    const auto& pedPos = ped->GetPosition();

    const auto rndChance = CGeneral::GetRandomNumberInRange(0, 100);
    if (rndChance < 5) { // 5% chance
        // Find a nearby vehicle to lean onto
        for (auto& veh : ped->GetIntelligence()->GetVehicleScanner().GetEntities<CVehicle>()) {
            // 0x65E2C1
            if (!veh.IsSubAutomobile()) {
                continue;
            }

            if (veh.GetStatus() != STATUS_ABANDONED) {
                continue;
            }

            if (veh.vehicleFlags.bHasGangLeaningOn) {
                continue;
            }

            if (veh.IsMissionVehicle()) {
                continue;
            }

            // 0x65E351 || 0x65E330 (in that order)
            if (const auto vehToPed = pedPos - veh.GetPosition(); std::abs(vehToPed.z) >= 5.f || vehToPed.SquaredMagnitude() >= 300.f) {
                continue;
            }

            m_Gang->GetIntelligence().AddEvent(
                CEventGroupEvent{
                    ped,
                    new CEventLeanOnVehicle{ &veh, CGeneral::GetRandomNumberInRange(10'000, 25'000) }
            }
            );

            m_ScanTimer.Start(60'000);

            // 0x65E3E8
#ifdef FIX_BUGS
            break;
#endif
        }
    } else if (rndChance == 20) { // 1% chance
        // Try recruiting one nearby ped to the gang
        for (auto& scannedPed : ped->GetIntelligence()->GetPedScanner().GetEntities<CPed>()) {
            if (!scannedPed.IsCreatedBy(PED_GAME) || scannedPed.m_nPedType != ped->m_nPedType || scannedPed.bInVehicle) {
                continue;
            }

            auto* const scannedPedGrp = scannedPed.GetGroup();

            // Already in the gang
            if (scannedPedGrp == m_Gang) {
                continue;
            }

            // In the player's gang
            if (FindPlayerPed()->GetPlayerGroup().GetMembership().IsMember(&scannedPed)) { // 0x65E494
                continue;
            }

            // Can it join a gang at all?
#ifdef FIX_BUGS
            if (auto* const wander = scannedPed.GetTaskManager().Find<CTaskComplexWander>()) { // 0x65E4BE
#else
            if (auto* const wander = ped->GetTaskManager().Find<CTaskComplexWander>()) {
#endif
                if (wander->GetWanderType() == WANDER_TYPE_GANG) {
                    if (!static_cast<CTaskComplexWanderGang*>(wander)->CanJoinGang()) {
                        continue;
                    }
                }
            }

            // If scanned ped has no group try to add them to this gang
            if (!scannedPedGrp && m_Gang->GetMembership().CountMembersExcludingLeader() < TOTAL_PED_GROUP_MEMBERS - 1) { // 0x65E4E8
                scannedPed.GetEventGroup().Add(
                    CEventScriptCommand{
                        TASK_PRIMARY_PRIMARY,
                        new CTaskComplexBeInGroup{ m_Gang->GetId() } }
                );
                m_Gang->GetMembership().AddFollower(&scannedPed);
                m_Gang->Process();
            }

            // Find a member close enough to the scanned ped, and make them partners
            if (const auto [closestMem, distSq] = m_Gang->GetMembership().GetMemberClosestTo(&scannedPed); // 0x65E610
                closestMem && sq(10.f) >= distSq && distSq >= sq(4.f)) {
                const auto partnerType = CGeneral::GetRandomNumberInRange(0, 7);
                scannedPed.GetEventGroup().Add(CEventCreatePartnerTask{ partnerType, closestMem, true, 0.5f });
                closestMem->GetEventGroup().Add(CEventCreatePartnerTask{ partnerType, &scannedPed, false, 0.5f });
            }

            m_ScanTimer.Start(10'000);

            break;
        }
    }
}

// NOTSA
void CTaskComplexGangLeader::UnrefAnimBlock() {
    CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
    m_AnimsReferenced = false;
}

void CTaskComplexGangLeader::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangLeader, 0x86F8FC, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x65DED0, "1", {}, CPedGroup*);
    RH_ScopedDestructorInstall(0x65DF30);

    RH_ScopedInstall(GetRandomGangAmbientAnim, 0x65E730);
    RH_ScopedInstall(ShouldLoadGangAnims, 0x65E7F0);
    RH_ScopedInstall(DoGangAbuseSpeech, 0x65E860);
    RH_ScopedInstall(TryToPassObject, 0x65EA50);

    RH_ScopedVMTInstall(Clone, 0x661FA0);
    RH_ScopedVMTInstall(GetTaskType, 0x65DF20);
    RH_ScopedVMTInstall(MakeAbortable, 0x65DFA0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65DFF0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65E1F0);
    RH_ScopedVMTInstall(ControlSubTask, 0x662370);
    RH_ScopedVMTInstall(ScanForStuff, 0x65E200);
}
