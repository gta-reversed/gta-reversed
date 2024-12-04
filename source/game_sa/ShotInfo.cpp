#include "StdInc.h"
#include "ShotInfo.h"

constexpr auto      MAX_SHOT_INFOS = 100u;
static inline auto& aShotInfos = StaticRef<std::array<CShotInfo, MAX_SHOT_INFOS>>(0xC89690);

void CShotInfo::InjectHooks() {
    RH_ScopedClass(CShotInfo);
    RH_ScopedCategory();

    RH_ScopedInstall(Initialise, 0x739B60, {.reversed = false});
    RH_ScopedInstall(Shutdown, 0x739C20, {.reversed = false});
    RH_ScopedInstall(AddShot, 0x739C30, {.reversed = false});
    RH_ScopedInstall(GetFlameThrowerShotPosn, 0x739DE0, {.reversed = false});
    RH_ScopedInstall(Update, 0x739E60);
}

// 0x739B60
void CShotInfo::Initialise() {
    return plugin::CallAndReturn<void, 0x739B60>();
}

// 0x739C20
void CShotInfo::Shutdown() {
    return plugin::CallAndReturn<void, 0x739C20>();
}

// 0x739C30
bool CShotInfo::AddShot(CEntity* creator, eWeaponType weaponType, CVector origin, CVector target) {
    return plugin::CallAndReturn<bool, 0x739C30, CEntity*, eWeaponType, CVector, CVector>(creator, weaponType, origin, target);
}

// 0x739DE0
bool CShotInfo::GetFlameThrowerShotPosn(uint8 shotId, CVector* pPosn) {
    return plugin::CallAndReturn<bool, 0x739DE0, uint8, CVector*>(shotId, pPosn);
}

// 0x739E60
void CShotInfo::Update() {
    auto iteration = -1;
    for (auto& shot : aShotInfos) {
        iteration++;
        if (shot.m_pCreator) {
            if (shot.m_pCreator->IsPed() && !shot.m_pCreator->AsPed()->IsPointerValid()) {
                shot.m_pCreator = nullptr;
            }
        }

        if (!shot.m_bExist) {
            continue;
        }

        auto  weaponType = static_cast<eWeaponType>(shot.m_nWeaponType);
        auto& weaponInfo = *CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD);
        if (float(CTimer::GetTimeInMS()) > shot.m_DestroyTime) {
            shot.m_bExist = false;
        }

        if (weaponInfo.flags.bExpands) {
            shot.m_vecTargetOffset *= pow(0.96f, CTimer::GetTimeStep());
        }

        if (weaponInfo.flags.bRangeIncreasesOverTime) {
            shot.m_fRange += CTimer::GetTimeStep() * 0.0075f;
        }

        shot.m_vecOrigin += shot.m_vecTargetOffset * CTimer::GetTimeStep();

        if (shot.m_pCreator) {
            auto range = std::max(shot.m_fRange, 1.0f);
            for (auto i = 15; i >= 0; i--) {
                auto* ped = shot.m_pCreator->AsPed()->GetIntelligence()->GetPedEntity(i)->AsPed();
                if (!ped || !ped->IsPointerValid() || ped->bInVehicle) {
                    continue;
                }
                if (ped->physicalFlags.bFireProof) {
                    continue;
                }
                if (!ped->IsPedInControl()) {
                    continue;
                }

                auto& pedPos = ped->GetPosition();
                auto  distFromShotSq = DistanceBetweenPointsSquared(pedPos, shot.m_vecOrigin);
                if (distFromShotSq >= range) {
                    continue;
                }

                if (weaponType != eWeaponType::WEAPON_SPRAYCAN && weaponType != eWeaponType::WEAPON_EXTINGUISHER) {
                    gFireManager.StartFire(ped, shot.m_pCreator, 0.8f, 1, 7'000u, 100);
                    continue;
                }

                if (shot.m_bExecuted) {
                    continue;
                }

                auto&     creatorPos = shot.m_pCreator->GetPosition();
                CVector2D vecDir     = (creatorPos - pedPos);
                auto      localDirection = ped->GetLocalDirection(vecDir);
                CWeapon::GenerateDamageEvent(ped, shot.m_pCreator, weaponType, weaponInfo.m_nDamage, ePedPieceTypes::PED_PIECE_TORSO, localDirection);
                shot.m_bExecuted = true;
            }
        }

        switch (weaponType) {
        case eWeaponType::WEAPON_SPRAYCAN: {
            if (shot.m_bExecuted) {
                break;
            }
            CVector sprayDir{};
            auto    sprayState = CWorld::SprayPaintWorld(shot.m_vecOrigin, sprayDir, shot.m_fRange, true);
            if (sprayState) {
                shot.m_bExecuted = true;
                auto sprayDot    = shot.m_vecTargetOffset.Dot(sprayDir);
                shot.m_vecTargetOffset -= sprayDir * sprayDot;
                shot.m_vecTargetOffset += shot.m_vecTargetOffset;
                if (sprayState == 2 && shot.m_pCreator == FindPlayerPed()) {
                    AudioEngine.ReportFrontendAudioEvent(eAudioEvents::AE_FRONTEND_PART_MISSION_COMPLETE, 0.f, 1.f);
                }
            }
            break;
        }
        case eWeaponType::WEAPON_EXTINGUISHER: {
            if (!shot.m_bExecuted && gFireManager.ExtinguishPointWithWater(shot.m_vecOrigin, shot.m_fRange, 2.f)) {
                shot.m_bExecuted = true;
            }
            break;
        }
        default: {
            if ((iteration + CTimer::GetFrameCounter() % 4) == 0) {
                CWorld::SetCarsOnFire(shot.m_vecOrigin.x, shot.m_vecOrigin.y, shot.m_vecOrigin.z, 4.f, shot.m_pCreator);
            }
            CWorld::SetCarsOnFire(shot.m_vecOrigin.x, shot.m_vecOrigin.y, shot.m_vecOrigin.z, 0.1f, shot.m_pCreator);
        }
        }

    }
}
