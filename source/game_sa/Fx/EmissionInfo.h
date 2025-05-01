#pragma once

#include "Vector.h"

struct EmissionInfo_t {
    float m_fCount;
    float m_fRadius;

    CVector m_SizeMin;
    CVector m_SizeMax;

    float   m_fSpeed;
    float   m_fSpeedBias;
    CVector m_Direction;

    float   m_fAngleMin;
    float   m_fAngleMax;

    float   m_fLife;
    float   m_fLifeBias;
    CVector m_Pos;

    float   m_fRotationMinAngle;
    float   m_fRotationMaxAngle;

    float   m_fMinWind;
    float   m_fMaxWind;
    float   m_fMinRain;
    float   m_fMaxRain;

    void Process(float mult) {
        m_fCount  = mult * 10.0f;
        m_fLife   = 1.0f;
        m_fRadius = 0.0f;

        m_fSpeed     = 0.0f;
        m_fSpeedBias = 0.0f;

        m_fAngleMin = 0.0f;
        m_fAngleMax = 0.0f;

        m_fLifeBias = 0.0f;

        m_fRotationMinAngle = 0.0f;
        m_fRotationMaxAngle = 0.0f;

        m_fMinWind = 0.0f;
        m_fMaxWind = 2.0f;

        m_fMinRain = 0.0f;
        m_fMaxRain = 2.0f;

        m_SizeMin.Set(0.0f, 0.0f, 0.0f);
        m_SizeMax.Set(0.0f, 0.0f, 0.0f);
        m_Direction.Set(0.0f, 0.0f, 1.0f);
        m_Pos.Set(0.0f, 0.0f, 0.0f);
    }
};
VALIDATE_SIZE(EmissionInfo_t, 0x68);
