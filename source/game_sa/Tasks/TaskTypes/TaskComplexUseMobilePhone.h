/*
Plugin-SDK (Grand Theft Auto San Andreas) header file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class CTaskComplexUseMobilePhone : public CTaskComplex {
public:
    int32 m_nDuration;
    CTaskTimer m_timer;
    bool m_bIsAborting;
    bool m_bQuit;

    CTaskComplexUseMobilePhone(int32 nDuration);
};

VALIDATE_SIZE(CTaskComplexUseMobilePhone, 0x20);
