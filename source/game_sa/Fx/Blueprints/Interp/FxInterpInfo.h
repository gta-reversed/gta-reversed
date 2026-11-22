#pragma once

#include "FxTools.h"

class NOTSA_EXPORT_VTABLE FxInterpInfo_c {
public:
    bool    m_bLooped;  // (4 bytes in Manhunt)
    int8    m_nNumKeys;
    int16   m_nCount;
    uint16* m_pTimes;

public:
    FxInterpInfo_c();
    ~FxInterpInfo_c() = default; // 0x4A8430

    virtual void Load(FILESTREAM file) = 0;
};
