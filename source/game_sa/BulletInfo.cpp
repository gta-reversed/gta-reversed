#include "StdInc.h"

#include "BulletInfo.h"

#include "FireManager.h"
#include "AudioEngine.h"
#include "Glass.h"

// Note: This class is only used by CWeapon::FireSniper

CBulletInfo (&CBulletInfo::aBulletInfos)[8] = *(CBulletInfo(*)[8])0xC88740;
CVector& CBulletInfo::PlayerSniperBulletStart = *(CVector*)0xC888A0;
CVector& CBulletInfo::PlayerSniperBulletEnd = *(CVector*)0xC888AC;

void CBulletInfo::InjectHooks() {
    RH_ScopedClass(CBulletInfo);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x735FD0);
    RH_ScopedInstall(Shutdown, 0x736000);
    RH_ScopedInstall(AddBullet, 0x736010);
    RH_ScopedInstall(Update, 0x7360D0);
}

// 0x735FD0
void CBulletInfo::Initialise() {
    for (auto& info : aBulletInfos) {
        info.m_bExists = 0;
        info.m_nWeaponType = WEAPON_PISTOL;
        info.m_nDestroyTime = 0.0f;
        info.m_pCreator = 0;
    }
}

// 0x736000
void CBulletInfo::Shutdown() {
    // NOP
}

CBulletInfo* CBulletInfo::GetFree() {
    for (auto& info : aBulletInfos) {
        if (!info.m_bExists) {
            return &info;
        }
    }
    return nullptr;
}

bool CBulletInfo::IsTimeToBeDestroyed() const noexcept {
    return (float)CTimer::GetTimeInMS() > m_nDestroyTime;
}

// 0x736010
void CBulletInfo::AddBullet(CEntity* creator, eWeaponType weaponType, CVector posn, CVector velocity) {
    if (auto info = GetFree()) {
        info->m_pCreator     = creator;
        info->m_nWeaponType  = weaponType;
        info->m_nDamage      = CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD)->m_nDamage;
        info->m_vecPosition  = posn;
        info->m_vecVelocity  = velocity;
        info->m_nDestroyTime = (float)(CTimer::GetTimeInMS() + 1000);
        info->m_bExists      = true;
    }
}

