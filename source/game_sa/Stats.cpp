/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "Stats.h"
#include "MenuSystem.h"
#include "Hud.h"
#include "TagManager.h"

// NOTSA
// NOTE: Not `CTimer::GetTimeStepInMS()`: The game multiplies by 0.02 (instead of dividing by 50) and drops the fraction
static uint32 GetTimeStepInWholeMS() {
    return (uint32)(CTimer::GetTimeStep() * 0.02f * 1000.0f);
}

void CStats::InjectHooks() {
    RH_ScopedClass(CStats);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x55C0C0);
    RH_ScopedOverloadedInstall(GetStatValue, "-OG", 0x558E40, float(*)(eStats));
    RH_ScopedInstall(SetStatValue, 0x55A070);
    RH_ScopedInstall(IsStatFloat, 0x558E30);
    /*RH_ScopedInstall(GetFullFavoriteRadioStationList, 0x558F90); - different return type*/
    RH_ScopedInstall(FindCriminalRatingNumber, 0x559080);
    RH_ScopedInstall(GetPercentageProgress, 0x5591E0);
    RH_ScopedInstall(ConvertToMins, 0x559540);
    RH_ScopedInstall(ConvertToSecs, 0x559560);
    RH_ScopedInstall(SafeToShowThisStat, 0x559590);
    RH_ScopedInstall(CheckForThreshold, 0x5595F0);
    RH_ScopedInstall(IsStatCapped, 0x559630);
    RH_ScopedInstall(LoadActionReactionStats, 0x5599B0);
    RH_ScopedInstall(FindMaxNumberOfGroupMembers, 0x559A50);
    RH_ScopedInstall(ProcessReactionStatsOnDecrement, 0x559730);
    RH_ScopedInstall(DecrementStat, 0x559FA0);
    RH_ScopedInstall(SetNewRecordStat, 0x55C410);
    RH_ScopedInstall(RegisterFastestTime, 0x55A0B0);
    RH_ScopedInstall(RegisterBestPosition, 0x55A160);
    RH_ScopedInstall(ProcessReactionStatsOnIncrement, 0x55B900);
    RH_ScopedInstall(DisplayScriptStatUpdateMessage, 0x55B980);
    RH_ScopedInstall(IncrementStat, 0x55C180);
    RH_ScopedInstall(UpdateStatsWhenSprinting, 0x55C660);
    RH_ScopedInstall(UpdateStatsWhenRunning, 0x55C6F0);
    RH_ScopedInstall(UpdateStatsWhenOnMotorBike, 0x55CD60);
    RH_ScopedInstall(UpdateStatsWhenFighting, 0x55CFA0);
    RH_ScopedInstall(ModifyStat, 0x55D090);

    // unused
    RH_ScopedInstall(GetStatID, 0x558DE0);
    RH_ScopedInstall(GetTimesMissionAttempted, 0x558E70);
    RH_ScopedInstall(RegisterMissionAttempted, 0x558E80);
    RH_ScopedInstall(RegisterMissionPassed, 0x558EA0);

    RH_ScopedInstall(Load, 0x5D3BF0);
    RH_ScopedInstall(Save, 0x5D3B40);
    RH_ScopedInstall(PopulateFavoriteRadioStationList, 0x558EC0);
    RH_ScopedInstall(FindMostFavoriteRadioStation, 0x558FA0);
    RH_ScopedInstall(FindLeastFavoriteRadioStation, 0x559010);
    RH_ScopedInstall(BuildStatLine, 0x559230, { .Reversed = false });
    RH_ScopedInstall(CheckForStatsMessage, 0x559760);
    RH_ScopedInstall(LoadStatUpdateConditions, 0x559860);
    RH_ScopedInstall(GetFatAndMuscleModifier, 0x559AF0);
    RH_ScopedInstall(FindCriminalRatingString, 0x55A210, { .Reversed = false });
    RH_ScopedInstall(ConstructStatLine, 0x55A780, { .Reversed = false });
    RH_ScopedInstall(UpdateRespectStat, 0x55BC50);
    RH_ScopedInstall(UpdateSexAppealStat, 0x55BF20);
    RH_ScopedInstall(UpdateFatAndMuscleStats, 0x55C470);
    RH_ScopedInstall(UpdateStatsWhenCycling, 0x55C780);
    RH_ScopedInstall(UpdateStatsWhenSwimming, 0x55C990);
    RH_ScopedInstall(UpdateStatsWhenDriving, 0x55CAC0);
    RH_ScopedInstall(UpdateStatsWhenFlying, 0x55CC00);
    RH_ScopedInstall(UpdateStatsWhenWeaponHit, 0x55CEB0);
    RH_ScopedInstall(UpdateStatsOnRespawn, 0x55CFC0);
    RH_ScopedInstall(UpdateStatsAddToHealth, 0x55D030);
}

// 0x55C0C0
void CStats::Init() {
    std::ranges::fill(StatTypesFloat, 0.0f);
    std::ranges::fill(StatTypesInt, 0);
    std::ranges::fill(PedsKilledOfThisType, 0);
    std::ranges::fill(TimesMissionAttempted, 0);

    bStatUpdateMessageDisplayed = false;
    CTimer::SetTimeInMS(0);
    std::ranges::fill(LastMissionPassedName, 0);
    m_SprintStaminaCounter = 0;
    m_CycleStaminaCounter = 0;
    m_SwimStaminaCounter = 0;
    m_DrivingCounter = 0;
    m_FlyingCounter = 0;
    m_BoatCounter = 0;
    m_BikeCounter = 0;
    m_FatCounter = 0;
    m_RunningCounter = 0;

    StatTypesFloat[STAT_MAX_HEALTH] = 569.0f;
    CheckForStatsMessage();
    StatTypesFloat[STAT_STAMINA] = 100.0f;
    CheckForStatsMessage();

    PopulateFavoriteRadioStationList();
    LoadActionReactionStats();
    LoadStatUpdateConditions();
}

// 0x558E40
float CStats::GetStatValue(eStats stat) {
    if (!IsStatFloat(stat)) { // int32
        assert(stat >= FIRST_INT_STAT);

        return static_cast<float>(StatTypesInt[stat - FIRST_INT_STAT]);
    }

    return StatTypesFloat[stat];
}

