/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eStats.h"
#include "eStatModAbilities.h"
#include "eStatsReactions.h"
#include "Text/Text.h"
#include <Enums/eRadioID.h>

enum eStatUpdateState : uint8 {
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

    static inline auto& StatMessage = StaticRef<tStatMessage[128]>(0xB78200);
    static inline auto& TotalNumStatMessages = StaticRef<uint32>(0xB794D0);
    static inline auto& LastMissionPassedName = StaticRef<char[8]>(0xB78A00);
    static inline auto& TimesMissionAttempted = StaticRef<int32[100]>(0xB78CC8);
    static inline auto& FavoriteRadioStationList = StaticRef<int32[14]>(0xB78E58);
    static inline auto& PedsKilledOfThisType = StaticRef<int32[32]>(0xB78E90);
    static inline auto& StatReactionValue = StaticRef<float[59]>(0xB78F10);
    static inline auto& StatTypesInt = StaticRef<int32[223]>(0xB79000);
    static inline auto& StatTypesFloat = StaticRef<float[82]>(0xB79380);
    static int16& m_ThisStatIsABarChart;
    static inline auto& bStatUpdateMessageDisplayed = StaticRef<bool>(0xB794D4);
    static inline auto& m_SprintStaminaCounter = StaticRef<uint32>(0xB794D8);
    static inline auto& m_CycleStaminaCounter = StaticRef<uint32>(0xB794DC);
    static inline auto& m_CycleSkillCounter = StaticRef<uint32>(0xB794E0);
    static inline auto& m_SwimStaminaCounter = StaticRef<uint32>(0xB794E4);
    static inline auto& m_SwimUnderWaterCounter = StaticRef<uint32>(0xB794E8);
    static inline auto& m_DrivingCounter = StaticRef<uint32>(0xB794EC);
    static inline auto& m_FlyingCounter = StaticRef<uint32>(0xB794F0);
    static inline auto& m_BoatCounter = StaticRef<uint32>(0xB794F4);
    static inline auto& m_BikeCounter = StaticRef<uint32>(0xB794F8);
    static inline auto& m_FatCounter = StaticRef<uint32>(0xB794FC);
    static inline auto& m_RunningCounter = StaticRef<uint32>(0xB79500);
    static inline auto& m_WeaponCounter = StaticRef<uint32>(0xB79504);
    static inline auto& m_DeathCounter = StaticRef<uint32>(0xB79508);
    static inline auto& m_MaxHealthCounter = StaticRef<uint32>(0xB7950C);
    static inline auto& m_AddToHealthCounter = StaticRef<uint32>(0xB79510);
    static inline auto& m_LastWeaponTypeFired = StaticRef<uint32>(0xB79514);
    static inline auto& bShowUpdateStats = StaticRef<bool>(0x8CDE56);

    static void InjectHooks();

    static char* GetStatID(eStats stat);
    static bool IsStatFloat(eStats stat);
    static float GetStatValue(eStats stat);
    static int8 GetTimesMissionAttempted(uint8 missionId);
    static void RegisterMissionAttempted(uint8 missionId);
    static void RegisterMissionPassed(uint8 missionId);
    static bool PopulateFavoriteRadioStationList();
    static int32* GetFullFavoriteRadioStationList();
    static eRadioID FindMostFavoriteRadioStation();
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
    static GxtChar* FindCriminalRatingString();
    static int32 ConstructStatLine(int32 arg0, uint8 arg1);
    static void ProcessReactionStatsOnIncrement(eStats stat);
    static void DisplayScriptStatUpdateMessage(eStatUpdateState state, eStats stat, float value);
    static void UpdateRespectStat(uint8 arg0);
    static void UpdateSexAppealStat();
    static void Init();
    static void IncrementStat(eStats stat, float value = 1.f);
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

    // NOTSA
    template<typename T> requires std::is_arithmetic_v<T>
    static T GetStatValue(eStats stat) {
        if constexpr (std::is_integral_v<T>) {
            // NOTE: First func checks if it's not a float stat, allowing unused ones.
            // Second check eliminates them as well.
            assert(!IsStatFloat(stat) && stat >= FIRST_INT_STAT);
        }
        if constexpr (std::is_floating_point_v<T>) {
            assert(IsStatFloat(stat));
        }
        const float r = CStats::GetStatValue(stat);

        // there shouldn't be any stat value bigger than int64::max.
        assert(std::is_floating_point_v<T> || std::in_range<T>(static_cast<int64>(r)));
        return static_cast<T>(r);
    }
};