// 0x7360D0
void CBulletInfo::Update() {
    for (auto& info : aBulletInfos) {
        if (info.IsTimeToBeDestroyed())
            info.m_bExists = false; /* next line checks */
        if (!info.m_bExists)
            continue;

        CVector newPosition = info.m_vecPosition + info.m_vecVelocity * (CTimer::GetTimeStep() / 2.0f);
        if (!CWorld::IsInWorldBounds(newPosition)) {
            info.m_bExists = false;
            continue;
        }

        CWorld::bIncludeDeadPeds = true;
        CWorld::bIncludeCarTyres = true;
        CWorld::bIncludeBikers   = true;
        CWorld::pIgnoreEntity    = info.m_pCreator;

        CColPoint colPoint;
        CEntity* hitEntity;
        if (CWorld::ProcessLineOfSight(info.m_vecPosition, newPosition, colPoint, hitEntity, true, true, true, true, true, false, false, true)) {
            CWeapon::CheckForShootingVehicleOccupant(&hitEntity, &colPoint, info.m_nWeaponType, info.m_vecPosition, newPosition);

            switch (hitEntity->m_nType) {
            case ENTITY_TYPE_PED: {
                auto hitPed = hitEntity->AsPed();

                if (hitEntity != info.m_pCreator) {
                    if (hitPed->IsAlive()) {
                        CWeapon::GenerateDamageEvent(
                            hitPed,
                            info.m_pCreator,
                            info.m_nWeaponType,
                            info.m_nDamage,
                            (ePedPieceTypes)colPoint.m_nPieceTypeB,
                            hitPed->GetLocalDirection(hitPed->GetPosition2D() - CVector2D{ colPoint.m_vecPoint })
                        );
                        CCrime::ReportCrime(
                            (hitPed->m_nPedType == PED_TYPE_COP) ? eCrimeType::CRIME_DAMAGE_COP_CAR : eCrimeType::CRIME_DAMAGE_CAR,
                            hitPed,
                            info.m_pCreator->AsPed()
                        );
                        newPosition = colPoint.m_vecPoint;
                    }
                }

                if (CLocalisation::Blood()) {
                    g_fx.AddBlood(colPoint.m_vecPoint, colPoint.m_vecNormal, 8, hitPed->m_fContactSurfaceBrightness);
                    // std::cout << "Create blood\n";
                    if (hitPed->m_nPedState == PEDSTATE_DEAD) {
                        const auto anim = RpAnimBlendClumpGetFirstAssociation(hitPed->m_pRwClump, ANIMATION_800) ? ANIM_ID_FLOOR_HIT_F : ANIM_ID_FLOOR_HIT;

                        if (auto assoc = CAnimManager::BlendAnimation(hitPed->m_pRwClump, ANIM_GROUP_DEFAULT, anim, 8.0f)) {
                            assoc->SetCurrentTime(0.0f);
                            assoc->SetFlag(ANIMATION_IS_FINISH_AUTO_REMOVE, false);
                            // std::cout << "Blood anim\n";
                        }
                    }
                    newPosition = colPoint.m_vecPoint;
                }
                break;
            }
            case ENTITY_TYPE_VEHICLE: {
                // std::cout << "Hit vehicle\n";
                auto hitVehicle = hitEntity->AsVehicle();

                if (info.m_pCreator && info.m_pCreator->IsPed())
                    if (info.m_pCreator->AsPed()->m_pAttachedTo == hitVehicle)
                        break;

                // Originally: if (colPoint.m_nPieceTypeB < 13u || colPoint.m_nPieceTypeB > 16u)
                switch ((eCarPiece)colPoint.m_nPieceTypeB) {
                default: /* originally `if` body */
                {
                    hitVehicle->InflictDamage(info.m_pCreator, info.m_nWeaponType, info.m_nDamage, colPoint.m_vecPoint);
                    if (info.m_nWeaponType == eWeaponType::WEAPON_FLAMETHROWER) {
                        gFireManager.StartFire(hitVehicle, info.m_pCreator, 0.8f, true, 7000, 100);
                    } else if (TheCamera.IsSphereVisible(colPoint.m_vecNormal, 1.0f)) {
                        g_fx.AddBulletImpact(colPoint.m_vecPoint, colPoint.m_vecNormal, colPoint.m_nSurfaceTypeB, 8, colPoint.m_nLightingB.GetCurrentLighting());
                    }
                    break;
                }
                case eCarPiece::CAR_PIECE_WHEEL_LF:
                case eCarPiece::CAR_PIECE_WHEEL_RF:
                case eCarPiece::CAR_PIECE_WHEEL_RL:
                case eCarPiece::CAR_PIECE_WHEEL_RR: { /* originally `else` body */
                    hitVehicle->BurstTyre(colPoint.m_nPieceTypeB, true);
                    g_fx.AddTyreBurst(colPoint.m_vecPoint, colPoint.m_vecNormal);
                    // std::cout << "Brust vehicle tyre " << (int)colPoint.m_nPieceTypeB << "\n";
                    break;
                }
                }

                break;
            }
            default: {
                if (TheCamera.IsSphereVisible(colPoint.m_vecNormal, 1.0f))
                    g_fx.AddBulletImpact(colPoint.m_vecPoint, colPoint.m_vecNormal, colPoint.m_nSurfaceTypeB, 8, colPoint.m_nLightingB.GetCurrentLighting());

                if (info.m_pCreator && info.m_pCreator->IsPed())
                    if (info.m_pCreator->AsPed()->m_pAttachedTo == hitEntity)
                        break;

                switch (hitEntity->m_nType) {
                case ENTITY_TYPE_OBJECT: {
                    // std::cout << "Hit object\n";
                    auto hitObject = hitEntity->AsObject();

                    const auto DoDamageToObject = [&](float dmg) {
                        hitObject->ObjectDamage(dmg, &colPoint.m_vecPoint, &colPoint.m_vecNormal, info.m_pCreator, info.m_nWeaponType);
                    };

                    if (hitObject->m_nColDamageEffect < 200u) {
                        if (hitObject->physicalFlags.bDisableCollisionForce || hitObject->m_pObjectInfo->m_fColDamageMultiplier >= 99.9f) {
                            /* empty */
                        } else {
                            if (hitObject->IsStatic() && hitObject->m_pObjectInfo->m_fUprootLimit <= 0.0f) {
                                hitObject->SetIsStatic(false);
                                hitObject->AddToMovingList();
                            }
                            if (!hitObject->IsStatic()) {
                                hitObject->ApplyMoveForce(colPoint.m_vecNormal * -7.5f);
                            }
                        }
                    } else {
                        switch (hitObject->m_pObjectInfo->m_nGunBreakMode) {
                        case eObjectBreakMode::BY_GUN: {
                            DoDamageToObject(151.0f);
                            break;
                        }
                        case eObjectBreakMode::SMASHABLE: {
                            DoDamageToObject(hitObject->m_pObjectInfo->m_fSmashMultiplier * 151.0f);
                            break;
                        }
                        }
                    }
                    DoDamageToObject(50.0f);
                    break;
                }
                case ENTITY_TYPE_BUILDING: {
                    // std::cout << "Hit building\n";
                    if (info.m_pCreator && info.m_pCreator->IsPed()) {
                        if (auto playerData = info.m_pCreator->AsPed()->m_pPlayerData) {
                            playerData->m_nModelIndexOfLastBuildingShot = hitEntity->m_nModelIndex;
                        }
                    }
                    break;
                }
                }
            }
            }

            if (info.m_nWeaponType == eWeaponType::WEAPON_SNIPERRIFLE) {
                // std::cout << "Register sniper bullet hit\n";
                CVector dir = newPosition - info.m_vecPosition;
                dir.Normalise();
                const float dirDotColPointNorm = DotProduct(dir, colPoint.m_vecNormal);
                if (dirDotColPointNorm < 0.0f) {
                    AudioEngine.ReportBulletHit(hitEntity, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, RadiansToDegrees(asin(-dirDotColPointNorm)));
                }
            }
            CGlass::WasGlassHitByBullet(hitEntity, colPoint.m_vecPoint);
        }

        CWorld::bIncludeDeadPeds = false;
        CWorld::bIncludeCarTyres = false;
        CWorld::bIncludeBikers   = false;
        CWorld::pIgnoreEntity    = nullptr;

        if (info.m_nWeaponType == eWeaponType::WEAPON_SNIPERRIFLE) {
            PlayerSniperBulletStart = info.m_vecPosition;
            PlayerSniperBulletEnd = newPosition;
            CHeli::TestSniperCollision(&PlayerSniperBulletStart, &PlayerSniperBulletEnd);
        }

        info.m_vecPosition = newPosition;
    }
}