// 0x55A070
void CStats::SetStatValue(eStats stat, float value) {
    if (IsStatFloat(stat)) {
        StatTypesFloat[stat] = value;
    } else { // int32
        assert(stat >= FIRST_INT_STAT);

        StatTypesInt[stat - FIRST_INT_STAT] = (int16)(value); // NOTE: The game keeps the low 16 bits only (0x55A098)
    }
    CheckForStatsMessage();
}

// 0x558E30
bool CStats::IsStatFloat(eStats stat) {
    return stat < FIRST_UNUSED_STAT;
}

// 0x558EC0
bool CStats::PopulateFavoriteRadioStationList() {
    const auto* listenTimes = AudioEngine.GetRadioStationListenTimes();
    auto allZero = true;
    for (auto&& [i, time] : rngv::enumerate(FavoriteRadioStationList)) {
        time = listenTimes[i];
        if (time != 0) {
            allZero = false;
        }
    }
    return allZero;
}

// 0x558FA0
eRadioID CStats::FindMostFavoriteRadioStation() {
    auto favorite = RADIO_CLASSIC_HIP_HOP;
    auto longestTime = 0;
    for (auto station = (int32)RADIO_CLASSIC_HIP_HOP; station <= RADIO_USER_TRACKS; station++) {
        if (FavoriteRadioStationList[station] > longestTime) {
            longestTime = FavoriteRadioStationList[station];
            favorite = (eRadioID)station;
        }
    }
    return favorite;
}

// 0x559010
int32 CStats::FindLeastFavoriteRadioStation() {
    auto favorite = (int32)RADIO_CLASSIC_HIP_HOP;
    auto shortestTime = FavoriteRadioStationList[favorite];
    for (auto station = (int32)RADIO_COUNTRY; station <= RADIO_USER_TRACKS; station++) {
        if (FavoriteRadioStationList[station] < shortestTime) {
            shortestTime = FavoriteRadioStationList[station];
            favorite = station;
        }
    }
    return favorite;
}

// 0x559080
int32 CStats::FindCriminalRatingNumber() {
    CPlayerInfo* playerInfo = FindPlayerPed()->GetPlayerInfoForThisPlayerPed();

    auto value = (int32)(
        GetStatValue(STAT_TOTAL_LEGITIMATE_KILLS)
        - (GetStatValue(STAT_TIMES_BUSTED) - GetStatValue(STAT_NUMBER_OF_HOSPITAL_VISITS)) * 3.0f
        + (GetStatValue(STAT_HIGHEST_FIREFIGHTER_MISSION_LEVEL) + GetStatValue(STAT_HIGHEST_PARAMEDIC_MISSION_LEVEL)) * 10.0f
        + int32((float)playerInfo->m_nMoney / 5000.0f)
        + GetStatValue(STAT_PLANES_HELICOPTERS_DESTROYED) * 30.0f
        + GetStatValue(STAT_TOTAL_FIRES_EXTINGUISHED)
        + GetStatValue(STAT_CRIMINALS_KILLED_ON_VIGILANTE_MISSION)
        + GetStatValue(STAT_PEOPLE_SAVED_IN_AN_AMBULANCE)
    );

    if (CCheat::m_bHasPlayerCheated || GetStatValue(STAT_TIMES_CHEATED) > 0.0f) {
        value -= 10 * (int32)GetStatValue(STAT_TIMES_CHEATED);

        value = std::max(value, -10000);
    } else {
        value = std::max(value, 0);
    }

    float bulletsFired = GetStatValue(STAT_BULLETS_FIRED);

    if (bulletsFired >= 100.0f) {
        value += (int32)(500 * (GetStatValue(STAT_BULLETS_THAT_HIT) / bulletsFired));
    }

    return value + (int32)(10 * GetPercentageProgress());
}

// 0x5591E0
float CStats::GetPercentageProgress() {
    return std::min(StatTypesFloat[STAT_PROGRESS_MADE] / 187.0f * 100.0f, 100.0f);
}

// 0x559230
void CStats::BuildStatLine(char* line, void* pValue1, int32 metrics, void* pValue2, int32 type) {
    plugin::Call<0x559230, char*, void*, int32, void*, int32>(line, pValue1, metrics, pValue2, type);
}

// 0x559540
int32 CStats::ConvertToMins(int32 statValue) {
    if (statValue > 59)
        return (statValue - 60) / 60 + 1;
    return 0;
}

// 0x559560
int32 CStats::ConvertToSecs(int32 statValue) {
    int32 seconds = statValue;
    if (statValue > 59)
        seconds = -60 - 60 * ((statValue - 60) / 60) + statValue;
    if (seconds < 0)
        seconds = -seconds;
    return seconds;
}

// 0x559590
bool CStats::SafeToShowThisStat(eStats stat) {
    if (!CLocalisation::GermanGame()) {
        return true;
    }

    switch (stat) {
    case STAT_RAMPAGES_ATTEMPTED:
    case STAT_RAMPAGES_PASSED:
    case STAT_TOTAL_LEGITIMATE_KILLS:
    case STAT_HIGHEST_CIVILIAN_PEDS_KILLED_ON_RAMPAGE:
    case STAT_HIGHEST_POLICE_PEDS_KILLED_ON_RAMPAGE:
    case STAT_HIGHEST_CIVILIAN_VEHICLES_DESTROYED_ON_RAMPAGE:
    case STAT_HIGHEST_POLICE_VEHICLES_DESTROYED_ON_RAMPAGE:
    case STAT_HIGHEST_NUMBER_OF_TANKS_DESTROYED_ON_RAMPAGE:
        return false;
    default:
        return true;
    }
}

// 0x5595F0
bool CStats::CheckForThreshold(float* pValue, float range) {
    if (*pValue + 40.0f >= range && *pValue - 40.0f <= range) {
        return false;
    }
    *pValue = range;
    return true;
}

