#include "StdInc.h"

void cTransmission::InjectHooks()
{
    ReversibleHooks::Install("cTransmission", "DisplayGearRatios", 0x6D0590, &cTransmission::DisplayGearRatios);
    ReversibleHooks::Install("cTransmission", "InitGearRatios", 0x6D0460, &cTransmission::InitGearRatios);
    ReversibleHooks::Install("cTransmission", "CalculateGearForSimpleCar", 0x6D0530, &cTransmission::CalculateGearForSimpleCar);
    ReversibleHooks::Install("cTransmission", "CalculateDriveAcceleration", 0x6D05E0, &cTransmission::CalculateDriveAcceleration);
}

// unused
//
// Usage:
//     auto vehicle = FindPlayerVehicle(-1, false);
//     if (vehicle) {
//         vehicle->m_pHandlingData->GetTransmission().DisplayGearRatios();
//     }
//
// 0x6D0590
void cTransmission::DisplayGearRatios()
{
    // 1000 millimeters / 1 hour in seconds
    // flt_858630
    static constexpr float magic_0 = 1000.0f / 3600.0f;
    static constexpr float magic = magic_0 / 50.0f;

    for (uint8 i = 0; i <= m_nNumberOfGears; i++)
    {
        tTransmissionGear& gear = m_aGears[i];
        printf(
            "%d, max v = %3.2f, up at = %3.2f, down at = %3.2f\n",
            i,
            (1.0f / magic) * gear.m_maxVelocity,
            (1.0f / magic) * gear.m_changeUpVelocity,
            (1.0f / magic) * gear.m_changeDownVelocity);
    }
}

// 0x6D0460
void cTransmission::InitGearRatios()
{
    memset(m_aGears, 0, sizeof(m_aGears));
    float averageHalfGearVelocity = 0.5f * m_fMaxGearVelocity / m_nNumberOfGears;
    float maxGearVelocity = m_fMaxGearVelocity - averageHalfGearVelocity;
    for (uint8 i = 1; i <= m_nNumberOfGears; i++)
    {
        static tTransmissionGear* gear = nullptr;
        static tTransmissionGear* previousGear = nullptr;
        gear = &m_aGears[i];
        previousGear = &m_aGears[i - 1];
        gear->m_maxVelocity = (static_cast<float>(i) * maxGearVelocity / m_nNumberOfGears) + averageHalfGearVelocity;
        float velocityDifference = gear->m_maxVelocity - previousGear->m_maxVelocity;
        if (i >= m_nNumberOfGears)
        {
            gear->m_changeUpVelocity = m_fMaxGearVelocity;
        }
        else
        {
            tTransmissionGear& nextGear = m_aGears[i + 1];
            nextGear.m_changeDownVelocity = 0.42f * velocityDifference + previousGear->m_maxVelocity;
            gear->m_changeUpVelocity = 0.6667f * velocityDifference + previousGear->m_maxVelocity;
        }
    }
    m_aGears[0].m_maxVelocity = m_maxReverseGearVelocity;
    m_aGears[0].m_changeUpVelocity = -0.01f;
    m_aGears[0].m_changeDownVelocity = m_maxReverseGearVelocity;
    m_aGears[1].m_changeDownVelocity = -0.01f;
}

// 0x6D0530
void cTransmission::CalculateGearForSimpleCar(float speed, uint8& currentGear)
{
    m_currentVelocity = speed;
    tTransmissionGear& gear = m_aGears[currentGear];
    if (speed > gear.m_changeUpVelocity)
    {
        if (currentGear < m_nNumberOfGears)
            currentGear++;
    }
    else if (speed < gear.m_changeDownVelocity)
    {
        if (currentGear > 0)
            currentGear--;
    }
}

