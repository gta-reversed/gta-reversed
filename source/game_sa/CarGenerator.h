/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CompressedVector.h"

#include "Vector.h"

class CFileCarGenerator {
public:
    CVector m_vecPosn;
    float   m_fAngle;
    int32   m_nModelId;
    int32   m_nPrimaryColor;
    int32   m_nSecondaryColor;
    union {
        struct {
            uint32 m_bForceSpawn : 1;            // & 1
            uint32 m_bIgnorePopulationLimit : 1; // & 2
        };
        uint32 m_nFlags;
    };
    int32 m_nAlarmChance;
    int32 m_nDoorLockChance;
    int32 m_nMinDelay;
    int32 m_nMaxDelay;
};
VALIDATE_SIZE(CFileCarGenerator, 0x30);

class CCarGenerator {
public:
    int16            m_nModelId;
    int8             m_nPrimaryColor;
    int8             m_nSecondaryColor;
    CompressedVector m_vecPosn;
    char             m_nAngle;
    uint8            m_nAlarmChance;    // in percentage 0-100
    uint8            m_nDoorLockChance; // in percentage 0-100

    union {
        struct {
            uint8 bWaitUntilFarFromPlayer : 1;
            uint8 bHighPriority : 1;
            uint8 bActive : 1;
            uint8 bPlayerHasAlreadyOwnedCar : 1;
            uint8 bIgnorePopulationLimit : 1;
        };
        uint8 m_nFlags;
    };
    uint16 m_nMinDelay;
    uint16 m_nMaxDelay;
    // char             _pad12[2];
    uint32       m_nNextGenTime;
    int16        m_nVehicleHandle;
    uint16       m_nGenerateCount; // (0) Don't Spawn , (>0) Spawn X number of times , (0xFFFF) Always Spawn.
    uint8        m_nIplId;
    bool         m_bIsUsed;
    // char             _pad1E[2];

public:
    static bool& m_bHotdogVendorPositionOffsetInitialized;
    static CVector& m_HotdogVendorPositionOffset;

public:
    static void InjectHooks();

    bool CheckForBlockage(int32 modelId);
    bool CheckIfWithinRangeOfAnyPlayers();
    void DoInternalProcessing();
    void Process();
    void Setup(CVector posn, float angle, int32 modelId, int16 color1, int16 color2, uint8 bForceSpawn,
               uint8 alarmChance, uint8 doorLockChance, uint16 minDelay, uint16 maxDelay,
               uint8 iplId, bool ignorePopulationLimit);
    void SwitchOff();
    void SwitchOn();

    static uint32 CalcNextGen();
};

VALIDATE_SIZE(CCarGenerator, 0x20);
