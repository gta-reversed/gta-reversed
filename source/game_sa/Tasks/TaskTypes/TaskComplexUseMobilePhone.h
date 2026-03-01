/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseMobilePhone : public CTaskComplex {
public:
    int32      m_nDuration;
    CTaskTimer m_timer;
    bool       m_bIsAborting;
    bool       m_bQuit;

public:
    static constexpr auto Type = TASK_COMPLEX_USE_MOBILE_PHONE;

    CTaskComplexUseMobilePhone(int32 nDuration);
};

VALIDATE_SIZE(CTaskComplexUseMobilePhone, 0x20);
