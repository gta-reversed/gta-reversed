#include "StdInc.h"

#include "Explosion.h"
#include "CreepingFire.h"
#include "FireManager.h"
#include "InterestingEvents.h"
#include "Shadows.h"

CAEExplosionAudioEntity& CExplosion::m_ExplosionAudioEntity = *(CAEExplosionAudioEntity*)0xC888D0;
CExplosion (&CExplosion::aExplosions)[16] = *(CExplosion(*)[16])0xC88950;

void CExplosion::InjectHooks() {
    RH_ScopedClass(CExplosion);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ClearAllExplosions, 0x736840);
    RH_ScopedInstall(Shutdown, 0x7368F0);
    RH_ScopedInstall(GetExplosionActiveCounter, 0x736900);
    RH_ScopedInstall(ResetExplosionActiveCounter, 0x736910);
    RH_ScopedInstall(DoesExplosionMakeSound, 0x736920);
    RH_ScopedInstall(GetExplosionType, 0x736930);
    RH_ScopedInstall(GetExplosionPosition, 0x736940);
    RH_ScopedInstall(TestForExplosionInArea, 0x736950);
    RH_ScopedInstall(RemoveAllExplosionsInArea, 0x7369E0);
    RH_ScopedInstall(Initialise, 0x736A40);
    RH_ScopedInstall(AddExplosion, 0x736A50);
    RH_ScopedInstall(Update, 0x737620);
}

// 0x736A40
void CExplosion::Initialise() {
    ClearAllExplosions();
}

// 0x7368F0
void CExplosion::Shutdown() {
    // NOP
}

// 0x736840
void CExplosion::ClearAllExplosions() {
    for (auto& exp : aExplosions) {
        exp.m_vecPosition = CVector{0.0f, 0.0f, 0.0f};
        exp.m_nType = EXPLOSION_GRENADE;
        exp.m_fRadius = 1.0f;
        exp.m_fVisibleDistance = 0.0f;
        exp.m_fPropagationRate = 0.0f;
        exp.m_fGroundZ = 0.0f;
        exp.m_pCreator = nullptr;
        exp.m_pVictim = nullptr;
        exp.m_nExpireTime = 0.0f;
        exp.m_nActiveCounter = 0;
        exp.m_nCreatedTime = 0.0f;
        exp.m_bMakeSound = true;
        exp.m_nFuelTimer = 0;

        for (auto i = 0; i < NUM_FUEL; i++) {
            exp.m_vecFuelDirection[i] = CVector{0.0f, 0.0f, 0.0f};
            exp.m_fFuelOffsetDistance[i] = 0.0f;
            exp.m_fFuelSpeed[i] = 0.0f;
        }
    }
}

uint8 CExplosion::GetExplosionActiveCounter(uint8 id) {
    return aExplosions[id].m_nActiveCounter;
}

// 0x736910
void CExplosion::ResetExplosionActiveCounter(uint8 id) {
    aExplosions[id].m_nActiveCounter = 0;
}

// 0x736920
bool CExplosion::DoesExplosionMakeSound(uint8 id) {
    return aExplosions[id].m_bMakeSound;
}

// 0x736930
int32 CExplosion::GetExplosionType(uint8 id) {
    return aExplosions[id].m_nType;
}

// 0x736940
const CVector& CExplosion::GetExplosionPosition(uint8 id) {
    return aExplosions[id].m_vecPosition;
}

// 0x736950
bool CExplosion::TestForExplosionInArea(eExplosionType type, float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    for (auto& exp : aExplosions) {
        if (!exp.m_nActiveCounter)
            continue;

        if (exp.m_nType != type && type != eExplosionType::EXPLOSION_UNDEFINED)
            continue;

        const CBoundingBox boundingBox{{ minX, minY, minZ }, { maxX, maxY, maxZ }};
        if (boundingBox.IsPointWithin(exp.m_vecPosition))
            return true;
    }
    return false;
}

// 0x7369E0
void CExplosion::RemoveAllExplosionsInArea(CVector pos, float radius) {
    for (auto& exp : aExplosions) {
        if (!exp.m_nActiveCounter)
            continue;

        if (DistanceBetweenPointsSquared(exp.m_vecPosition, pos) < sq(radius)) {
            exp.m_nActiveCounter = 0;
        }
    }
}

// NOTSA
CExplosion* CExplosion::GetFree() {
    for (auto& exp : aExplosions) {
        if (!exp.m_nActiveCounter)
            return &exp;
    }
    return nullptr;
}

