#include "StdInc.h"

#include "ProjectileInfo.h"

void CProjectileInfo::InjectHooks() {
    RH_ScopedClass(CProjectileInfo);
    RH_ScopedCategoryGlobal();

    // Install("CProjectileInfo", "", , &CProjectileInfo::);
    RH_ScopedInstall(Initialise, 0x737B40, { .reversed = false });
    RH_ScopedInstall(Shutdown, 0x737BC0, { .reversed = false });
    RH_ScopedInstall(GetProjectileInfo, 0x737BF0, { .reversed = false });
    RH_ScopedInstall(RemoveNotAdd, 0x737C00, { .reversed = false });
    RH_ScopedInstall(AddProjectile, 0x737C80, { .reversed = false });
    RH_ScopedInstall(RemoveDetonatorProjectiles, 0x738860, { .reversed = false });
    RH_ScopedInstall(RemoveProjectile, 0x7388F0, { .reversed = false });
    RH_ScopedInstall(Update, 0x738B20, { .reversed = false });
    RH_ScopedInstall(IsProjectileInRange, 0x739860, { .reversed = false });
    RH_ScopedInstall(RemoveAllProjectiles, 0x7399B0, { .reversed = false });
    RH_ScopedInstall(RemoveIfThisIsAProjectile, 0x739A40, { .reversed = false });
    RH_ScopedInstall(RemoveFXSystem, 0x737B80, { .reversed = false });
}

// 0x737B40
void CProjectileInfo::Initialise() {
    plugin::Call<0x737B40>();
}

// 0x737BC0
void CProjectileInfo::Shutdown() {
    plugin::Call<0x737BC0>();
}

// 0x737BF0
CProjectileInfo* CProjectileInfo::GetProjectileInfo(int32 infoId) {
    return plugin::CallAndReturn<CProjectileInfo*, 0x737BF0, int32>(infoId);
}

// 0x737C00
void CProjectileInfo::RemoveNotAdd(CEntity* creator, eWeaponType weaponType, CVector pos) {
    plugin::Call<0x737C00, CEntity*, eWeaponType, CVector>(creator, weaponType, pos);
}

// 0x737C80
bool CProjectileInfo::AddProjectile(CEntity* creator, eWeaponType projectileType, CVector origin, float force, const CVector* dir, CEntity* target) {
    return plugin::CallAndReturn<bool, 0x737C80>(creator, projectileType, origin, force, dir, target);
}

// 0x738860
void CProjectileInfo::RemoveDetonatorProjectiles() {
    plugin::Call<0x738860>();
}

// 0x7388F0
void CProjectileInfo::RemoveProjectile(CProjectileInfo* info, CProjectile* object) {
    plugin::Call<0x7388F0, CProjectileInfo*, CProjectile*>(info, object);
}

// 0x738B20
void CProjectileInfo::Update() {
    return plugin::Call<0x738B20>();
}

// 0x739860
bool CProjectileInfo::IsProjectileInRange(float x1, float x2, float y1, float y2, float z1, float z2, bool bDestroy) {
    return plugin::CallAndReturn<bool, 0x739860, float, float, float, float, float, float, bool>(x1, x2, y1, y2, z1, z2, bDestroy);
}

// 0x7399B0
void CProjectileInfo::RemoveAllProjectiles() {
    plugin::Call<0x7399B0>();
}

// 0x739A40
bool CProjectileInfo::RemoveIfThisIsAProjectile(CObject* object) {
    return plugin::CallAndReturn<bool, 0x739A40, CObject*>(object);
}

// 0x737B80
void CProjectileInfo::RemoveFXSystem(bool bInstantly) {
    plugin::CallMethod<0x737B80, CProjectileInfo*, bool>(this, bInstantly);
}
