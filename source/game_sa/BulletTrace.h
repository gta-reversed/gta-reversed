/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"

#include "Vector.h"

class CBulletTrace {
public:
    CVector m_vecStart;
    CVector m_vecEnd;
    bool    m_bExists;
    uint32  m_nCreationTime;
    uint32  m_nLifeTime;
    float   m_fRadius;
    uint8   m_nTransparency;

public:
    void Update() {
        if (CTimer::GetTimeInMS() - m_nCreationTime >= m_nLifeTime) {
            m_bExists = false;
        }
    }

    // NOTSA
    CVector GetDirection() const noexcept { return m_vecEnd - m_vecStart; }
    uint32 GetRemainingLifetime() const noexcept { return CTimer::GetTimeInMS() - m_nCreationTime; }
};

VALIDATE_SIZE(CBulletTrace, 0x2C);
