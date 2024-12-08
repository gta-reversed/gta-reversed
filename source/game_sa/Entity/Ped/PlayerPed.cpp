/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PlayerPed.h"
#include "TagManager.h"
#include "PedClothesDesc.h"
#include "PedStats.h"
#include "TaskSimpleUseGun.h"
#include "EntryExitManager.h"
#include "MBlur.h"

bool (&abTempNeverLeavesGroup)[7] = *(bool (*)[7])0xC0BC08;
int32& gPlayIdlesAnimBlockIndex = *(int32*)0xC0BC10;
bool& CPlayerPed::bHasDisplayedPlayerQuitEnterCarHelpText = *(bool*)0xC0BC15;

bool CPlayerPed::bDebugPlayerInvincible;
bool CPlayerPed::bDebugTargeting;
bool CPlayerPed::bDebugTapToTarget;

void CPlayerPed::InjectHooks() {
    RH_ScopedVirtualClass(CPlayerPed, 0x86D168, 26);
    RH_ScopedCategory("Entity/Ped");

    RH_ScopedInstall(ResetSprintEnergy, 0x60A530);
    RH_ScopedInstall(ResetPlayerBreath, 0x60A8A0);
    RH_ScopedInstall(RemovePlayerPed, 0x6094A0);
    RH_ScopedInstall(Busted, 0x609EF0);
    RH_ScopedInstall(GetWantedLevel, 0x41BE60);
    RH_ScopedInstall(SetWantedLevel, 0x609F10);
    RH_ScopedInstall(SetWantedLevelNoDrop, 0x609F30);
    RH_ScopedInstall(CheatWantedLevel, 0x609F50);
    RH_ScopedInstall(DoStuffToGoOnFire, 0x60A020);
    RH_ScopedVMTInstall(Load, 0x5D46E0, { .reversed = false });
    RH_ScopedVMTInstall(Save, 0x5D57E0, { .reversed = false });
    RH_ScopedInstall(DeactivatePlayerPed, 0x609520);
    RH_ScopedInstall(ReactivatePlayerPed, 0x609540);
    RH_ScopedInstall(GetPadFromPlayer, 0x609560);
    RH_ScopedInstall(CanPlayerStartMission, 0x609590);
    RH_ScopedInstall(IsHidden, 0x609620);
    RH_ScopedInstall(ReApplyMoveAnims, 0x609650);
    RH_ScopedInstall(DoesPlayerWantNewWeapon, 0x609710);
    RH_ScopedInstall(ProcessPlayerWeapon, 0x6097F0);
    RH_ScopedInstall(PickWeaponAllowedFor2Player, 0x609800);
    RH_ScopedInstall(UpdateCameraWeaponModes, 0x609830);
    RH_ScopedInstall(ClearWeaponTarget, 0x609c80);
    RH_ScopedInstall(GetWeaponRadiusOnScreen, 0x609CD0);
    RH_ScopedInstall(PedCanBeTargettedVehicleWise, 0x609D90);
    RH_ScopedInstall(FindTargetPriority, 0x609DE0);
    RH_ScopedInstall(Clear3rdPersonMouseTarget, 0x609ED0);
    RH_ScopedInstall(CanIKReachThisTarget, 0x609F80);
    RH_ScopedInstall(GetPlayerInfoForThisPlayerPed, 0x609FF0);
    RH_ScopedInstall(AnnoyPlayerPed, 0x60A040);
    RH_ScopedInstall(ClearAdrenaline, 0x60A070);
    RH_ScopedInstall(DisbandPlayerGroup, 0x60A0A0);
    RH_ScopedInstall(MakeGroupRespondToPlayerTakingDamage, 0x60A110);
    RH_ScopedInstall(TellGroupToStartFollowingPlayer, 0x60A1D0);
    RH_ScopedInstall(MakePlayerGroupDisappear, 0x60A440);
    RH_ScopedInstall(MakePlayerGroupReappear, 0x60A4B0);
    RH_ScopedInstall(HandleSprintEnergy, 0x60A550);
    RH_ScopedInstall(GetButtonSprintResults, 0x60A820, { .reversed = false });
    RH_ScopedInstall(HandlePlayerBreath, 0x60A8D0);
    RH_ScopedOverloadedInstall(MakeChangesForNewWeapon, "", 0x60B460, void(CPlayerPed::*)(eWeaponType));
    RH_ScopedGlobalInstall(LOSBlockedBetweenPeds, 0x60B550);
    RH_ScopedInstall(DoesTargetHaveToBeBroken, 0x60C0C0);
    RH_ScopedInstall(SetPlayerMoveBlendRatio, 0x60C520);
    RH_ScopedInstall(FindPedToAttack, 0x60C5F0);
    RH_ScopedInstall(ForceGroupToAlwaysFollow, 0x60C7C0);
    RH_ScopedInstall(ForceGroupToNeverFollow, 0x60C800);
    RH_ScopedOverloadedInstall(MakeChangesForNewWeapon, "BySlot", 0x60D000, void(CPlayerPed::*)(uint32));
    RH_ScopedInstall(EvaluateTarget, 0x60D020);
    RH_ScopedInstall(PlayerHasJustAttackedSomeone, 0x60D5A0);
    RH_ScopedInstall(SetupPlayerPed, 0x60D790);

    RH_ScopedVMTInstall(ProcessControl, 0x60EA90);
    RH_ScopedVMTInstall(SetMoveAnim, 0x609490);
}

struct WorkBufferSaveData {
    uint32          SaveSize = sizeof(WorkBufferSaveData); // Never read, but written
    uint32          ChaosLevel{};
    uint32          WantedLevel{};
    CPedClothesDesc ClothesDesc{};
    uint32          ChosenWeapon{};
};
VALIDATE_SIZE(WorkBufferSaveData, 132u + 4u);

// calls of LoadDataFromWorkBuffer are optimized
// todo: fix

// 0x5D46E0
bool CPlayerPed::Load() {
    return plugin::CallMethodAndReturn<bool, 0x5D46E0, CPlayerPed*>(this);

    CPed::Load();

    WorkBufferSaveData sd{};
    CGenericGameStorage::LoadDataFromWorkBuffer(&sd, sizeof(WorkBufferSaveData));
    assert(sd.SaveSize == sizeof(sd));

    CWanted* wanted = m_pPlayerData->m_pWanted;
    wanted->m_nChaosLevel = sd.ChaosLevel;
    wanted->m_nWantedLevel= sd.WantedLevel;

    m_pPlayerData->m_nChosenWeapon   = sd.ChosenWeapon;
    *m_pPlayerData->m_pPedClothesDesc = sd.ClothesDesc;

    return true;
}

// calls of SaveDataToWorkBuffer are optimized
// todo: fix