// 0x559630
bool CStats::IsStatCapped(eStats stat) {
    switch (stat) {
    case STAT_GIRLFRIEND_RESPECT:
    case STAT_CLOTHES_RESPECT:
    case STAT_FITNESS_RESPECT:
    case STAT_FAT:
    case STAT_STAMINA:
    case STAT_MUSCLE:
    case STAT_MAX_HEALTH:
    case STAT_SEX_APPEAL:
    case STAT_PISTOL_SKILL:
    case STAT_SILENCED_PISTOL_SKILL:
    case STAT_DESERT_EAGLE_SKILL:
    case STAT_SHOTGUN_SKILL:
    case STAT_SAWN_OFF_SHOTGUN_SKILL:
    case STAT_COMBAT_SHOTGUN_SKILL:
    case STAT_MACHINE_PISTOL_SKILL:
    case STAT_SMG_SKILL:
    case STAT_AK_47_SKILL:
    case STAT_M4_SKILL:
    case STAT_RIFLE_SKILL:
    case STAT_APPEARANCE:
    case STAT_ARMOR:
    case STAT_ENERGY:
    case STAT_DRIVING_SKILL:
    case STAT_FLYING_SKILL:
    case STAT_LUNG_CAPACITY:
    case STAT_BIKE_SKILL:
    case STAT_LUCK:
    case STAT_HORSESHOES_COLLECTED:
    case STAT_TOTAL_HORSESHOES:
    case STAT_OYSTERS_COLLECTED:
    case STAT_TOTAL_OYSTERS:
    case STAT_CYCLING_SKILL:
        return true;
    }
    return false;
}

// 0x559760
void CStats::CheckForStatsMessage() {
    if (CPad::GetPad(0)->JustOutOfFrontEnd
        || !bShowUpdateStats
        || TheCamera.m_bWideScreenOn
        || CCutsceneMgr::ms_cutsceneProcessing
        || CHud::HelpMessageDisplayed()
        || CMenuSystem::num_menus_in_use) {
        return;
    }

    for (auto& message : StatMessage | rngv::take(TotalNumStatMessages)) {
        if (message.displayed) {
            continue;
        }
        const auto value = GetStatValue((eStats)(message.stat_num));
        if (message.condition != STATMESSAGE_LESSTHAN ? value < message.value : value > message.value) {
            continue;
        }
        message.displayed = true;
        CHud::SetHelpMessage(TheText.Get(message.text_id));
        bStatUpdateMessageDisplayed = true;
    }
}

// 0x559860
void CStats::LoadStatUpdateConditions() {
    CFileMgr::SetDir("");

    auto* const file     = CFileMgr::OpenFile("DATA\\STATDISP.DAT", "rb");

    TotalNumStatMessages = 0;

    uint32 numMessages   = 0;
    for (char* line = CFileLoader::LoadLine(file); line != nullptr; line = CFileLoader::LoadLine(file)) {
        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }

        // NOTE/BUG: The original has no check for the size of the array
        if (notsa::bugfixes::GenericOOB && numMessages >= StatMessage.size()) {
            break;
        }

        int32 statId{};
        float value{};
        char  statName[80]{}, condition[12]{}, textId[8]{};
        VERIFY(sscanf_s(line, "%d %s %s %f %s", &statId, SCANF_S_STR(statName), SCANF_S_STR(condition), &value, SCANF_S_STR(textId)) == 5);

        auto& message     = StatMessage.data()[numMessages++];
        message.stat_num  = (int16)(statId);
        message.displayed = false;
        if (!strcmp(condition, "lessthan")) {
            message.condition = STATMESSAGE_LESSTHAN;
        } else if (!strcmp(condition, "morethan")) {
            message.condition = STATMESSAGE_MORETHAN;
        }
        message.value = value;
        strcpy(message.text_id, textId);
    }
    TotalNumStatMessages = numMessages;

    CFileMgr::CloseFile(file);
}

// 0x5599B0
void CStats::LoadActionReactionStats() {
    CFileMgr::SetDir("");

    auto* file = CFileMgr::OpenFile("DATA\\AR_STATS.DAT", "rb");

    for (char* line = CFileLoader::LoadLine(file); line != nullptr; line = CFileLoader::LoadLine(file)) {
        int32 reactId;
        float reactValue;

        if (line[0] != '#' && line[0] != NULL) {
            VERIFY(sscanf_s(line, "%d %*s %f", &reactId, &reactValue) == 2);

            StatReactionValue[reactId] = reactValue;
        }
    }

    CFileMgr::CloseFile(file);
}

// 0x559A50
int32 CStats::FindMaxNumberOfGroupMembers() {
    float respect = StatTypesFloat[STAT_TOTAL_RESPECT];

    if (respect < 10.0f)
        return 0;
    if (respect < 60.0f)
        return 2;
    if (respect < 160.0f)
        return 3;
    if (respect < 330.0f)
        return 4;
    if (respect < 540.0f)
        return 5;
    if (respect < 800.0f)
        return 6;

    return 7;
}

