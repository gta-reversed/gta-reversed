#include "StdInc.h"

#include "Fire.h"
#include "FireManager.h"
#include "CreepingFire.h"

void CFire::InjectHooks() {
    RH_ScopedClass(CFire);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x539D90);
    RH_ScopedInstall(Initialise, 0x538B30);
    RH_ScopedInstall(CreateFxSysForStrength, 0x539360);
    RH_ScopedInstall(Extinguish, 0x5393F0);
    RH_ScopedInstall(ProcessFire, 0x53A570);
}

// 0x539D90
CFire::CFire() {
    Initialise();
}

CFire* CFire::Constructor() {
    this->CFire::CFire();
    return this;
}

// 0x538B30
void CFire::Initialise() {
    // Originally m_nFlags = (m_nFlags & 0xF4) | 0x14; - Clear 1st, 2nd, 4th and set 3rd, 5th bits (1-based numbering)
    active = false;
    createdByScript = false;
    makesNoise = true;
    beingExtinguished = false;
    firstGeneration = true;
    m_vecPosition = CVector{};
    m_nScriptReferenceIndex = 1;
    m_nTimeToBurn = 0;
    m_pEntityTarget = nullptr;
    m_pEntityCreator = nullptr;
    m_fStrength = 1.0f;
    m_pFxSystem = nullptr;
    m_nNumGenerationsAllowed = 100;
    m_nRemovalDist = 60;
}

void CFire::ExtinguishWithWater(float fWaterStrength) {
    const float fOriginalStrength = m_fStrength;
    m_fStrength -= fWaterStrength * CTimer::GetTimeStepInSeconds();

    /* Create particles */
    CVector particlePos = m_vecPosition + CVector{
        CGeneral::GetRandomNumberInRange(-1.28f, 1.28f),
        CGeneral::GetRandomNumberInRange(-1.28f, 1.28f),
        CGeneral::GetRandomNumberInRange(-0.64f, 0.64f)
        /* Original code:
        (float)((CGeneral::GetRandomNumber() % 256) - 128) / 100.0f,
        (float)((CGeneral::GetRandomNumber() % 256) - 128) / 100.0f,
        (float)((CGeneral::GetRandomNumber() % 256) - 128) / 200.0f
        */
    };
    FxPrtMult_c prtMult{ 1.0f, 1.0f, 1.0f, 0.6f, 0.75f, 0.0f, 0.4f };
    const auto AddParticle = [&](CVector velocity) {
        g_fx.m_SmokeII3expand->AddParticle(&particlePos, &velocity, 0.0f, &prtMult, -1.0f, 1.2f, 0.6f, false);
    };
    /* The two particles only differ in velocity */
    AddParticle({ 0.0f, 0.0f, 0.8f });
    AddParticle({ 0.0f, 0.0f, 1.4f });

    /* Re-create fx / extinguish */
    beingExtinguished = true;
    if (m_fStrength >= 0.0f) {
        if ((intmax_t)fOriginalStrength != (intmax_t)m_fStrength) { /* Check if integer part has changed */
            CreateFxSysForStrength(m_vecPosition, nullptr); /* Yes, so needs a new fx */
        }
    } else {
        Extinguish();
    }
}

// see 0x539F00 CFireManager::StartFire
void CFire::Start(CEntity* creator, CVector pos, uint32 nTimeToBurn, uint8 nGens) {
    active = true;
    createdByScript = false;
    makesNoise = true;
    beingExtinguished = false;
    firstGeneration = true;

    m_nTimeToBurn = CTimer::GetTimeInMS() + (uint32_t)(CGeneral::GetRandomNumberInRange(1.0f, 1.3f) * (float)nTimeToBurn);

    SetTarget(nullptr);
    SetCreator(creator);

    m_nNumGenerationsAllowed = nGens;
    m_fStrength = 1.0f;
    m_vecPosition = pos;

    CreateFxSysForStrength(m_vecPosition, nullptr);
}

