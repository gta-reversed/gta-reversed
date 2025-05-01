#pragma once

#include "AtomicModelInfo.h"
#include "TimeInfo.h"

class NOTSA_EXPORT_VTABLE CLodTimeModelInfo : public CLodAtomicModelInfo {
public:
    CTimeInfo m_lodTimeInfo;

public:
    static void InjectHooks();

    // virtual
    ModelInfoType GetModelType() override;
    CTimeInfo*    GetTimeInfo() override;

    // virtual implementations
};

VALIDATE_SIZE(CLodTimeModelInfo, 0x28);
