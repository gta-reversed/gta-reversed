#pragma once

#include "Base.h"

class CPed;

class CAccident {
public:
    CPed* m_pPed{ nullptr };
    bool  m_bIsTreated{ false };
    bool  m_bIsRevived{ false };

public:
    CAccident() = default; // 0x56CDE0

    // 0x56CE00
    bool IsFree() const {
        return    m_pPed
               && !m_bIsTreated
               && !m_bIsRevived
               && !m_pPed->physicalFlags.bSubmergedInWater;
    }
};
VALIDATE_SIZE(CAccident, 0x8);
