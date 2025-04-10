#include "StdInc.h"

#include "FireManager.h"
#include "TheScripts.h"
#include "Shadows.h"

void CFireManager::InjectHooks() {
    RH_ScopedClass(CFireManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x539DA0);
    RH_ScopedInstall(Destructor, 0x538BB0);
    RH_ScopedInstall(Init, 0x538BC0);
    RH_ScopedInstall(Shutdown, 0x539DD0);
    RH_ScopedInstall(GetNumOfNonScriptFires, 0x538F10);
    RH_ScopedInstall(FindNearestFire, 0x538F40);
    RH_ScopedInstall(PlentyFiresAvailable, 0x539340);
    RH_ScopedInstall(ExtinguishPoint, 0x539450);
    RH_ScopedInstall(ExtinguishPointWithWater, 0x5394C0);
    RH_ScopedInstall(IsScriptFireExtinguished, 0x5396E0);
    RH_ScopedInstall(RemoveScriptFire, 0x539700);
    RH_ScopedInstall(RemoveAllScriptFires, 0x539720);
    RH_ScopedInstall(ClearAllScriptFireFlags, 0x5397A0);
    RH_ScopedInstall(SetScriptFireAudio, 0x5397B0);
    RH_ScopedInstall(GetScriptFireCoords, 0x5397E0);
    RH_ScopedInstall(GetNumFiresInRange, 0x5397F0);
    RH_ScopedInstall(GetNumFiresInArea, 0x539860);
    RH_ScopedInstall(DestroyAllFxSystems, 0x539D10);
    RH_ScopedInstall(CreateAllFxSystems, 0x539D50);
    RH_ScopedInstall(GetNextFreeFire, 0x539E50);
    RH_ScopedOverloadedInstall(StartFire, "NoTarget", 0x539F00, CFire*(CFireManager::*)(CVector, float, uint8, CEntity*, uint32, int8, uint8));
    RH_ScopedOverloadedInstall(StartFire, "", 0x53A050, CFire *(CFireManager::*)(CEntity*, CEntity*, float, uint8, uint32, int8));
    RH_ScopedInstall(StartScriptFire, 0x53A270);
    RH_ScopedInstall(Update, 0x53AF00);
}

// 0x539DA0
CFireManager::CFireManager() {
    Init();
}

CFireManager* CFireManager::Constructor() {
    this->CFireManager::CFireManager();
    return this;
}

CFireManager* CFireManager::Destructor() {
    this->CFireManager::~CFireManager();
    return this;
}

// 0x538BC0
void CFireManager::Init() {
    for (CFire& fire : m_aFires) {
        fire.Initialise();
    }
    m_nMaxFireGenerationsAllowed = 1'000'000 - 1; // aka 999'999
}

// 0x539DD0
void CFireManager::Shutdown() {
    for (CFire& fire : m_aFires) {
        fire.createdByScript = false;
        if (fire.IsActive()) {
            fire.Extinguish();
        }
    }
}

// 0x538F10
uint32 CFireManager::GetNumOfNonScriptFires() {
    uint32 c = 0;
    for (CFire& fire : m_aFires)
        if (fire.IsActive() && !fire.IsScript())
            c++;
    return c;
}

// NOTSA
uint32 CFireManager::GetNumOfFires() {
    uint32 c = 0;
    for (CFire& fire : m_aFires) {
        if (fire.IsActive())
            c++;
    }
    return c;
}

// 0x538F40
CFire* CFireManager::FindNearestFire(const CVector& point, bool bCheckIsBeingExtinguished, bool bCheckWasCreatedByScript) {
    float fNearestDist2DSq = std::numeric_limits<float>::max(); // Izzotop :thinking
    CFire* nearestFire{};
    for (CFire& fire : m_aFires) {
        if (!fire.IsActive())
            continue;

        if (bCheckWasCreatedByScript && fire.IsScript())
            continue;

        if (bCheckIsBeingExtinguished && fire.IsBeingExtinguished())
            continue;

        if (fire.m_pEntityTarget && fire.m_pEntityTarget->IsPed())
            continue;

        const float fDist2DSq = (fire.m_vecPosition - point).SquaredMagnitude2D();
        if (fDist2DSq < fNearestDist2DSq) {
            fNearestDist2DSq = fDist2DSq;
            nearestFire = &fire;
        }
    }
    return nearestFire;
}

// 0x539340
bool CFireManager::PlentyFiresAvailable() {
    uint32 c = 0;
    for (CFire& fire : m_aFires) {
        if (fire.IsActive())
            c++;
        if (c >= 6)
            return true;
    }
    return false;
}

