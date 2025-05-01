#include "StdInc.h"

#include "LodTimeModelInfo.h"

void CLodTimeModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CLodTimeModelInfo, 0x85BCF0, 16);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(GetModelType, 0x4C5660);
    RH_ScopedVMTInstall(GetTimeInfo, 0x4C5670);
}
ModelInfoType CLodTimeModelInfo::GetModelType()
{
    return ModelInfoType::MODEL_INFO_TIME;
}

// 0x0
CTimeInfo* CLodTimeModelInfo::GetTimeInfo()
{
    return &m_lodTimeInfo;
}