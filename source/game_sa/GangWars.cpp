/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "GangWars.h"

int32& CGangWars::ZoneInfoForTraining = *reinterpret_cast<int32*>(0x8A5F40); // -1
int32(&CGangWars::GangRatingStrength)[3] = *reinterpret_cast<int32(*)[3]>(0x96AB2C);
int32(&CGangWars::GangRatings)[3] = *reinterpret_cast<int32(*)[3]>(0x96AB38);
int32& CGangWars::FightTimer = *reinterpret_cast<int32*>(0x96AB44);
float& CGangWars::TimeTillNextAttack = *reinterpret_cast<float*>(0x96AB48);
int32& CGangWars::Gang2 = *reinterpret_cast<int32*>(0x96AB50);
uint32& CGangWars::LastTimeInArea = *reinterpret_cast<uint32*>(0x96AB54);
int32& CGangWars::WarFerocity = *reinterpret_cast<int32*>(0x96AB58);
float& CGangWars::Provocation = *reinterpret_cast<float*>(0x96AB5C);
uint32& CGangWars::TimeStarted = *reinterpret_cast<uint32*>(0x96AB60);
eGangWarState& CGangWars::State = *reinterpret_cast<eGangWarState*>(0x96AB64);
eGangAttackState& CGangWars::State2 = *reinterpret_cast<eGangAttackState*>(0x96AB4C);
int32(&CGangWars::aSpecificZones)[6] = *reinterpret_cast<int32(*)[6]>(0x96AB68);
float& CGangWars::Difficulty = *reinterpret_cast<float*>(0x96AB80);
int32& CGangWars::Gang1 = *reinterpret_cast<int32*>(0x96AB84);
CZone*& CGangWars::pZoneToFightOver = *reinterpret_cast<CZone**>(0x96AB88);
CZoneInfo*& CGangWars::pZoneInfoToFightOver = *reinterpret_cast<CZoneInfo**>(0x96AB8C);
bool& CGangWars::bGangWarsActive = *reinterpret_cast<bool*>(0x96AB90);
bool& CGangWars::bTrainingMission = *reinterpret_cast<bool*>(0x96AB91);
bool& CGangWars::bPlayerIsCloseby = *reinterpret_cast<bool*>(0x96AB92);
bool& CGangWars::bCanTriggerGangWarWhenOnAMission = *reinterpret_cast<bool*>(0x96AB93);
int32& CGangWars::NumSpecificZones = *reinterpret_cast<int32*>(0x96AB94);
CRadar*& CGangWars::RadarBlip = *reinterpret_cast<CRadar**>(0x96AB98);
float& CGangWars::TerritoryUnderControlPercentage = *reinterpret_cast<float*>(0x96AB9C);
bool& CGangWars::bIsPlayerOnAMission = *reinterpret_cast<bool*>(0x96ABA0);
CVector& CGangWars::CoorsOfPlayerAtStartOfWar = *reinterpret_cast<CVector*>(0x96ABBC);
CVector& CGangWars::PointOfAttack = *reinterpret_cast<CVector*>(0x96ABC8);

