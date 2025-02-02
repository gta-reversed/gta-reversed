#include "StdInc.h"
#include "IKChainManager_c.h"
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

void CTaskComplexGangLeader::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangLeader, 0x86F8FC, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65DED0);
    RH_ScopedInstall(Destructor, 0x65DF30);

    /*
    * There are some weird crashes when these are hooked.
    * I'm not entirely sure which function it's caused by.
    * See PR#449 for more info.
    */

    RH_ScopedInstall(GetRandomGangAmbientAnim, 0x65E730, { .enabled = false, .locked = true });
    RH_ScopedInstall(ShouldLoadGangAnims, 0x65E7F0, { .enabled = false, .locked = true });
    RH_ScopedInstall(DoGangAbuseSpeech, 0x65E860, { .enabled = false, .locked = true });
    RH_ScopedInstall(TryToPassObject, 0x65EA50, { .enabled = false, .locked = true });

    RH_ScopedVMTInstall(Clone, 0x661FA0, { .enabled = false, .locked = true });
    RH_ScopedVMTInstall(GetTaskType, 0x65DF20, { .enabled = false, .locked = true });
    RH_ScopedVMTInstall(MakeAbortable, 0x65DFA0, { .enabled = false, .locked = true });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65DFF0, { .enabled = false, .locked = true });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65E1F0, { .enabled = false, .locked = true });
    RH_ScopedVMTInstall(ControlSubTask, 0x662370, { .enabled = false, .locked = true });
    RH_ScopedVMTInstall(ScanForStuff, 0x65E200, { .enabled = false, .locked = true });
}

// 0x65DED0
CTaskComplexGangLeader::CTaskComplexGangLeader(CPedGroup* gang) :
    m_gang{gang}
{
}

CTaskComplexGangLeader::CTaskComplexGangLeader(const CTaskComplexGangLeader& o) :
    CTaskComplexGangLeader{ o.m_gang }
{
}

// 0x65DF30
CTaskComplexGangLeader::~CTaskComplexGangLeader() {
    if (m_animsReferenced) {
        UnrefAnimBlock();
    }
}

// 0x65E730
AnimationId CTaskComplexGangLeader::GetRandomGangAmbientAnim(CPed* ped, CEntity* entity) {
    if (!entity) {
        return CGeneral::RandomChoice(s_gangTalkAnims);
    }

    if (entity->m_nModelIndex == ModelIndices::MI_GANG_DRINK) {
        if (CGeneral::DoCoinFlip()) {
            return CGeneral::RandomChoice(s_gangTalkAnims | rng::views::take(4));
        }
    } else if (entity->m_nModelIndex != ModelIndices::MI_GANG_SMOKE || CGeneral::DoCoinFlip()) {
        return CGeneral::RandomChoice(s_gangTalkAnims);
    }

    switch (ped->m_nModelIndex) {
    case MODEL_BALLAS2:
    case MODEL_FAM1:
        return ANIM_ID_DRNKBR_PRTL_F;
    default:
        return ANIM_ID_DRNKBR_PRTL;
    }    
}

