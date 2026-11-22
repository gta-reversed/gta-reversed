/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

struct tBoatHandlingData {
    int32   m_nVehicleId;
    float   m_fThrustY;
    float   m_fThrustZ;
    float   m_fThrustAppZ;
    float   m_fAqPlaneForce;
    float   m_fAqPlaneLimit;
    float   m_fAqPlaneOffset;
    float   m_fWaveAudioMult;
    float   m_fLookLRBehindCamHeight;
    CVector m_vecMoveRes;
    CVector m_vecTurnRes;

    int32 InitFromData(int32 id, const char* line);
};
VALIDATE_SIZE(tBoatHandlingData, 0x3C);
