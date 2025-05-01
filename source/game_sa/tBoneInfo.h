#pragma once

#include "Base.h"
#include "eBoneTag.h"

struct tBoneInfo {
    eBoneTag m_current; // This bone
    eBoneTag m_prev;    // Previous connected bone
    CVector   m_Max;
    CVector   m_Min;
    CVector   m_ABC;
};
VALIDATE_SIZE(tBoneInfo, 0x28);
