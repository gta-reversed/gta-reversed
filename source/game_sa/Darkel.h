/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eWeaponType.h"

class CVehicle;

enum class eDarkelStatus : uint16 {
    INITIAL,
    FRENZY_ON_GOING,
    FRENZY_PASSED,
    FRENZY_FAILED,
    FRENZY_ON_GOING_2P,
};

class CDarkel {
public:
    static inline auto& RegisteredKills = StaticRef<std::array<int16[2], 800>>(0x969A50);
    static inline auto& pStartMessage = StaticRef<const GxtChar*>(0x96A6D0);
    static inline auto& AmmoInterruptedWeapon = StaticRef<uint32>(0x96A6D4);
    static inline auto& InterruptedWeaponType = StaticRef<eWeaponType>(0x96A6D8);
    static inline auto& InterruptedWeaponTypeSelected = StaticRef<eWeaponType>(0x96A6DC);
    static inline auto& TimeOfFrenzyStart = StaticRef<uint32>(0x96A6E0);
    static inline auto& PreviousTime = StaticRef<int32>(0x96A6E4);
    static inline auto& TimeLimit = StaticRef<int32>(0x96A6E8);
    static inline auto& KillsNeeded = StaticRef<int32>(0x96A6EC);
    static inline auto& ModelToKill = StaticRef<std::array<int32, 4>>(0x96A6F0);
    static inline auto& WeaponType = StaticRef<eWeaponType>(0x96A700);
    static inline auto& Status = StaticRef<eDarkelStatus>(0x96A704);

    static inline auto& bHeadShotRequired = StaticRef<bool>(0x969A49);
    static inline auto& bStandardSoundAndMessages = StaticRef<bool>(0x969A4A);
    static inline auto& bProperKillFrenzy = StaticRef<bool>(0x969A48);

public:
    static void InjectHooks();

    static bool FrenzyOnGoing();
    static void Init();
    static void DrawMessages();
    static eDarkelStatus ReadStatus();
    static void RegisterKillNotByPlayer(const CPed* killedPed);
    static bool ThisPedShouldBeKilledForFrenzy(const CPed& ped);
    static bool ThisVehicleShouldBeKilledForFrenzy(const CVehicle& vehicle);
    static void StartFrenzy(eWeaponType weaponType, int32 timeLimit, uint16 killsNeeded, int32 modelToKill, const GxtChar* startMessage, int32 modelToKill2, int32 modelToKill3, int32 modelToKill4, bool standardSoundAndMessages, bool needHeadShot);
    static void ResetModelsKilledByPlayer(int32 playerId);
    static int16 QueryModelsKilledByPlayer(eModelID modelId, int32 playerId);
    static int32 FindTotalPedsKilledByPlayer(int32 playerId);
    static void DealWithWeaponChangeAtEndOfFrenzy();
    static bool CheckDamagedWeaponType(eWeaponType damageWeaponId, eWeaponType expectedDamageWeaponId);
    static void Update();
    static void ResetOnPlayerDeath();
    static void FailKillFrenzy();
    static void RegisterKillByPlayer(const CPed& killedPed, eWeaponType damageWeaponId, bool headShotted, int32 playerId);
    static void RegisterCarBlownUpByPlayer(CVehicle& vehicle, int32 playerId);

    static uint8 CalcFade(uint32 t, uint32 begin, uint32 end);
};