// 0x559AF0
float CStats::GetFatAndMuscleModifier(eStatModAbilities statMod) {
    constexpr auto FAT_START = 200.0f, MUSCLE_START = 50.0f, STAT_SCALE = 0.001f, MAX_HEALTH = 176.0f;

    const auto GetFat = [](float mult) {
        return (StatTypesFloat[STAT_FAT] - FAT_START) * mult / (1000.0f - FAT_START);
    };
    const auto GetFatProgress = [] {
        return std::max(0.0f, (StatTypesFloat[STAT_FAT] - FAT_START) / (1000.0f - FAT_START));
    };
    const auto GetMuscle = [](float mult) {
        return (StatTypesFloat[STAT_MUSCLE] - MUSCLE_START) * mult / (1000.0f - MUSCLE_START);
    };
    const auto GetBikeSkill = [] {
        return std::min(1.0f, (float)(StatTypesInt[STAT_BIKE_SKILL - FIRST_INT_STAT]) * STAT_SCALE);
    };
    const auto GetCycleSkill = [] {
        return (float)(StatTypesInt[STAT_CYCLING_SKILL - FIRST_INT_STAT]) * STAT_SCALE;
    };

    switch (statMod) {
    case STAT_MOD_0: // 0x559B0A
        return StatTypesFloat[STAT_FAT] > 800.0f
            ? 2.0f
            : StatTypesFloat[STAT_FAT] > 400.0f
            ? 1.0f
            : 0.0f;
    case STAT_MOD_1: // 0x559B41
        return std::max(GetFatProgress() * -0.4f + 1.0f + GetMuscle(0.2f), 0.7f);
    case STAT_MOD_2: // 0x559BB3
        return std::max(GetFatProgress() * -0.2f + 1.0f + GetMuscle(-0.1f), 0.8f);
    case STAT_MOD_3: // 0x559C25
        return std::max(GetFat(-0.2f) + 1.0f + GetMuscle(-0.1f), 0.8f);
    case STAT_MOD_4: // 0x559C61
        return std::min(GetFat(0.5f) + 1.0f + GetMuscle(1.0f), 2.0f);
    case STAT_MOD_5: // 0x559CC5
        return std::clamp(GetFat(-1.0f) + 1.0f + GetMuscle(0.5f) + StatTypesFloat[STAT_STAMINA] * STAT_SCALE * 0.5f + GetCycleSkill(), 0.25f, 2.0f);
    case STAT_MOD_6: // 0x559D62
        return std::clamp(GetFat(-0.5f) + 1.0f + GetCycleSkill(), 0.5f, 2.0f);
    case STAT_MOD_TIME_CAN_RUN: // 0x559DC9
        return StatTypesFloat[STAT_STAMINA] * STAT_SCALE * 3000.0f + 150.0f;
    case STAT_MOD_AIR_IN_LUNG: // 0x559DE4
        return ((float)(StatTypesInt[STAT_LUNG_CAPACITY - FIRST_INT_STAT]) + StatTypesFloat[STAT_STAMINA]) * 0.0005f * 3000.0f + 1000.0f;
    case STAT_MOD_MAX_HEALTH: // 0x559E05
        return StatTypesFloat[STAT_MAX_HEALTH] * STAT_SCALE * MAX_HEALTH;
    case STAT_MOD_10: // 0x559E1A
        return MAX_HEALTH;
    case STAT_MOD_11: // 0x559E23
        return GetBikeSkill() * 0.5f + 1.0f;
    case STAT_MOD_12: // 0x559E4F
        return GetBikeSkill() * 0.3f + 1.0f;
    case STAT_MOD_13: // 0x559E7B
        return GetBikeSkill() * 0.25f + 1.0f;
    case STAT_MOD_DRIVING_SKILL: // 0x559EA7
        return std::min(GetFat(0.5f) + ((float)(StatTypesInt[STAT_DRIVING_SKILL - FIRST_INT_STAT]) * STAT_SCALE * 0.5f + 1.0f), 1.0f);
    case STAT_MOD_15: // 0x559EF9
        return std::max(GetFat(-0.5f) + 1.0f + GetMuscle(-0.25f), 0.5f);
    default:
        return 1.0f;
    }
}

// 0x559730
void CStats::ProcessReactionStatsOnDecrement(eStats stat) {
    if (stat == STAT_ENERGY && GetStatValue(STAT_ENERGY) < 0.0f)
        DecrementStat(STAT_FAT, 23.0f);
}

// 0x559FA0
void CStats::DecrementStat(eStats stat, float value) {
    if (value <= 0.0f)
        return;

    if (IsStatFloat(stat)) {
        StatTypesFloat[stat] = notsa::step_down_to(StatTypesFloat[stat], 0.0f, value);
    } else {
        assert(stat >= FIRST_INT_STAT);

        auto& statValue = StatTypesInt[stat - FIRST_INT_STAT];
        statValue       = std::max(statValue - (int16)(value), 0); // NOTE: The game uses the low 16 bits of the value only (0x55A008)
    }

    ProcessReactionStatsOnDecrement(stat);
    CheckForStatsMessage();
}

// 0x55C410
void CStats::SetNewRecordStat(eStats stat, float value) {
    float currentValue = GetStatValue(stat);

    if (currentValue < value)
        SetStatValue(stat, value);

    CheckForStatsMessage();
}

// 0x55A0B0
void CStats::RegisterFastestTime(eStats stat, int32 fastestTime) {
    SetNewRecordStat(stat, (float)fastestTime);
}

// 0x55A160
void CStats::RegisterBestPosition(eStats stat, int32 position) {
    SetNewRecordStat(stat, (float)position);
}

// 0x55A210
GxtChar* CStats::FindCriminalRatingString() {
    return plugin::CallAndReturn<GxtChar*, 0x55A210>();
}

// 0x55A780
int32 CStats::ConstructStatLine(int32 arg0, uint8 arg1) {
    return plugin::CallAndReturn<int32, 0x55A780, int32, uint8>(arg0, arg1);
}

// 0x55B900
void CStats::ProcessReactionStatsOnIncrement(eStats stat) {
    if (stat != STAT_STAMINA && stat != STAT_ENERGY && stat != STAT_LUNG_CAPACITY)
        return;

    float energy = GetStatValue(STAT_ENERGY);

    if (stat == STAT_STAMINA || stat == STAT_LUNG_CAPACITY) {
        if (energy < 0.0f) {
            StatTypesFloat[STAT_FAT] = std::max(StatTypesFloat[STAT_FAT] - 23.0f, 0.0f);
            CheckForStatsMessage();
        }
        return;
    }

    if (energy > 1000.0f)
        IncrementStat(STAT_FAT, energy - 1000.0f);
}

// 0x55B980
void CStats::DisplayScriptStatUpdateMessage(eStatUpdateState state, eStats stat, float value) {
    if (CPad::GetPad(0)->JustOutOfFrontEnd
        || !bShowUpdateStats
        || TheCamera.m_bWideScreenOn
        || CHud::HelpMessageDisplayed()
        || bStatUpdateMessageDisplayed
        || CMenuSystem::num_menus_in_use)
    {
        bStatUpdateMessageDisplayed = false;

        return;
    }

    if (IsStatCapped(stat) && GetStatValue(stat) >= 1000.0f)
        return;

    switch (stat) {
    case STAT_FAT:
    case STAT_STAMINA:
    case STAT_MUSCLE:
    case STAT_MAX_HEALTH:
    case STAT_SEX_APPEAL:
    case STAT_PISTOL_SKILL:
    case STAT_SILENCED_PISTOL_SKILL:
    case STAT_DESERT_EAGLE_SKILL:
    case STAT_SHOTGUN_SKILL:
    case STAT_SAWN_OFF_SHOTGUN_SKILL:
    case STAT_COMBAT_SHOTGUN_SKILL:
    case STAT_MACHINE_PISTOL_SKILL:
    case STAT_SMG_SKILL:
    case STAT_AK_47_SKILL:
    case STAT_M4_SKILL:
    case STAT_RIFLE_SKILL:
    case STAT_GAMBLING:
    case STAT_DRIVING_SKILL:
    case STAT_ARMOR:
    case STAT_ENERGY:
        if (value > 1.0f)
            CHud::SetHelpMessageStatUpdate(state, stat, value, 1000.0f);
        break;

    case STAT_TOTAL_RESPECT:
        CHud::SetHelpMessageStatUpdate(state, stat, value, 1000.0f);
        break;

    case STAT_FLYING_SKILL:
    case STAT_LUNG_CAPACITY:
    case STAT_BIKE_SKILL:
    case STAT_CYCLING_SKILL:
    case STAT_LUCK:
        if (value > 1.0f)
            CHud::SetHelpMessageStatUpdate(state, stat, value, 1000.0f);
        break;

    case STAT_PROGRESS_WITH_DENISE:
    case STAT_PROGRESS_WITH_MICHELLE:
    case STAT_PROGRESS_WITH_HELENA:
    case STAT_PROGRESS_WITH_BARBARA:
    case STAT_PROGRESS_WITH_KATIE:
    case STAT_PROGRESS_WITH_MILLIE:
        CHud::SetHelpMessageStatUpdate(state, stat, value, 100.0f);
        break;

    case STAT_PIMPING_LEVEL:
        CHud::SetHelpMessageStatUpdate(state, STAT_PIMPING_LEVEL, value, 10.0f);
        break;

    case STAT_GANG_STRENGTH: {
        if (auto player = FindPlayerPed())
        {
            auto maxGroup = std::min<uint8>(FindMaxNumberOfGroupMembers(), player->GetPlayerData()->m_nScriptLimitToGangSize);
            CHud::SetHelpMessageStatUpdate(state, stat, value, maxGroup);
        }
        break;
    }
    default:
        return;
    }
}