// 0x539450
void CFireManager::ExtinguishPoint(CVector point, float fRadius) {
    for (CFire& fire : m_aFires) {
        if (!fire.IsActive())
            continue;

        if ((fire.m_vecPosition - point).SquaredMagnitude() > fRadius * fRadius)
            continue;

        fire.createdByScript = false;
        fire.Extinguish();
    }
}

// 0x5394C0
bool CFireManager::ExtinguishPointWithWater(CVector point, float fRadius, float fWaterStrength) {
    bool bSuccess = false;
    for (CFire& fire : m_aFires) {
        if (!fire.IsActive())
            continue;

        if ((fire.m_vecPosition - point).SquaredMagnitude() > fRadius * fRadius)
            continue;

        fire.ExtinguishWithWater(fWaterStrength);
        bSuccess = true;
    }
    return bSuccess;
}

// 0x5396E0
bool CFireManager::IsScriptFireExtinguished(int16 id) {
    return !Get(id).IsActive();
}

// 0x539700
void CFireManager::RemoveScriptFire(uint16_t fireID) {
    CFire& fire = Get(fireID);
    if (fire.IsScript()) {
        fire.createdByScript = false;
        fire.Extinguish();
    }
}

// 0x539720
void CFireManager::RemoveAllScriptFires() {
    for (CFire& fire : m_aFires) {
        if (fire.IsScript()) {
            fire.createdByScript = false;
            fire.Extinguish();
        }
    }
}

// 0x5397A0
void CFireManager::ClearAllScriptFireFlags() {
    for (CFire& fire : m_aFires) {
        fire.createdByScript = false;
    }
}

// 0x5397B0
void CFireManager::SetScriptFireAudio(int16 fireID, bool bFlag) {
    Get(fireID).makesNoise = bFlag;
}

// 0x5397E0
const CVector& CFireManager::GetScriptFireCoords(int16 fireID) {
    return Get(fireID).m_vecPosition;
}

// 0x5397F0
uint32 CFireManager::GetNumFiresInRange(const CVector& point, float fRadius) {
    uint32 c = 0;
    for (CFire& fire : m_aFires) {
        if (fire.IsActive() && !fire.IsScript()) {
            if ((fire.m_vecPosition - point).Magnitude2D() <= fRadius) {
                c++;
            }
        }
    }
    return c;
}

// 0x539860
uint32 CFireManager::GetNumFiresInArea(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
    CBoundingBox boundingBox{
        {minX, minY, minZ},
        {maxX, maxY, maxZ}
    };
    uint32 c = 0;
    for (CFire& fire : m_aFires) {
        if (fire.IsActive() && !fire.IsScript()) {
            if (boundingBox.IsPointWithin(fire.m_vecPosition)) {
                c++;
            }
        }
    }
    return c;
}

// 0x539D10
void CFireManager::DestroyAllFxSystems() {
    for (CFire& fire : m_aFires) {
        if (fire.IsActive()) {
            fire.DestroyFx();
        }
    }
}

// 0x539D50
void CFireManager::CreateAllFxSystems() {
    for (CFire& fire : m_aFires) {
        if (fire.IsActive()) {
            fire.CreateFxSysForStrength(fire.m_vecPosition, nullptr);
        }
    }
}

// 0x539E50
CFire* CFireManager::GetNextFreeFire(bool bMayExtinguish) {
    for (CFire& fire : m_aFires) {
        if (!fire.IsActive() && !fire.IsScript()) {
            return &fire;
        }
    }

    if (!bMayExtinguish)
        return nullptr;

    // At this point there are no inactive fires in the pool
    // So try to extinguish a script / first generation fire
    for (CFire& fire : m_aFires) {
        if (fire.IsFirstGen() || fire.IsScript()) {
            fire.createdByScript = false;
            fire.Extinguish();
            return &fire;
        }
    }
    return nullptr;
}

// 0x539F00
CFire* CFireManager::StartFire(CVector pos, float size, uint8 unused, CEntity* creator, uint32 nTimeToBurn, int8 nGenerations, uint8 unused_) {
    if (CWaterLevel::IsPointUnderwaterNoWaves(pos))
        return nullptr;

    if (CFire* fire = GetNextFreeFire(false)) {
        fire->Start(creator, pos, nTimeToBurn, std::min<uint8>((uint8)m_nMaxFireGenerationsAllowed, nGenerations));
        return fire;
    }
    return nullptr;
}