// 0x5D57E0
bool CPlayerPed::Save() {
    return plugin::CallMethodAndReturn<bool, 0x5D57E0>(this);

    WorkBufferSaveData saveData{};

    CWanted* wanted = m_pPlayerData->m_pWanted;
    saveData.ChaosLevel = wanted->m_nChaosLevel;
    saveData.WantedLevel = wanted->m_nWantedLevel;
    saveData.ChosenWeapon = m_pPlayerData->m_nChosenWeapon;
    saveData.ClothesDesc  = *m_pPlayerData->m_pPedClothesDesc;

    CGenericGameStorage::SaveDataToWorkBuffer(&saveData, sizeof(WorkBufferSaveData));

    return true;
}

// 0x60D5B0
CPlayerPed::CPlayerPed(int32 playerId, bool bGroupCreated) : CPed(PED_TYPE_PLAYER1) {
    m_pPlayerData = &CWorld::Players[playerId].m_PlayerData;
    m_pPlayerData->AllocateData();

    CPed::SetModelIndex(MODEL_PLAYER);

    CPlayerPed::SetInitialState(bGroupCreated);

    CEntity::ClearReference(m_pTargetedObject);

    SetPedState(PEDSTATE_IDLE);

    gPlayIdlesAnimBlockIndex = CAnimManager::GetAnimationBlockIndex("playidles");

    if (!bGroupCreated) {
        m_pPlayerData->m_nPlayerGroup = CPedGroups::AddGroup();

        auto& group = CPedGroups::GetGroup(m_pPlayerData->m_nPlayerGroup);
        group.GetIntelligence().SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType::RANDOM);
        group.m_bIsMissionGroup = true;
        group.m_groupMembership.SetLeader(this);
        group.Process();

        m_pPlayerData->m_bGroupStuffDisabled = false;
        m_pPlayerData->m_bGroupAlwaysFollow  = false;
        m_pPlayerData->m_bGroupNeverFollow   = false;
    }

    m_fMaxHealth = CStats::GetFatAndMuscleModifier(STAT_MOD_MAX_HEALTH);
    m_fHealth    = m_fMaxHealth;

    m_nFightingStyle      = STYLE_GRAB_KICK;
    m_nAllowedAttackMoves = 15;

    m_p3rdPersonMouseTarget = nullptr;
    field_7A0 = 0;
    m_pedSpeech.Initialise(this);
    m_pIntelligence->m_fDmRadius = 30.0f;
    m_pIntelligence->m_nDmNumPedsToScan = 2;

    bUsedForReplay = bGroupCreated;
}

// 0x6094A0
void CPlayerPed::RemovePlayerPed(int32 playerId) {
    CPed* player = FindPlayerPed(playerId);
    CPlayerInfo* playerInfo = &FindPlayerInfo(playerId);
    if (player)
    {
        CVehicle* playerVehicle = player->m_pVehicle;
        if (playerVehicle && playerVehicle->m_pDriver == player)
        {
            playerVehicle->m_nStatus = STATUS_PHYSICS;
            playerVehicle->m_fGasPedal = 0.0f;
            playerVehicle->m_fBreakPedal = 0.1f;
        }
        CWorld::Remove(static_cast<CEntity*>(player));
        delete player;
        playerInfo->m_pPed = nullptr;
    }
}

// 0x609520
void CPlayerPed::DeactivatePlayerPed(int32 playerId) {
    assert(playerId >= 0);
    CWorld::Remove(FindPlayerPed(playerId));
}

// 0x609540
void CPlayerPed::ReactivatePlayerPed(int32 playerId) {
    assert(playerId >= 0);
    CWorld::Add(FindPlayerPed(playerId));
}

// 0x609560
CPad* CPlayerPed::GetPadFromPlayer() const {
    switch (m_nPedType) {
    case PED_TYPE_PLAYER1:
        return CPad::GetPad(0);

    case PED_TYPE_PLAYER2:
        return CPad::GetPad(1);
    }
    assert(0); // shouldn't happen
    return nullptr;
}

// 0x609590
bool CPlayerPed::CanPlayerStartMission() {
    if (CGameLogic::GameState != GAME_STATE_INITIAL || CGameLogic::IsCoopGameGoingOn())
        return false;

    if (!IsPedInControl() && !IsStateDriving())
        return false;

    auto& taskMgr = GetTaskManager();
    if (taskMgr.GetTaskPrimary(TASK_PRIMARY_PHYSICAL_RESPONSE))
        return false;

    if (taskMgr.GetTaskPrimary(TASK_PRIMARY_EVENT_RESPONSE_NONTEMP))
        return false;

    auto primaryTask = taskMgr.GetTaskPrimary(TASK_PRIMARY_PRIMARY);
    if (primaryTask != nullptr && primaryTask->GetTaskType() != TASK_SIMPLE_CAR_DRIVE)
        return false;

    if (taskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK))
        return false;

    if (!IsAlive())
        return false;

    return !GetEventGroup().GetEventOfType(EVENT_SCRIPT_COMMAND);
}

// 0x609620
bool CPlayerPed::IsHidden() {
    return bInVehicle && GetLightingTotal() <= 0.05f;
}

// 0x609650
void CPlayerPed::ReApplyMoveAnims() {
    constexpr AnimationId anims[]{
        ANIM_ID_WALK,
        ANIM_ID_RUN,
        ANIM_ID_SPRINT,
        ANIM_ID_IDLE,
        ANIM_ID_WALK_START
    };
    for (const AnimationId& id : anims) {
        if (CAnimBlendAssociation* anim = RpAnimBlendClumpGetAssociation(m_pRwClump, id)) {
            if (anim->GetHashKey() != CAnimManager::GetAnimAssociation(m_nAnimGroup, id)->GetHashKey()) {
                CAnimBlendAssociation* addedAnim = CAnimManager::AddAnimation(m_pRwClump, m_nAnimGroup, id);
                addedAnim->m_BlendDelta = anim->m_BlendDelta;
                addedAnim->m_BlendAmount = anim->m_BlendAmount;

                anim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
                anim->m_BlendDelta = -1000.0f;
            }
        }
    }
}

// 0x609710
bool CPlayerPed::DoesPlayerWantNewWeapon(eWeaponType weaponType, bool arg1) {
    // GetPadFromPlayer(); // Called, but not used

    auto weaponSlot = GetWeaponSlot(weaponType);
    auto weaponInSlotType = GetWeaponInSlot(weaponSlot).m_Type;
    if (weaponInSlotType == weaponType)
        return true;

    if (weaponInSlotType == eWeaponType::WEAPON_UNARMED)
        return true;

    if (arg1)
        return false;

    if (m_pIntelligence->GetTaskJetPack())
        return false;

    /* !See comment!
    if (m_nActiveWeaponSlot == weaponSlot
        && CWeaponInfo::GetWeaponInfo(weaponInSlotType, GetWeaponSkill(weaponInSlotType))->flags.bAimWithArm   \ One of these two is always false, so
        && !CWeaponInfo::GetWeaponInfo(weaponInSlotType, GetWeaponSkill(weaponInSlotType))->flags.bAimWithArm) / the whole expression is always false
        return false;
    */

    if (m_nActiveWeaponSlot == weaponSlot) {
        switch (m_nPedState) {
        case PEDSTATE_ATTACK:
        case PEDSTATE_AIMGUN:
            return false;
        }
    }

    return true;
}