// 0x55BC50
void CStats::UpdateRespectStat(bool reset) {
    constexpr auto RESPECT_NOT_SET    = -99.0f;

    static auto& s_LastRespectDisplay = StaticRef<float>(0x8CDEC4); // The respect the last message was shown at
    static auto& s_LastRespect        = StaticRef<float>(0x8CDEC8);

    if (reset) {
        s_LastRespect        = RESPECT_NOT_SET;
        s_LastRespectDisplay = RESPECT_NOT_SET;
        return;
    }

    const auto missionRespect = (float)(StatTypesInt[STAT_RESPECT_MISSION - FIRST_INT_STAT]) * 1000.0f
        / std::max((float)(StatTypesInt[STAT_RESPECT_MISSION_TOTAL - FIRST_INT_STAT]), 1.0f);
    const auto runningRespect   = StatTypesFloat[STAT_RESPECT] * 0.4f + missionRespect * 0.36f + StatTypesFloat[STAT_GIRLFRIEND_RESPECT] * 0.03f;
    const auto territoryRespect = std::max(CGangWars::TerritoryUnderControlPercentage - 0.2f, 0.0f) * 1250.0f * 0.05f + runningRespect;
    const auto moneyRespect     = std::min((float)(FindPlayerInfo().m_nMoney) * 0.0000001f, 1.0f);
    const auto tagRespect       = (float)(CTagManager::GetPercentageTagged() * 10) * 0.05f;
    const auto totalRespect     = tagRespect
        + (moneyRespect * 1000.0f * 0.05f + territoryRespect + StatTypesFloat[STAT_MUSCLE] * 0.03f + StatTypesFloat[STAT_CLOTHES_RESPECT] * 0.03f);
    const auto respect = CCheat::IsActive(CHEAT_MAX_RESPECT)
        ? 1000.0f
        : std::max(totalRespect, 0.0f);

    if ((int32)(s_LastRespect) == (int32)(respect)) {
        return;
    }

    StatTypesFloat[STAT_TOTAL_RESPECT] = respect;
    CheckForStatsMessage();

    const auto wasSet = s_LastRespect != RESPECT_NOT_SET && s_LastRespectDisplay != RESPECT_NOT_SET;
    if (respect > s_LastRespectDisplay || s_LastRespect < 2.0f) { // 0x55BE97
        if (wasSet && (CheckForThreshold(&s_LastRespectDisplay, respect) || s_LastRespect < 2.0f)) {
            DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_TOTAL_RESPECT, respect);
        }
        if (respect < s_LastRespect) {
            s_LastRespectDisplay = respect;
        }
    } else {
        if (wasSet && (CheckForThreshold(&s_LastRespectDisplay, respect) || respect < 2.0f)) {
            DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_TOTAL_RESPECT, respect);
        }
        if (respect > s_LastRespect) {
            s_LastRespectDisplay = respect;
        }
    }
    s_LastRespect = respect;
}

// 0x55BF20
void CStats::UpdateSexAppealStat() {
    static auto& s_LastVehicle = StaticRef<CVehicle*>(0xB79530);

    if (auto* const vehicle = FindPlayerVehicle()) {
        s_LastVehicle = vehicle;
        CEntity::RegisterReference(s_LastVehicle); // NOTE: The reference of the previous vehicle isn't removed, so no `notsa::EntityRef`
    }

    const auto GetVehicleAppeal = [] {
        if (!s_LastVehicle || (CVector2D{ s_LastVehicle->GetPosition() } - CVector2D{ FindPlayerCoors() }).SquaredMagnitude() >= sq(35.0f)) { // NOTE: Using MagSq here
            return 0.0f;
        }
        const auto condition = std::clamp(s_LastVehicle->GetHealth() * 2.0f - 1000.0f, 0.0f, 1000.0f);
        switch (s_LastVehicle->GetVehicleModelInfo()->m_nVehicleClass) {
        case VEHICLE_CLASS_POORFAMILY:
        case VEHICLE_CLASS_BIG:
        case VEHICLE_CLASS_MOPED:
        case VEHICLE_CLASS_BICYCLE: // 0x55C038
            return condition * 0.1f;
        case VEHICLE_CLASS_RICHFAMILY:
        case VEHICLE_CLASS_EXECUTIVE:
        case VEHICLE_CLASS_LEISUREBOAT: // 0x55C04C
            return condition;
        case VEHICLE_CLASS_WORKER:
        case VEHICLE_CLASS_TAXI:
        case VEHICLE_CLASS_WORKERBOAT: // 0x55C030
            return condition * 0.3f;
        default: // 0x55C040
            return condition * 0.5f;
        }
    };

    StatTypesFloat[STAT_SEX_APPEAL] = CCheat::IsActive(CHEAT_MAX_SEX_APPEAL)
        ? 1000.0f
        : std::clamp(GetVehicleAppeal() * 0.5f + StatTypesFloat[STAT_APPEARANCE] * 0.5f, 0.0f, 1000.0f);
    CheckForStatsMessage();
}

