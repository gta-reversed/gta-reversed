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
public:
    CTimeInfo m_timeInfo;

public:
    static void InjectHooks();

    ModelInfoType GetModelType() override;
    CTimeInfo* GetTimeInfo() override;

private:
};

VALIDATE_SIZE(CTimeModelInfo, 0x24);