void CGangWars::InjectHooks() {
    RH_ScopedClass(CGangWars);
    RH_ScopedCategoryGlobal();

    // RH_ScopedInstall(AddKillToProvocation, 0x443950);
    // RH_ScopedInstall(AttackWaveOvercome, 0x445B30);
    RH_ScopedInstall(CalculateTimeTillNextAttack, 0x443DB0);
    // RH_ScopedInstall(CanPlayerStartAGangWarHere, 0x443F80);
    // RH_ScopedInstall(CheerVictory, 0x444040);
    RH_ScopedInstall(ClearSpecificZonesToTriggerGangWar, 0x443FF0);
    // RH_ScopedInstall(ClearTheStreets, 0x4444B0);
    // RH_ScopedInstall(CreateAttackWave, 0x444810);
    // RH_ScopedInstall(CreateDefendingGroup, 0x4453D0);
    RH_ScopedInstall(DoStuffWhenPlayerVictorious, 0x446400);
    RH_ScopedInstall(DontCreateCivilians, 0x4439C0);
    RH_ScopedInstall(EndGangWar, 0x4464C0);
    RH_ScopedInstall(GangWarFightingGoingOn, 0x443AC0);
    RH_ScopedInstall(GangWarGoingOn, 0x443AA0);
    RH_ScopedInstall(InitAtStartOfGame, 0x443920);
    // RH_ScopedInstall(Load, 0x5D3EB0);
    // RH_ScopedInstall(MakeEnemyGainInfluenceInZone, 0x445FD0);
    // RH_ScopedInstall(MakePlayerGainInfluenceInZone, 0x445E80);
    // RH_ScopedInstall(PedStreamedInForThisGang, 0x4439D0);
    // RH_ScopedInstall(PickStreamedInPedForThisGang, 0x443A20);
    // RH_ScopedInstall(PickZoneToAttack, 0x443B00);
    // RH_ScopedInstall(ReleaseCarsInAttackWave, 0x445E20);
    // RH_ScopedInstall(ReleasePedsInAttackWave, 0x445C30);
    // RH_ScopedInstall(Save, 0x5D5530);
    RH_ScopedInstall(SetGangWarsActive, 0x446570);
    // RH_ScopedInstall(SetSpecificZoneToTriggerGangWar, 0x444010);
    // RH_ScopedInstall(StartDefensiveGangWar, 0x444300);
    // RH_ScopedInstall(StartOffensiveGangWar, 0x446050);
    // RH_ScopedInstall(StrengthenPlayerInfluenceInZone, 0x445F50);
    // RH_ScopedInstall(SwitchGangWarsActive, 0x4465F0);
    // RH_ScopedInstall(TellGangMembersTo, 0x444530);
    // RH_ScopedInstall(TellStreamingWhichGangsAreNeeded, 0x443D50);
    // RH_ScopedInstall(Update, 0x446610);
    // RH_ScopedInstall(UpdateTerritoryUnderControlPercentage, 0x443DE0);
}

// 0x5D3EB0
void CGangWars::Load() {
    plugin::Call<0x5D3EB0>();
}

// 0x5D5530
void CGangWars::Save() {
    plugin::Call<0x5D5530>();
}

// 0x443920
void CGangWars::InitAtStartOfGame() {
    State               = NOT_IN_WAR;
    State2              = NO_ATTACK;
    NumSpecificZones    = 0;
    Provocation         = 0.0f;
    bGangWarsActive     = false;
    bIsPlayerOnAMission = false;
}

// 0x443950
void CGangWars::AddKillToProvocation(ePedType pedType) {
    plugin::Call<0x443950, ePedType>(pedType);
}

// 0x445B30
bool CGangWars::AttackWaveOvercome() {
    return plugin::CallAndReturn<bool, 0x445B30>();
}

