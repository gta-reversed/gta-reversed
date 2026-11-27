/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CarCtrl.h"
#include "Radar.h"
#include "PedType.h"

class CZone;
class CZoneInfo;

enum eGangAttackState {
    NO_ATTACK          = 0,
    WAR_NOTIFIED       = 1,
    PLAYER_CAME_TO_WAR = 2
};

enum eGangWarState {
    NOT_IN_WAR      = 0,
    PRE_FIRST_WAVE  = 1,
    FIRST_WAVE      = 2,
    PRE_SECOND_WAVE = 3,
    SECOND_WAVE     = 4,
    PRE_THIRD_WAVE  = 5,
    THIRD_WAVE      = 6
};

constexpr union {
    struct {
        uint8 r, g, b;
    };
    uint8 components[3];
} gaGangColors[TOTAL_GANGS]{ // based on gaGangColorsR, gaGangColorsG, gaGangColorsB
    { 200, 0,   200 },
    { 70,  200, 0   },
    { 255, 200, 0   },
    { 0,   0,   200 },
    { 255, 220, 190 },
    { 200, 200, 200 },
    { 240, 140, 240 },
    { 0,   200, 255 },
    { 255, 255, 255 },
    { 255, 255, 255 },
};

class CGangWars {
public:
    static void InjectHooks();

    static void InitAtStartOfGame();

    static bool Load();
    static bool Save();

    static void AddKillToProvocation(ePedType pedType);
    static bool AttackWaveOvercome();
    static uint32 CalculateTimeTillNextAttack();
    static bool CanPlayerStartAGangWarHere(CZoneInfo* zoneInfo);
    static void CheerVictory();
    static void ClearSpecificZonesToTriggerGangWar();
    static void ClearTheStreets();
    static bool   CreateAttackWave(int32 warFerocity, int32 waveId);
    static bool CreateDefendingGroup(int32 unused);
    static void DoStuffWhenPlayerVictorious();
    static bool DoesPlayerControlThisZone(CZoneInfo* zoneInfo);
    static bool DontCreateCivilians();
    static void   EndGangWar(bool restoreGangPedsAcquaintance);

    static bool GangWarFightingGoingOn();
    static bool GangWarGoingOn();

    static void MakeEnemyGainInfluenceInZone(int32 gangId, int32 density);
    static bool MakePlayerGainInfluenceInZone(float removeMult);

    static bool PedStreamedInForThisGang(eGangID gangId);
    static bool PickStreamedInPedForThisGang(eGangID gangId, int32& outPedId);
    static bool PickZoneToAttack();

    static void ReleaseCarsInAttackWave();
    static uint32 ReleasePedsInAttackWave(bool isEndOfWar, bool restoreGangPedsAcquaintance);

    static void SetGangWarsActive(bool active);
    static void SetSpecificZoneToTriggerGangWar(int32 zoneId);

    static void StartDefensiveGangWar();
    static void StartOffensiveGangWar();

    static void StrengthenPlayerInfluenceInZone(int32 density);
    static void SwitchGangWarsActive();

    static void TellGangMembersTo(bool isGangWarEnding);
    static void TellStreamingWhichGangsAreNeeded(uint32& gangsBitFlags);

    static void Update();
    static void UpdateTerritoryUnderControlPercentage();

    // NOTSA
    static auto GetSpecificZones() { return ms_SpecificZones | rngv::take(ms_NumSpecificZones); }

private:
    static eBlipColour GetGangColor(int32 gang);

public:
    static inline auto& ms_ZoneInfoForTraining             = StaticRef<int32>(0x8A5F40); // -1
    static inline auto& ms_DriveByCar                      = StaticRef<CVehicle*>(0x96AB28);
    static inline auto& ms_GangRatingStrength              = StaticRef<std::array<int32, 3>>(0x96AB2C); //!< Number of zones held by the gang at the same rank in `ms_GangRatings`
    static inline auto& ms_GangRatings                     = StaticRef<std::array<int32, 3>>(0x96AB38); //!< Gang IDs (Grove, Ballas, Vagos) sorted by the number of zones they hold, most first
    static inline auto& ms_FightTimer                      = StaticRef<int32>(0x96AB44);
    static inline auto& ms_TimeTillNextAttack              = StaticRef<float>(0x96AB48);
    static inline auto& ms_Gang2                           = StaticRef<int32>(0x96AB50);
    static inline auto& ms_LastTimeInArea                  = StaticRef<uint32>(0x96AB54);
    static inline auto& ms_WarFerocity                     = StaticRef<int32>(0x96AB58);
    static inline auto& ms_Provocation                     = StaticRef<float>(0x96AB5C);
    static inline auto& ms_TimeStarted                     = StaticRef<uint32>(0x96AB60);
    static inline auto& ms_State                           = StaticRef<eGangWarState>(0x96AB64);    //!< The war the player starts in an enemy zone (attack waves)
    static inline auto& ms_State2                          = StaticRef<eGangAttackState>(0x96AB4C); //!< An enemy gang attacking one of the player's zones
    static inline auto& ms_SpecificZones                   = StaticRef<std::array<int32, 6>>(0x96AB68);
    static inline auto& ms_Difficulty                      = StaticRef<float>(0x96AB80);
    static inline auto& ms_Gang1                           = StaticRef<eGangID>(0x96AB84);
    static inline auto& ms_ZoneToFightOver                 = StaticRef<CZone*>(0x96AB88);
    static inline auto& ms_ZoneInfoToFightOver             = StaticRef<CZoneInfo*>(0x96AB8C);
    static inline auto& ms_GangWarsActive                  = StaticRef<bool>(0x96AB90);
    static inline auto& ms_IsTrainingMission               = StaticRef<bool>(0x96AB91);
    static inline auto& ms_IsPlayerCloseby                 = StaticRef<bool>(0x96AB92);
    static inline auto& ms_CanTriggerGangWarWhenOnAMission = StaticRef<bool>(0x96AB93);
    static inline auto& ms_NumSpecificZones                = StaticRef<int32>(0x96AB94);
    static inline auto& ms_RadarBlip                       = StaticRef<int32>(0x96AB98);
    static inline auto& ms_TerritoryUnderControlPercentage = StaticRef<float>(0x96AB9C);
    static inline auto& ms_IsPlayerOnAMission              = StaticRef<bool>(0x96ABA0);
    static inline auto& ms_CoorsOfPlayerAtStartOfWar       = StaticRef<CVector>(0x96ABBC);
    static inline auto& ms_PointOfAttack                   = StaticRef<CVector>(0x96ABC8);
};
