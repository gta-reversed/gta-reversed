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
    static inline int32& ZoneInfoForTraining = *(int32*)0x8A5F40; // -1
    static inline CVehicle*& pDriveByCar = *(CVehicle**)0x96AB28;
    static inline std::array<int32, 3>& GangRatingStrength = *(std::array<int32, 3>*)0x96AB2C;
    static inline std::array<int32, 3>& GangRatings = *(std::array<int32, 3>*)0x96AB38;
    static inline uint32& FightTimer = *(uint32*)0x96AB44;
    static inline float& TimeTillNextAttack = *(float*)0x96AB48;
    static inline int32& Gang2 = *(int32*)0x96AB50;
    static inline uint32& LastTimeInArea = *(uint32*)0x96AB54;
    static inline int32& WarFerocity = *(int32*)0x96AB58;
    static inline float& Provocation = *(float*)0x96AB5C;
    static inline uint32& TimeStarted = *(uint32*)0x96AB60;
    // State of defending gang war (i.e. fighting for enemy zone)
    static inline eGangWarState& State = *(eGangWarState*)0x96AB64;
    // State of attacking gang war (i.e. fighting for own zone)
    static inline eGangAttackState& State2 = *(eGangAttackState*)0x96AB4C;
    static inline std::array<int32, 6> aSpecificZones = *(std::array<int32, 6>*)0x96AB68;
    static inline float& Difficulty = *(float*)0x96AB80;
    static inline eGangID& Gang1 = *(eGangID*)0x96AB84;
    static inline CZone*& pZoneToFightOver = *(CZone**)0x96AB88;
    static inline CZoneInfo*& pZoneInfoToFightOver = *(CZoneInfo**)0x96AB8C;
    static inline bool& bGangWarsActive = *(bool*)0x96AB90;
    static inline bool& bTrainingMission = *(bool*)0x96AB91;
    static inline bool& bPlayerIsCloseby = *(bool*)0x96AB92;
    static inline bool& bCanTriggerGangWarWhenOnAMission = *(bool*)0x96AB93;
    static inline int32& NumSpecificZones = *(int32*)0x96AB94;
    static inline int32& RadarBlip = *(int32*)0x96AB98;
    static inline float& TerritoryUnderControlPercentage = *(float*)0x96AB9C;
    static inline bool& bIsPlayerOnAMission = *(bool*)0x96ABA0;
    static inline CVector& CoorsOfPlayerAtStartOfWar = *(CVector*)0x96ABBC;
    static inline CVector& PointOfAttack = *(CVector*)0x96ABC8;

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

private:
    static eBlipColour GetGangColor(int32 gang);
};
