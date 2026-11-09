#include "StdInc.h"

#include "ProjectileInfo.h"

#include "Entity/Object/Projectile.h"
#include "Radar.h"
#include "World.h"
#include "Pools/Pools.h"
#include "Collision/Box.h"
#include "Timer.h"
#include "General.h"
#include "Replay.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Physical.h"
#include "Fx/Fx.h"
#include "Fx/FxSystem.h"
#include "Fx/FxPrtMult.h"
#include "Audio/AudioEngine.h"

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
    RH_ScopedInstall(RemoveProjectile, 0x7388F0, { .Reversed = false });
    RH_ScopedInstall(Update, 0x738B20);
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
void CProjectileInfo::RemoveProjectile(CProjectileInfo* info, CProjectile* object) {
    plugin::Call<0x7388F0, CProjectileInfo*, CProjectile*>(info, object);
}

// 0x738B20
void CProjectileInfo::Update() {
    if (CReplay::Mode == MODE_PLAYBACK) { // DAT_00A43088
        return;
    }

    for (auto i = 0u; i < MAX_PROJECTILE_INFOS; i++) {
        auto&       info       = gaProjectileInfo[i];
        auto* const projectile = ms_apProjectile[i];
        if (!info.m_bActive || !projectile) {
            continue;
        }

        // Kill the fx once the projectile is submerged (original checks physicalFlags bit 0x100)
        if (projectile->physicalFlags.bSubmergedInWater && info.m_pFxSystem) {
            info.m_pFxSystem->Kill();
            info.m_pFxSystem = nullptr;
        }

        // Get rid of dangling creators (original: type == PED && !CPed::IsPointerValid (0x5E4220))
        if (info.m_pCreator
            && info.m_pCreator->GetType() == ENTITY_TYPE_PED
            && !static_cast<CPed*>(info.m_pCreator)->IsPointerValid()
        ) {
            info.m_pCreator = nullptr;
        }

        const auto weaponType = info.m_nWeaponType;

        // Grenade-like projectiles harden once they come to a halt; tear gas chokes nearby peds
        if (weaponType == WEAPON_GRENADE || weaponType == WEAPON_TEARGAS || weaponType == WEAPON_REMOTE_SATCHEL_CHARGE) {
            if (projectile->m_fElasticity > StaticRef<float>(0x858B1C) // 0.1f
                && std::abs(projectile->m_vecMoveSpeed.x) < StaticRef<float>(0x858C28) // 0.05f
                && std::abs(projectile->m_vecMoveSpeed.y) < StaticRef<float>(0x858C28)
                && std::abs(projectile->m_vecMoveSpeed.z) < StaticRef<float>(0x858C28)
            ) {
                projectile->m_fElasticity = 0.03f; // 0x3CF5C28F
            }

            // Choking starts 0x445C (17500) ms before the tear gas is destroyed
            if (weaponType == WEAPON_TEARGAS
                && info.m_nDestroyTime != 0
                && (uint32)(info.m_nDestroyTime - 0x445C) < CTimer::GetTimeInMS()
                && CGeneral::GetRandomNumberInRange(0, 100) < 10 // 0x407180
            ) {
                const auto pos = projectile->GetPosition();
                CWorld::SetPedsChoking(pos.x, pos.y, pos.z, 6.0f, info.m_pCreator); // 0x565800, radius 0x40C00000
            }
        }

        // Rocket / heat seeking missile smoke trail
        if (weaponType == WEAPON_ROCKET || weaponType == WEAPON_ROCKET_HS) {
            const FxPrtMult_c fxMults{ 0.3f, 0.3f, 0.3f, 0.3f, 0.5f, 1.0f, 0.08f }; // FxPrtMult_c ctor (0x4AB290)

            const CVector velPerFrame = projectile->m_vecMoveSpeed * CTimer::GetTimeStep(); // 0xB7CB5C

            // The original converts the scaled velocity magnitude to an integer (x87 ftol).
            const auto numPuffs = std::max(1, static_cast<int32>(std::sqrt(
                (sq(velPerFrame.x) + sq(velPerFrame.z)) + sq(velPerFrame.y)
            )));

            for (auto j = 0; j < numPuffs; j++) {
                auto mults{ fxMults };
                const auto shade = (float)CGeneral::GetRandomNumber() * StaticRef<float>(0x858C7C) * StaticRef<float>(0x858C84) + StaticRef<float>(0x858C84); // * 1/32768 * 0.25f + 0.25f
                mults.m_Color.red   = shade;
                mults.m_Color.green = shade;
                mults.m_Color.blue  = shade;
                mults.m_fLife       = (float)CGeneral::GetRandomNumber() * StaticRef<float>(0x858C7C) * StaticRef<float>(0x858CEC) + StaticRef<float>(0x859018); // * 1/32768 * 0.04f + 0.08f

                // Position trails behind the projectile
                const auto frac = 1.0f - (float)j / (float)numPuffs;
                const CVector particlePos = projectile->GetPosition() - velPerFrame * frac;

                // Random spread
                CVector jitter{
                    ((float)CGeneral::GetRandomNumber() * StaticRef<float>(0x858C7C)) * 2.0f - 1.0f,
                    ((float)CGeneral::GetRandomNumber() * StaticRef<float>(0x858C7C)) * 2.0f - 1.0f,
                    ((float)CGeneral::GetRandomNumber() * StaticRef<float>(0x858C7C)) * 2.0f - 1.0f
                };

                jitter.Normalise();

                CVector dirNorm = projectile->m_vecMoveSpeed;
                dirNorm.Normalise();
                const CVector particleVel = CrossProduct(dirNorm, jitter) * StaticRef<float>(0x858CE8); // * 1.5f

                g_fx.m_SmokeHuge->AddParticle(particlePos, particleVel, 0.0f, mults, -1.0f, 1.2f, 0.6f, false); // FxSystem_c::AddParticle (0x4AA440)
            }
        }

        // Life time expired? (original compares unsigned: (uint)destroyTime < CTimer::GetTimeInMS())
        if (info.m_nDestroyTime != 0 && static_cast<uint32>(info.m_nDestroyTime) < CTimer::GetTimeInMS()) {
            if (weaponType == WEAPON_REMOTE_SATCHEL_CHARGE) {
                // Satchels are only deactivated when the player is out of detonators
                if (info.m_pCreator
                    && info.m_pCreator->GetType() == ENTITY_TYPE_PED
                    && static_cast<CPed*>(info.m_pCreator)->IsPlayer() // 0x5DF8F0
                ) {
                    auto* ped = static_cast<CPed*>(info.m_pCreator);
                    const auto& wep = ped->m_aWeapons[ped->GetWeaponSlot(WEAPON_DETONATOR)]; // 0x5DF200(0x28); m_aWeapons @ +0x5A0
                    if (wep.m_Type != WEAPON_DETONATOR || wep.m_TotalAmmo == 0) {
                        info.m_nDestroyTime = 0; // Deactivate - stays forever now
                    }
                }
            } else {
                RemoveProjectile(&info, projectile); // 0x7388F0
            }
        } else { // Not expired yet
            switch (weaponType) {
            case WEAPON_ROCKET: {
                // Accelerate along the forward row (original: matrix row +0x10)
                projectile->m_vecMoveSpeed += projectile->GetMatrix().GetForward() * (CTimer::GetTimeStep() * StaticRef<float>(0x863984)); // * 0.008f

                if (const auto speed = projectile->m_vecMoveSpeed.Magnitude(); speed > StaticRef<float>(0x872C00)) { // 9.9f
                    projectile->m_vecMoveSpeed *= StaticRef<float>(0x872C00) / speed;
                }
                break; // -> collision check below (LAB_007396BB)
            }
            case WEAPON_FLARE: {
                projectile->m_bUsesCollision = false;
                CWorld::pIgnoreEntity = info.m_pCreator; // 0xB7CD68
                const bool bClear = CWorld::GetIsLineOfSightClear(info.m_vecLastPosn, projectile->GetPosition(), true, true, true, true, false, false, false); // 0x56A490
                projectile->m_bUsesCollision = true;
                CWorld::pIgnoreEntity = nullptr;

                if (!bClear) { // Hit something - stop moving
                    projectile->m_vecMoveSpeed = CVector(0.0f, 0.0f, 0.0f);
                    projectile->SetPosn(info.m_vecLastPosn);
                }
                break;
            }
            case WEAPON_MOLOTOV:
            case WEAPON_FREEFALL_BOMB: {
                const auto pos = projectile->GetPosition();
                CWorld::pIgnoreEntity = info.m_pCreator;
                projectile->m_bUsesCollision = false;

                // Only start colliding/exploding once far enough away from the creator (0x858CA0 == 2.0f)
                const auto creatorDelta = info.m_pCreator ? info.m_vecLastPosn - info.m_pCreator->GetPosition() : CVector{};
                if (!info.m_pCreator
                    || (sq(creatorDelta.x) + sq(creatorDelta.z)) + sq(creatorDelta.y) >= StaticRef<float>(0x858CA0)
                ) { // LAB_007397C7
                    if (projectile->physicalFlags.bOnSolidSurface // Original: physicalFlags bit 0x200
                        || !CWorld::GetIsLineOfSightClear(info.m_vecLastPosn, pos, true, true, true, true, false, false, false)
                    ) {
                        RemoveProjectile(&info, projectile);
                    }
                }

                CWorld::pIgnoreEntity = nullptr;
                projectile->m_bUsesCollision = true;
                break;
            }
            case WEAPON_ROCKET_HS: {
                // Heat seeking steering
                if (info.m_pVictim) {
                    if (info.m_pVictim == FindPlayerVehicle(-1, false)) {
                        AudioEngine.ReportFrontendAudioEvent(AE_MISSILE_LOCK, 0.0f, 1.0f); // 0x506EA0, 0x65
                    }

                    const auto aimingDir  = projectile->GetMatrix().GetForward(); // forward row (+0x10)
                    const auto origin     = projectile->GetPosition() + aimingDir;

                    // Score of the currently locked target (0x73E560)
                    const float victimScore = CWeapon::EvaluateTargetForHeatSeekingMissile(info.m_pVictim, origin, aimingDir, StaticRef<float>(0x858F08), true, nullptr); // 1.2f

                    // Flares are chased too - find the best one
                    CEntity* bestTarget{};
                    float    bestScore{};
                    for (auto j = 0u; j < MAX_PROJECTILE_INFOS; j++) {
                        const auto& otherInfo = gaProjectileInfo[j];
                        if (otherInfo.m_nWeaponType != WEAPON_FLARE || !otherInfo.m_bActive) {
                            continue;
                        }
                        if (const float score = CWeapon::EvaluateTargetForHeatSeekingMissile(ms_apProjectile[j], origin, aimingDir, StaticRef<float>(0x858F08), true, nullptr); score >= bestScore) {
                            bestScore  = score;
                            bestTarget = ms_apProjectile[j];
                        }
                    }

                    CEntity* target;
                    if (!bestTarget || bestScore <= victimScore) {
                        target = info.m_pVictim;
                    } else {
                        target = bestTarget;
                    }

                    // Special handling: local player fired at their own plane
                    const bool bIsLocalPlayerPlane = target->GetType() == ENTITY_TYPE_VEHICLE
                        && (info.m_pCreator == FindPlayerPed(-1) || info.m_pCreator == FindPlayerVehicle(-1, false))
                        && static_cast<CVehicle*>(target)->m_nVehicleSubType == VEHICLE_TYPE_PLANE; // +0x594

                    const auto targetPos = target->GetPosition();

                    CVector predictedPos = projectile->GetPosition() + projectile->m_vecMoveSpeed * StaticRef<float>(0x858628); // * 100.0f
                    if (bIsLocalPlayerPlane) {
                        predictedPos = projectile->GetPosition();
                    }

                    // Desired direction & knock strength
                    const auto targetDelta = projectile->GetPosition() - targetPos;
                    const auto targetDistance = std::sqrt((sq(targetDelta.x) + sq(targetDelta.z)) + sq(targetDelta.y));
                    float knock = std::min(StaticRef<float>(0x858B40), targetDistance); // 50.0f
                    if (bIsLocalPlayerPlane) {
                        knock = std::min(StaticRef<float>(0x858CE8), targetDistance); // 1.5f
                    }

                    CVector toTarget = (targetPos + static_cast<CPhysical*>(target)->m_vecMoveSpeed * knock) - predictedPos;
                    CVector oldVel = projectile->m_vecMoveSpeed;
                    oldVel.Normalise();

                    // Remove the velocity component that goes away from the target
                    if (const float dot = (toTarget.z * oldVel.z + toTarget.y * oldVel.y) + toTarget.x * oldVel.x; dot < StaticRef<float>(0x858B50)) { // 0.0f
                        toTarget -= oldVel * dot;
                    }
                    toTarget.Normalise();

                    float damping = 1.0f;
                    float steer   = StaticRef<float>(0x872BFC); // 0.0117f
                    if (info.m_pCreator != FindPlayerPed(-1) && info.m_pCreator != FindPlayerVehicle(-1, false)) {
                        steer = 0.009f;
                    }
                    if (static_cast<CPhysical*>(target)->m_vecMoveSpeed.Magnitude() > StaticRef<float>(0x858C98)) { // 0.8f
                        steer *= StaticRef<float>(0x858F08); // 1.2f
                    }
                    if (bIsLocalPlayerPlane) {
                        damping = static_cast<float>(std::pow(static_cast<double>(StaticRef<float>(0x8D6104)), static_cast<double>(CTimer::GetTimeStep()))); // 0.95^timestep
                        steer   = StaticRef<float>(0x8D6108);               // 0.15f
                    }

                    projectile->m_vecMoveSpeed *= damping;
                    projectile->m_vecMoveSpeed += toTarget * (steer * CTimer::GetTimeStep());

                    if (const auto speed = projectile->m_vecMoveSpeed.Magnitude(); speed > StaticRef<float>(0x872C00)) {
                        projectile->m_vecMoveSpeed *= StaticRef<float>(0x872C00) / speed;
                    }

                    // Nose follows the normalized previous velocity (original: matrix row +0x10)
                    projectile->GetMatrix().GetForward() = oldVel;
                }
                break; // -> collision check below (LAB_007396BB)
            }
            default: { // WEAPON_GRENADE / WEAPON_TEARGAS / WEAPON_REMOTE_SATCHEL_CHARGE
                if (weaponType == WEAPON_REMOTE_SATCHEL_CHARGE) {
                    // Stick to whatever it damaged (+0xD8 m_fDamageIntensity, +0xDC m_pDamageEntity, +0xFC m_pAttachedTo)
                    if (projectile->m_fDamageIntensity > StaticRef<float>(0x858B50) // 0.0f
                        && projectile->m_pDamageEntity
                        && !projectile->m_pAttachedTo
                    ) {
                        projectile->AttachEntityToEntity(static_cast<CPhysical*>(projectile->m_pDamageEntity), nullptr, nullptr); // 0x54D690
                        projectile->m_bUsesCollision = false; // Original: m_nFlags &= ~1
                    }
                }
                break;
            }
            }

            // Rockets & heat seeking missiles: explode on whatever they hit (LAB_007396BB).
            // bOnSolidSurface (bit 0x200) only skips the LOS test - the collided-entities check
            // below still runs and removes the projectile.
            if (weaponType == WEAPON_ROCKET || weaponType == WEAPON_ROCKET_HS) {
                bool bBlocked = true;
                if (!projectile->physicalFlags.bOnSolidSurface) {
                    CWorld::pIgnoreEntity = info.m_pCreator;
                    projectile->m_bUsesCollision = false;
                    bBlocked = !CWorld::GetIsLineOfSightClear(info.m_vecLastPosn, projectile->GetPosition(), true, true, true, true, false, false, false);
                    projectile->m_bUsesCollision = true;
                    CWorld::pIgnoreEntity = nullptr;
                    projectile->m_pEntityIgnoredCollision = info.m_pCreator;
                }

                if (bBlocked) {
                    if (CEntity* collided{}; // Original: +0xB9 = m_nNumEntitiesCollided, +0xBC = m_apCollidedEntities[0]
                        projectile->m_nNumEntitiesCollided == 0
                        || !(collided = projectile->m_apCollidedEntities[0])
                        || (collided != info.m_pCreator && collided->GetModelIndex() != MODEL_MISSILE) // 0x159
                    ) {
                        RemoveProjectile(&info, projectile);
                    }
                }
            }
        }

        // LAB_00739812 - note: the original reads the (possibly freed) projectile here too
        info.m_vecLastPosn = projectile->GetPosition();
    }
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