// 0x6097F0
void CPlayerPed::ProcessPlayerWeapon(CPad* pad) {
    /* empty */
}

// 0x609800
void CPlayerPed::PickWeaponAllowedFor2Player() {
    if (!GetWeaponInSlot(m_pPlayerData->m_nChosenWeapon).CanBeUsedFor2Player())
        m_pPlayerData->m_nChosenWeapon = eWeaponType::WEAPON_UNARMED;
}

// 0x609830
void CPlayerPed::UpdateCameraWeaponModes(CPad* pad) {
    switch (GetActiveWeapon().m_Type) {
    case eWeaponType::WEAPON_M4:
        TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_M16_1STPERSON, 0, 0);
        break;

    case eWeaponType::WEAPON_SNIPERRIFLE:
        TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_SNIPER, 0, 0);
        break;

    case eWeaponType::WEAPON_RLAUNCHER:
        TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_ROCKETLAUNCHER, 0, 0);
        break;

    case eWeaponType::WEAPON_RLAUNCHER_HS:
        TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_ROCKETLAUNCHER_HS, 0, 0);
        break;

    case eWeaponType::WEAPON_CAMERA:
        TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_CAMERA, 0, 0);
        break;

    default:
        TheCamera.ClearPlayerWeaponMode();
        break;
    }
}

// 0x6098F0
void CPlayerPed::ProcessAnimGroups() {
    plugin::CallMethod<0x6098F0, CPlayerPed *>(this);
}

// 0x609C80
void CPlayerPed::ClearWeaponTarget() {
    if (IsPlayer()) {
        CEntity::ClearReference(m_pTargetedObject);
        TheCamera.ClearPlayerWeaponMode();
        CWeaponEffects::ClearCrossHair(m_nPedType);
    }
}

// 0x609CD0
float CPlayerPed::GetWeaponRadiusOnScreen() {
    CWeapon& wep = GetActiveWeapon();
    CWeaponInfo& wepInfo = wep.GetWeaponInfo(this);

    if (wep.IsTypeMelee())
        return 0.0f;

    const float accuracyProg = 0.5f / wepInfo.m_fAccuracy;
    switch (wep.m_Type) {
    case eWeaponType::WEAPON_SHOTGUN:
    case eWeaponType::WEAPON_SPAS12_SHOTGUN:
    case eWeaponType::WEAPON_SAWNOFF_SHOTGUN:
        return std::max(0.2f, accuracyProg); // here they multiply *accuracyProg * 1.0f* :thinking

    default: {
        const float rangeProg = std::min(1.0f, 15.0f / wepInfo.m_fWeaponRange);
        const float radius = (m_pPlayerData->m_fAttackButtonCounter * 0.5f + 1.0f) * rangeProg * accuracyProg;
        if (bIsDucking)
            return std::max(0.2f, radius / 2.0f);
        return std::max(0.2f, radius);
    }
    }
}

// 0x609D90
bool CPlayerPed::PedCanBeTargettedVehicleWise(CPed* ped) {
    if (ped->bInVehicle) {
        CVehicle* veh = ped->m_pVehicle;
        return veh && (veh->IsBike() || veh->vehicleFlags.bVehicleCanBeTargetted);
    }
    return true;
}

// 0x609DE0
float CPlayerPed::FindTargetPriority(CEntity* entity) {
    switch (entity->m_nType) {
    case ENTITY_TYPE_VEHICLE:
        return 0.1f;

    case ENTITY_TYPE_PED: {
        auto ped = entity->AsPed();

        if (ped->bThisPedIsATargetPriority)
            return 1.0f;

        if (ped->GetTaskManager().HasAnyOf<TASK_COMPLEX_KILL_PED_ON_FOOT, TASK_COMPLEX_ARREST_PED>()) {
            return 0.8f;
        }

        if (CPedGroups::AreInSameGroup(this, ped))
            return 0.05f;

        if (ped->m_nPedType == PED_TYPE_GANG2)
            return 0.06f;

        if (ped->IsCreatedByMission())
            return 0.25f;

        return 0.1f;
    }

    case ENTITY_TYPE_OBJECT: {
        switch (entity->AsObject()->m_nObjectType) {
        case eObjectType::OBJECT_MISSION:
        case eObjectType::OBJECT_MISSION2:
            return 0.1f;

        default:
            return 0.0f;
        }
    }
    default: {
        return 0.1f;
    }
    }
}

// 0x609ED0
void CPlayerPed::Clear3rdPersonMouseTarget() {
    CEntity::ClearReference(m_p3rdPersonMouseTarget);
}

// 0x609EF0
void CPlayerPed::Busted() {
    CWanted* wanted = GetWanted();
    if (wanted) {
        wanted->m_nChaosLevel = 0;
    }
}

// 0x41BE60
uint32 CPlayerPed::GetWantedLevel() const {
    if (const auto* wanted = GetWanted()) {
        return wanted->m_nWantedLevel;
    }

    return 0;
}

// 0x609F10
void CPlayerPed::SetWantedLevel(int32 level) {
    CWanted* wanted = GetWanted();
    wanted->SetWantedLevel(level);
}

// 0x609F30
void CPlayerPed::SetWantedLevelNoDrop(int32 level) {
    CWanted* wanted = GetWanted();
    wanted->SetWantedLevelNoDrop(level);
}

// 0x609F50
void CPlayerPed::CheatWantedLevel(int32 level) {
    CWanted* wanted = GetWanted();
    wanted->CheatWantedLevel(level);
}

// 0x609F80
bool CPlayerPed::CanIKReachThisTarget(CVector posn, CWeapon* weapon, bool arg2) {
    if (!weapon->GetWeaponInfo(this).flags.bAimWithArm) {
        const CVector thisPos = GetPosition();
        return (posn - thisPos).Magnitude2D() >= thisPos.z - posn.z;
    }
    return true;
}

// 0x609FF0
CPlayerInfo* CPlayerPed::GetPlayerInfoForThisPlayerPed() {
    // TODO: Use range for here
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (FindPlayerPed(i) == this)
            return &FindPlayerInfo(i);
    }
    return nullptr;
}

// 0x60A020
void CPlayerPed::DoStuffToGoOnFire() {
    if (m_nPedState == PEDSTATE_SNIPER_MODE)
        TheCamera.ClearPlayerWeaponMode();
}

// 0x60A040
void CPlayerPed::AnnoyPlayerPed(bool arg0) {
    auto& temper = m_pStats->m_nTemper;

    if (temper < 52) {
        temper++;
    } else if (arg0) {
        if (temper < 55)
            temper++;
        else
            temper = 46;
    }
}

