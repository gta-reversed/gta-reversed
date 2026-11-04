/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "FxSphere.h"
#include "FxPlane.h"

class FxFrustumInfo_c {
public:
    FxSphere_c m_Sphere;
    std::array<FxPlane_c, 4> m_Planes;

    bool IsCollision(FxSphere_c& sphere);
};
VALIDATE_SIZE(FxFrustumInfo_c, 0x54);
