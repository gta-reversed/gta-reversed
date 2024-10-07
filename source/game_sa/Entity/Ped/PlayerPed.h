/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Ped.h"
#include "Pad.h"

#include "eWeaponType.h"
#include "eSprintType.h"

class CEventDamage;
class CPlayerInfo;

class NOTSA_EXPORT_VTABLE CPlayerPed : public CPed {
public:
    CPed* m_p3rdPersonMouseTarget;
    int32 field_7A0;

    // did we display "JCK_HLP" message
    static bool& bHasDisplayedPlayerQuitEnterCarHelpText;

    // Android
    static bool bDebugPlayerInvincible;
    static bool bDebugTargeting;
    static bool bDebugTapToTarget;

public:
    static void InjectHooks();

    CPlayerPed(int32 playerId, bool bGroupCreated);


    void ProcessControl() override;
    bool Load() override;
    bool Save() override;

    CPad* GetPadFromPlayer() const;
    bool CanPlayerStartMission();
    bool IsHidden();
    void ReApplyMoveAnims();
    bool DoesPlayerWantNewWeapon(eWeaponType weaponType, bool arg1);
    void ProcessPlayerWeapon(CPad* pad);
    void PickWeaponAllowedFor2Player();
    void UpdateCameraWeaponModes(CPad* pad);
    void ProcessAnimGroups();
    void ClearWeaponTarget();
    float GetWeaponRadiusOnScreen();
    float FindTargetPriority(CEntity* entity);
    void Clear3rdPersonMouseTarget();
    // GetWanted()->m_nWantedLevel = 0;
    void Busted();
    uint32 GetWantedLevel() const;
    void SetWantedLevel(int32 level);
    void SetWantedLevelNoDrop(int32 level);
    void CheatWantedLevel(int32 level);
    bool CanIKReachThisTarget(CVector posn, CWeapon* weapon, bool arg2);
    CPlayerInfo* GetPlayerInfoForThisPlayerPed();
    void DoStuffToGoOnFire();
    void AnnoyPlayerPed(bool arg0);
    void ClearAdrenaline();
    void DisbandPlayerGroup();
    void MakeGroupRespondToPlayerTakingDamage(CEventDamage & damageEvent);
    void TellGroupToStartFollowingPlayer(bool arg0, bool arg1, bool arg2);
    void MakePlayerGroupDisappear();
    void MakePlayerGroupReappear();
    void ResetSprintEnergy();
    bool HandleSprintEnergy(bool sprint, float adrenalineConsumedPerTimeStep);
    float ControlButtonSprint(eSprintType sprintType);
    float GetButtonSprintResults(eSprintType sprintType);
    void ResetPlayerBreath();
    void HandlePlayerBreath(bool bDecreaseAir, float fMultiplier);
    void SetRealMoveAnim();
    void MakeChangesForNewWeapon(eWeaponType weaponType);
    void Compute3rdPersonMouseTarget(bool meleeWeapon);
    void DrawTriangleForMouseRecruitPed();
    bool DoesTargetHaveToBeBroken(CEntity* entity, CWeapon* weapon);
    void KeepAreaAroundPlayerClear();
    void SetPlayerMoveBlendRatio(CVector* arg0);
    CPed* FindPedToAttack();
    void ForceGroupToAlwaysFollow(bool enable);
    void ForceGroupToNeverFollow(bool enable);
    void MakeThisPedJoinOurGroup(CPed* ped);
    bool PlayerWantsToAttack();
    void SetInitialState(bool bGroupCreated);
    void MakeChangesForNewWeapon(uint32 weaponSlot);
    void EvaluateTarget(CEntity* target, CEntity *& outTarget, float & outTargetPriority, float maxDistance, float arg4, bool arg5);
    void EvaluateNeighbouringTarget(CEntity* target, CEntity** outTarget, float* outTargetPriority, float maxDistance, float arg4, bool arg5);
    void ProcessGroupBehaviour(CPad* pad);
    // return PlayerWantsToAttack();
    bool PlayerHasJustAttackedSomeone();
    void ProcessWeaponSwitch(CPad* pad);
    bool FindWeaponLockOnTarget();
    bool FindNextWeaponLockOnTarget(CEntity* arg0, bool arg1);
    // Thanks to Silent
    // 0x41BE60
    inline CWanted* GetWanted() {
        return m_pPlayerData ? m_pPlayerData->m_pWanted : nullptr;
    }

    inline const CWanted* GetWanted() const {
        return m_pPlayerData ? m_pPlayerData->m_pWanted : nullptr;
    }

    static void RemovePlayerPed(int32 playerId);
    static void DeactivatePlayerPed(int32 playerId);
    static void ReactivatePlayerPed(int32 playerId);
    static bool PedCanBeTargettedVehicleWise(CPed* ped);
    static void SetupPlayerPed(int playerId);

    // NOTSA
    CPedGroup& GetPlayerGroup() const noexcept { return CPedGroups::GetGroup(m_pPlayerData->m_nPlayerGroup); }
};

VALIDATE_SIZE(CPlayerPed, 0x7A4);

extern bool (&abTempNeverLeavesGroup)[7];
extern int32& gPlayIdlesAnimBlockIndex;

bool LOSBlockedBetweenPeds(CEntity* entity1, CEntity* entity2);