// 0x53A050
CFire* CFireManager::StartFire(CEntity* target, CEntity* creator, float size, uint8 unused, uint32 lifetime, int8 numGenerations) {
    /* Do few checks, and clear `m_pFire` if `target` */
    switch (target->m_nType) {
    case ENTITY_TYPE_PED: {
        auto pedTarget = target->AsPed();
        if (!pedTarget->IsPedInControl())
            return nullptr;
        if (pedTarget->m_pFire)
            return nullptr;
        if (pedTarget->physicalFlags.bFireProof)
            return nullptr;
        break;
    }
    case ENTITY_TYPE_VEHICLE: {
        auto vehTarget = target->AsVehicle();
        if (vehTarget->m_pFire)
            return nullptr;

        if (vehTarget->IsAutomobile()) {
            if (target->AsAutomobile()->m_damageManager.GetEngineStatus() >= 225)
                return nullptr;
        }

        if (vehTarget->physicalFlags.bFireProof)
            return nullptr;
        break;
    }
    case ENTITY_TYPE_OBJECT: {
        if (target->AsObject()->m_pFire)
            return nullptr;
        break;
    }
    }

    if (auto fire = GetNextFreeFire(false))
        fire->Start(creator, target, lifetime, std::min<uint8>((uint8)m_nMaxFireGenerationsAllowed, numGenerations));

    return nullptr;
}

// 0x53A270
int32 CFireManager::StartScriptFire(const CVector& pos, CEntity* target, float _fUnused, uint8 _nUnused, int8 nGenerations, int32 nStrength) {
    if (target) {
        /* Extinguish current fire (if any) of target */
        const auto StopFire = [](CFire* fire) {
            fire->Extinguish();
            fire->createdByScript = false;
        };
        switch (target->m_nType) {
        case ENTITY_TYPE_PED: {
            auto pedTarget = target->AsPed();
            if (pedTarget->m_pFire)
                StopFire(pedTarget->m_pFire);
            break;
        }
        case ENTITY_TYPE_VEHICLE: {
            auto vehTarget = target->AsVehicle();
            if (vehTarget->m_pFire)
                StopFire(vehTarget->m_pFire);

            /* Set engine status for automobiles */
            if (vehTarget->IsAutomobile()) {
                auto& dmgMgr = vehTarget->AsAutomobile()->m_damageManager;
                if (dmgMgr.GetEngineStatus() >= 225)
                    dmgMgr.SetEngineStatus(215);
            }
            break;
        }
        }
    }

    if (auto fire = GetNextFreeFire(true)) {
        fire->Start(pos, (float)nStrength, target, std::min<uint8>((uint8)m_nMaxFireGenerationsAllowed, nGenerations));
        return CTheScripts::GetNewUniqueScriptThingIndex(GetIndexOf(fire), SCRIPT_THING_FIRE);
    }
    return -1;
}