// 0x60A070
void CPlayerPed::ClearAdrenaline() {
    if (m_pPlayerData->m_bAdrenaline && m_pPlayerData->m_nAdrenalineEndTime != 0) {
        m_pPlayerData->m_nAdrenalineEndTime = 0;
        CTimer::ResetTimeScale();
    }
}

// 0x60A0A0
void CPlayerPed::DisbandPlayerGroup() {
    auto& ms = GetPlayerGroup().GetMembership();
    if (const auto numMembers = ms.CountMembersExcludingLeader()) {
        Say(numMembers > 1 ? CTX_GLOBAL_ORDER_DISBAND_MANY : CTX_GLOBAL_ORDER_ATTACK_SINGLE);
    } else {
        ms.RemoveAllFollowers(true);
    }
}

// 0x60A110
void CPlayerPed::MakeGroupRespondToPlayerTakingDamage(CEventDamage& damageEvent) {
    auto& group = GetPlayerGroup();
    if (!damageEvent.m_pSourceEntity)
        return;
    if (group.GetMembership().CountMembersExcludingLeader() < 1)
        return;
    if (!group.m_bMembersEnterLeadersVehicle)
        return;

    CEventGroupEvent groupEvent(this, damageEvent.Clone());
    group.GetIntelligence().AddEvent(&groupEvent);
}

// 0x60A1D0
void CPlayerPed::TellGroupToStartFollowingPlayer(bool arg0, bool arg1, bool arg2) {
    if (m_pPlayerData->m_bGroupAlwaysFollow && !arg0)
        return;
    if (m_pPlayerData->m_bGroupNeverFollow && arg0)
        return;

    CPedGroup& group = GetPlayerGroup();
    CPedGroupIntelligence& groupIntel = group.GetIntelligence();
    CPedGroupMembership& membership = group.GetMembership();
    if (!arg2 && !membership.CountMembersExcludingLeader())
        return;

    group.m_bMembersEnterLeadersVehicle = arg0;
    groupIntel.SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType::RANDOM);
    if (arg0) {
        CEventPlayerCommandToGroup playerCmdEvent;
        playerCmdEvent.ComputeResponseTaskType(&group);
        if (playerCmdEvent.WillRespond()) {
            auto gatherCmdEvent = new CEventPlayerCommandToGroup(ePlayerGroupCommand::PLAYER_GROUP_COMMAND_GATHER);
            gatherCmdEvent->m_taskId = playerCmdEvent.m_taskId;

            CEventGroupEvent groupEvent(this, gatherCmdEvent);
            groupIntel.AddEvent(&groupEvent);
        }
    }

    if (arg1) {
        const uint32 nMembers = membership.CountMembersExcludingLeader();
        if (!nMembers)
            return;

        if (arg0) {
            const float distToFurthest = group.FindDistanceToFurthestMember();
            if (nMembers > 1) {
                if (distToFurthest >= 3.0f)
                    Say(distToFurthest >= 10.0f ? CTX_GLOBAL_ORDER_FOLLOW_FAR_MANY : CTX_GLOBAL_ORDER_FOLLOW_NEAR_MANY);
                else
                    Say(CTX_GLOBAL_ORDER_FOLLOW_VNEAR_MANY);
            } else {
                if (distToFurthest >= 3.0f)
                    Say(distToFurthest >= 10.0f ? CTX_GLOBAL_ORDER_FOLLOW_FAR_ONE : CTX_GLOBAL_ORDER_FOLLOW_NEAR_ONE);
                else
                    Say(CTX_GLOBAL_ORDER_FOLLOW_VNEAR_ONE);
            }
        } else if (nMembers > 1) {
            Say(CTX_GLOBAL_ORDER_WAIT_MANY);
        } else {
            Say(CTX_GLOBAL_ORDER_WAIT_ONE);
        }
    }
}

// 0x60A440
void CPlayerPed::MakePlayerGroupDisappear() {
    CPedGroupMembership& membership = GetPlayerGroup().GetMembership();
    for (int i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        if (CPed* member = membership.GetMember(i)) {
            if (!member->IsCreatedByMission()) {
                member->m_bCollisionProcessed = false;
                member->m_bIsVisible = false;
                abTempNeverLeavesGroup[i] = member->bNeverLeavesGroup;
                member->bNeverLeavesGroup = true;
            }
        }
    }
}

// 0x60A4B0
void CPlayerPed::MakePlayerGroupReappear() {
    CPedGroupMembership& membership = GetPlayerGroup().GetMembership();
    for (int i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        if (CPed* member = membership.GetMember(i)) {
            if (!member->IsCreatedByMission()) {
                member->m_bIsVisible = true;
                if (!member->bInVehicle)
                    member->m_bUsesCollision = true;
                member->bNeverLeavesGroup = abTempNeverLeavesGroup[i];
            }
        }
    }
}

// 0x60A530
void CPlayerPed::ResetSprintEnergy()
{
    m_pPlayerData->m_fTimeCanRun = CStats::GetFatAndMuscleModifier(STAT_MOD_TIME_CAN_RUN);
}

// 0x60A550
bool CPlayerPed::HandleSprintEnergy(bool sprint, float adrenalineConsumedPerTimeStep) {
    float& timeCanRun = m_pPlayerData->m_fTimeCanRun;
    if (sprint) {
        if (FindPlayerInfo().m_bDoesNotGetTired)
            return true;
        if (m_pPlayerData->m_bAdrenaline || adrenalineConsumedPerTimeStep == 0.0f)
            return true;

        if (timeCanRun > -150.0f) { // TODO: Find out what this magic number is
            timeCanRun = std::max(-150.0f, timeCanRun - CTimer::GetTimeStep() * adrenalineConsumedPerTimeStep);
            return true;
        }
    } else {
        if (CStats::GetFatAndMuscleModifier(STAT_MOD_TIME_CAN_RUN) > timeCanRun) {
            timeCanRun += CTimer::GetTimeStep() * adrenalineConsumedPerTimeStep / 2.0f;
        }
    }
    return false;
}

constexpr auto PLAYER_SPRINT_THRESHOLD{ 5.0f }; // 0x8D2458
constexpr struct tPlayerSprintSet { // From 0x8D2460
    float field_0;
    float field_4;
    float field_8;
    float field_C;
    float field_10;
    float field_14;
    float field_18;
    float field_1C;
} PLAYER_SPRINT_SET[] = {
    // 0x0, 0x4,  0x8,  0xC,  0x10,  0x14, 0x18, 0x1C
    { 4.0f, 0.7f, 0.2f, 5.0f, 10.0f, 1.0f, 0.5f, 0.3f }, // GROUND
    { 4.0f, 0.7f, 0.2f, 5.0f, 10.0f, 0.0f, 0.4f, 1.0f }, // BMX
    { 4.0f, 0.7f, 0.2f, 5.0f, 10.0f, 1.0f, 0.3f, 0.3f }, // WATER
    { 4.0f, 0.7f, 0.2f, 5.0f, 10.0f, 0.0f, 0.0f, 1.0f }  // UNDERWATER
};

