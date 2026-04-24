#pragma once

#include "Base.h"
#include "eBoneTag.h"

// BoneInfo_t
struct tBoneInfo {
    eBoneTag  m_current; // This bone
    eBoneTag  m_prev;    // Previous connected bone

    CVector   m_PosRots;
    CVector2D m_LimitsX;
    CVector2D m_LimitsY;
    CVector2D m_LimitsZ;
};

VALIDATE_SIZE(tBoneInfo, 0x28);
