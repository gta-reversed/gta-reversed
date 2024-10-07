#include "StdInc.h"

#include "Building.h"

int32& gBuildings = *(int32*)0xB71804;

void CBuilding::InjectHooks()
{
    RH_ScopedVirtualClass(CBuilding, 0x8585C8, 22);
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
    return GetBuildingPool()->New();
}

void CBuilding::operator delete(void* data)
{
    GetBuildingPool()->Delete(static_cast<CBuilding*>(data));
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

    return GetBuildingPool()->IsObjectValid(building);
}
