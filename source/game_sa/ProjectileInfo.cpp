#include "StdInc.h"

#include "ProjectileInfo.h"

#include "Entity/Object/Projectile.h"
#include "Explosion.h"
#include "AudioEngine.h"
#include "Radar.h"
#include "World.h"
#include "Pools/Pools.h"
#include "Collision/Box.h"

void CProjectileInfo::InjectHooks() {
    RH_ScopedClass(CProjectileInfo);
    RH_ScopedCategoryGlobal();

    // Install("CProjectileInfo", "", , &CProjectileInfo::);
    RH_ScopedInstall(Initialise, 0x737B40);
    RH_ScopedInstall(Shutdown, 0x737BC0);
    RH_ScopedInstall(GetProjectileInfo, 0x737BF0);
    RH_ScopedInstall(RemoveNotAdd, 0x737C00);
    RH_ScopedInstall(AddProjectile, 0x737C80, { .reversed = false });
    RH_ScopedInstall(RemoveDetonatorProjectiles, 0x738860);
    RH_ScopedInstall(RemoveProjectile, 0x7388F0);
    RH_ScopedInstall(Update, 0x738B20, { .reversed = false });
    RH_ScopedInstall(IsProjectileInRange, 0x739860);
    RH_ScopedInstall(RemoveAllProjectiles, 0x7399B0);
    RH_ScopedInstall(RemoveIfThisIsAProjectile, 0x739A40);
    RH_ScopedInstall(RemoveFXSystem, 0x737B80);
}

// 0x737B40
void CProjectileInfo::Initialise() {
    for (auto& proj : ms_apProjectile) {
        proj = nullptr;
    }
    for (auto& info : gaProjectileInfo) {
        info.m_nWeaponType  = WEAPON_GRENADE;
        info.m_pCreator     = nullptr;
        info.m_nDestroyTime = 0;
        info.m_bActive      = false;
        info.m_pVictim      = nullptr;
        info.m_pFxSystem    = nullptr;
    }
}

// 0x737BC0
void CProjectileInfo::Shutdown() {
    for (auto& info : gaProjectileInfo) {
        if (info.m_pFxSystem) {
            g_fxMan.DestroyFxSystem(info.m_pFxSystem);
            info.m_pFxSystem = nullptr;
        }
    }
}

// 0x737BF0
CProjectileInfo* CProjectileInfo::GetProjectileInfo(int32 infoId) {
    return &gaProjectileInfo[infoId];
}

// 0x737C00
void CProjectileInfo::RemoveNotAdd(CEntity* creator, eWeaponType weaponType, CVector pos) {
    eExplosionType explosionType;
    switch (weaponType) {
    case WEAPON_GRENADE:
    case WEAPON_REMOTE_SATCHEL_CHARGE:
        explosionType = EXPLOSION_GRENADE;
        break;
    case WEAPON_MOLOTOV:
        explosionType = EXPLOSION_MOLOTOV;
        break;
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS:
        explosionType = EXPLOSION_ROCKET;
        break;
    default:
        return;
    }
    CExplosion::AddExplosion(nullptr, creator, explosionType, pos, 0, true, -1.0f, false);
}

// 0x737C80
// NOTSA: kept as plugin call - flag stays {.reversed = false}.
// 432-line body dump available (c5_addproj.txt): per-weapon init of launch
// velocity/flags/fuse (grenade/teargas via fpatan+LimitRadianAngle+sin/cos,
// rocket via dir or matrix, satchel/flare/thermal via matrix paths), free-slot
// scan, `new (0x17C via CObject::operator new 0x5A1EE0) CProjectile(modelId)`
// through the 0x5A4030 new-operator trampoline (PUSH -1; JMP new), col-model fixups,
// and world/radar/audio registration. Blockers: unmapped ctor trampoline 0x5A4030
// (not in hooks.csv - needs its own owner/file) and ~15 unidentified float
// constants read live from the exe during this ticket; inventing them as named
// locals would be unfaithful.
bool CProjectileInfo::AddProjectile(CEntity* creator, eWeaponType projectileType, CVector origin, float force, const CVector* dir, CEntity* target) {
    return plugin::CallAndReturn<bool, 0x737C80>(creator, projectileType, origin, force, dir, target);
}

// 0x738860
void CProjectileInfo::RemoveDetonatorProjectiles() {
    for (auto i = 0u; i < MAX_PROJECTILES; i++) {
        auto& info   = gaProjectileInfo[i];
        auto* object = ms_apProjectile[i];
        if (!info.m_bActive || info.m_nWeaponType != WEAPON_REMOTE_SATCHEL_CHARGE) {
            continue;
        }
        CExplosion::AddExplosion(nullptr, info.m_pCreator, EXPLOSION_GRENADE, object->GetPosition(), 0, true, -1.0f, false);
        info.m_bActive = false;
        if (info.m_pFxSystem) {
            info.m_pFxSystem->Kill();
            info.m_pFxSystem = nullptr;
        }
        object->m_bRemoveFromWorld = true;
    }
}

