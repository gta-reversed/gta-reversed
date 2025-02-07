/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CEntity;
class CPed;

class CCoverPoint {
public:
    CCoverPoint() = default;

    bool CanAccomodateAnotherPed();
    void ReleaseCoverPointForPed(CPed* ped);
    void ReserveCoverPointForPed(CPed* ped);

public:
    char     m_nMaxPedsInCover{};
    char     m_nCoverType{};
    char     m_nDirection{};
    CVector  m_vecOffset{};
    CEntity* m_pCoverEntity{};
    CPed*    m_apCoveredPeds[2]{};
};
VALIDATE_SIZE(CCoverPoint, 0x1C);