// 0x60A610
float CPlayerPed::ControlButtonSprint(eSprintType sprintType) {
    return plugin::CallMethodAndReturn<float, 0x60A610, CPlayerPed *, eSprintType>(this, sprintType);
}

// Reverse CPlayerPed::SetRealMoveAnim before hooking this func
// 0x60A820
float CPlayerPed::GetButtonSprintResults(eSprintType sprintType) {
    return plugin::CallMethodAndReturn<float, 0x60A820, CPlayerPed *, eSprintType>(this, sprintType);

    // Forces the compiler to preserve the value of `edx`.
    // Otherwise it's value is lost when called from 0x60B44C.
    // which causes a crash (as it is used to store a pointer to an anim blend assoc)
    __asm { and edx, edx };

    if (m_pPlayerData->m_fMoveSpeed <= PLAYER_SPRINT_THRESHOLD) {
        return m_pPlayerData->m_fMoveSpeed <= 0.0f ? 0.0f : 1.0f;
    } else {
        const float progress = std::max(0.0f, m_pPlayerData->m_fMoveSpeed / PLAYER_SPRINT_THRESHOLD - 1.0f);
        return PLAYER_SPRINT_SET[sprintType].field_1C * progress + 1.0f;
    }
}

// 0x60A8A0
void CPlayerPed::ResetPlayerBreath() {
    m_pPlayerData->m_fBreath = CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG);
    m_pPlayerData->m_bRequireHandleBreath = false;
}

// 0x60A8D0
void CPlayerPed::HandlePlayerBreath(bool bDecreaseAir, float fMultiplier) {
    float& breath = m_pPlayerData->m_fBreath;
    float  decreaseAmount = CTimer::GetTimeStep() * fMultiplier;
    if (!bDecreaseAir || CCheat::IsActive(CHEAT_INFINITE_OXYGEN)) {
        if (CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG) > breath)
            breath += decreaseAmount * 2.0f;
    } else {
        if (breath > 0.0f && bDrownsInWater)
            breath = std::max(0.0f, breath - decreaseAmount);
        else
            CWeapon::GenerateDamageEvent(this, this, eWeaponType::WEAPON_DROWNING, (int32)(decreaseAmount * 3.0f), PED_PIECE_TORSO, 0);
    }
    m_pPlayerData->m_bRequireHandleBreath = false;
}

// 0x60A9C0
void CPlayerPed::SetRealMoveAnim() {
    plugin::CallMethod<0x60A9C0, CPlayerPed *>(this);
}

// 0x60B460
void CPlayerPed::MakeChangesForNewWeapon(eWeaponType weaponType) {
    GetActiveWeapon().StopWeaponEffect();
    if (m_nPedState == PEDSTATE_SNIPER_MODE)
        TheCamera.ClearPlayerWeaponMode();

    SetCurrentWeapon(weaponType);

    m_pPlayerData->m_nChosenWeapon = m_nActiveWeaponSlot;
    m_pPlayerData->m_fAttackButtonCounter= 0.0f;

    CWeapon& wep = GetActiveWeapon();
    CWeaponInfo& wepInfo = wep.GetWeaponInfo(this);

    wep.m_AmmoInClip = std::min<uint32>(wep.m_TotalAmmo, (uint32)wepInfo.m_nAmmoClip);

    if (!wepInfo.flags.bCanAim)
        ClearWeaponTarget();

    if (!wepInfo.flags.bOnlyFreeAim)
        m_pPlayerData->m_bFreeAiming = false;


    if (auto anim = RpAnimBlendClumpGetAssociation(m_pRwClump, ANIM_ID_FIRE))
        anim->m_Flags |= ANIMATION_IS_PLAYING & ANIMATION_IS_FINISH_AUTO_REMOVE;

    TheCamera.ClearPlayerWeaponMode();
}

// 0x60B550
bool LOSBlockedBetweenPeds(CEntity* entity1, CEntity* entity2) {
    CVector origin{};
    if (entity1->IsPed()) {
        origin = entity1->AsPed()->GetBonePosition(eBoneTag::BONE_NECK, false);
        if (entity1->AsPed()->bIsDucking)
            origin.z += 0.35f;
    } else {
        origin = entity1->GetPosition();
    }

    const auto target = entity2->IsPed()
        ? entity1->AsPed()->GetBonePosition(eBoneTag::BONE_NECK, false)
        : entity1->GetPosition();

    CColPoint colPoint;
    CEntity* hitEntity;
    if (CWorld::ProcessLineOfSight(origin, target, colPoint, hitEntity, true, false, false, true, false, false, false, true))
        return hitEntity != entity2;
    return false;
}

// 0x60B650
void CPlayerPed::Compute3rdPersonMouseTarget(bool meleeWeapon) {
    plugin::CallMethod<0x60B650, CPlayerPed *, bool>(this, meleeWeapon);
}

// 0x60BA80
void CPlayerPed::DrawTriangleForMouseRecruitPed() {
    plugin::CallMethod<0x60BA80, CPlayerPed *>(this);
}

// 0x60C0C0
bool CPlayerPed::DoesTargetHaveToBeBroken(CEntity* entity, CWeapon* weapon) {
    if (entity->m_bIsVisible)
        return true;

    if (weapon->GetWeaponInfo(this).m_fTargetRange < (entity->GetPosition() - GetPosition()).Magnitude())
        return true;

    if (weapon->m_Type == eWeaponType::WEAPON_SPRAYCAN) {
        if (entity->IsBuilding()) {
            if (CTagManager::IsTag(entity)) {
                if (CTagManager::GetAlpha(entity) == 255) { // they probably used -1
                    return true;
                }
            }
        }
    }

    return CanIKReachThisTarget(entity->GetPosition(), weapon, false);
}

// 0x60C1E0
void CPlayerPed::KeepAreaAroundPlayerClear() {
    plugin::CallMethod<0x60C1E0, CPlayerPed *>(this);
}

// 0x60C520
void CPlayerPed::SetPlayerMoveBlendRatio(CVector* point) {
    float& moveBlendRatio = m_pPlayerData->m_fMoveBlendRatio;
    if (point) {
        moveBlendRatio = std::min(2.0f, (*point - GetPosition()).Magnitude2D() * 2.0f);
    } else {
        switch (m_nMoveState)
        {
        case PEDMOVE_WALK:
            moveBlendRatio = 1.0f;
            break;

        case PEDMOVE_RUN:
            moveBlendRatio = 1.8f;
            break;

        case PEDMOVE_SPRINT:
            moveBlendRatio = 2.5f;
            break;

        default:
            moveBlendRatio = 0.0f;
            break;
        }
    }
    SetRealMoveAnim();
}

