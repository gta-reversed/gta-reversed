/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AtomicModelInfo.h"
#include "TimeInfo.h"

class NOTSA_EXPORT_VTABLE CTimeModelInfo : public CAtomicModelInfo {
protected:
    CTimeInfo m_Time;

public:
    CTimeModelInfo() = default;

    ModelInfoType GetModelType() override;
    CTimeInfo* GetTimeInfo() override;
private:
    friend void InjectHooksMain();
    static void InjectHooks();
};

VALIDATE_SIZE(CTimeModelInfo, 0x24);
