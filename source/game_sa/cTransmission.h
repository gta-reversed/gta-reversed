/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "tTransmissionGear.h"

const float TRANSMISSION_FREE_ACCELERATION = 0.1f;
const float TRANSMISSION_SMOOTHER_FRAC = 0.85f;
const float TRANSMISSION_AI_CHEAT_MULT = 1.2f;
const float TRANSMISSION_NITROS_MULT = 2.0f;
const float TRANSMISSION_AI_CHEAT_INERTIA_MULT = 0.75f;
const float TRANSMISSION_NITROS_INERTIA_MULT = 0.5f;

enum {
    CHEAT_HANDLING_NONE = 0,
    CHEAT_HANDLING_PERFECT = 1,
    CHEAT_HANDLING_NITROS = 2
};

class cTransmission {
public:
    tTransmissionGear m_aGears[6];      // 0 = reverse
    uint8             m_nDriveType;     // F/R/4
    uint8             m_nEngineType;    // P/D/E
    uint8             m_nNumberOfGears; // 1 to 6
    uint32            m_handlingFlags;
    float             m_EngineAcceleration; // 0.1 to 10.0
    float             m_EngineInertia;      // 0.0 to 50.0
    float             m_MaxVelocity;    // 5.0 to 150.0
    float             m_MaxFlatVelocity;
    float             m_MaxReverseVelocity;
    float             m_Velocity;

public:
    static void InjectHooks();

    cTransmission() = default; // 0x6D0450

    void InitGearRatios();
    void DisplayGearRatios();
    void CalculateGearForSimpleCar(float speed, uint8& currentGear);
    float CalculateDriveAcceleration(const float& gasPedal, uint8& currentGear, float& gearChangeCount, float& velocity, float* a6, float* a7, uint8 allWheelsOnGround, uint8 handlingType);
};

VALIDATE_SIZE(cTransmission, 0x68);