// 0x7388F0
void CProjectileInfo::RemoveProjectile(CProjectileInfo* info, CProjectile* object) {
    const CVector pos = object->GetPosition();
    switch (static_cast<eWeaponType>(info->m_nWeaponType)) {
    case WEAPON_GRENADE:
    case WEAPON_FREEFALL_BOMB:
        CExplosion::AddExplosion(nullptr, info->m_pCreator, EXPLOSION_GRENADE, pos, 0, true, -1.0f, false);
        break;
    case WEAPON_MOLOTOV:
        CExplosion::AddExplosion(nullptr, info->m_pCreator, EXPLOSION_MOLOTOV, pos, 0, true, -1.0f, false);
        AudioEngine.ReportObjectDestruction(object);
        break;
    case WEAPON_ROCKET: {
        CEntity* creator = info->m_pCreator;
        if (creator && creator->GetType() == ENTITY_TYPE_VEHICLE) {
            creator = creator->AsVehicle()->GetDriver();
        }
        CExplosion::AddExplosion(nullptr, creator, EXPLOSION_ROCKET, pos, 0, true, -1.0f, false);
        break;
    }
    case WEAPON_ROCKET_HS:
        if (info->m_pCreator == FindPlayerPed()) {
            CExplosion::AddExplosion(nullptr, info->m_pCreator, EXPLOSION_ROCKET, pos, 0, true, -1.0f, false);
        } else {
            CExplosion::AddExplosion(nullptr, info->m_pCreator, EXPLOSION_WEAK_ROCKET, pos, 0, true, -1.0f, false);
        }
        break;
    default:
        break;
    }
    info->m_bActive = false;
    if (info->m_pFxSystem) {
        info->m_pFxSystem->Kill();
        info->m_pFxSystem = nullptr;
    }
    CRadar::ClearBlipForEntity(BLIP_OBJECT, GetObjectPool()->GetRef(object));
    CWorld::Remove(object);
    delete object;
}

// 0x738B20
// NOTSA: kept as plugin call - flag stays {.reversed = false}.
// 496-line body dump available (tmp_M5_update.c): per-slot update over the 32
// infos (rocket/teargas/grenade fuse handling, heat-seeker steering via
// CWeapon::EvaluateTargetForHeatSeekingMissile + CVector::Normalise (0x59C910),
// CWorld LOS checks with pIgnoreEntity, CWorld::Add/Remove, radar blips).
// Blockers: FUN_00822130 is the CRT sin() wrapper (disasm: MXCSR/FPU-control
// dispatch to sin, used for the flare wobble factor) - no named owner in-repo to
// call; plus ~12 unidentified float constants read live from the exe during this
// ticket; naming them without a source would be unfaithful.
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
    for (auto i = 0u; i < MAX_PROJECTILES; i++) {
        auto& info   = gaProjectileInfo[i];
        auto* object = ms_apProjectile[i];
        if (!info.m_bActive) {
            continue;
        }
        info.m_bActive = false;
        if (info.m_pFxSystem) {
            g_fxMan.DestroyFxSystem(info.m_pFxSystem);
            info.m_pFxSystem = nullptr;
        }
        CRadar::ClearBlipForEntity(BLIP_OBJECT, GetObjectPool()->GetRef(object));
        CWorld::Remove(object);
        delete object;
    }
}

// 0x739A40
bool CProjectileInfo::RemoveIfThisIsAProjectile(CObject* object) {
    for (auto i = 0u; i < MAX_PROJECTILES; i++) {
        if (ms_apProjectile[i] != object || !gaProjectileInfo[i].m_bActive) {
            continue;
        }
        auto& info = gaProjectileInfo[i];
        info.m_bActive = false;
        if (info.m_pFxSystem) {
            info.m_pFxSystem->Kill();
            info.m_pFxSystem = nullptr;
        }
        CRadar::ClearBlipForEntity(BLIP_OBJECT, GetObjectPool()->GetRef(object));
        CWorld::Remove(object);
        delete object;
        ms_apProjectile[i] = nullptr;
        return true;
    }
    return false;
}

// 0x737B80
void CProjectileInfo::RemoveFXSystem(bool bInstantly) {
    if (!m_pFxSystem) {
        return;
    }
    if (bInstantly) {
        g_fxMan.DestroyFxSystem(m_pFxSystem);
    } else {
        m_pFxSystem->Kill();
    }
    m_pFxSystem = nullptr;
}
