#include "StdInc.h"

#include "ModelInfo.h"

void CTimeModelInfo::InjectHooks() {
    RH_ScopedVirtualClass(CTimeModelInfo, 0x85BCB0, 16);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(GetModelType, 0x4C5660);
    RH_ScopedVMTInstall(GetTimeInfo, 0x4C5670);
}

// 0x4C5660
ModelInfoType CTimeModelInfo::GetModelType() {
    return ModelInfoType::MODEL_INFO_TIME;
}

// 0x4C5670
CTimeInfo* CTimeModelInfo::GetTimeInfo() {
    return &m_timeInfo;
}

