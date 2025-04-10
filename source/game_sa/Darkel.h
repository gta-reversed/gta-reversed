/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eWeaponType.h"

class CVehicle;

enum eDarkelStatus {
    DARKEL_STATUS_0,
    DARKEL_STATUS_1,
    DARKEL_STATUS_2,
    DARKEL_STATUS_3,
    DARKEL_STATUS_4,
};

class CDarkel {
public:
    inline static std::array<int16[2], 800>& RegisteredKills = *reinterpret_cast<std::array<int16[2], 800>*>(0x969A50);
    inline static const GxtChar*& pStartMessage = *reinterpret_cast<const GxtChar**>(0x96A6D0);
    inline static uint32& AmmoInterruptedWeapon = *reinterpret_cast<uint32*>(0x96A6D4);
    inline static eWeaponType& InterruptedWeaponType = *reinterpret_cast<eWeaponType*>(0x96A6D8);
    inline static eWeaponType& InterruptedWeaponTypeSelected = *reinterpret_cast<eWeaponType*>(0x96A6DC);
    inline static uint32& TimeOfFrenzyStart = *reinterpret_cast<uint32*>(0x96A6E0);
    inline static int32& PreviousTime = *reinterpret_cast<int32*>(0x96A6E4);
    inline static int32& TimeLimit = *reinterpret_cast<int32*>(0x96A6E8);
    inline static int32& KillsNeeded = *reinterpret_cast<int32*>(0x96A6EC);
    inline static std::array<int32, 4>& ModelToKill = *reinterpret_cast<std::array<int32, 4>*>(0x96A6F0);
    inline static eWeaponType& WeaponType = *reinterpret_cast<eWeaponType*>(0x96A700);
    inline static eDarkelStatus& Status = *reinterpret_cast<eDarkelStatus*>(0x96A704);

    inline static bool& bHeadShotRequired = *(bool*)0x969A49;
    inline static bool& bStandardSoundAndMessages = *(bool*)0x969A4A;
    inline static bool& bProperKillFrenzy = *(bool*)0x969A48;

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
    static int16 QueryModelsKilledByPlayer(int32 modelId, int32 playerId);
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
