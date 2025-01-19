/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "Vector.h"
#include <PlayerPedData.h>

class CPed;
class CVehicle;
class CPlayerPed;
class CPlayerInfoSaveStructure;

enum ePlayerState : uint8 {
    PLAYERSTATE_PLAYING,
    PLAYERSTATE_HAS_DIED,
    PLAYERSTATE_HAS_BEEN_ARRESTED,
    PLAYERSTATE_FAILED_MISSION,
    PLAYERSTATE_LEFT_GAME
};

class CPlayerInfo {
public:
    CPlayerPed*    m_pPed;                    // Pointer to the player ped (should always be set)
    CPlayerPedData m_PlayerData;              // instance of player variables
    CVehicle*      m_pRemoteVehicle;          // Pointer to vehicle player is driving remotely at the moment.(NULL if on foot)
    CVehicle*      m_pSpecCar;                // which car is using the special collision model
    int32          m_nMoney;                  // Points for this player
    int32          m_nDisplayMoney;           // Points as they would be displayed
    uint32         m_nCollectablesPickedUp;   // How many bags of sugar do we have
    uint32         m_nTotalNumCollectables;   // How many bags of sugar are there to be had in the game
    uint32         m_nLastBumpPlayerCarTimer; // Keeps track of when the last ped bumped into the player car
    uint32         m_nTaxiTimer;              // Keeps track of how long the player has been in a taxi with a passenger (in msecs)
    uint32         m_nVehicleTimeCounter;     // keeps track of how long player has been in car for driving skill
    bool           m_bTaxiTimerScore;         // If TRUE then add 1 to score for each second that the player is driving a taxi
    bool           m_bTryingToExitCar;        // if player holds exit car button, want to trigger getout once car slowed enough with a passenger
    CVehicle*      m_pLastTargetVehicle;      // Last vehicle player tried to enter.
    ePlayerState   m_nPlayerState;
    bool           m_bAfterRemoteVehicleExplosion;
    bool           m_bCreateRemoteVehicleExplosion;
    bool           m_bFadeAfterRemoteVehicleExplosion;
    uint32         m_nTimeOfRemoteVehicleExplosion;
    uint32         m_nLastTimeEnergyLost;     // To make numbers flash on the HUD
    uint32         m_nLastTimeArmourLost;
    uint32         m_nLastTimeBigGunFired;   // Tank guns etc
    uint32         m_nTimesUpsideDownInARow; // Make car blow up if car upside down
    uint32         m_nTimesStuckInARow;      // Make car blow up if player cannot get out.
    uint32         m_nCarTwoWheelCounter;    // how long has player's car been on two wheels
    float          m_fCarTwoWheelDist;       // Make car blow up if player cannot get out.
    uint32         m_nCarLess3WheelCounter;  // how long has player's car been on less than 3 wheels
    uint32         m_nBikeRearWheelCounter;  // how long has player's bike been on rear wheel only
    float          m_fBikeRearWheelDist;
    uint32         m_nBikeFrontWheelCounter; // how long has player's bike been on front wheel only
    float          m_fBikeFrontWheelDist;
    uint32         m_nTempBufferCounter;     // so wheels can leave the ground for a few frames without stopping above counters
    uint32         m_nBestCarTwoWheelsTimeMs;
    float          m_fBestCarTwoWheelsDistM;
    uint32         m_nBestBikeWheelieTimeMs;
    float          m_fBestBikeWheelieDistM;
    uint32         m_nBestBikeStoppieTimeMs;
    float          m_fBestBikeStoppieDistM;
    uint16         m_nCarDensityForCurrentZone;
    float          m_fRoadDensityAroundPlayer; // 1.0f for an average city.
    uint32         m_nTimeOfLastCarExplosionCaused;
    uint32         m_nExplosionMultiplier;
    uint32         m_nHavocCaused;        // A counter going up when the player does bad stuff.
    uint16         m_nNumHoursDidntEat;
    float          m_fCurrentChaseValue;  // How 'ill' is the chase at the moment
    bool           m_bDoesNotGetTired;
    bool           m_bFastReload;
    bool           m_bFireProof;
    uint8          m_nMaxHealth;
    uint8          m_nMaxArmour;
    bool           m_bGetOutOfJailFree; // Player doesn't lose money/weapons next time arrested
    bool           m_bFreeHealthCare;   // Player doesn't lose money nexed time patched up at hospital
    bool           m_bCanDoDriveBy;
    uint8          m_nBustedAudioStatus;
    uint16         m_nLastBustMessageNumber;
    uint32         m_nCrosshairActivated;
    CVector2D      m_vecCrosshairTarget; // -1 ... 1 on screen
    char           m_szSkinName[32];
    RwTexture*     m_pSkinTexture;
    bool           m_bParachuteReferenced;
    uint32         m_nRequireParachuteTimer;

public:
    CPlayerInfo();
    CPlayerInfo(const CPlayerInfo&) = delete;
    CPlayerInfo(CPlayerInfo&&) = delete;
    ~CPlayerInfo() = default; // 0x45B110

    CPlayerInfo& operator=(const CPlayerInfo& rhs);

    static void CancelPlayerEnteringCars(CVehicle* vehicle);
    static CEntity* FindObjectToSteal(CPed* ped);

