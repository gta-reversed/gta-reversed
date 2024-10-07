#pragma once

#include "Vector.h"

struct MovementInfo_t {
    CVector m_Pos;
    CVector m_Vel;
    float   m_Rot[4];
    bool    m_bHasFloatInfo;
    bool    m_bHasUnderwaterInfo;

    void Process() {
        for (auto& r : m_Rot) { r = 0.0f; }
        m_bHasFloatInfo = false;
        m_bHasUnderwaterInfo = false;
    }
};
VALIDATE_SIZE(MovementInfo_t, 0x2C);