// 0x60C5F0
CPed* CPlayerPed::FindPedToAttack() {
    CVector origin = FindPlayerCoors();
    origin.z = 0.0f;

    CVector end = origin + TheCamera.GetForward() * 100.0f;
    end.z = 0.0f;

    CPed* closestPed{};
    float closestDistance = std::numeric_limits<float>::max();

    CPedGroupMembership& membership = GetPlayerGroup().GetMembership();
    for (int i = 0; GetPedPool()->GetSize(); i++) {
        CPed* ped = GetPedPool()->GetAt(i);
        if (!ped)
            continue;
        if (ped->IsPlayer())
            continue;
        if (!ped->IsAlive())
            continue;
        if (membership.IsMember(ped))
            continue;
        if (ped->m_nPedType == PED_TYPE_GANG2)
            continue;

        CVector point = ped->GetPosition();
        point.z = 0.0f;

        float dist = CCollision::DistToLine(origin, end, point);
        float pointDist = (point - origin).Magnitude2D();
        if (pointDist > 20.0f)
            dist += (pointDist - 20.0f) / 5.0f;

        if (IsPedTypeGang(ped->m_nPedType))
            dist = std::max(0.0f, dist / 2.0f - 2.0f);

        if (dist < closestDistance) {
            closestDistance = dist;
            closestPed = ped;
        }
    }
    return closestPed;
}

// 0x60C7C0
void CPlayerPed::ForceGroupToAlwaysFollow(bool enable) {
    m_pPlayerData->m_bGroupAlwaysFollow = enable;
    if (enable)
        TellGroupToStartFollowingPlayer(true, false, true);
}

// 0x60C800
void CPlayerPed::ForceGroupToNeverFollow(bool enable) {
    m_pPlayerData->m_bGroupNeverFollow = enable;
    if (enable)
        TellGroupToStartFollowingPlayer(false, false, true);
}

// 0x60C840
void CPlayerPed::MakeThisPedJoinOurGroup(CPed* ped) {
    plugin::CallMethod<0x60C840, CPlayerPed *, CPed*>(this, ped);
}

// 0x60CC50
bool CPlayerPed::PlayerWantsToAttack() {
    return plugin::CallMethodAndReturn<bool, 0x60CC50, CPlayerPed *>(this);
}

// 0x60CD20
void CPlayerPed::SetInitialState(bool bGroupCreated) {
    plugin::CallMethod<0x60CD20, CPlayerPed *, bool>(this, bGroupCreated);
}

// 0x60D000
void CPlayerPed::MakeChangesForNewWeapon(uint32 weaponSlot) {
    if (weaponSlot != -1)
        MakeChangesForNewWeapon(GetWeaponInSlot(weaponSlot).m_Type);
}

static auto& PLAYER_MAX_TARGET_VIEW_ANGLE = *(float*)0x8D243C; // 140.0f

// 0x60D020
void CPlayerPed::EvaluateTarget(CEntity* target, CEntity *& outTarget, float & outTargetPriority, float maxDistance, float compensationRotRad, bool arg5) {
    const CVector dir = target->GetPosition() - GetPosition();
    const float dist = dir.Magnitude();

    if (dist > maxDistance)
        return;

    if (DoesTargetHaveToBeBroken(target, &GetActiveWeapon()))
        return;

    const float targetAngleDeg = std::fabs(RadiansToDegrees(CGeneral::LimitRadianAngle(CGeneral::GetATanOf(dir) - compensationRotRad)));

    float viewAngleMultiplier = 1.0f - targetAngleDeg / PLAYER_MAX_TARGET_VIEW_ANGLE;
    if (dist > 1.0f)
        viewAngleMultiplier /= sqrt(sqrt(dist)); // Take quad root of dist

    const float targetPriority = FindTargetPriority(target) * viewAngleMultiplier;
    if (targetPriority > outTargetPriority && !LOSBlockedBetweenPeds(this, target)) {
        outTarget = target;
        outTargetPriority = targetPriority;
    }
}

// 0x60D1C0
void CPlayerPed::EvaluateNeighbouringTarget(CEntity* target, CEntity** outTarget, float* outTargetPriority, float maxDistance, float arg4, bool arg5) {
    plugin::CallMethod<0x60D1C0, CPlayerPed *, CEntity*, CEntity**, float*, float, float, bool>(this, target, outTarget, outTargetPriority, maxDistance, arg4, arg5);
}

// 0x60D350
void CPlayerPed::ProcessGroupBehaviour(CPad* pad) {
    plugin::CallMethod<0x60D350, CPlayerPed *, CPad*>(this, pad);
}

// 0x60D5A0
bool CPlayerPed::PlayerHasJustAttackedSomeone() {
    return PlayerWantsToAttack();
}

// 0x60D790
void CPlayerPed::SetupPlayerPed(int32 playerId) {
    auto ped = new CPlayerPed(playerId, false);
    auto& playerInfo = FindPlayerInfo(playerId);
    playerInfo.m_pPed = ped;

    if (playerId == 1)
        ped->m_nPedType = PED_TYPE_PLAYER2;

    ped->SetOrientation(0.0f, 0.0f, 0.0f);
    CWorld::Add(ped);
    ped->m_nWeaponAccuracy = 100;
    playerInfo.m_nPlayerState = ePlayerState::PLAYERSTATE_PLAYING;
}

// 0x60D850
void CPlayerPed::ProcessWeaponSwitch(CPad* pad) {
    plugin::CallMethod<0x60D850, CPlayerPed *, CPad*>(this, pad);
}

// 0x60DC50
bool CPlayerPed::FindWeaponLockOnTarget() {
    return plugin::CallMethodAndReturn<bool, 0x60DC50, CPlayerPed *>(this);
}

// 0x60E530
bool CPlayerPed::FindNextWeaponLockOnTarget(CEntity* arg0, bool arg1) {
    return plugin::CallMethodAndReturn<bool, 0x60E530, CPlayerPed *, CEntity*, bool>(this, arg0, arg1);
}

