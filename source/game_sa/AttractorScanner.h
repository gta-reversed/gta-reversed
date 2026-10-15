/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskTimer.h"

struct C2dEffect;

class CAttractorScanner {
public:
    char       field_0;
    char       _pad[3];
    CTaskTimer field_4;
    C2dEffect* m_pEffectInUse;
    int32      field_14;
    int32      field_18[10];
    int32      field_40[10];
    int32      field_68[10];

    void Clear();
};

VALIDATE_SIZE(CAttractorScanner, 0x90);