// NOTSA
void CExplosion::SetCreator(CEntity* newCreator) noexcept {
    CEntity::SafeCleanUpRef(m_pCreator);
    CEntity::SafeRegisterRef(newCreator);
    m_pCreator = newCreator;
}

// NOTSA
void CExplosion::SetVictim(CEntity* newVictim) noexcept {
    CEntity::SafeCleanUpRef(m_pVictim);
    CEntity::SafeRegisterRef(newVictim);
    m_pVictim = newVictim;
}

bool DoesNeedToVehProcessBombTimer(eExplosionType type) {
    switch (type) {
    case eExplosionType::EXPLOSION_ROCKET:
    case eExplosionType::EXPLOSION_QUICK_CAR:
    case eExplosionType::EXPLOSION_MINE:
    case eExplosionType::EXPLOSION_OBJECT:
    case eExplosionType::EXPLOSION_TANK_FIRE:
        return true;
    }
    return false;
};

// 0x736A50
void CExplosion::AddExplosion(CEntity* victim, CEntity* creator, eExplosionType type, CVector pos, uint32 lifetime, uint8 usesSound, float cameraShake, uint8 bInvisible) {
    if (FindPlayerPed() == creator) {
        auto& info = FindPlayerInfo();
        info.m_nHavocCaused += 5;
        info.m_fCurrentChaseValue += 7.0f;
    }

    auto exp = GetFree();
    if (!exp)
        return;

    exp->m_vecPosition = pos;
    exp->m_nType = type;
    exp->m_fRadius = 1.0f;
    exp->m_fVisibleDistance = 0.0f;
    exp->m_fGroundZ = 0.0f;
    exp->m_fDamagePercentage = 1.0f;
    exp->m_nActiveCounter = 1;
    exp->m_bMakeSound = usesSound;
    exp->m_nFuelTimer = 0;

    exp->SetCreator(creator);
    exp->SetVictim(victim);

    for (auto i = 0; i < NUM_FUEL; i++) {
        float& fOffsetDistance = exp->m_fFuelOffsetDistance[i];
        float& fFuelSpeed = exp->m_fFuelSpeed[i];
        CVector& vecFuelDir = exp->m_vecFuelDirection[i];

        if (i && CGeneral::GetRandomNumber() >= RAND_MAX / 2) {
            fOffsetDistance = 0.0f;
        } else {
            vecFuelDir = CVector{
                2 * CGeneral::GetRandomNumberInRange(0.0f, 1.0f) - 1.0f,
                2 * CGeneral::GetRandomNumberInRange(0.0f, 1.0f) - 1.0f,
                    CGeneral::GetRandomNumberInRange(0.2f, 1.0f)
            };
            fOffsetDistance = CGeneral::GetRandomNumberInRange(0.5f, 2.0f);
            fFuelSpeed = CGeneral::GetRandomNumberInRange(20.0f, 30.0f);
        }
    }

    exp->m_nParticlesExpireTime = lifetime ? CTimer::GetTimeInMS() + lifetime : 0;

    const auto PlaySoundIfEnabled = [&]() {
        if (exp->m_bMakeSound) {
            m_ExplosionAudioEntity.AddAudioEvent(eAudioEvents::AE_EXPLOSION, exp->m_vecPosition, .0f);
        }
    };

    // Originally most likely a separate function
    // but this way its nicer
    const auto CreateAndPlayFxWithSound = [&](const char* name) {
        FxSystem_c* fx{nullptr};
        if (exp->m_pVictim) {
            if (exp->m_pVictim->m_pRwObject) {
                if (RwMatrix* matrix = exp->m_pVictim->GetModellingMatrix()) {
                    CVector expToVictimDir = pos - exp->m_pVictim->GetPosition();
                    fx = g_fxMan.CreateFxSystem(name, expToVictimDir, matrix, false);
                }
            }
        } else {
            fx = g_fxMan.CreateFxSystem(name, exp->m_vecPosition, nullptr, false);
        }
        if (fx) {
            PlaySoundIfEnabled();
            fx->PlayAndKill();
        }
    };

    if (bInvisible) {
        exp->m_fRadius = 0.0f;
        exp->m_fVisibleDistance = 0.0f;
    }

    bool bNoFire = false;
    switch (type) {
    case eExplosionType::EXPLOSION_GRENADE: {
        if (!bInvisible) {
            exp->m_fRadius = 9.0f;
            exp->m_fVisibleDistance = 300.0f;
        }
        exp->m_nExpireTime = (float)(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;
        if (exp->m_pVictim) {

        } else {
            CreateAndPlayFxWithSound("explosion_small");
        }
        break;
    }
    case eExplosionType::EXPLOSION_MOLOTOV: {
        if (!bInvisible) {
            exp->m_fRadius = 6.0f;
        }
        exp->m_nExpireTime = (float)(CTimer::GetTimeInMS() + lifetime + 3000);

        bool bHit = false;
        const float fGroundPos = CWorld::FindGroundZFor3DCoord({pos.x, pos.y, pos.z + 3.0f}, &bHit, nullptr);
        if (bHit)
            pos.z = fGroundPos;

        float fWaterLevel{};
        if (CWaterLevel::GetWaterLevelNoWaves(pos, &fWaterLevel, nullptr, nullptr)) {
            /* must be done like this because of call order */
            if (pos.z < fWaterLevel) {
                bNoFire = true;
                break;
            }
        }

        CreateAndPlayFxWithSound("explosion_molotov");

        break;
    }
    case eExplosionType::EXPLOSION_ROCKET:
    case eExplosionType::EXPLOSION_WEAK_ROCKET: {
        if (!bInvisible) {
            exp->m_fRadius = 10.0f;
            exp->m_fVisibleDistance = 300.0f;
        }
        if (type == eExplosionType::EXPLOSION_WEAK_ROCKET) {
            exp->m_fVisibleDistance = 200.0f;
            exp->m_fDamagePercentage = 0.2f;
        }
        exp->m_nExpireTime = (float)(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;

        CreateAndPlayFxWithSound("explosion_small");
        break;
    }
    case eExplosionType::EXPLOSION_CAR:
    case eExplosionType::EXPLOSION_QUICK_CAR: {
        if (!bInvisible) {
            exp->m_fRadius = 9.0f;
            exp->m_fVisibleDistance = 300.0f;
        }
        exp->m_nExpireTime = (float)(CTimer::GetTimeInMS() + lifetime + 4250);
        exp->m_fPropagationRate = 0.5f;
        exp->m_nCreatedTime = (float)CTimer::GetTimeInMS();

        if (exp->m_pVictim) {
            CCrime::ReportCrime(eCrimeType::CRIME_EXPLOSION, exp->m_pVictim->AsPed(), nullptr); /* won't do anything as second ped is nullptr */
        }
        CreateAndPlayFxWithSound("explosion_medium");
        break;
    }
    case eExplosionType::EXPLOSION_BOAT:
    case eExplosionType::EXPLOSION_AIRCRAFT: {
        if (!bInvisible) {
            exp->m_fRadius = 25.0f;
            exp->m_fVisibleDistance = 600.0f;
        }
        exp->m_nExpireTime = (float)(CTimer::GetTimeInMS() + lifetime + 3000);
        exp->m_nCreatedTime = (float)CTimer::GetTimeInMS();

        CreateAndPlayFxWithSound("explosion_large");
        break;
    }
    case eExplosionType::EXPLOSION_MINE: {
        if (!bInvisible) {
            exp->m_fRadius = 10.0f;
            exp->m_fVisibleDistance = 150.0f;
        }
        exp->m_nExpireTime = (float)(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;

        PlaySoundIfEnabled();
        /* No fx for this */
        break;
    }
    case eExplosionType::EXPLOSION_OBJECT: {
        if (!bInvisible) {
            exp->m_fRadius = 10.0f;
            exp->m_fVisibleDistance = 150.0f;
        }
        exp->m_nExpireTime = (float)(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;
        if (exp->m_bMakeSound)
            m_ExplosionAudioEntity.AddAudioEvent(AE_EXPLOSION, exp->m_vecPosition, 0.0f);
        break;
    }
    case eExplosionType::EXPLOSION_TANK_FIRE: {
        if (!bInvisible) {
            exp->m_fRadius = 10.0f;
            exp->m_fVisibleDistance = 150.0f;
        }
        exp->m_nExpireTime = (float)(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;

        CreateAndPlayFxWithSound("explosion_large");
        break;
    }
    case eExplosionType::EXPLOSION_SMALL: {
        if (!bInvisible) {
            exp->m_fRadius = 3.0f;
            exp->m_fVisibleDistance = 90.0f;
        }
        exp->m_nExpireTime = (float)(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;

        CreateAndPlayFxWithSound("explosion_small");
        break;
    }
    case eExplosionType::EXPLOSION_RC_VEHICLE: {
        if (!bInvisible) {
            exp->m_fRadius = 3.0f;
            exp->m_fVisibleDistance = 90.0f;
        }
        exp->m_nExpireTime = (float)(CTimer::GetTimeInMS() + lifetime + 750);
        exp->m_fPropagationRate = 0.5f;

        CreateAndPlayFxWithSound("explosion_tiny");
        break;
    }
    }

    if (!bNoFire) {
        switch (type) {
        case eExplosionType::EXPLOSION_MOLOTOV:
        case eExplosionType::EXPLOSION_ROCKET:
        case eExplosionType::EXPLOSION_WEAK_ROCKET:
        case eExplosionType::EXPLOSION_OBJECT: {
            const auto numFires = (type == eExplosionType::EXPLOSION_MOLOTOV) ? (CGeneral::GetRandomNumber() - 2) % 4 : (CGeneral::GetRandomNumber() + 1) % 4;

            if (numFires) {
                for (auto i = 0; i < numFires; i++) {
                    CVector firePos = exp->m_vecPosition + CVector{CGeneral::GetRandomNumberInRange(-4.0f, 4.0f), CGeneral::GetRandomNumberInRange(-4.0f, 4.0f), 0.0f};
                    bool bHitGround{};
                    firePos.z = CWorld::FindGroundZFor3DCoord({firePos.x, firePos.y, firePos.z + 3.0f}, &bHitGround, nullptr); // 0x73735C
                    if (bHitGround && std::fabs(firePos.z - exp->m_vecPosition.z) < 10.0f) {
                        gFireManager.StartFire(firePos, 0.8f, 0, exp->m_pCreator, (uint32)(CGeneral::GetRandomNumberInRange(5'600.0f, 12'600.0f) * 0.4f), 3, 1);
                    }
                }
                if (creator && creator->IsPed() && creator->AsPed()->IsPlayer()) {
                    CStats::IncrementStat(eStats::STAT_FIRES_STARTED, 1.0f);
                }
            }
            break;
        }
        }
    }

    if (victim)
        g_InterestingEvents.Add(CInterestingEvents::EType::EVENT_ATTRACTOR, victim);

    CShadows::AddPermanentShadow(eShadowType::SHADOW_DEFAULT, gpShadowHeliTex, &pos, 8.0f, 0.0f, 0.0f, -8.0f, 200, 0, 0, 0, 10.0f, 30000, 1.0f);

    if (exp->m_fVisibleDistance != 0.0f && !exp->m_nParticlesExpireTime) {
        CWorld::TriggerExplosion(pos, exp->m_fRadius, exp->m_fVisibleDistance, victim, creator, DoesNeedToVehProcessBombTimer(type), exp->m_fDamagePercentage);
    }

    if (type == eExplosionType::EXPLOSION_MOLOTOV) {
        TheCamera.CamShake(cameraShake == -1.0f ? 0.2f : cameraShake, pos);
    } else {
        if (cameraShake == -1.0f)
            cameraShake = 0.6f;
        TheCamera.CamShake(cameraShake, pos);

        CPad::GetPad(0)->StartShake_Distance(300, 128, pos);
        if (CGameLogic::IsCoopGameGoingOn())
            CPad::GetPad(1)->StartShake_Distance(300, 128, pos);
    }
}

// 0x737620
void CExplosion::Update() {
    for (auto& exp : aExplosions) {
        if (!exp.m_nActiveCounter)
            continue;

        if (exp.m_nParticlesExpireTime) {
            if (CTimer::GetTimeInMS() > exp.m_nParticlesExpireTime) {
                exp.m_nParticlesExpireTime = 0;
                if (exp.m_fVisibleDistance != 0.0f) {
                    CWorld::TriggerExplosion(
                        exp.m_vecPosition,
                        exp.m_fRadius,
                        exp.m_fVisibleDistance,
                        exp.m_pVictim,
                        exp.m_pCreator,
                        DoesNeedToVehProcessBombTimer(exp.m_nType),
                        exp.m_fDamagePercentage
                    );
                }
            }
        } else {
            exp.m_fRadius += CTimer::GetTimeStep() * exp.m_fPropagationRate;
            switch (exp.m_nType) {
            case eExplosionType::EXPLOSION_GRENADE:
            case eExplosionType::EXPLOSION_ROCKET:
            case eExplosionType::EXPLOSION_WEAK_ROCKET:
            case eExplosionType::EXPLOSION_AIRCRAFT:
            case eExplosionType::EXPLOSION_MINE:
            case eExplosionType::EXPLOSION_OBJECT: {
                if (CTimer::GetFrameCounter() % 2) {
                    CPointLights::AddLight(ePointLightType::PLTYPE_POINTLIGHT, exp.m_vecPosition, {}, 20.0f, 1.0f, 1.0f, 0.5f, 0, false, nullptr);
                }
                if (exp.m_nType == eExplosionType::EXPLOSION_AIRCRAFT && CGeneral::GetRandomNumberInRange(0, 100) < 5) {
                    if (exp.m_pVictim) {
                        CExplosion::AddExplosion(exp.m_pVictim, exp.m_pCreator, eExplosionType::EXPLOSION_ROCKET, exp.m_pVictim->GetPosition(), 0, true, -1.0f, false);
                    }
                }
                break;
            }
            case eExplosionType::EXPLOSION_MOLOTOV: {
                const CVector& pos = exp.m_vecPosition;
                CWorld::SetPedsOnFire(pos.x, pos.y, pos.z, 6.0f, exp.m_pCreator);
                CWorld::SetWorldOnFire(pos.x, pos.y, pos.z, 6.0f, exp.m_pCreator);
                CWorld::SetCarsOnFire(pos.x, pos.y, pos.z, 0.1f, exp.m_pCreator);

                if (exp.m_nActiveCounter < 10 && exp.m_nActiveCounter == 1) {
                    CEntity* hitEntity;
                    CColPoint colPoint{};
                    const bool bGroundHit = CWorld::ProcessVerticalLine(pos, -1000.0f, colPoint, hitEntity, true, false, false, false, true, false, nullptr);
                    exp.m_fGroundZ = bGroundHit ? colPoint.m_vecPoint.z : pos.z;
                }
                break;
            }
            case eExplosionType::EXPLOSION_CAR:
            case eExplosionType::EXPLOSION_QUICK_CAR:
            case eExplosionType::EXPLOSION_BOAT: {
                if (exp.m_pVictim && CGeneral::GetRandomNumber() % 32 == 0) {
                    CVector rndOffset = {CGeneral::GetRandomNumberInRange(-0.5f, 0.5f), CGeneral::GetRandomNumberInRange(-0.5f, 0.5f), 0.0f};
                    rndOffset.Normalise();
                    rndOffset *= CGeneral::GetRandomNumberInRange(1.0f, 2.0f);
                    CCreepingFire::TryToStartFireAtCoors(exp.m_vecPosition + rndOffset, 0, true, false, 10.0f);
                }
                if (CTimer::GetFrameCounter() % 2) {
                    CPointLights::AddLight(ePointLightType::PLTYPE_POINTLIGHT, exp.m_vecPosition, {}, 15.0f, 1.0f, 0.7f, 0.5f, 0, true, nullptr);
                }
                break;
            }
            }

            if ((uint32)exp.m_nExpireTime - CTimer::GetTimeInMS() <= 0)
                exp.m_nActiveCounter = 0;
            else
                exp.m_nActiveCounter++;

            exp.m_nFuelTimer += (int32)CTimer::GetTimeStepInMS();

            if (exp.m_nFuelTimer > 200)
                continue;

            switch (exp.m_nType) {
            case eExplosionType::EXPLOSION_CAR:
            case eExplosionType::EXPLOSION_QUICK_CAR:
            case eExplosionType::EXPLOSION_BOAT:
            case eExplosionType::EXPLOSION_AIRCRAFT: {
                const float fFuelTimerProgress = (float)exp.m_nFuelTimer / 1000.0f;
                for (auto i = 0; i < NUM_FUEL; i++) {
                    const float& fOffsetDistance = exp.m_fFuelOffsetDistance[i];
                    if (fOffsetDistance > 0.0f) {
                        CVector fxPos = exp.m_vecPosition + exp.m_vecFuelDirection[i] * (fOffsetDistance + fFuelTimerProgress * exp.m_fFuelSpeed[i]);
                        if (auto fx = g_fxMan.CreateFxSystem("explosion_fuel_car", fxPos, nullptr, false)) {
                            fx->PlayAndKill();
                        }
                    }
                }
                break;
            }
            }
        }
    }
}
