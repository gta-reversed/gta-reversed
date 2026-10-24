#include "StdInc.h"

#include "ProjectileInfo.h"

#include "Entity/Object/Projectile.h"
#include "Radar.h"
#include "World.h"
#include "Pools/Pools.h"
#include "Collision/Box.h"
#include "Explosion.h"
#include "Fx/FxSystem.h"
#include "Audio/AudioEngine.h"
#include "Entity/Vehicle/Vehicle.h"

void CProjectileInfo::InjectHooks() {
    RH_ScopedClass(CProjectileInfo);
    RH_ScopedCategoryGlobal();

    // Install("CProjectileInfo", "", , &CProjectileInfo::);
    RH_ScopedInstall(Initialise, 0x737B40, { .Reversed = false });
    RH_ScopedInstall(Shutdown, 0x737BC0, { .Reversed = false });
    RH_ScopedInstall(GetProjectileInfo, 0x737BF0, { .Reversed = false });
    RH_ScopedInstall(RemoveNotAdd, 0x737C00, { .Reversed = false });
    RH_ScopedInstall(AddProjectile, 0x737C80, { .Reversed = false });
    RH_ScopedInstall(RemoveDetonatorProjectiles, 0x738860, { .Reversed = false });
    RH_ScopedInstall(RemoveProjectile, 0x7388F0);
    RH_ScopedInstall(Update, 0x738B20, { .Reversed = false });
    RH_ScopedInstall(IsProjectileInRange, 0x739860);
    RH_ScopedInstall(RemoveAllProjectiles, 0x7399B0, { .Reversed = false });
    RH_ScopedInstall(RemoveIfThisIsAProjectile, 0x739A40, { .Reversed = false });
    RH_ScopedInstall(RemoveFXSystem, 0x737B80, { .Reversed = false });
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
void CProjectileInfo::RemoveProjectile(CProjectileInfo* info, CProjectile* projectile) {
    const auto  pos     = projectile->GetPosition();
    auto        creator = info->m_pCreator;
    const auto& type    = info->m_nWeaponType;

    eExplosionType explosionType{};
    bool bExplode = true;
    bool reportObjectDestruction{};
    switch (type) {
    case WEAPON_GRENADE:
        explosionType = EXPLOSION_GRENADE;
        break;
    case WEAPON_MOLOTOV:
        explosionType = EXPLOSION_MOLOTOV;
        reportObjectDestruction = true;
        break;
    case WEAPON_ROCKET:
        explosionType = EXPLOSION_ROCKET;
        if (creator && creator->GetType() == ENTITY_TYPE_VEHICLE) {
            creator = static_cast<CVehicle*>(creator)->m_pDriver; // The driver takes the blame (+0x460)
        }
        break;
    case WEAPON_ROCKET_HS: {
        explosionType = creator == FindPlayerPed(-1) ? EXPLOSION_ROCKET : EXPLOSION_WEAK_ROCKET;
        break;
    }
    case WEAPON_FREEFALL_BOMB:
        explosionType = EXPLOSION_GRENADE;
        break;
    default:
        bExplode = false; // Unknown weapon type: no explosion, but still removed (original: 0x738AB6)
        break;
    }

    if (bExplode) {
        CExplosion::AddExplosion(nullptr, creator, explosionType, pos, 0, true, -1.0f, false); // 0x736A50
    }

    if (reportObjectDestruction) {
        AudioEngine.ReportObjectDestruction(projectile);
    }

    info->m_bActive = false;
    if (info->m_pFxSystem) {
        info->m_pFxSystem->Kill();
        info->m_pFxSystem = nullptr;
    }

    CRadar::ClearBlipForEntity(BLIP_OBJECT, GetObjectPool()->GetRef(projectile)); // 0x587C60
    CWorld::Remove(projectile); // 0x563280
    delete projectile;          // Original: vtable scalar-deleting-dtor(1)
}

// 0x738B20
void CProjectileInfo::Update() {
    return plugin::Call<0x738B20>();
}

// 0x739860
bool CProjectileInfo::IsProjectileInRange(float x1, float x2, float y1, float y2, float z1, float z2, bool bDestroy) {
    const CBox bb{
        CVector{ x1, y1, z1 },
        CVector{ x2, y2, z2 }
    };
    bool found = false;
    for (auto&& [info, proj] : rngv::zip(gaProjectileInfo, ms_apProjectile)) {
        if (!info.m_bActive) {
            continue;
        }

        if (!IsWeaponTypeProjectile(static_cast<eWeaponType>(info.m_nWeaponType))) {
            continue;
        }

        if (!bb.IsPointInside(proj->GetPosition())) {
            continue;
        }

        found = true;
        if (bDestroy) {
            info.m_bActive = false;
            info.RemoveFXSystem(false);
            CRadar::ClearBlipForEntity(BLIP_OBJECT, GetObjectPool()->GetRef(proj));
            CWorld::Remove(proj);
            delete proj;
        }
    }
    return found;
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
