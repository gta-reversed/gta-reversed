/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "CPlayerPedData.h"
#include "CVector.h"
#include "CPlayerPed.h"

enum ePlayerState : uint8 {
    PLAYERSTATE_PLAYING,
    PLAYERSTATE_HAS_DIED,
    PLAYERSTATE_HAS_BEEN_ARRESTED,
    PLAYERSTATE_FAILED_MISSION,
    PLAYERSTATE_LEFT_GAME
};

class CPed;
class CVehicle;

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
    char           _pad0[2];
    CVehicle*      m_pLastTargetVehicle; // Last vehicle player tried to enter.
    ePlayerState   m_nPlayerState;
    bool           m_bAfterRemoteVehicleExplosion;
    bool           m_bCreateRemoteVehicleExplosion;
    bool           m_bFadeAfterRemoteVehicleExplosion;
    uint32         m_nTimeOfRemoteVehicleExplosion;
    uint32         m_nLastTimeEnergyLost; // To make numbers flash on the HUD;
    uint32         m_nLastTimeArmourLost;
    uint32         m_nLastTimeBigGunFired;   // Tank guns etc
    uint32         m_nTimesUpsideDownInARow; // Make car blow up if car upside down
    uint32         m_nTimesStuckInARow;      // Make car blow up if player cannot get out.
    uint32         m_nCarTwoWheelCounter;    // how long has player's car been on two wheels
    float          m_fCarTwoWheelDist;       // Make car blow up if player cannot get out.
    uint32       m_nCarLess3WheelCounter;  // how long has player's car been on less than 3 wheels
    uint32         m_nBikeRearWheelCounter;  // how long has player's bike been on rear wheel only
    float          m_fBikeRearWheelDist;
    uint32         m_nBikeFrontWheelCounter; // how long has player's bike been on front wheel only
    float          m_fBikeFrontWheelDist;
    uint32         m_nTempBufferCounter; // so wheels can leave the ground for a few frames without stopping above counters
    uint32         m_nBestCarTwoWheelsTimeMs;
    float          m_fBestCarTwoWheelsDistM;
    uint32         m_nBestBikeWheelieTimeMs;
    float          m_fBestBikeWheelieDistM;
    uint32         m_nBestBikeStoppieTimeMs;
    float          m_fBestBikeStoppieDistM;
    uint16         m_nCarDensityForCurrentZone;
    char           _pad1[2];
    float          m_fRoadDensityAroundPlayer; // 1.0f for an average city.
    uint32         m_nTimeOfLastCarExplosionCaused;
    uint32         m_nExplosionMultiplier;
    uint32         m_nHavocCaused; // A counter going up when the player does bad stuff.
    uint16         m_nNumHoursDidntEat;
    char           _pad2[2];
    float          m_fCurrentChaseValue; // How 'ill' is the chase at the moment
    bool           m_bDoesNotGetTired;
    bool           m_bFastReload;
    bool           m_bFireProof;
    uint8          m_nMaxHealth;
    uint8          m_nMaxArmour;
    bool           m_bGetOutOfJailFree; // Player doesn't lose money/weapons next time arrested
    bool           m_bFreeHealthCare;   // Player doesn't lose money nexed time patched up at hospital
    bool           m_bCanDoDriveBy;
    uint8          m_nBustedAudioStatus;
    char           _pad3;
    uint16         m_nLastBustMessageNumber;
    uint32         m_nCrosshairActivated;
    CVector2D      m_vecCrosshairTarget; // -1 ... 1 on screen
    char           m_szSkinName[32];
    RwTexture*     m_pSkinTexture;
    bool           m_bParachuteReferenced;
    char           _pad4[3];
    uint32         m_nRequireParachuteTimer;

public:
    static void InjectHooks();
    bool IsPlayerInRemoteMode();
    void DeletePlayerSkin();
    void LoadPlayerSkin();
    void Clear();
};

VALIDATE_SIZE(CPlayerInfo, 0x190);
