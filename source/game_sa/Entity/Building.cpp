#include "StdInc.h"

#include "Building.h"

int32& gBuildings = *(int32*)0xB71804;

void CBuilding::InjectHooks()
{
    RH_ScopedClass(CBuilding);
    RH_ScopedCategory("Entity");

    RH_ScopedInstall(ReplaceWithNewModel, 0x403EC0);
    RH_ScopedGlobalInstall(IsBuildingPointerValid, 0x4040E0);
}

CBuilding::CBuilding() : CEntity()
{
    m_nType = ENTITY_TYPE_BUILDING;
    m_bUsesCollision = true;
}

void* CBuilding::operator new(unsigned size)
{
    return CPools::ms_pBuildingPool->New();
}

void CBuilding::operator delete(void* data)
{
    CPools::ms_pBuildingPool->Delete(static_cast<CBuilding*>(data));
}

void CBuilding::ReplaceWithNewModel(int32 newModelIndex)
{
    DeleteRwObject();
    if (!CModelInfo::GetModelInfo(m_nModelIndex)->m_nRefCount)
        CStreaming::RemoveModel(m_nModelIndex);

    m_nModelIndex = newModelIndex;
}

bool IsBuildingPointerValid(CBuilding* building)
{
    if (!building)
        return false;

    return CPools::ms_pBuildingPool->IsObjectValid(building);
}