    void Process(uint32 playerIndex);
    void FindClosestCarSectorList(CPtrList& ptrList, CPed* ped, float minX, float minY, float maxX, float maxY, float* outVehDist, CVehicle** outVehicle);
    void EvaluateCarPosition(CEntity* car, CPed* ped, float pedToVehDist, float* outDistance, CVehicle** outVehicle);
    void Clear();
    void GivePlayerParachute() const;
    void StreamParachuteWeapon(bool a2);
    void AddHealth(int32 amount) const;
    void BlowUpRCBuggy(bool bExplode) const;
    void MakePlayerSafe(bool canMove, float radius);
    void PlayerFailedCriticalMission();
    void WorkOutEnergyFromHunger();
    void ArrestPlayer();
    void KillPlayer();
    void LoadPlayerSkin();
    void DeletePlayerSkin();
    void SetPlayerSkin(const char* name);
    void SetLastTargetVehicle(CVehicle* vehicle);
    [[nodiscard]] bool IsRestartingAfterMissionFailed() const;
    [[nodiscard]] bool IsRestartingAfterArrest() const;
    [[nodiscard]] bool IsRestartingAfterDeath() const;
    [[nodiscard]] bool IsPlayerInRemoteMode() const;

    CVector GetPos() const;
    CVector GetSpeed() const;

    bool Load();
    bool Save();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CVector* GetSpeed_Hook(CVector* out);
    CVector* GetPos_Hook(CVector* outPos);

    CPlayerInfo* Constructor();
};

VALIDATE_SIZE(CPlayerInfo, 0x190);

class CPlayerInfoSaveStructure {
    int32        m_nMoney;
    uint16       m_nCarDensityForCurrentZone;
    ePlayerState m_nPlayerState;
    float        m_fRoadDensityAroundPlayer;
    int32        m_nDisplayMoney;
    uint8        m_nNumHoursDidntEat;
    uint32       m_nCollectablesPickedUp;
    uint32       m_nTotalNumCollectables;
    bool         m_bDoesNotGetTired;
    bool         m_bFastReload;
    bool         m_bFireProof;
    uint8        m_nMaxHealth;
    uint8        m_nMaxArmour;
    bool         m_bGetOutOfJailFree;
    bool         m_bFreeHealthCare;
    bool         m_bCanDoDriveBy;
    uint8        m_nBustedAudioStatus;
    uint16       m_nLastBustMessageNumber;

public:
    void Construct(CPlayerInfo* info) {
        m_nMoney =                          info->m_nMoney;
        m_nCarDensityForCurrentZone =       info->m_nCarDensityForCurrentZone;
        m_nPlayerState =                    info->m_nPlayerState;
        m_fRoadDensityAroundPlayer =        info->m_fRoadDensityAroundPlayer;
        m_nDisplayMoney =                   info->m_nDisplayMoney;
        m_nNumHoursDidntEat =               static_cast<uint8>(info->m_nNumHoursDidntEat);
        m_nCollectablesPickedUp =           info->m_nCollectablesPickedUp;
        m_nTotalNumCollectables =           info->m_nTotalNumCollectables;
        m_bDoesNotGetTired =                info->m_bDoesNotGetTired;
        m_bFastReload =                     info->m_bFastReload;
        m_bFireProof =                      info->m_bFireProof;
        m_nMaxHealth =                      info->m_nMaxHealth;
        m_nMaxArmour =                      info->m_nMaxArmour;
        m_bGetOutOfJailFree =               info->m_bGetOutOfJailFree;
        m_bFreeHealthCare =                 info->m_bFreeHealthCare;
        m_bCanDoDriveBy =                   info->m_bCanDoDriveBy;
        m_nBustedAudioStatus =              info->m_nBustedAudioStatus;
        m_nLastBustMessageNumber =          info->m_nLastBustMessageNumber;
    }

    void Extract(CPlayerInfo* info) {
        info->m_nMoney =                    m_nMoney;
        info->m_nCarDensityForCurrentZone = m_nCarDensityForCurrentZone;
        info->m_nPlayerState =              m_nPlayerState;
        // info->m_nCarDensityForCurrentZone = m_nCarDensityForCurrentZone; redundant
        info->m_fRoadDensityAroundPlayer =  m_fRoadDensityAroundPlayer;
        info->m_nDisplayMoney =             m_nDisplayMoney;
        info->m_nNumHoursDidntEat =         m_nNumHoursDidntEat;
        info->m_nCollectablesPickedUp =     m_nCollectablesPickedUp;
        info->m_nTotalNumCollectables =     m_nTotalNumCollectables;
        info->m_bDoesNotGetTired =          m_bDoesNotGetTired;
        info->m_bFastReload =               m_bFastReload;
        info->m_bFireProof =                m_bFireProof;
        info->m_nMaxHealth =                m_nMaxHealth;
        info->m_nMaxArmour =                m_nMaxArmour;
        info->m_bGetOutOfJailFree =         m_bGetOutOfJailFree;
        info->m_bFreeHealthCare =           m_bFreeHealthCare;
        info->m_bCanDoDriveBy =             m_bCanDoDriveBy;
        info->m_nBustedAudioStatus =        m_nBustedAudioStatus;
        info->m_nLastBustMessageNumber =    m_nLastBustMessageNumber;
    }
};

VALIDATE_SIZE(CPlayerInfoSaveStructure, 0x28);