// see 0x53A050 CFireManager::StartFire
void CFire::Start(CEntity* creator, CEntity* target, uint32 nTimeToBurn, uint8 nGens) {
    switch (target->m_nType) {
    case ENTITY_TYPE_PED: {
        auto targetPed = target->AsPed();
        targetPed->m_pFire = this;
        CCrime::ReportCrime(
            targetPed->m_nPedType == PED_TYPE_COP ? eCrimeType::CRIME_SET_COP_PED_ON_FIRE : eCrimeType::CRIME_SET_PED_ON_FIRE,
            targetPed,
            creator->AsPed()
        );
        break;
    }
    case ENTITY_TYPE_VEHICLE: {
        auto targetVehicle = target->AsVehicle();
        targetVehicle->m_pFire = this;
        CCrime::ReportCrime(
            eCrimeType::CRIME_SET_CAR_ON_FIRE,
            targetVehicle,
            creator->AsPed()
        );
        break;
    }
    case ENTITY_TYPE_OBJECT: {
        target->AsObject()->m_pFire = this;
        break;
    }
    }

    active = true;
    createdByScript = false;
    makesNoise = true;
    beingExtinguished = false;
    firstGeneration = true;

    m_nNumGenerationsAllowed = nGens;
    m_fStrength = 1.0f;
    m_vecPosition = target->GetPosition();

    if (target->IsPed() && target->AsPed()->IsPlayer())
        m_nTimeToBurn = CTimer::GetTimeInMS() + 2333;
    else if (target->IsVehicle())
        m_nTimeToBurn = CTimer::GetTimeInMS() + CGeneral::GetRandomNumberInRange(0, 1000) + 3000;
    else
        m_nTimeToBurn = CTimer::GetTimeInMS() + CGeneral::GetRandomNumberInRange(0, 1000) + nTimeToBurn;

    SetTarget(target);
    SetCreator(creator);

    CreateFxSysForStrength(m_vecPosition, nullptr);
}

// see 0x53A270 CFireManager::StartScriptFire
void CFire::Start(CVector pos, float fStrength, CEntity* target, uint8 nGens) {
    SetTarget(target);
    SetCreator(nullptr);

    active = true;
    createdByScript = true;
    makesNoise = true;
    beingExtinguished = false;
    firstGeneration = true;

    m_nNumGenerationsAllowed = nGens;
    m_fStrength = fStrength;
    m_vecPosition = pos;

    if (target) {
        switch (target->m_nType) { /* Set target's `m_pFire` to `this` */
        case ENTITY_TYPE_PED:
            target->AsPed()->m_pFire = this;
            break;
        case ENTITY_TYPE_VEHICLE:
            target->AsVehicle()->m_pFire = this;
            break;
        }
    }

    CreateFxSysForStrength(target ? target->GetPosition() : pos, nullptr);
}

void CFire::SetTarget(CEntity* target) {
    CEntity::SafeCleanUpRef(m_pEntityTarget); /* Assume old target's m_pFire is not pointing to `*this` */

    m_pEntityTarget = target; /* assign, even if its null, to clear it */
    CEntity::SafeRegisterRef(m_pEntityTarget); /* Assume caller set target->m_pFire */
}

void CFire::SetCreator(CEntity* creator) {
    CEntity::SafeCleanUpRef(m_pEntityCreator);

    m_pEntityCreator = creator; /* assign, even if its null, to clear it */
    CEntity::SafeRegisterRef(m_pEntityCreator);
}

void CFire::DestroyFx() {
    if (m_pFxSystem) {
        m_pFxSystem->Kill();
        m_pFxSystem = nullptr;
    }
}

auto CFire::GetFireParticleNameForStrength() const {
    if (m_fStrength > 1.0f)
        return (m_fStrength > 2.0f) ? "fire_large" : "fire_med";
    else
        return "fire";
};

// 0x539360
void CFire::CreateFxSysForStrength(const CVector& point, RwMatrix* matrix) {
    DestroyFx();
    m_pFxSystem = g_fxMan.CreateFxSystem(GetFireParticleNameForStrength(), point, matrix, true);
    if (m_pFxSystem)
        m_pFxSystem->Play();
}

