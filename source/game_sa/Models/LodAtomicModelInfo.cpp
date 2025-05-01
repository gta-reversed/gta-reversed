#include "StdInc.h"

#include "LodAtomicModelInfo.h"

void CLodAtomicModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CLodAtomicModelInfo, 0x85BC70, 16);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(AsLodAtomicModelInfoPtr, 0x4C5610);
    RH_ScopedVMTInstall(GetModelType, 0x4C5600);
    RH_ScopedVMTInstall(Init, 0x4C54D0);
}

CLodAtomicModelInfo::CLodAtomicModelInfo() : CAtomicModelInfo()
{
    m_numChildren = 0;
    m_numChildrenRendered = 0;
}

// 0x4C5610
CLodAtomicModelInfo* CLodAtomicModelInfo::AsLodAtomicModelInfoPtr()
{
    return this;
}

// 0x4C5600
ModelInfoType CLodAtomicModelInfo::GetModelType()
{
    return ModelInfoType::MODEL_INFO_LOD;
}

// 0x4C54D0
void CLodAtomicModelInfo::Init()
{
    CAtomicModelInfo::Init();
}