// 0x55C180
void CStats::IncrementStat(eStats stat, float value)
{
    if (value <= 0.0f)
        return;

    if (IsStatFloat(stat)) { // float
        StatTypesFloat[stat] += value;

        if (IsStatCapped(stat))
            StatTypesFloat[stat] = std::min(StatTypesFloat[stat], 1000.0f);

        ProcessReactionStatsOnIncrement(stat);
        CheckForStatsMessage();

        return;
    }

    CPlayerPed* player = FindPlayerPed();
    CPlayerInfo* playerInfo = player->GetPlayerInfoForThisPlayerPed();

    if (stat == STAT_CALORIES) {
        const auto healthDiff = (float)((int32)((float)(playerInfo->m_nMaxHealth) - player->m_fHealth));

        IncrementStat(STAT_RIOT_MISSION_ACCOMPLISHED, value);

        if (value > healthDiff) {
            IncrementStat(STAT_FAT, (value - healthDiff) * 0.5f);
        }

        ProcessReactionStatsOnIncrement(stat);
        CheckForStatsMessage();

        return;
    }

    if (stat != STAT_RIOT_MISSION_ACCOMPLISHED) {
        assert(stat >= FIRST_INT_STAT);

        StatTypesInt[stat - FIRST_INT_STAT] += (int16)(value); // NOTE: The game uses the low 16 bits of the value only (0x55C229)

        if (IsStatCapped(stat))
            StatTypesInt[stat - FIRST_INT_STAT] = std::min(StatTypesInt[stat - FIRST_INT_STAT], 1000);

        ProcessReactionStatsOnIncrement(stat);
        CheckForStatsMessage();

        return;
    }

    // STAT_RIOT_MISSION_ACCOMPLISHED increment, enum name incorrect?

    playerInfo->m_nNumHoursDidntEat = (int16)(std::clamp((float)(playerInfo->m_nNumHoursDidntEat) - value * 0.5f, 0.0f, 36.0f));

    const auto healthDiff           = (float)((int32)((float)(playerInfo->m_nMaxHealth) - player->m_fHealth));
    if (value >= healthDiff) {
        playerInfo->m_nNumHoursDidntEat = 0;
    }
    const auto healthToAdd = std::min(value, healthDiff);

    player->m_fHealth += healthToAdd;
    UpdateStatsAddToHealth((uint32)((int32)(healthToAdd)));
    ProcessReactionStatsOnIncrement(stat);
    CheckForStatsMessage();
}

// 0x55C470
void CStats::UpdateFatAndMuscleStats(uint32 exerciseRate) {
    enum class eMessage : uint8 {
        NONE          = 0,
        FAT_LOST      = 1,
        MUSCLE_GAINED = 2,
        MUSCLE_LOST   = 3,
    };
    static auto& s_LastMessage = StaticRef<eMessage>(0xB79534);

    if (StatReactionValue[STAT_TIMELIMIT_FAT_ADJUST] * 1000.0f >= (float)(m_FatCounter)) {
        m_FatCounter += GetTimeStepInWholeMS() * exerciseRate / 10;
    } else {
        m_FatCounter = 0;
        if (StatTypesFloat[STAT_FAT] > 0.0f) {
            if (StatReactionValue[STAT_DEC_FAT] > 0.0f) {
                StatTypesFloat[STAT_FAT] = notsa::step_down_to(StatTypesFloat[STAT_FAT], 0.0f, StatReactionValue[STAT_DEC_FAT]);
                CheckForStatsMessage();
            }
            IncrementStat(STAT_MUSCLE, StatReactionValue[STAT_INC_BODY_MUSCLE]);
            if (s_LastMessage == eMessage::FAT_LOST) { // 0x55C519
                s_LastMessage = eMessage::MUSCLE_GAINED;
                DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_MUSCLE, StatReactionValue[STAT_INC_BODY_MUSCLE]);
            } else {
                s_LastMessage = eMessage::FAT_LOST;
                DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_FAT, StatReactionValue[STAT_DEC_FAT]);
            }
        } else { // 0x55C556
            if (StatReactionValue[STAT_DEC_BODY_MUSCLE] > 0.0f) {
                StatTypesFloat[STAT_MUSCLE] = notsa::step_down_to(StatTypesFloat[STAT_MUSCLE], 0.0f, StatReactionValue[STAT_DEC_BODY_MUSCLE]);
                CheckForStatsMessage();
            }
            s_LastMessage = eMessage::MUSCLE_LOST;
            DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_MUSCLE, StatReactionValue[STAT_DEC_BODY_MUSCLE]);
        }
    }

    if (StatReactionValue[STAT_TIMELIMIT_MAX_HEALTH] * 1000.0f >= (float)(m_MaxHealthCounter)) {
        m_MaxHealthCounter += GetTimeStepInWholeMS();
    } else {
        IncrementStat(STAT_MAX_HEALTH, StatReactionValue[STAT_INC_MAX_HEALTH]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_MAX_HEALTH, StatReactionValue[STAT_INC_MAX_HEALTH]);
        m_MaxHealthCounter = 0;
    }
}

// 0x55C660
void CStats::UpdateStatsWhenSprinting() {
    UpdateFatAndMuscleStats(static_cast<uint32>(StatReactionValue[STAT_EXERCISE_RATE_SPRINT]));
    if (StatReactionValue[STAT_TIMELIMIT_SPRINT_STAMINA] * 1000.0f >= static_cast<float>(m_SprintStaminaCounter)) {
        m_SprintStaminaCounter += GetTimeStepInWholeMS();
    } else {
        m_SprintStaminaCounter = 0;
        IncrementStat(STAT_STAMINA, StatReactionValue[STAT_INC_SPRINT_STAMINA]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_STAMINA, StatReactionValue[STAT_INC_SPRINT_STAMINA]);
    }
}

// 0x55C6F0
void CStats::UpdateStatsWhenRunning() {
    UpdateFatAndMuscleStats((uint32)StatReactionValue[STAT_EXERCISE_RATE_RUN]);
    if (StatReactionValue[STAT_TIMELIMIT_RUNNING] * 1000.0f >= static_cast<float>(m_RunningCounter)) {
        m_RunningCounter += GetTimeStepInWholeMS();
    } else {
        m_RunningCounter = 0;
        IncrementStat(STAT_STAMINA, StatReactionValue[STAT_INC_RUNNING]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_STAMINA, StatReactionValue[STAT_INC_RUNNING]);
    }
}