// 0x60EA90
void CPlayerPed::ProcessControl() {
    if (m_pPlayerData->m_nCarDangerCounter)
        m_pPlayerData->m_nCarDangerCounter--;
    if (!m_pPlayerData->m_nCarDangerCounter)
        m_pPlayerData->m_pDangerCar = 0;
    if (m_pPlayerData->m_nFadeDrunkenness) {
        if (m_pPlayerData->m_nDrunkenness - 1 > 0) {
            --m_pPlayerData->m_nDrunkenness;
        } else {
            m_pPlayerData->m_nDrunkenness = 0;
            CMBlur::ClearDrunkBlur();
            m_pPlayerData->m_nFadeDrunkenness = 0;
        }
    }
    if (m_pPlayerData->m_nDrunkenness) 
        CMBlur::SetDrunkBlur(m_pPlayerData->m_nDrunkenness / 255.0f);
    if (m_pPlayerData->m_bRequireHandleBreath) {
        if (CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG) > m_pPlayerData->m_fBreath)
            m_pPlayerData->m_fBreath += CTimer::GetTimeStep() + CTimer::GetTimeStep();
        m_pPlayerData->m_bRequireHandleBreath = false;
    }
    m_pPlayerData->m_bRequireHandleBreath = true;
    CPed::ProcessControl();
    bCheckColAboveHead = true;
    float markColor = 1.0f;
    bool limitMarkColor = true;
    CVector effectPos;
    CPad* pad = CPad::GetPad(m_nPedType);
    if (!bCanPointGunAtTarget) {
        m_pPlayerData->m_pWanted->Update();
        PruneReferences();
        if (GetActiveWeapon().m_Type == WEAPON_MINIGUN) {
            auto weaponInfo = CWeaponInfo::GetWeaponInfo(WEAPON_MINIGUN, eWeaponSkill::STD);
            if (m_pIntelligence->GetTaskUseGun()) {
                auto animAssoc = m_pIntelligence->GetTaskUseGun()->m_Anim;
                if (animAssoc && animAssoc->m_CurrentTime - animAssoc->m_TimeStep < weaponInfo->m_fAnimLoopEnd) {
                    if (m_pPlayerData->m_fGunSpinSpeed < 0.45f) {
                        m_pPlayerData->m_fGunSpinSpeed += CTimer::GetTimeStep() * 0.025f;
                        m_pPlayerData->m_fGunSpinSpeed = std::min(m_pPlayerData->m_fGunSpinSpeed, 0.45f);
                    }
                    if (pad->GetWeapon(this) && GetActiveWeapon().m_TotalAmmo > 0 && animAssoc->m_CurrentTime >= weaponInfo->m_fAnimLoopStart) 
                        m_weaponAudio.AddAudioEvent(AE_WEAPON_FIRE_MINIGUN_AMMO);
                    else 
                        m_weaponAudio.AddAudioEvent(AE_WEAPON_FIRE_MINIGUN_NO_AMMO);
                }
            } else {
                if (m_pPlayerData->m_fGunSpinSpeed > 0.0f) {
                    m_pPlayerData->m_fGunSpinSpeed -= CTimer::GetTimeStep() * 0.003f;
                    m_pPlayerData->m_fGunSpinSpeed = std::max(m_pPlayerData->m_fGunSpinSpeed, 0.0f);
                }
            }
        }
        if (GetActiveWeapon().m_Type == WEAPON_CHAINSAW && m_nPedState != PEDSTATE_ATTACK && !bInVehicle) {
            m_pIntelligence->GetTaskSwim(); // hmmm?
        }
        if (m_pTargetedObject) {
            ClearReference(m_p3rdPersonMouseTarget);
            if (m_pTargetedObject->IsPed()) {
                CPed* targetPed = m_pTargetedObject->AsPed();
                auto weaponInfo = CWeaponInfo::GetWeaponInfo(GetActiveWeapon().m_Type, GetWeaponSkill());
                float targetHeadRange = weaponInfo->GetTargetHeadRange();
                markColor = targetPed->m_fHealth / targetPed->m_fMaxHealth;
                bool instantFireHit = false;
                if (targetPed->IsAlive()) {
                    auto stdWeaponInfo = CWeaponInfo::GetWeaponInfo(GetActiveWeapon().m_Type, eWeaponSkill::STD);
                    if (stdWeaponInfo->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT) {
                        instantFireHit = true;
                        CVector distance = targetPed->GetPosition() - GetPosition();
                        if (targetHeadRange * targetHeadRange > distance.SquaredMagnitude()) {
                            m_pPlayerData->m_nTargetBone = BONE_HEAD;
                            m_pPlayerData->m_vecTargetBoneOffset.x = 0.05f;
                        }
                    }
                }
                if (!instantFireHit) {
                    m_pPlayerData->m_nTargetBone = BONE_SPINE1;
                    m_pPlayerData->m_vecTargetBoneOffset.x = 0.2f;
                }
                effectPos = m_pPlayerData->m_vecTargetBoneOffset;
                targetPed->GetTransformedBonePosition(effectPos, static_cast<eBoneTag>(m_pPlayerData->m_nTargetBone), false);
                bool targetIsInVehicle = false;
                if (markColor > 0.0f) {
                    if (!targetPed->bInVehicle && targetPed->m_nMoveState != PEDMOVE_STILL) {
                        effectPos += targetPed->m_vecMoveSpeed * CTimer::GetTimeStep();
                    }
                }
                if (targetPed->bInVehicle) {
                    auto targetVeh = targetPed->m_pVehicle;
                    if (targetVeh)
                        effectPos += (targetVeh->m_vecMoveSpeed + targetVeh->m_vecTurnSpeed) * CTimer::GetTimeStep();
                }
            } else if (m_pTargetedObject->IsVehicle()) {
                CVehicle* targetVeh = m_pTargetedObject->AsVehicle();
                effectPos = (targetVeh->m_vecMoveSpeed + targetVeh->m_vecTurnSpeed) * CTimer::GetTimeStep();
                effectPos += targetVeh->GetPosition();
            } else if (m_pTargetedObject->IsObject()) {
                CObject* targetObj = m_pTargetedObject->AsObject();
                effectPos = targetObj->m_vecMoveSpeed * CTimer::GetTimeStep();
                effectPos += targetObj->GetPosition();
                markColor = targetObj->m_fHealth * 0.001f;
            } else {
                effectPos = m_pTargetedObject->GetPosition();
                limitMarkColor = false; 
            }
        }
    }
    if (m_pTargetedObject) {
        uint8 r = 0, g = 0, b = 0;
        bool setRGB = true;
        if (limitMarkColor) {
            if (markColor > 0.0f)
                markColor = std::min(markColor, 1.0f);
            else
                setRGB = false;
        }
        if (setRGB) {
            r = static_cast<uint8>((1.0f - markColor) * 255.0f);
            g = static_cast<uint8>(markColor * 255.0f);
            b = static_cast<uint8>(0.0f);
        }
        CVector distance = effectPos - GetPosition();
        float size = 1.0f - distance.Magnitude() * 0.02f;
        CWeaponEffects::MarkTarget(m_nPedType, effectPos, r, g, b, 255u, size, false);
    }
    if (m_nMoveState != PEDMOVE_NONE) {
        if (m_nMoveState != PEDMOVE_RUN) {
            if (m_nMoveState != PEDMOVE_SPRINT)
                HandleSprintEnergy(false, 1.0f);
        } else if (CStats::GetFatAndMuscleModifier(STAT_MOD_TIME_CAN_RUN) > m_pPlayerData->m_fTimeCanRun)
            m_pPlayerData->m_fTimeCanRun += CTimer::GetTimeStep() * 0.15f;
    } else if (bInVehicle) {
        if (m_pVehicle && !m_pVehicle->IsSubBMX())
            HandleSprintEnergy(false, 1.0f);
    }
    GetActiveWeapon().Update(this);
    if (m_nPedState == PEDSTATE_DEAD || m_nPedState == PEDSTATE_DIE) {
        ClearWeaponTarget();
        return;
    }
    if (pad) {
        if (pad->WeaponJustDown(this)) {
            auto& activeWeapon = GetActiveWeapon();
            auto weaponType = activeWeapon.m_Type;
            if (!TheCamera.Using1stPersonWeaponMode() || activeWeapon.m_State == WEAPONSTATE_OUT_OF_AMMO) {
                if (!m_pIntelligence->GetTaskSwim()) {
                    if (weaponType == WEAPON_SNIPERRIFLE) {
                        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_FIRE_FAIL_SNIPERRIFFLE, 0.0f, 1.0f);
                    } else if (weaponType == WEAPON_RLAUNCHER || weaponType == WEAPON_RLAUNCHER_HS) {
                        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_FIRE_FAIL_ROCKET, 0.0f, 1.0f);
                    }
                }
            }
        }
        if (IsPedShootable() && this->m_nPedState != PEDSTATE_ANSWER_MOBILE) {
            int32 slot = CWorld::FindPlayerSlotWithPedPointer(this);
            if (!CWorld::Players[slot].m_pRemoteVehicle)
                ProcessWeaponSwitch(pad);
        }
    }
    ProcessAnimGroups();
    if (pad && TheCamera.GetActiveCamera().m_nMode == MODE_FOLLOWPED && !TheCamera.GetActiveCamera().m_nDirectionWasLooking) {
        auto& activeCam = TheCamera.GetActiveCamera();
        m_nLookTime = 0;
        float lookDir = CGeneral::LimitRadianAngle(atan2(-activeCam.m_vecFront.x, activeCam.m_vecFront.y));
        float angle = fabs(lookDir - m_fCurrentRotation);
        if (m_nPedState != PEDSTATE_ATTACK && angle > RadiansToDegrees(30) && angle < RadiansToDegrees(330)) {
            if (angle > RadiansToDegrees(150) && angle < RadiansToDegrees(210)) {
                float dir1 = CGeneral::LimitRadianAngle(m_fCurrentRotation - RadiansToDegrees(150));
                float dir2 = CGeneral::LimitRadianAngle(m_fCurrentRotation + RadiansToDegrees(150));
                lookDir = dir1;
                if (m_fLookDirection != 999'999.f && !bIsDucking) {
                    if (fabs(dir2 - m_fLookDirection) <= fabs(dir1 - m_fLookDirection))
                        lookDir = dir2;
                }
            }
            SetLookFlag(lookDir, true, false);
            SetLookTimer(static_cast<uint32>((CTimer::GetTimeStep() * 0.02f * 1000.0f) * 5.0f));
        } else {
            ClearLookFlag();
        }
    }
    if (m_nMoveState == PEDMOVE_SPRINT && bIsLooking) {
        ClearLookFlag();
        SetLookTimer(250);
    }
    if (m_vecMoveSpeed.Magnitude() >= 0.1f) {
        m_pPlayerData->m_nStandStillTimer = 0;
        m_pPlayerData->m_bStoppedMoving = false;
    } else if (!m_pPlayerData->m_nStandStillTimer) {
        m_pPlayerData->m_nStandStillTimer = CTimer::GetTimeInMS() + 500;
    } else if (CTimer::GetTimeInMS() > m_pPlayerData->m_nStandStillTimer) {
        m_pPlayerData->m_bStoppedMoving = true;
    }
    if (m_pPlayerData->m_bDontAllowWeaponChange) {
        if (IsPlayer()) {
            if (!CPad::GetPad(0)->GetTarget())
                m_pPlayerData->m_bDontAllowWeaponChange = false;
        }
    }
    if (m_nPedState != PEDSTATE_SNIPER_MODE && GetActiveWeapon().m_State == WEAPONSTATE_FIRING)
        m_pPlayerData->m_nLastTimeFiring = CTimer::GetTimeInMS();
    ProcessGroupBehaviour(pad);
    if (bInVehicle)
        CCarCtrl::RegisterVehicleOfInterest(m_pVehicle);
    if (!m_bIsVisible)
        UpdateRpHAnim();
    if (bInVehicle) {
        CPad* pad = CPad::GetPad(0);
        if (!pad->IsDPadDownPressed()) {
            if (pad->IsDPadUpPressed())
                m_pPlayerData->m_bPlayersGangActive = true;
        } else {
            m_pPlayerData->m_bPlayersGangActive = false;
        }
    }
    if (physicalFlags.bSubmergedInWater) {
        CVector pos = GetPosition();
        pos.z += 1.5f;
        if (CWaterLevel::GetWaterLevel(pos.x, pos.y, pos.z, m_pPlayerData->m_fWaterHeight, true, nullptr)) {
            auto& box = CEntity::GetColModel()->GetBoundingBox();
            float playerMinZ = pos.z + box.m_vecMin.z;
            float playerMaxZ = pos.z + box.m_vecMax.z;
            if (m_pPlayerData->m_fWaterHeight < playerMaxZ) {
                if (m_pPlayerData->m_fWaterHeight > playerMinZ)
                    m_pPlayerData->m_nWaterCoverPerc = static_cast<uint8>((m_pPlayerData->m_fWaterHeight - playerMinZ) / (playerMaxZ - playerMinZ) * 100.0f);
                else
                    m_pPlayerData->m_nWaterCoverPerc = 0;
            } else {
                m_pPlayerData->m_nWaterCoverPerc = 100;
            }
        } else {
            physicalFlags.bSubmergedInWater = false;
        }
    } else {
        m_pPlayerData->m_nWaterCoverPerc = 0;
    }
    if ((CTimer::GetFrameCounter() & 0x7F) == 0 && !FindPlayerVehicle()) {
        auto& group = CPedGroups::GetGroup(m_pPlayerData->m_nPlayerGroup);
        if (group.m_bMembersEnterLeadersVehicle) {
            int32 memberCount = group.m_groupMembership.CountMembersExcludingLeader();
            if (memberCount > 0) {
                float distance = group.FindDistanceToNearestMember(nullptr);
                if (distance > 20.0f && distance < 100.0f && CGame::currArea == AREA_CODE_NORMAL_WORLD) {
                    if (memberCount == 1)
                        Say(CTX_GLOBAL_ORDER_KEEP_UP_ONE);
                    else
                        Say(CTX_GLOBAL_ORDER_KEEP_UP_MANY);
                    for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; ++i) {
                        CPed* member = group.m_groupMembership.GetMember(i);
                        if (member && CGeneral::GetRandomNumberInRange(0.0f, 1.0f) < 0.5f) {
                            int32 offset = CGeneral::GetRandomNumberInRange(3000, 4500);
                            member->Say(CTX_GLOBAL_FOLLOW_REPLY, offset);
                        }
                    }
                }
            }
        }
    }
    if (!bInVehicle && GetLightingTotal() <= 0.05f && !CEntryExitManager::WeAreInInteriorTransition())
        Say(CTX_GLOBAL_BREATHING);
}

// 0x609490
void CPlayerPed::SetMoveAnim() {
    //nop
}
