/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eStats.h"
#include "eStatModAbilities.h"
#include "eStatsReactions.h"

enum eStatUpdateState {
    STAT_UPDATE_DECREASE = 0,
    STAT_UPDATE_INCREASE = 1
};

enum eStatMessageCondition {
    STATMESSAGE_LESSTHAN = 0,
    STATMESSAGE_MORETHAN = 1
};

struct tStatMessage {
    int16 stat_num; // unique stat id
    bool displayed;
    uint8 condition; // this can be lessthan/morethan, see eStatMessageCondition
    float value;             // value stat must reach to display message
    char text_id[8];         // text id from american.gxt text file to display
};

VALIDATE_SIZE(tStatMessage, 0x10);

class CVehicle;
class CBike;
class CBmx;

class CStats {
public:
    static constexpr size_t FIRST_INT_STAT = 120;
    static constexpr size_t FIRST_UNUSED_STAT = 82;

    static tStatMessage (&StatMessage)[8];
    static uint32& TotalNumStatMessages;
    static char (&LastMissionPassedName)[8];
    static int32 (&TimesMissionAttempted)[100];
    static int32 (&FavoriteRadioStationList)[14];
    static int32 (&PedsKilledOfThisType)[32];
    static float (&StatReactionValue)[59];
    static int32 (&StatTypesInt)[223];
    static float (&StatTypesFloat)[82];
    static int16& m_ThisStatIsABarChart;
    static bool& bStatUpdateMessageDisplayed;
    static uint32& m_SprintStaminaCounter;
    static uint32& m_CycleStaminaCounter;
    static uint32& m_CycleSkillCounter;
    static uint32& m_SwimStaminaCounter;
    static uint32& m_SwimUnderWaterCounter;
    static uint32& m_DrivingCounter;
    static uint32& m_FlyingCounter;
    static uint32& m_BoatCounter;
    static uint32& m_BikeCounter;
    static uint32& m_FatCounter;
    static uint32& m_RunningCounter;
    static uint32& m_WeaponCounter;
    static uint32& m_DeathCounter;
    static uint32& m_MaxHealthCounter;
    static uint32& m_AddToHealthCounter;
    static uint32& m_LastWeaponTypeFired;

    static void InjectHooks();

    static char* GetStatID(eStats stat);
    static bool GetStatType(eStats stat);
    static float GetStatValue(eStats stat);
    static char GetTimesMissionAttempted(uint8 missionId);
    static void RegisterMissionAttempted(uint8 missionId);
    static void RegisterMissionPassed(uint8 missionId);
    static bool PopulateFavoriteRadioStationList();
    static int32* GetFullFavoriteRadioStationList();
    static int32 FindMostFavoriteRadioStation();
    static int32 FindLeastFavoriteRadioStation();
    static int32 FindCriminalRatingNumber();
    static float GetPercentageProgress();
    static void BuildStatLine(char* line, void* pValue1, int32 metrics, void* pValue2, int32 type);
    static int32 ConvertToMins(int32 value);
    static int32 ConvertToSecs(int32 value);
    static bool SafeToShowThisStat(eStats stat);
    static bool CheckForThreshold(float* pValue, float range);
    static bool IsStatCapped(eStats stat);
    static void ProcessReactionStatsOnDecrement(eStats stat);
    static void CheckForStatsMessage();
    static void LoadStatUpdateConditions();
    static void LoadActionReactionStats();
    static int32 FindMaxNumberOfGroupMembers();
    static float GetFatAndMuscleModifier(eStatModAbilities statMod);
    static void DecrementStat(eStats stat, float value);
    static void SetStatValue(eStats stat, float value);
    static void RegisterFastestTime(eStats stat, int32 fastestTime);
    static void RegisterBestPosition(eStats stat, int32 position);
    static char* FindCriminalRatingString();
    static int32 ConstructStatLine(int32 arg0, uint8 arg1);
    static void ProcessReactionStatsOnIncrement(eStats stat);
    static void DisplayScriptStatUpdateMessage(uint8 state, eStats stat, float value);
    static void UpdateRespectStat(uint8 arg0);
    static void UpdateSexAppealStat();
    static void Init();
    static void IncrementStat(eStats stat, float value);
    static void SetNewRecordStat(eStats stat, float value);
    static void UpdateFatAndMuscleStats(uint32 value);
    static void UpdateStatsWhenSprinting();
    static void UpdateStatsWhenRunning();
    static void UpdateStatsWhenCycling(bool arg0, CBmx* bmx);
    static void UpdateStatsWhenSwimming(bool arg0, bool arg1);
    static void UpdateStatsWhenDriving(CVehicle* vehicle);
    static void UpdateStatsWhenFlying(CVehicle* vehicle);
    static void UpdateStatsWhenOnMotorBike(CBike* bike);
    static void UpdateStatsWhenWeaponHit(eWeaponType weaponType);
    static void UpdateStatsWhenFighting();
    static void UpdateStatsOnRespawn();
    static void UpdateStatsAddToHealth(uint32 addToHealth);
    static void ModifyStat(eStats stat, float value);
    static bool Save();
    static bool Load();
};