// 0x55C780
void CStats::UpdateStatsWhenCycling(bool isSprinting, CBmx* bmx) {
    if (isSprinting || std::abs(bmx->m_GasPedal) > 0.0f) {
        UpdateFatAndMuscleStats((uint32)(StatReactionValue[isSprinting ? STAT_EXERCISE_RATE_CYCLE_SPRINT : STAT_EXERCISE_RATE_CYCLE]));
        if (StatReactionValue[STAT_TIMELIMIT_CYCLE_STAMINA] * 1000.0f >= (float)(m_CycleStaminaCounter)) {
            m_CycleStaminaCounter += GetTimeStepInWholeMS();
        } else {
            m_CycleStaminaCounter = 0;
            IncrementStat(STAT_STAMINA, StatReactionValue[STAT_INC_CYCLE_STAMINA]);
            DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_STAMINA, StatReactionValue[STAT_INC_CYCLE_STAMINA]);
        }
    }

    if (StatReactionValue[STAT_TIMELIMIT_CYCLE_SKILL] * 1000.0f < (float)(m_CycleSkillCounter)) {
        m_CycleSkillCounter = 0;
        IncrementStat(STAT_CYCLING_SKILL, StatReactionValue[STAT_INC_CYCLE_SKILL]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_CYCLING_SKILL, StatReactionValue[STAT_INC_CYCLE_SKILL]);
        return;
    }

    const auto speedSq = bmx->GetMoveSpeed().SquaredMagnitude();
    if (bmx->m_nNoOfContactWheels < 2 && speedSq > sq(0.05f)) { // 0x55C8A1
        m_CycleSkillCounter += (uint32)(std::ceil((float)(GetTimeStepInWholeMS()) * 3.0f));
    } else if (speedSq > sq(0.2f)) { // 0x55C8E6
        m_CycleSkillCounter += (uint32)(std::ceil((float)(GetTimeStepInWholeMS()) * (isSprinting ? 1.5f : 1.0f)));
    }
}

// 0x55C990
void CStats::UpdateStatsWhenSwimming(bool isSprintingUnderwater, bool isSprinting) {
    UpdateFatAndMuscleStats((uint32)(StatReactionValue[isSprintingUnderwater || isSprinting ? STAT_EXERCISE_RATE_SWIM_SPRINT : STAT_EXERCISE_RATE_SWIM]));
    if (StatReactionValue[STAT_TIMELIMIT_SWIM_STAMINA] * 1000.0f >= (float)(m_SwimStaminaCounter)) {
        m_SwimStaminaCounter += GetTimeStepInWholeMS();
    } else {
        m_SwimStaminaCounter = 0;
        IncrementStat(STAT_STAMINA, StatReactionValue[STAT_INC_SWIM_STAMINA]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_STAMINA, StatReactionValue[STAT_INC_SWIM_STAMINA]);
    }

    if (!isSprintingUnderwater) {
        return;
    }
    if (StatReactionValue[STAT_TIMELIMIT_BREATH_UNDERWATER] * 1000.0f >= (float)(m_SwimUnderWaterCounter)) {
        m_SwimUnderWaterCounter += GetTimeStepInWholeMS();
    } else {
        m_SwimUnderWaterCounter = 0;
        IncrementStat(STAT_LUNG_CAPACITY, StatReactionValue[STAT_INC_BREATH_UNDERWATER]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_LUNG_CAPACITY, StatReactionValue[STAT_INC_BREATH_UNDERWATER]);
    }
}

// 0x55CAC0
void CStats::UpdateStatsWhenDriving(CVehicle* vehicle) {
    const auto drivingCounter = (float)(m_DrivingCounter);
    if (StatReactionValue[STAT_TIMELIMIT_DRIVING_SKILL] * 1000.0f < drivingCounter) {
        m_DrivingCounter = 0;
        IncrementStat(STAT_DRIVING_SKILL, StatReactionValue[STAT_INC_DRIVING_SKILL]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_DRIVING_SKILL, StatReactionValue[STAT_INC_DRIVING_SKILL]);
        return;
    }

    const auto speed = vehicle->GetMoveSpeed().Magnitude();
    if (speed > 0.8f || (vehicle->IsAutomobile() && vehicle->AsAutomobile()->m_nNumContactWheels == 0)) {
        m_DrivingCounter = (uint32)((float)(GetTimeStepInWholeMS()) * 1.5f + drivingCounter);
    } else if (speed > 0.2f) {
        m_DrivingCounter = (uint32)((float)(GetTimeStepInWholeMS()) * 0.5f + drivingCounter);
    }
}

// 0x55CC00
void CStats::UpdateStatsWhenFlying(CVehicle* vehicle) {
    if (!vehicle->IsAutomobile()) {
        return;
    }

    const auto flyingCounter = (float)(m_FlyingCounter);
    if (StatReactionValue[STAT_TIMELIMIT_FLYING_SKILL] * 1000.0f < flyingCounter) {
        m_FlyingCounter = 0;
        IncrementStat(STAT_FLYING_SKILL, StatReactionValue[STAT_INC_FLYING_SKILL]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_FLYING_SKILL, StatReactionValue[STAT_INC_FLYING_SKILL]);
        return;
    }

    if (vehicle->AsAutomobile()->m_nNumContactWheels != 0) {
        return;
    }

    const auto speed = vehicle->GetMoveSpeed().Magnitude();
    if (speed > 1.3f || vehicle->GetUp().z < 0.0f) {
        m_FlyingCounter = (uint32)((float)(GetTimeStepInWholeMS()) * 1.5f + flyingCounter);
    } else if (speed > 0.5f) {
        m_FlyingCounter = (uint32)((float)(GetTimeStepInWholeMS()) * 0.5f + flyingCounter);
    }
}