// 0x6D05E0
float cTransmission::CalculateDriveAcceleration(float const& gasPedal, uint8& currentGear, float& gearChangeCount, float& velocity, float* a6, float* a7, uint8 allWheelsOnGround, uint8 handlingCheat)
{
    static float cheatMultiplier = 0.0f;
    static float driveAcceleration = 0.0f;
    static float currentVelocity = 0.0f;
    currentVelocity = velocity;
    if (currentVelocity < m_maxReverseGearVelocity)
        return 0.0f;

    while (currentVelocity <= m_fMaxGearVelocity)
    {
        m_currentVelocity = currentVelocity;
        tTransmissionGear& gear = m_aGears[currentGear];
        bool accelerate = false;
        bool shiftToLowerGear = false;
        if (currentVelocity > gear.m_changeUpVelocity)
        {
            if (currentGear == 0 && gasPedal <= 0.0f)
                accelerate = true;
            else
                currentGear++;
        }
        else {
            if (currentVelocity >= gear.m_changeDownVelocity
                || currentGear == 0
                || currentGear == 1 && gasPedal >= 0.0f)
            {
                accelerate = true;
            }
            shiftToLowerGear = true;
        }
        if (accelerate)
        {
            float speedMultiplier = 0.0f;
            float nitrosMultiplier = 0.0f;
            if (m_nNumberOfGears == 1)
            {
                speedMultiplier = 1.0f;
                nitrosMultiplier = 1.0f;
            }
            else if (currentGear >= 1)
            {
                float gearNumber = 1.0f - (static_cast<float>(currentGear) - 1.0f) / (static_cast<float>(m_nNumberOfGears) - 1.0f);
                gearNumber *= gearNumber;
                if (m_handlingFlags & VEHICLE_HANDLING_1G_BOOST)
                    speedMultiplier = gearNumber * 5.0f;
                else if (m_handlingFlags & VEHICLE_HANDLING_2G_BOOST)
                    speedMultiplier = gearNumber * 4.0f;
                else
                    speedMultiplier = gearNumber * 3.0f;
                speedMultiplier += 1.0f;
                nitrosMultiplier = 1.0f;
            }
            else
            {   // reverse gear
                speedMultiplier = 4.5f;
                nitrosMultiplier = 1.0f;
            }
            cheatMultiplier = 1.0f;
            if (handlingCheat == CHEAT_HANDLING_PERFECT )
                cheatMultiplier = TRANSMISSION_AI_CHEAT_MULT;
            else if (handlingCheat == CHEAT_HANDLING_NITROS)
                nitrosMultiplier = TRANSMISSION_NITROS_MULT;
            driveAcceleration = speedMultiplier * (cheatMultiplier * m_fEngineAcceleration)
                * nitrosMultiplier * 0.4f * gasPedal * CTimer::ms_fTimeStep;
            if (a6 && a7)
            {

                if (allWheelsOnGround)
                {
                    float currentDownVelocityDiff = 0.0f;
                    float upDownVelocityDiff = 0.0f;
                    if (currentGear)
                    {
                        if (currentGear == 1)
                        {
                            currentDownVelocityDiff = currentVelocity
                                + m_fMaxGearVelocity / static_cast<float>(m_nNumberOfGears) * (1.0f - 0.6667f);
                            upDownVelocityDiff = m_fMaxGearVelocity / static_cast<float>(m_nNumberOfGears) * (1.0f - 0.6667f)
                                + m_aGears[1].m_changeUpVelocity;
                        }
                        else
                        {
                            currentDownVelocityDiff = currentVelocity - gear.m_changeDownVelocity;
                            upDownVelocityDiff = gear.m_changeUpVelocity - gear.m_changeDownVelocity;
                        }
                    }
                    else
                    {
                        // reverse gear
                        currentDownVelocityDiff = m_fMaxGearVelocity / static_cast<float>(m_nNumberOfGears) * (1.0f - 0.6667f)
                            - currentVelocity;
                        upDownVelocityDiff = m_fMaxGearVelocity / static_cast<float>(m_nNumberOfGears) * (1.0f - 0.6667f)
                            - m_aGears[0].m_changeDownVelocity;
                    }
                    const float velocityDiffRatio = currentDownVelocityDiff / upDownVelocityDiff;
                    float inertiaMultiplier = velocityDiffRatio - *a6;
                    if (handlingCheat == CHEAT_HANDLING_PERFECT )
                    {
                        inertiaMultiplier *= TRANSMISSION_AI_CHEAT_INERTIA_MULT;
                    }
                    else if (handlingCheat == CHEAT_HANDLING_NITROS)
                    {
                        inertiaMultiplier *= TRANSMISSION_NITROS_INERTIA_MULT;
                    }
                    float acceleration = 1.0f - inertiaMultiplier * m_fEngineInertia;
                    acceleration = clamp<float>(acceleration, 0.1f, 1.0f);
                    *a6 = velocityDiffRatio;
                    *a7 = acceleration * (1.0f - TRANSMISSION_SMOOTHER_FRAC) + TRANSMISSION_SMOOTHER_FRAC * *a7;
                    driveAcceleration *= *a7;
                }
                else
                {
                    *a6 += fabs(gasPedal) / m_fEngineInertia * CTimer::ms_fTimeStep * TRANSMISSION_FREE_ACCELERATION;
                    *a6 = std::min(*a6, 1.0f);
                    *a7 = 0.1f;
                }
            }
            const float gearMaxVelocity = gear.m_maxVelocity;
            const float gearCheatMaxVelocity = cheatMultiplier * gearMaxVelocity;
            float changeInVelocity = 0.0f;
            if (gearMaxVelocity >= 0.0f || currentVelocity >= gearCheatMaxVelocity)
            {
                if (gearMaxVelocity <= 0.0f)
                    return driveAcceleration;
                if (currentVelocity <= gearCheatMaxVelocity)
                    return driveAcceleration;
                changeInVelocity = currentVelocity - gearCheatMaxVelocity;
            }
            else
            {
                changeInVelocity = gearCheatMaxVelocity - currentVelocity;
            }
            driveAcceleration *= (1.0f - std::min(changeInVelocity / 0.05f, 1.0f));
            return driveAcceleration;
        }
        if (shiftToLowerGear)
            currentGear--;
        a6 = nullptr;
        a7 = nullptr;
        allWheelsOnGround = false;
        handlingCheat = CHEAT_HANDLING_NONE;
        if (currentVelocity < m_maxReverseGearVelocity)
            return 0.0f;
    }
    return 0.0f;
}
