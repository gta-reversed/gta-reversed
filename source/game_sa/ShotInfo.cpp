#include "StdInc.h"
#include "ShotInfo.h"
#include <algorithm>

constexpr auto      MAX_SHOT_INFOS = 100u;
static inline auto& aShotInfos = StaticRef<std::array<CShotInfo, MAX_SHOT_INFOS>>(0xC89690);

// Originally CShotInfo::ms_afRandTable
static inline std::array<float, 20>& RandTable = StaticRef<std::array<float, 20>>(0xC89628);

void CShotInfo::InjectHooks() {
    RH_ScopedClass(CShotInfo);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x739B60);
    RH_ScopedInstall(Shutdown, 0x739C20);
    RH_ScopedInstall(AddShot, 0x739C30);
    RH_ScopedInstall(GetFlameThrowerShotPosn, 0x739DE0);
    RH_ScopedInstall(Update, 0x739E60);
}

// 0x739B60
void CShotInfo::Initialise() {
    for (auto& shot : aShotInfos) {
        shot.m_vecOrigin.Reset();
        shot.m_vecTargetOffset.Reset();
        shot.m_bExist      = false;
        shot.m_bExecuted   = false;
        shot.m_nWeaponType = WEAPON_PISTOL;
        shot.m_fRange      = 1.0f;
        shot.m_pCreator    = nullptr;
        shot.m_DestroyTime = 0.0f;
    }
    for (auto&& [i, rd] : rngv::enumerate(RandTable)) {
        rd = -0.05f + 0.005f * i;
    }
}

// 0x739C20
void CShotInfo::Shutdown() {
    // NOP
}

// 0x739C30
bool CShotInfo::AddShot(CEntity* creator, eWeaponType weaponType, CVector origin, CVector target) {
    auto shot = rng::find_if(aShotInfos, [](const auto& s) { return !s.m_bExist; });
    if (shot == aShotInfos.end()) {
        NOTSA_LOG_WARN("Shotinfo slots are full!");
        return false;
    }

    const auto weapInfo     = CWeaponInfo::GetWeaponInfo(weaponType);
    shot->m_bExist          = true;
    shot->m_bExecuted       = false;
    shot->m_nWeaponType     = weaponType;
    shot->m_pCreator        = creator;
    shot->m_fRange          = weapInfo->m_fRadius;
    shot->m_vecOrigin       = origin;
    shot->m_vecTargetOffset = target - origin;
    if (weapInfo->m_fSpread != 0.0f) {
        shot->m_vecTargetOffset.x += CGeneral::RandomChoice(RandTable) * weapInfo->m_fSpread;
        shot->m_vecTargetOffset.y += CGeneral::RandomChoice(RandTable) * weapInfo->m_fSpread;
        shot->m_vecTargetOffset.z += CGeneral::RandomChoice(RandTable);
    }
    shot->m_vecTargetOffset.Normalise();

    const auto speed = weapInfo->m_Speed + (weapInfo->flags.bRandSpeed ? CGeneral::RandomChoice(RandTable) : 0.0f);
    shot->m_vecTargetOffset *= speed;

    // TODO: overflow handling?
    const auto now      = (float)(double(CTimer::GetTimeInMS()) + ((CTimer::GetTimeInMS() & 0x80000000) ? 4294967300.0 : 0.0));
    shot->m_DestroyTime = now + weapInfo->m_fLifespan;
    return true;
}

// 0x739DE0
bool CShotInfo::GetFlameThrowerShotPosn(uint8 shotId, CVector& outPos) {
    if (const auto s = aShotInfos[shotId]; s.m_bExist && s.m_nWeaponType == WEAPON_FLAMETHROWER) {
        outPos = s.m_vecOrigin;
        return true;
    }
    outPos.Reset();
    return false;
}

// 0x739E60
void CShotInfo::Update() {
    for (auto&& [i, shot] : rngv::enumerate(aShotInfos)) {
        if (shot.m_pCreator) {
            if (shot.m_pCreator->GetIsTypePed() && !shot.m_pCreator->AsPed()->IsPointerValid()) {
                shot.m_pCreator = nullptr;
            }
        }

        if (!shot.m_bExist) {
            continue;
        }

        auto& weaponInfo = *CWeaponInfo::GetWeaponInfo(shot.m_nWeaponType);
        if (static_cast<float>(CTimer::GetTimeInMS()) > shot.m_DestroyTime) {
            shot.m_bExist = false;
        }

        if (weaponInfo.flags.bSlowdown) {
            shot.m_vecTargetOffset *= pow(0.96f, CTimer::GetTimeStep());
        }

        if (weaponInfo.flags.bRangeIncreasesOverTime) {
            shot.m_fRange += CTimer::GetTimeStep() * 0.0075f;
        }

        shot.m_vecOrigin += shot.m_vecTargetOffset * CTimer::GetTimeStep();

        if (shot.m_pCreator) {
            const auto range = std::max(shot.m_fRange, 1.0f);
            auto* creatorIntel = shot.m_pCreator->AsPed()->GetIntelligence();
            for (auto i = 16; i > 0; i--) {
                // originally OOBs like a motherfucker, which ends up with the closest ped member.
                auto* ped = (i==16) ? creatorIntel->GetPedScanner().GetClosestPedInRange() : creatorIntel->GetPedEntity(i)->AsPed();
                if (!ped || !ped->IsPointerValid() || ped->bInVehicle) {
                    continue;
                }
                if (ped->physicalFlags.bFireProof) {
                    continue;
                }
                if (!ped->IsPedInControl()) {
                    continue;
                }
                if (DistanceBetweenPointsSquared(ped->GetPosition(), shot.m_vecOrigin) >= range) {
                    continue;
                }

                if (shot.m_nWeaponType != eWeaponType::WEAPON_SPRAYCAN && shot.m_nWeaponType != eWeaponType::WEAPON_EXTINGUISHER) {
                    gFireManager.StartFire(ped, shot.m_pCreator, 0.8f, 1, 7'000u, 100);
                    continue;
                }

                if (shot.m_bExecuted) {
                    continue;
                }

                const auto localDirection = ped->GetLocalDirection(shot.m_pCreator->GetPosition() - ped->GetPosition());
                CWeapon::GenerateDamageEvent(
                    ped,
                    shot.m_pCreator,
                    shot.m_nWeaponType,
                    weaponInfo.m_nDamage,
                    ePedPieceTypes::PED_PIECE_TORSO,
                    localDirection
                );
                shot.m_bExecuted = true;
            }
        }

        switch (shot.m_nWeaponType) {
        case eWeaponType::WEAPON_SPRAYCAN: {
            if (shot.m_bExecuted) {
                break;
            }
            CVector sprayDir{};
            const auto sprayState = CWorld::SprayPaintWorld(shot.m_vecOrigin, sprayDir, shot.m_fRange, true);
            if (sprayState != eSprayPaintState::NOT_FOUND) {
                shot.m_bExecuted = true;

                shot.m_vecTargetOffset -= sprayDir * shot.m_vecTargetOffset.Dot(sprayDir);
                shot.m_vecTargetOffset += shot.m_vecTargetOffset;
                if (sprayState == eSprayPaintState::PAINTED && shot.m_pCreator == FindPlayerPed()) {
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
            if ((CTimer::GetFrameCounter() + i) % 4 == 0) {
                CWorld::SetCarsOnFire(shot.m_vecOrigin, 4.f, shot.m_pCreator);
            }
            CWorld::SetWorldOnFire(shot.m_vecOrigin, 0.1f, shot.m_pCreator);
        }
        }

    }
}
