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
    static inline auto& ZoneInfoForTraining = StaticRef<int32>(0x8A5F40); // -1
    static inline auto& pDriveByCar = StaticRef<CVehicle*>(0x96AB28);
    static inline auto& GangRatingStrength = StaticRef<std::array<int32, 3>>(0x96AB2C);
    static inline auto& GangRatings = StaticRef<std::array<int32, 3>>(0x96AB38);
    static inline auto& FightTimer = StaticRef<uint32>(0x96AB44);
    static inline auto& TimeTillNextAttack = StaticRef<float>(0x96AB48);
    static inline auto& Gang2 = StaticRef<int32>(0x96AB50);
    static inline auto& LastTimeInArea = StaticRef<uint32>(0x96AB54);
    static inline auto& WarFerocity = StaticRef<int32>(0x96AB58);
    static inline auto& Provocation = StaticRef<float>(0x96AB5C);
    static inline auto& TimeStarted = StaticRef<uint32>(0x96AB60);
    // State of defending gang war (i.e. fighting for enemy zone)
    static inline auto& State = StaticRef<eGangWarState>(0x96AB64);
    // State of attacking gang war (i.e. fighting for own zone)
    static inline auto& State2 = StaticRef<eGangAttackState>(0x96AB4C);
    static inline auto& aSpecificZones = StaticRef<std::array<int32, 6>>(0x96AB68);
    static inline auto& Difficulty = StaticRef<float>(0x96AB80);
    static inline auto& Gang1 = StaticRef<eGangID>(0x96AB84);
    static inline auto& pZoneToFightOver = StaticRef<CZone*>(0x96AB88);
    static inline auto& pZoneInfoToFightOver = StaticRef<CZoneInfo*>(0x96AB8C);
    static inline auto& bGangWarsActive = StaticRef<bool>(0x96AB90);
    static inline auto& bTrainingMission = StaticRef<bool>(0x96AB91);
    static inline auto& bPlayerIsCloseby = StaticRef<bool>(0x96AB92);
    static inline auto& bCanTriggerGangWarWhenOnAMission = StaticRef<bool>(0x96AB93);
    static inline auto& NumSpecificZones = StaticRef<int32>(0x96AB94);
    static inline auto& RadarBlip = StaticRef<int32>(0x96AB98);
    static inline auto& TerritoryUnderControlPercentage = StaticRef<float>(0x96AB9C);
    static inline auto& bIsPlayerOnAMission = StaticRef<bool>(0x96ABA0);
    static inline auto& CoorsOfPlayerAtStartOfWar = StaticRef<CVector>(0x96ABBC);
    static inline auto& PointOfAttack = StaticRef<CVector>(0x96ABC8);

public:
    static void InjectHooks();

    static void InitAtStartOfGame();

    static bool Load();
    static bool Save();

    static void AddKillToProvocation(ePedType pedType);
    static bool AttackWaveOvercome();
    static float CalculateTimeTillNextAttack();
    static bool CanPlayerStartAGangWarHere(CZoneInfo* zoneInfo);
    static void CheerVictory();
    static void ClearSpecificZonesToTriggerGangWar();
    static void ClearTheStreets();
    static bool CreateAttackWave(int32 warFerocity, int32 waveID);
    static bool CreateDefendingGroup(int32 unused);
    static void DoStuffWhenPlayerVictorious();
    static bool DoesPlayerControlThisZone(CZoneInfo* zoneInfo);
    static bool DontCreateCivilians();
    static void EndGangWar(bool bEnd);

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

    // notsa
    static auto GetSpecificZones() { return aSpecificZones | rngv::take(NumSpecificZones); }

private:
    static eBlipColour GetGangColor(int32 gang);
};
