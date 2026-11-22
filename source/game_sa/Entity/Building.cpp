#include "StdInc.h"

#include "Building.h"

void CBuilding::InjectHooks()
{
    RH_ScopedVirtualClass(CBuilding, 0x8585C8, 22);
    RH_ScopedCategory("Entity");

    RH_ScopedInstall(ReplaceWithNewModel, 0x403EC0);
    RH_ScopedGlobalInstall(IsBuildingPointerValid, 0x4040E0);
}

CBuilding::CBuilding() : CEntity()
{
    SetTypeBuilding();
    SetUsesCollision(true);
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
    if (!CModelInfo::GetModelInfo(GetModelIndex())->m_nRefCount)
        CStreaming::RemoveModel(GetModelIndex());

    m_nModelIndex = newModelIndex;
}

bool IsBuildingPointerValid(CBuilding* building)
{
    if (!building)
        return false;

    return GetBuildingPool()->IsObjectValid(building);
}