// 0x55CD60
void CStats::UpdateStatsWhenOnMotorBike(CBike* bike) {
    auto bikeCounter = static_cast<float>(m_BikeCounter);
    if (StatReactionValue[STAT_TIMELIMIT_MOTORBIKE_SKILL] * 1000.0f >= bikeCounter) {
        const float bikeMoveSpeed = bike->m_vecMoveSpeed.Magnitude();
        const auto  timeStep      = (float)(GetTimeStepInWholeMS());

        if (bikeMoveSpeed > 0.6f || bike->m_nNoOfContactWheels < 3u && bikeMoveSpeed > 0.1f)
            m_BikeCounter = static_cast<uint32>(timeStep * 1.5f + bikeCounter);
        else if (bikeMoveSpeed > 0.2f)
            m_BikeCounter = static_cast<uint32>(timeStep * 0.5f + bikeCounter);
    } else {
        m_BikeCounter = 0;
        IncrementStat(STAT_BIKE_SKILL, StatReactionValue[STAT_INC_MOTORBIKE_SKILL]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_BIKE_SKILL, StatReactionValue[STAT_INC_MOTORBIKE_SKILL]);
    }
}

// 0x55CEB0
void CStats::UpdateStatsWhenWeaponHit(eWeaponType weaponType) {
    const auto stat = CWeaponInfo::GetSkillStatIndex(weaponType);
    const auto weaponIndex = (uint32)(stat - STAT_PISTOL_SKILL);
    const auto skill = GetStatValue(stat);
    if (CGameLogic::IsCoopGameGoingOn() || skill >= 1000.0f) {
        return;
    }

    IncrementStat(stat, StatReactionValue[STAT_INC_PISTOL_SKILL + weaponIndex]);
    if (m_LastWeaponTypeFired != weaponIndex) {
        m_LastWeaponTypeFired = weaponIndex;
        m_WeaponCounter = 0;
    } else if ((float)m_WeaponCounter > StatReactionValue[STAT_TIMELIMIT_PISTOL_SKILL + weaponIndex]) {
        DisplayScriptStatUpdateMessage(
            STAT_UPDATE_INCREASE,
            stat,
            (float)m_WeaponCounter * StatReactionValue[STAT_INC_PISTOL_SKILL + weaponIndex]
        );
        m_WeaponCounter = 0;
    } else {
        m_WeaponCounter++;
    }
}

// 0x55CFA0
void CStats::UpdateStatsWhenFighting() {
    UpdateFatAndMuscleStats(static_cast<uint32>(StatReactionValue[STAT_EXERCISE_RATE_FIGHT]));
}

// 0x55CFC0
void CStats::UpdateStatsOnRespawn() {
    if ((float)m_DeathCounter > StatReactionValue[STAT_TIMELIMIT_DEATH_HEALTH]) {
        if (GetStatValue(STAT_MAX_HEALTH) > 400.0f) {
            IncrementStat(STAT_MAX_HEALTH, StatReactionValue[STAT_DEC_MAX_HEALTH]);
            DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_MAX_HEALTH, StatReactionValue[STAT_DEC_MAX_HEALTH]);
        }
        m_DeathCounter = 0;
    } else {
        m_DeathCounter++;
    }
}

// 0x55D030
void CStats::UpdateStatsAddToHealth(uint32 addToHealth) {
    m_AddToHealthCounter += addToHealth;
    if ((float)m_AddToHealthCounter > StatReactionValue[STAT_TIMELIMIT_ADD_TO_HEALTH]) {
        IncrementStat(STAT_MAX_HEALTH, StatReactionValue[STAT_INC_MAX_HEALTH]);
        DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, STAT_MAX_HEALTH, StatReactionValue[STAT_INC_MAX_HEALTH]);
        m_AddToHealthCounter = 0;
    }
}

// 0x55D090
void CStats::ModifyStat(eStats stat, float value) {
    if (value < 0.0f) {
        CStats::DecrementStat(stat, -value);
    } else {
        CStats::IncrementStat(stat, value);
    }
}

// 0x5D3B40
bool CStats::Save() {
    IncrementStat(STAT_TOTAL_LEGITIMATE_KILLS, GetStatValue(STAT_KILLS_SINCE_LAST_CHECKPOINT));
    SetStatValue(STAT_KILLS_SINCE_LAST_CHECKPOINT, 0.0f);

    CGenericGameStorage::SaveDataToWorkBuffer(StatTypesFloat);
    CGenericGameStorage::SaveDataToWorkBuffer(StatTypesInt);
    CGenericGameStorage::SaveDataToWorkBuffer(PedsKilledOfThisType);
    CGenericGameStorage::SaveDataToWorkBuffer(LastMissionPassedName);
    CGenericGameStorage::SaveDataToWorkBuffer(FavoriteRadioStationList);
    CGenericGameStorage::SaveDataToWorkBuffer(TimesMissionAttempted);
    // TODO: NOTSA: CGenericGameStorage::SaveDataToWorkBuffer(StatMessage);
    for (auto& statMessage : StatMessage) {
        CGenericGameStorage::SaveDataToWorkBuffer(statMessage.displayed);
    }
    return true;
}

// 0x5D3BF0
bool CStats::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer(StatTypesFloat);
    CGenericGameStorage::LoadDataFromWorkBuffer(StatTypesInt);
    CGenericGameStorage::LoadDataFromWorkBuffer(PedsKilledOfThisType);
    CGenericGameStorage::LoadDataFromWorkBuffer(LastMissionPassedName);
    CGenericGameStorage::LoadDataFromWorkBuffer(FavoriteRadioStationList);
    CGenericGameStorage::LoadDataFromWorkBuffer(TimesMissionAttempted);
    // TODO: NOTSA: CGenericGameStorage::LoadDataFromWorkBuffer(StatMessage);
    for (auto& statMessage : StatMessage) {
        CGenericGameStorage::LoadDataFromWorkBuffer(statMessage.displayed);
    }
    return true;
}

// Unused
// 0x558DE0
char* CStats::GetStatID(eStats stat) {
    if (!IsStatFloat(stat)) // int32
        sprintf_s(gString, "stat_i_%d", stat);
    else
        sprintf_s(gString, "stat_f_%d", stat);

    return gString;
}

// Unused
// 0x558E70
int8 CStats::GetTimesMissionAttempted(uint8 missionId) {
    return TimesMissionAttempted[missionId];
}

// Unused
// 0x558E80
void CStats::RegisterMissionAttempted(uint8 missionId) {
    if (TimesMissionAttempted[missionId] != -1) {
        TimesMissionAttempted[missionId]++;
    }
}

// Unused
// 0x558EA0
void CStats::RegisterMissionPassed(uint8 missionId) {
    TimesMissionAttempted[missionId] = -1;
}
