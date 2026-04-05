/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CrimeBeingQd.h"
#include "AEPoliceScannerAudioEntity.h"

#include "Enums/eWantedLevel.h"
#include "Enums/eCrimeType.h"

class CPed;
class CCopPed;

class CWanted {
public:
    static constexpr auto MAX_COPS_IN_PURSUIT{ 10u };

    uint32 ChaosLevel; // m_nWantedLevel
    uint32 ChaosLevelBeforeParole; // m_nWantedLevelBeforeParole
    uint32 LastTimeWantedDecreased;
    uint32 LastTimeWantedLevelChanged;
    uint32 TimeOfParole;
    float  Multiplier; // New crimes have their wanted level contribution multiplied by this
    uint8  NumCopsInPursuit; // m_nCopsInPursuit
    uint8  MaxCopsInPursuit;
    uint8  MaxCopCarsInPursuit;

    uint8  CopsBeatingSuspect;
    uint16 ChanceOnRoadBlock;

    union {
        struct {
            uint8 bPoliceBackOff : 1;       // If this is set the police will leave player alone (for cut-scenes)
            uint8 bPoliceBackOffGarage : 1; // If this is set the police will leave player alone (for garages)
            uint8 bEverybodyBackOff : 1;    // If this is set then everybody (including police) will leave the player alone (for cut-scenes)
            uint8 bSwatRequired : 1;        // These three booleans are needed so that the
            uint8 bFbiRequired : 1;         // streaming required vehicle stuff can be overrided
            uint8 bArmyRequired : 1;
        };
        uint8 Flags;
    };
    uint32                      CurrentChaseTime;
    uint32                      CurrentChaseTimeCounter;
    bool                        TimeCounting; // todo: good name

    eWantedLevel                WantedLevel; // m_WantedLevel
    eWantedLevel                WantedLevelBeforeParole; // m_WantedLevelBeforeParole

    CCrimeBeingQd               CrimesBeingQd[16]; // Crimes Being Queued

    CCopPed*                    CopsInPursuit[MAX_COPS_IN_PURSUIT];

    CAEPoliceScannerAudioEntity PoliceScannerAudioEntity;

private:
    bool                        StoredPoliceBackOff;

public:
    static inline auto& MaximumWantedLevel = StaticRef<eWantedLevel>(0x8CDEE4); // 6
    static inline auto& MaximumChaosLevel = StaticRef<uint32>(0x8CDEE8); // 9200; nMaximumWantedLevel
    static inline auto& UseNewsHeliInAdditionToPolice = StaticRef<bool>(0xB7CB8C);

public:
    void Initialise();
    static void InitialiseStaticVariables();
    void Reset();
    void Update();
    void UpdateWantedLevel();
    void RegisterCrime(eCrimeType crimeType, const CVector& pos, uint32 IdKey, bool policeDontReallyCare);
    void RegisterCrime_Immediately(eCrimeType crimeType, const CVector& pos, uint32 IdKey, bool policeDontReallyCare);

    void SetWantedLevel(eWantedLevel newLev);
    void CheatWantedLevel(eWantedLevel newLev);
    void SetWantedLevelNoDrop(eWantedLevel newLev);
    void ClearWantedLevelAndGoOnParole();

    static void SetMaximumWantedLevel(eWantedLevel newMaxLev);

    [[nodiscard]] eWantedLevel GetWantedLevel() const { return WantedLevel; }

    void SetSwatRequired(bool s) { bSwatRequired = s; }
    void SetFbiRequired(bool s) { bFbiRequired = s; }
    void SetArmyRequired(bool s) { bArmyRequired = s; }

    [[nodiscard]] bool AreMiamiViceRequired() const;
    [[nodiscard]] bool AreSwatRequired() const;
    [[nodiscard]] bool AreFbiRequired() const;
    [[nodiscard]] bool AreArmyRequired() const;
    [[nodiscard]] int32 NumOfHelisRequired() const;
    void ResetPolicePursuit();
    [[nodiscard]] bool PoliceBackOff() const { return bPoliceBackOff || bPoliceBackOffGarage || bEverybodyBackOff; }

    void ClearQdCrimes();
    bool AddCrimeToQ(eCrimeType crimeType, uint32 crimeId, const CVector& coors, bool alreadyReported, bool policeDontReallyCare);
    void UpdateCrimesQ();
    void ReportCrimeNow(eCrimeType crimeType, const CVector& coors, bool policeDontReallyCare);

    bool CanCopJoinPursuit(CCopPed* copPed);
    bool SetPursuitCop(CCopPed* copPed);
    void RemovePursuitCop(CCopPed* copPed);
    bool IsInPursuit(CCopPed* copPed);
    void RemoveExcessPursuitCops();
    static int32 WorkOutPolicePresence(CVector coors, float radius);
    static void UpdateEachFrame();

    bool IsClosestCop(CPed* ped, const int32 closest) const;

private:
    static bool CanCopJoinPursuit(CCopPed* copPed, const uint8 maxCopsInPursuit, CCopPed** copsInPursuitArray, uint8& copsInPursuit);
    static CCopPed* ComputePursuitCopToDisplace(CCopPed* copPed, CCopPed** copsInPursuitArray);
    static void RemovePursuitCop(CCopPed* copPed, CCopPed** copsInPursuitArray, uint8& copsInPursuit);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};

VALIDATE_SIZE(CWanted, 0x29C);