// 0x5393F0
void CFire::Extinguish() {
    if (!active)
        return;

    m_nTimeToBurn = 0;

    // Originally m_nFlags = (m_nFlags & 0xF6) | 0x10; - Clear 1st and 4th, set 5th
    active = false;
    beingExtinguished = false;
    firstGeneration = true;

    DestroyFx();

    if (m_pEntityTarget) {
        switch (m_pEntityTarget->m_nType) {
        case ENTITY_TYPE_PED: {
            m_pEntityTarget->AsPed()->m_pFire = nullptr;
            break;
        }
        case ENTITY_TYPE_VEHICLE: {
            m_pEntityTarget->AsVehicle()->m_pFire = nullptr;
            break;
        }
        }
        CEntity::ClearReference(m_pEntityTarget);
    }
}

// 0x53A570
void CFire::ProcessFire() {
    {
        const float fNewStrength = std::min(3.0f, m_fStrength + CTimer::GetTimeStep() / 500.0f); // Limited to 3.0f
        if ((uint32)m_fStrength == (uint32)fNewStrength)
            m_fStrength = fNewStrength; // Not sure why they do this, probably just some hack
    }

    if (m_pEntityTarget) {
        m_vecPosition = m_pEntityTarget->GetPosition();

        switch (m_pEntityTarget->m_nType) {
        case ENTITY_TYPE_PED: {
            auto targetPed = m_pEntityTarget->AsPed();

            if (targetPed->m_pFire != this) {
                Extinguish();
                return;
            }

            switch (targetPed->m_nPedState) {
            case PEDSTATE_DIE:
            case PEDSTATE_DEAD: {
                m_vecPosition.z -= 1.0f; /* probably because ped is laying on the ground */
                break;
            }
            }

            if (auto vehicle = targetPed->GetVehicleIfInOne()) {
                if (!ModelIndices::IsFireTruck(vehicle->m_nModelIndex) && vehicle->IsAutomobile()) {
                    vehicle->m_fHealth = 75.0f;
                }
            } else if (!targetPed->IsPlayer() && !targetPed->IsAlive()) {
                targetPed->physicalFlags.bRenderScorched = true;
            }

            break;
        }
        case ENTITY_TYPE_VEHICLE: {
            auto targetVehicle = m_pEntityTarget->AsVehicle();

            if (targetVehicle->m_pFire != this) {
                Extinguish();
                return;
            }

            if (!createdByScript) {
                targetVehicle->InflictDamage(m_pEntityCreator, eWeaponType::WEAPON_FLAMETHROWER, CTimer::GetTimeStep() * 1.2f, CVector{});
            }

            if (targetVehicle->IsAutomobile()) {
                m_vecPosition = targetVehicle->GetDummyPosition(eVehicleDummy::DUMMY_LIGHT_FRONT_MAIN) + CVector{0.0f, 0.0f, 0.15f};
            }
            break;
        }
        }

        if (m_pFxSystem) {
            auto targetPhysical = m_pEntityTarget->AsPhysical();
            m_pFxSystem->SetOffsetPos(m_vecPosition + CTimer::GetTimeStep() * 2.0f * targetPhysical->m_vecMoveSpeed);
        }
    }

    CPlayerPed* player = FindPlayerPed();
    if (!m_pEntityTarget || !m_pEntityTarget->IsVehicle()) {
        // Check if we can set player's ped on fire
        if (!FindPlayerVehicle()
         && !player->m_pFire /* not already on fire */
         && !player->physicalFlags.bFireProof
         && !player->m_pAttachedTo
         ) {
            if ((player->GetPosition() - m_vecPosition).SquaredMagnitude() < 1.2f) { /* Note: Squared distance */
                player->DoStuffToGoOnFire();
                gFireManager.StartFire(player, m_pEntityCreator, 0.8f, true, 7000, 100);
            }
        }
    }

    if (CGeneral::GetRandomNumber() % 32 == 0) {
        for (auto i = GetVehiclePool()->GetSize() - 1; i >= 0; i--) { /* backwards loop, like original code */
            CVehicle* vehicle = GetVehiclePool()->GetAt(i);
            if (!vehicle)
                continue;

            if (DistanceBetweenPoints(m_vecPosition, vehicle->GetPosition()) >= 2.0f)
                continue;

            if (vehicle->IsBike()) { // FIX_BUG: BurstTyre expects a wheel index, not a car piece like GTA IV.
                player->DoStuffToGoOnFire();
                gFireManager.StartFire(player, m_pEntityCreator, 0.8f, true, 7000, 100);
                vehicle->BurstTyre(vehicle->FindTyreNearestPoint(m_vecPosition) + CAR_PIECE_WHEEL_LF, true);
            } else {
                gFireManager.StartFire(vehicle, m_pEntityCreator, 0.8f, true, 7000, 100);
            }
        }
    }

    if (CGeneral::GetRandomNumber() % 4 == 0) {
        for (auto i = GetObjectPool()->GetSize(); i --> 0;) { /* backwards loop, like original code */
            CObject* obj = GetObjectPool()->GetAt(i);
            if (!obj)
                continue;

            if (DistanceBetweenPoints(m_vecPosition, obj->GetPosition()) >= 3.0f)
                continue;

            obj->ObjectFireDamage(CTimer::GetTimeStep() * 8.0f, m_pEntityCreator);
        }
    }

    if (m_nNumGenerationsAllowed > 0 && CGeneral::GetRandomNumber() % 128 == 0) {
        if (gFireManager.GetNumOfFires() < 25) {
            const CVector dir{ CGeneral::GetRandomNumberInRange(-1.0f, 1.0f), CGeneral::GetRandomNumberInRange(-1.0f, 1.0f), 0.0f };
            CCreepingFire::TryToStartFireAtCoors(m_vecPosition + dir * CGeneral::GetRandomNumberInRange(2.0f, 3.0f), m_nNumGenerationsAllowed, false, IsScript(), 10.0f);
        }
    }

    if (m_fStrength <= 2.0f && m_nNumGenerationsAllowed && CGeneral::GetRandomNumber() % 16 == 0) {
        CFire& nearby = gFireManager.GetRandomFire();
        if (&nearby != this && nearby.active && !nearby.createdByScript && nearby.m_fStrength <= 1.0f) {
            if (DistanceBetweenPoints(nearby.m_vecPosition, m_vecPosition) < 3.5f) {
                nearby.m_vecPosition = nearby.m_vecPosition * 0.3f + m_vecPosition * 0.7f;
                m_fStrength += 1.0f;
                m_nTimeToBurn = std::max(m_nTimeToBurn, CTimer::GetTimeInMS() + 7000);
                CreateFxSysForStrength(m_vecPosition, nullptr);
                m_nNumGenerationsAllowed = std::max(m_nNumGenerationsAllowed, nearby.m_nNumGenerationsAllowed);
                nearby.Extinguish();
            }
        }
    }

    if (m_pFxSystem) {
        float unused;
        const float fFractPart = std::modf(m_fStrength, &unused); // R* way: m_fStrength - (float)(int)m_fStrength
        m_pFxSystem->SetConstTime(true, std::min((float)CTimer::GetTimeInMS() / 3500.0f, fFractPart));
    }

    if (createdByScript || (HasTimeToBurn() && IsNotInRemovalDistance())) {
        // Original code:
        // const float fColorRG = (float)(CGeneral::GetRandomNumber() % 128) / 512.0f;
        const float fColorRG = CGeneral::GetRandomNumberInRange(0.0f, 0.25f);
        CPointLights::AddLight(ePointLightType::PLTYPE_POINTLIGHT, m_vecPosition, CVector{}, 8.0f, fColorRG, fColorRG, 0.0f, 0, false, nullptr);
    } else {
        if (m_fStrength <= 1.0f) {
            Extinguish();
        } else {
            m_fStrength -= 1.0f;
            m_nTimeToBurn = CTimer::GetTimeInMS() + 7000;
            CreateFxSysForStrength(m_vecPosition, nullptr);
        }
    }
}

bool CFire::HasTimeToBurn() const {
    return CTimer::GetTimeInMS() < m_nTimeToBurn;
}

bool CFire::IsNotInRemovalDistance() const {
    return m_nRemovalDist > (TheCamera.GetPosition() - m_vecPosition).Magnitude();
}
