/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "RGBA.h"

class CRegisteredMotionBlurStreak {
public:
    uint32  m_nId;
    CRGBA   m_color;
    std::array<CVector, 3> m_avecLeftPoints;
    std::array<CVector, 3> m_avecRightPoints;
    std::array<bool, 3>    m_acRenderHistory;

public:
    static void InjectHooks();

    void Update();
    void Render();
};

VALIDATE_SIZE(CRegisteredMotionBlurStreak, 0x54);
