/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

#include "Enums/eCrimeType.h"

class CCrimeBeingQd {
public:
    eCrimeType m_nCrimeType;
    uint32     m_nCrimeId;
    uint32     m_nStartTime;
    CVector    m_vecCoors;
    bool       m_bAlreadyReported;
    bool       m_bPoliceDontReallyCare;
};

VALIDATE_SIZE(CCrimeBeingQd, 0x1C);
