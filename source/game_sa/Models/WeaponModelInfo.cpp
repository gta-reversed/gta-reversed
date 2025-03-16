#include "StdInc.h"

#include "WeaponModelInfo.h"

void CWeaponModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CWeaponModelInfo, 0x85BD78, 17);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(GetModelType, 0x4C5780);
    RH_ScopedVMTInstall(Init, 0x4C98F0);
    RH_ScopedVMTInstall(SetClump, 0x4C9910);
}

// 0x4C5780
ModelInfoType CWeaponModelInfo::GetModelType()
{
    return ModelInfoType::MODEL_INFO_WEAPON;
}

// 0x4C98F0
void CWeaponModelInfo::Init()
{
    CClumpModelInfo::Init();
    m_weaponInfo = eWeaponType::WEAPON_UNARMED;
}

// 0x4C9910
void CWeaponModelInfo::SetClump(RpClump* clump)
{
    CClumpModelInfo::SetClump(clump);
    RpClumpForAllAtomics(clump, CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderWeaponCB);

    auto flashFrame = CClumpModelInfo::GetFrameFromName(clump, "gunflash");
    if (!flashFrame)
        return;

    auto firstAtomic = reinterpret_cast<RpAtomic*>(GetFirstObject(flashFrame));
    if (!firstAtomic)
        return;

    CVehicle::SetComponentAtomicAlpha(firstAtomic, 0);
    RpAtomicSetFlags(firstAtomic, 0);
    auto geometry = RpAtomicGetGeometry(firstAtomic);
    RpMaterialGetSurfaceProperties(RpGeometryGetMaterial(geometry, 0))->ambient = 16.0F;

}