// 0x443DB0
float CGangWars::CalculateTimeTillNextAttack() {
    return CGeneral::GetRandomNumberInRange(648'000.0f, 1'620'000.0f);
}

// 0x443F80
bool CGangWars::CanPlayerStartAGangWarHere(CZoneInfo* zoneInfo) {
    return plugin::CallAndReturn<bool, 0x443F80, CZoneInfo*>(zoneInfo);
}

// 0x444040
void CGangWars::CheerVictory() {
    plugin::Call<0x444040>();
}

// 0x443FF0
void CGangWars::ClearSpecificZonesToTriggerGangWar() {
    NumSpecificZones = 0;
    CTheZones::FillZonesWithGangColours(false);
}

// 0x4444B0
void CGangWars::ClearTheStreets() {
    plugin::Call<0x4444B0>();
}

// 0x444810
bool CGangWars::CreateAttackWave(int32 warFerocity, int32 waveID) {
    return plugin::CallAndReturn<bool, 0x444810, int32, int32>(warFerocity, waveID);
}

// 0x4453D0
bool CGangWars::CreateDefendingGroup(int32 unused) {
    return plugin::CallAndReturn<bool, 0x4453D0, int32>(unused);
}

// 0x446400
void CGangWars::DoStuffWhenPlayerVictorious() {
    ReleasePedsInAttackWave(true, false);
    ReleaseCarsInAttackWave();
    CheerVictory();
    State = NOT_IN_WAR;
    CMessages::AddMessage(TheText.Get("GW_YRS"), 4500, 1, true);
    CMessages::AddToPreviousBriefArray(TheText.Get("GW_YRS"), -1, -1, -1, -1, -1, -1, nullptr);
    Provocation = 0.0f;
    TellGangMembersTo(true);
    CStats::IncrementStat(STAT_RESPECT, 45.0);
    CTheZones::FillZonesWithGangColours(false);

    auto v2 = TimeTillNextAttack - 240000.0f;
    if ( v2 <= 30000.0f )
        TimeTillNextAttack = 30000.0f;
    else
        TimeTillNextAttack = v2;
}

// inlined
// 0x443AE0
bool CGangWars::DoesPlayerControlThisZone(CZoneInfo* zoneInfo) {
    return plugin::CallAndReturn<bool, 0x443AE0, CZoneInfo*>(zoneInfo);
}

// 0x4439C0
bool CGangWars::DontCreateCivilians() {
    return State != NOT_IN_WAR;
}

// 0x4464C0
void CGangWars::EndGangWar(bool bEnd) {
    State = NOT_IN_WAR;
    if (State2 == WAR_NOTIFIED) {
        State2 = NO_ATTACK;
        TimeTillNextAttack = CalculateTimeTillNextAttack();
        uint32 nReleasedPeds = ReleasePedsInAttackWave(true, false);
        MakeEnemyGainInfluenceInZone(Gang1, 3u * nReleasedPeds);
    }
    Provocation = 0.0f;
    CTheZones::FillZonesWithGangColours(false);
    ReleasePedsInAttackWave(true, bEnd);
    ReleaseCarsInAttackWave();
}

// 0x443AC0
bool CGangWars::GangWarFightingGoingOn() {
    return State == PREFIRST_WAVE || State2 == PLAYER_CAME_TO_WAR;
}

// 0x443AA0
bool CGangWars::GangWarGoingOn() {
    return State == PREFIRST_WAVE || State2 == WAR_NOTIFIED;
}

// 0x445FD0
void CGangWars::MakeEnemyGainInfluenceInZone(int32 gangId, int32 gangDensityIncreaser) {
    plugin::Call<0x445FD0, int32, int32>(gangId, gangDensityIncreaser);
}

// 0x445E80
bool CGangWars::MakePlayerGainInfluenceInZone(float removeMult) {
    return plugin::CallAndReturn<bool, 0x445E80, float>(removeMult);
}

// 0x4439D0
bool CGangWars::PedStreamedInForThisGang(int32 gangId) {
    return plugin::CallAndReturn<bool, 0x4439D0, int32>(gangId);
}

// 0x443A20
bool CGangWars::PickStreamedInPedForThisGang(int32 gangId, int32* outPedId) {
    return plugin::CallAndReturn<bool, 0x443A20, int32, int32*>(gangId, outPedId);
}

// 0x443B00
bool CGangWars::PickZoneToAttack() {
    return plugin::CallAndReturn<bool, 0x443B00>();
}

// 0x445E20
void CGangWars::ReleaseCarsInAttackWave() {
    plugin::Call<0x445E20>();
}

// Returns num of released peds
// 0x445C30
uint32 CGangWars::ReleasePedsInAttackWave(bool isEndOfWar, bool restoreGangPedsAcquaintance) {
    return plugin::CallAndReturn<uint32, 0x445C30, bool, bool>(isEndOfWar, restoreGangPedsAcquaintance);
}

// 0x446570
void CGangWars::SetGangWarsActive(bool active) {
    if (active != bGangWarsActive) {
        CTheZones::FillZonesWithGangColours(active == false);
        TimeTillNextAttack = CalculateTimeTillNextAttack();
        if (!active)
            EndGangWar(false);
    }
    bGangWarsActive = active;
}

// 0x444010
void CGangWars::SetSpecificZoneToTriggerGangWar(int32 zoneId) {
    aSpecificZones[NumSpecificZones] = zoneId;
    NumSpecificZones += 1;
    CTheZones::FillZonesWithGangColours(false);
}

// 0x444300
void CGangWars::StartDefensiveGangWar() {
    plugin::Call<0x444300>();
}

// 0x446050
void CGangWars::StartOffensiveGangWar() {
    plugin::Call<0x446050>();
}

// 0x445F50
void CGangWars::StrengthenPlayerInfluenceInZone(int32 groveDensityIncreaser) {
    plugin::Call<0x445F50, int32>(groveDensityIncreaser);
}

// 0x4465F0
// unused
void CGangWars::SwitchGangWarsActive() {
    SetGangWarsActive(bGangWarsActive ^ true);
}

// 0x444530
void CGangWars::TellGangMembersTo(bool bIsGangWarEnding) {
    plugin::Call<0x444530, bool>(bIsGangWarEnding);
}

// fix_bugs: originally has int32 type, but changed to unsigned due possible UB 
// 0x443D50
void CGangWars::TellStreamingWhichGangsAreNeeded(uint32* GangsBitFlags) {
    plugin::Call<0x443D50, uint32*>(GangsBitFlags);
}

// 0x446610
void CGangWars::Update() {
    plugin::Call<0x446610>();
}

// 0x443DE0
void CGangWars::UpdateTerritoryUnderControlPercentage() {
    plugin::Call<0x443DE0>();
}