// 0x53AF00
void CFireManager::Update() {
    ZoneScoped;

    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    for (CFire& fire : m_aFires) {
        if (fire.IsActive())
            fire.ProcessFire();
    }

    if (CGameLogic::LaRiotsActiveHere()
     && CTimer::GetTimeInMS() / 500u != CTimer::GetPreviousTimeInMS() / 500u
    ) {
        const float fRandomAngleRad = CGeneral::GetRandomNumberInRange(0.0f, TWO_PI);
        const float fRandomDir = CGeneral::GetRandomNumberInRange(35.0f, 60.0f);
        CVector point = TheCamera.GetPosition() + CVector{
            std::sin(fRandomAngleRad) * fRandomDir,
            std::cos(fRandomAngleRad) * fRandomDir,
            10.0f
        };

        CEntity* hitEntity{};
        bool bHit{};
        point.z = CWorld::FindGroundZFor3DCoord(point, &bHit, &hitEntity);

        CVector pointToCamDirNorm = (TheCamera.GetPosition() - point);
        pointToCamDirNorm.Normalise();

        // In the PC version 0.2f and "riot_smoke" is accessed through an array
        // On the Android version it's inlined
        // Android values used here, and on the PC version the array access isn't
        // a 100% clear to me.
        // TODO: Use the array here
        if (DotProduct(TheCamera.GetForward(), pointToCamDirNorm) > 0.2f || CGeneral::GetRandomNumber() < RAND_MAX / 2) {
            auto fx = g_fxMan.CreateFxSystem("riot_smoke", point, nullptr, true);
            if (fx)
                fx->PlayAndKill();
        } else {
            StartFire(point, CGeneral::GetRandomNumberInRange(0.5f, 2.5f), true, nullptr, 30000u, 5, 1);
        }
    }

    auto nFires = (int32)GetNumOfFires();
    bool firesVisited[MAX_NUM_FIRES] = { false }; // Lookup table to see if a fire's strength was already included into a group of fires
    while (nFires > 0) {
        // Repeat until there are no active fires left
        // Find strongest un-visited fire, and sum of the strength of all fires within 6.0 units of it
        // Based on this strength possibly create a shadow (if combined strength > 4), and coronas (combined strength > 6)

        // Find the strongest fire, which hasn't yet been visited
        CFire* strongest{};
        for (size_t i = 0; i < MAX_NUM_FIRES; i++) {
            CFire& fire = m_aFires[i];
            if (firesVisited[i] || !fire.IsActive())
                continue;
            if (!strongest || strongest->m_fStrength < fire.m_fStrength)
                strongest = &fire;
        }

        // Sum up strengths of all fires (that haven't yet been visited) within 6.0 units range
        float fCombinedStrength{};
        int32 nCombinedCeilStrength{};
        for (size_t i = 0; i < MAX_NUM_FIRES; i++) {
            CFire& fire = m_aFires[i];
            if (firesVisited[i] || !fire.IsActive())
                continue;

            if ((fire.m_vecPosition - strongest->m_vecPosition).Magnitude2D() < 6.0f) {
                fCombinedStrength += fire.m_fStrength;
                nCombinedCeilStrength += (int32)std::ceil(fire.m_fStrength);
                nFires--;
                firesVisited[i] = true;

                // R* also does some weird vector point calculation,
                // which doesn't seem to end up being used. Probably old code left in?
                // <vector defined where `fCombinedStrength` is, lets name it averagedFirePos = {}>
                // averagedFirePos += fire.m_vecPosition * std::ceil(fire.m_fStrength);
            }
        }

        if (fCombinedStrength > 4.0f && nCombinedCeilStrength) {
            const CRGBA baseColor{ 40, 32, 20 }; // From addresses 0x8CCFF0, 0x8CCFF1, 0x8CCFF2

            const float fDir = std::min(7.0f, fCombinedStrength - 6.0f + 3.0f);

            CVector shdwPos = strongest->m_vecPosition;
            shdwPos.z += 5.0f;
            const float fColorMult = CGeneral::GetRandomNumberInRange(0.6f, 1.0f);
            {
                const CRGBA shdwColor = baseColor * fColorMult;
                CShadows::StoreStaticShadow(
                    reinterpret_cast<uint32>(strongest),
                    SHADOW_ADDITIVE,
                    gpShadowExplosionTex,
                    shdwPos,
                    fDir * -1.2f,
                    0.0f,
                    0.0f,
                    fDir * 1.2f,
                    0,
                    shdwColor.r,
                    shdwColor.g,
                    shdwColor.b,
                    10.0f,
                    1.0f,
                    40.0f,
                    false,
                    0.0f
                );
            }

            if (fCombinedStrength > 6.0f) {
                // Create coronas in a _|_ like shape.
                // Keep in mind, the right line's end is always pointing towards the camera,
                // so what you see is more like |

                CVector point = strongest->m_vecPosition
                    + CVector{ 0.0f, 0.0f, 2.6f }
                    + (TheCamera.GetPosition() - point).Normalized() * 3.5f;

                auto strongestId = reinterpret_cast<uint32>(strongest);
                // Wrapper lambda for code readability
                const auto RegisterCorona = [&](auto idx, CVector pos, eCoronaFlareType flare = eCoronaFlareType::FLARETYPE_NONE) {
                    const CRGBA crnaColor = baseColor * (fColorMult * 0.8f);
                    CCoronas::RegisterCorona(
                        strongestId,
                        nullptr,
                        crnaColor.r,
                        crnaColor.g,
                        crnaColor.b,
                        0xFF,
                        pos,
                        fDir * 0.5f,
                        70.0f,
                        eCoronaType::CORONATYPE_HEADLIGHT,
                        flare,
                        false,
                        false,
                        0,
                        0.0f,
                        false,
                        1.5f,
                        0,
                        15.0f,
                        false,
                        false
                    );
                };

                // Each corona only differs in position, and ID
                RegisterCorona(strongestId, point, eCoronaFlareType::FLARETYPE_HEADLIGHTS);

                point.z += 2.0f;
                RegisterCorona(strongestId + 1, point);

                point.z -= 2.0f;
                CVector camRightNorm = TheCamera.GetRight();
                camRightNorm.z = 0.0f;
                camRightNorm.Normalise();
                point += camRightNorm * 2.0f;
                RegisterCorona(strongestId + 2, point);

                point -= camRightNorm * 4.0f;
                RegisterCorona(strongestId + 3, point);
            }
        }

    }
}

// NOTSA
CFire& CFireManager::GetRandomFire() {
    return CGeneral::RandomChoice(m_aFires);
}