// 0x65E7F0
bool CTaskComplexGangLeader::ShouldLoadGangAnims() {
    if (CStreaming::IsVeryBusy()) {
        return false;
    }

    const auto player = FindPlayerPed();
    return !player->IsInVehicle() || player->m_pVehicle->m_vecMoveSpeed.SquaredMagnitude() <= sq(0.02f);
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
    if (closestPed && sq(4.f) >= distSq) {
        if (!closestPed->IsPed()) {
            return closestPed;
        }
    }

    return nullptr;
    /*
    * Using this code causes a crash for some reason (probably some register gets fucked, idk)
    * Crash always on the same offset: 0xebf328
    float distSq{};
    if (const auto closestPed = group->GetClosestGroupPed(ped, &distSq)) {
        if (distSq < sq(4.f)) {
            if (!closestPed->IsPed()) {
                return closestPed;
            }
        }
    }
    return nullptr;
    */
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
        case TASK_COMPLEX_HANDSIGNAL_ANIM: {
            if (const auto mem = m_gang->GetMembership().GetRandom()) {
                return new CTaskComplexTurnToFaceEntityOrCoord{ mem };
            }
        }
        }
    }

    if (CGeneral::RandomBool(5.f) || 3 > m_gang->GetMembership().CountMembers()) {
        m_wanderTimer.Start(CGeneral::GetRandomNumberInRange(0, 15'000));
        return new CTaskComplexWanderGang{ PEDMOVE_WALK, (uint8)CGeneral::GetRandomNumberInRange(0, 8), 5000, true, 0.05f };
    }

    return new CTaskSimpleStandStill{ 5000 };
}

// 0x65E1F0
CTask* CTaskComplexGangLeader::CreateFirstSubTask(CPed* ped) {
    return CreateNextSubTask(ped);
}

// 0x662370
CTask* CTaskComplexGangLeader::ControlSubTask(CPed* ped) {
    // Make sure anmims are loaded (if they can/need to be)
    if (m_animsReferenced) { // 0x66239B
        if (!ShouldLoadGangAnims()) {
            UnrefAnimBlock();
        }
    } else if (ShouldLoadGangAnims()) {
        const auto blk = CAnimManager::GetAnimationBlockIndex("gangs");
        if (CAnimManager::GetAnimBlocks()[blk].IsLoaded) {
            CAnimManager::AddAnimBlockRef(blk);
            m_animsReferenced = true;
        } else {
            CStreaming::RequestModel(IFPToModelId(blk), STREAMING_KEEP_IN_MEMORY);
        }
    }

    // If we're wandering and the wander time is out of time...
    if (const auto tWander = CTask::DynCast<CTaskComplexWander>(m_pSubTask)) { // 0x66241F
        if (m_wanderTimer.IsOutOfTime()) {
            if (tWander->GetDistSqOfClosestPathNodeToPed(ped) <= 2.f) {
                m_gang->GetIntelligence().SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType::RANDOM);
                // Above call causes this task to be flushed (deleted), and changes our vfptr to `CTaskComplex`'s.
                // If we return non-null here, `CTaskManager::ParentsControlChildren` will be called, and calls our
                // `ControlSubTask` causing an assert (as CTaskComplex defines it as `pure` (`= 0`))
                // This is an OG bug, and we can't even wrap it into `FIX_BUGS` because it literally aborts the process, 
                // makes no sense keeping it.
                return nullptr; //return new CTaskSimpleStandStill{ 500 };
            }
        }
    }

    if (m_exhaleTimer.IsOutOfTime()) { // 0x6624C1
        if (ped->m_pRwClump) {
            if (auto matrix = RwFrameGetMatrix(RpClumpGetFrame(ped->m_pRwClump))) {
                if (const auto fx = g_fxMan.CreateFxSystem("exhale", CVector{ 0.f, 0.1f, 0.f }, matrix)) {
                    fx->AttachToBone(ped, eBoneTag::BONE_HEAD);
                    fx->PlayAndKill();
                }
            }
            m_exhaleTimer.Stop();
        }
    }

    ScanForStuff(ped);

    if (!ped->IsVisible()) {
        return m_pSubTask;
    }

    // If ped isn't already looking at someone, find a random meber to look at them
    if (!g_ikChainMan.IsLooking(ped) && CGeneral::RandomBool(5.f)) { // 0x662574
        // The random logic has changed a little here for the sole reason
        // that I want to use `GetRandom()`.
        // This code path is very infrequent anyways (5% chance)...
        if (const auto mem = m_gang->GetMembership().GetRandom()) { 
            if (mem != ped) {
                g_ikChainMan.LookAt(
                    "TaskGangLeader",
                    ped,
                    mem,
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
    }

    if (!m_animsReferenced || ped->IsRunningOrSprinting()) { // 0x66261A
        return m_pSubTask;
    }

    const auto pedHeldEntity = ped->GetEntityThatThisPedIsHolding();

    if (!pedHeldEntity) {
        // If they're already playing an anim, early out
        if (ped->IsPlayingHandSignal() || ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
            return m_pSubTask;
        }

        // Otherwise create a new partial anim task
        ped->GetTaskManager().SetTaskSecondary([this, ped]() -> CTask* {
            const auto rnd = CGeneral::GetRandomNumberInRange(0, 500);
            if (50 >= rnd || rnd >= 56) { // 5%
                if (rnd != 100) { // 99.8%
                    return m_pSubTask;
                }

                // Overall chance to reach this point... 0.02% * 5% = 0.1%
                return new CTaskComplexPlayHandSignalAnim{};
            } else { // 95%
                return new CTaskSimpleRunAnim{ ANIM_GROUP_GANGS, CGeneral::RandomChoice(s_gangTalkAnims) };
            }
        }(), TASK_SECONDARY_PARTIAL_ANIM);

        return m_pSubTask;
    }

    // Enum to be in the same order as anims below
    enum {
        DRNKBR_PRTL,
        SMKCIG_PRTL,
        DRNKBR_PRTL_F,
        SMKCIG_PRTL_F,
    };
    const auto GetAnim = [ped](AnimationId id) { return RpAnimBlendClumpGetAssociation(ped->m_pRwClump, id); };
    const CAnimBlendAssociation* anims[]{
        GetAnim(ANIM_ID_DRNKBR_PRTL),
        GetAnim(ANIM_ID_SMKCIG_PRTL),
        GetAnim(ANIM_ID_DRNKBR_PRTL_F),
        GetAnim(ANIM_ID_SMKCIG_PRTL_F),
    };
    const bool anyOfTheAnimsPlaying = rng::any_of(anims, notsa::NotIsNull{});

    // If any of the anims are playing, stop looking, start exhale timer of smkcig anims
    if (anyOfTheAnimsPlaying) { // 0x662696
        if (g_ikChainMan.IsLooking(ped)) {
            g_ikChainMan.AbortLookAt(ped);
        }

        ped->bDontAcceptIKLookAts = true;

        // Start exhale timer (for smkcig anims)
        if (!m_exhaleTimer.IsStarted()) {
            if (rng::any_of(
                std::array{ SMKCIG_PRTL, SMKCIG_PRTL_F },
                [&](auto idx) {
                    const auto anim = anims[idx];
                    return anim && anim->m_CurrentTime < 0.5f;
                })
            ) {
                m_exhaleTimer.Start(2700);
            }
        }
    }

    if (pedHeldEntity->m_nModelIndex == ModelIndices::MI_GANG_DRINK) { // 0x662729
        ped->Say(CTX_GLOBAL_BOOZE_RECEIVE, 0, 0.2f);
    } else if (pedHeldEntity->m_nModelIndex == ModelIndices::MI_GANG_SMOKE) {
        ped->Say(CTX_GLOBAL_SPLIFF_RECEIVE, 0, 0.2f);
    }

    // Now, pass on the entity held in hand (if not already)

    if (ped->GetTaskManager().Find<TASK_COMPLEX_PASS_OBJECT>()) { // 0x662766
        return m_pSubTask;
    }

    if (CGeneral::GetRandomNumberInRange(0, 500) != 200) { // 99.8%
        if (CGeneral::DoCoinFlip()) {
            if (const auto task = ped->GetTaskManager().Find<CTaskSimpleHoldEntity>()) {
                task->PlayAnim(GetRandomGangAmbientAnim(ped, pedHeldEntity), ANIM_GROUP_GANGS);
            }
        }
        return m_pSubTask;
    }

    if (anyOfTheAnimsPlaying) { // 0x66279A
        return m_pSubTask;
    }

    if (const auto passObjTo = TryToPassObject(ped, m_gang)) { // 0x6627CD
        if (!passObjTo->GetEntityThatThisPedIsHolding() && passObjTo->IsCurrentlyUnarmed()) {    
            // Very similar to code above, but not quite the same!
            if (pedHeldEntity->m_nModelIndex == ModelIndices::MI_GANG_DRINK) {
                passObjTo->Say(CTX_GLOBAL_BOOZE_REQUEST);
            } else if (pedHeldEntity->m_nModelIndex == ModelIndices::MI_GANG_SMOKE) {
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
    if (!m_scanTimer.IsOutOfTime()) {
        return;
    }

    const auto& pedPos = ped->GetPosition();

    const auto rndChance = CGeneral::GetRandomNumberInRange(0, 100);
    if (rndChance < 5) { // 5% chance
        // Find a nearby vehicle to lean onto
        for (auto& veh : ped->GetIntelligence()->GetVehicleScanner().GetEntities<CVehicle>()) {
            // 0x65E2C1
            if (!veh.IsAutomobile()) {
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

            m_gang->GetIntelligence().AddEvent(
                CEventGroupEvent{
                    ped,
                    new CEventLeanOnVehicle{&veh, CGeneral::GetRandomNumberInRange(10'000, 25'000)}
                }
            );

            m_scanTimer.Start(60'000);

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

            const auto scannedPedGrp = scannedPed.GetGroup();

            // Already in the gang
            if (scannedPedGrp == m_gang) {
                continue;
            }

            // In the player's gang
            if (FindPlayerPed()->GetPlayerGroup().GetMembership().IsMember(&scannedPed)) { // 0x65E494
                continue;
            }

            // Can it join a gang at all?
#ifdef FIX_BUGS
            if (const auto wander = scannedPed.GetTaskManager().Find<CTaskComplexWander>()) { // 0x65E4BE
#else
            if (const auto wander = ped->GetTaskManager().Find<CTaskComplexWander>()) {
#endif
                if (wander->GetWanderType() == WANDER_TYPE_GANG) {
                    if (!static_cast<CTaskComplexWanderGang*>(wander)->CanJoinGang()) {
                        continue;
                    }
                }
            }

            // If scanned ped has no group try to add them to this gang
            if (!scannedPedGrp && m_gang->GetMembership().CanAddFollower()) { // 0x65E4EA
                m_gang->GetIntelligence().AddEvent(
                    CEventScriptCommand{
                        TASK_PRIMARY_PRIMARY,
                        new CTaskComplexBeInGroup{m_gang->GetId()}
                    }
                );
                m_gang->GetMembership().AddFollower(&scannedPed);
            }

            // Find a member close enough to the scanned ped, and make the partners
            if (const auto [closestMem, distSq] = m_gang->GetMembership().GetMemberClosestTo(&scannedPed); // 0x65E61F
                closestMem && sq(10.f) >= distSq && distSq >= sq(4.f)
            ) {           
                const auto partnerType = CGeneral::GetRandomNumberInRange(0, 7);
                scannedPed.GetEventGroup().Add(CEventCreatePartnerTask{ partnerType, closestMem, true, 0.5f });
                closestMem->GetEventGroup().Add(CEventCreatePartnerTask{ partnerType, &scannedPed, false, 0.5f });
            }

            m_scanTimer.Start(10'000);

            break;
        }
    }
}

void CTaskComplexGangLeader::UnrefAnimBlock() {
    CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
    m_animsReferenced = false;
}
