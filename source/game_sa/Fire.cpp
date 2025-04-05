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
    m_IsActive = false;
    m_IsCreatedByScript = false;
    m_MakesNoise = true;
    m_IsBeingExtinguished = false;
    m_IsFirstGeneration = true;
    m_Position = CVector{};
    m_ScriptReferenceIndex = 1;
    m_TimeToBurn = 0;
    m_EntityOnFire = nullptr;
    m_EntityStartedFire = nullptr;
    m_Strength = 1.0f;
    m_FxSystem = nullptr;
    m_NumGenerationsAllowed = 100;
    m_RemovalDist = 60;
}

void CFire::ExtinguishWithWater(float fWaterStrength) {
    const float fOriginalStrength = m_Strength;
    m_Strength -= fWaterStrength * CTimer::GetTimeStepInSeconds();

    /* Create particles */
    CVector particlePos = m_Position + CVector{
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
    m_IsBeingExtinguished = true;
    if (m_Strength >= 0.0f) {
        if ((intmax_t)fOriginalStrength != (intmax_t)m_Strength) { /* Check if integer part has changed */
            CreateFxSysForStrength(m_Position, nullptr); /* Yes, so needs a new fx */
        }
    } else {
        Extinguish();
    }
}

// see 0x539F00 CFireManager::StartFire
void CFire::Start(CEntity* creator, CVector pos, uint32 nTimeToBurn, uint8 nGens) {
    m_IsActive = true;
    m_IsCreatedByScript = false;
    m_MakesNoise = true;
    m_IsBeingExtinguished = false;
    m_IsFirstGeneration = true;

    m_TimeToBurn = CTimer::GetTimeInMS() + (uint32_t)(CGeneral::GetRandomNumberInRange(1.0f, 1.3f) * (float)nTimeToBurn);

    SetEntityOnFire(nullptr);
    SetEntityStartedFire(creator);

    m_NumGenerationsAllowed = nGens;
    m_Strength = 1.0f;
    m_Position = pos;

    CreateFxSysForStrength(m_Position, nullptr);
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

    m_IsActive = true;
    m_IsCreatedByScript = false;
    m_MakesNoise = true;
    m_IsBeingExtinguished = false;
    m_IsFirstGeneration = true;

    m_NumGenerationsAllowed = nGens;
    m_Strength = 1.0f;
    m_Position = target->GetPosition();

    if (target->IsPed() && target->AsPed()->IsPlayer())
        m_TimeToBurn = CTimer::GetTimeInMS() + 2333;
    else if (target->IsVehicle())
        m_TimeToBurn = CTimer::GetTimeInMS() + CGeneral::GetRandomNumberInRange(0, 1000) + 3000;
    else
        m_TimeToBurn = CTimer::GetTimeInMS() + CGeneral::GetRandomNumberInRange(0, 1000) + nTimeToBurn;

    SetEntityOnFire(target);
    SetEntityStartedFire(creator);

    CreateFxSysForStrength(m_Position, nullptr);
}

// see 0x53A270 CFireManager::StartScriptFire
void CFire::Start(CVector pos, float fStrength, CEntity* target, uint8 nGens) {
    SetEntityOnFire(target);
    SetEntityStartedFire(nullptr);

    m_IsActive = true;
    m_IsCreatedByScript = true;
    m_MakesNoise = true;
    m_IsBeingExtinguished = false;
    m_IsFirstGeneration = true;

    m_NumGenerationsAllowed = nGens;
    m_Strength = fStrength;
    m_Position = pos;

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

void CFire::SetEntityOnFire(CEntity* target) {
    CEntity::SafeCleanUpRef(m_EntityOnFire); /* Assume old target's m_pFire is not pointing to `*this` */

    m_EntityOnFire = target; /* assign, even if its null, to clear it */
    CEntity::SafeRegisterRef(m_EntityOnFire); /* Assume caller set target->m_pFire */
}

void CFire::SetEntityStartedFire(CEntity* creator) {
    CEntity::SafeCleanUpRef(m_EntityStartedFire);

    m_EntityStartedFire = creator; /* assign, even if its null, to clear it */
    CEntity::SafeRegisterRef(m_EntityStartedFire);
}

void CFire::DestroyFx() {
    if (m_FxSystem) {
        m_FxSystem->Kill();
        m_FxSystem = nullptr;
    }
}

auto CFire::GetFireParticleNameForStrength() const {
    if (m_Strength > 1.0f)
        return (m_Strength > 2.0f) ? "fire_large" : "fire_med";
    else
        return "fire";
};

// 0x539360
void CFire::CreateFxSysForStrength(const CVector& point, RwMatrix* matrix) {
    DestroyFx();
    m_FxSystem = g_fxMan.CreateFxSystem(GetFireParticleNameForStrength(), point, matrix, true);
    if (m_FxSystem)
        m_FxSystem->Play();
}

// 0x5393F0
void CFire::Extinguish() {
    if (!m_IsActive)
        return;

    m_TimeToBurn = 0;

    // Originally m_nFlags = (m_nFlags & 0xF6) | 0x10; - Clear 1st and 4th, set 5th
    m_IsActive = false;
    m_IsBeingExtinguished = false;
    m_IsFirstGeneration = true;

    DestroyFx();

    if (m_EntityOnFire) {
        switch (m_EntityOnFire->m_nType) {
        case ENTITY_TYPE_PED: {
            m_EntityOnFire->AsPed()->m_pFire = nullptr;
            break;
        }
        case ENTITY_TYPE_VEHICLE: {
            m_EntityOnFire->AsVehicle()->m_pFire = nullptr;
            break;
        }
        }
        CEntity::ClearReference(m_EntityOnFire);
    }
}

// 0x53A570
void CFire::ProcessFire() {
    {
        const float newStrength = std::min(3.0f, m_Strength + CTimer::GetTimeStep() / 500.0f);
        if ((uint32)(m_Strength) != (uint32)(newStrength)) {
            m_Strength = newStrength; // Not sure why they do this, probably just some hack
        }
    }

    if (m_EntityOnFire) {
        m_Position = m_EntityOnFire->GetPosition();

        switch (m_EntityOnFire->m_nType) {
        case ENTITY_TYPE_PED: {
            auto targetPed = m_EntityOnFire->AsPed();

            if (targetPed->m_pFire != this) {
                Extinguish();
                return;
            }

            switch (targetPed->m_nPedState) {
            case PEDSTATE_DIE:
            case PEDSTATE_DEAD: {
                m_Position.z -= 1.0f; /* probably because ped is laying on the ground */
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
            auto targetVehicle = m_EntityOnFire->AsVehicle();

            if (targetVehicle->m_pFire != this) {
                Extinguish();
                return;
            }

            if (!m_IsCreatedByScript) {
                targetVehicle->InflictDamage(m_EntityStartedFire, eWeaponType::WEAPON_FLAMETHROWER, CTimer::GetTimeStep() * 1.2f, CVector{});
            }

            if (targetVehicle->IsAutomobile()) {
                m_Position = targetVehicle->GetDummyPosition(eVehicleDummy::DUMMY_LIGHT_FRONT_MAIN) + CVector{0.0f, 0.0f, 0.15f};
            }
            break;
        }
        }

        if (m_FxSystem) {
            auto targetPhysical = m_EntityOnFire->AsPhysical();
            m_FxSystem->SetOffsetPos(m_Position + CTimer::GetTimeStep() * 2.0f * targetPhysical->m_vecMoveSpeed);
        }
    }

    CPlayerPed* player = FindPlayerPed();
    if (!m_EntityOnFire || !m_EntityOnFire->IsVehicle()) {
        // Check if we can set player's ped on fire
        if (!FindPlayerVehicle()
         && !player->m_pFire /* not already on fire */
         && !player->physicalFlags.bFireProof
         && !player->m_pAttachedTo
         ) {
            if ((player->GetPosition() - m_Position).SquaredMagnitude() < 1.2f) { /* Note: Squared distance */
                player->DoStuffToGoOnFire();
                gFireManager.StartFire(player, m_EntityStartedFire, 0.8f, true, 7000, 100);
            }
        }
    }

    if (CGeneral::GetRandomNumber() % 32 == 0) {
        for (auto& veh : CPools::GetVehiclePool()->GetAllValid()) { // NOTSA: Original loop was backwards [not that it matters]
            if (DistanceBetweenPoints(m_Position, veh.GetPosition()) >= 2.0f)
                continue;

            if (veh.IsSubBMX()) {
                player->DoStuffToGoOnFire();
                gFireManager.StartFire(player, m_EntityStartedFire, 0.8f, true, 7000, 100);
                veh.BurstTyre(veh.FindTyreNearestPoint(m_Position) + CAR_PIECE_WHEEL_LF, false);
            } else {
                gFireManager.StartFire(&veh, m_EntityStartedFire, 0.8f, true, 7000, 100);
            }
        }
    }

    if (CGeneral::GetRandomNumber() % 4 == 0) {
        for (auto& obj : CPools::GetObjectPool()->GetAllValid()) { // NOTSA: Original loop was backwards [not that it matters]
            if (DistanceBetweenPoints(m_Position, obj.GetPosition()) >= 3.0f)
                continue;

            obj.ObjectFireDamage(CTimer::GetTimeStep() * 8.0f, m_EntityStartedFire);
        }
    }

    if (m_NumGenerationsAllowed > 0 && CGeneral::GetRandomNumber() % 128 == 0) {
        if (gFireManager.GetNumOfFires() < 25) {
            const CVector dir{ CGeneral::GetRandomNumberInRange(-1.0f, 1.0f), CGeneral::GetRandomNumberInRange(-1.0f, 1.0f), 0.0f };
            CCreepingFire::TryToStartFireAtCoors(m_Position + dir * CGeneral::GetRandomNumberInRange(2.0f, 3.0f), m_NumGenerationsAllowed, false, IsScript(), 10.0f);
        }
    }

    if (m_Strength <= 2.0f && m_NumGenerationsAllowed && CGeneral::GetRandomNumber() % 16 == 0) {
        CFire& nearby = gFireManager.GetRandomFire();
        if (&nearby != this && nearby.m_IsActive && !nearby.m_IsCreatedByScript && nearby.m_Strength <= 1.0f) {
            if (DistanceBetweenPoints(nearby.m_Position, m_Position) < 3.5f) {
                nearby.m_Position = nearby.m_Position * 0.3f + m_Position * 0.7f;
                m_Strength += 1.0f;
                m_TimeToBurn = std::max(m_TimeToBurn, CTimer::GetTimeInMS() + 7000);
                CreateFxSysForStrength(m_Position, nullptr);
                m_NumGenerationsAllowed = std::max(m_NumGenerationsAllowed, nearby.m_NumGenerationsAllowed);
                nearby.Extinguish();
            }
        }
    }

    if (m_FxSystem) {
        float unused;
        const float fFractPart = std::modf(m_Strength, &unused); // R* way: m_fStrength - (float)(int)m_fStrength
        m_FxSystem->SetConstTime(true, std::min((float)CTimer::GetTimeInMS() / 3500.0f, fFractPart));
    }

    if (m_IsCreatedByScript || (HasTimeToBurn() && IsNotInRemovalDistance())) {
        const float fColorRG = (float)(CGeneral::GetRandomNumber() % 128) / 512.0f; // todo: GetRandomNumberInRange
        CPointLights::AddLight(ePointLightType::PLTYPE_POINTLIGHT, m_Position, CVector{}, 8.0f, fColorRG, fColorRG, 0.0f, 0, false, nullptr);
    } else {
        if (m_Strength <= 1.0f) {
            Extinguish();
        } else {
            m_Strength -= 1.0f;
            m_TimeToBurn = CTimer::GetTimeInMS() + 7000;
            CreateFxSysForStrength(m_Position, nullptr);
        }
    }
}

bool CFire::HasTimeToBurn() const {
    return CTimer::GetTimeInMS() < m_TimeToBurn;
}

bool CFire::IsNotInRemovalDistance() const {
    return m_RemovalDist > (TheCamera.GetPosition() - m_Position).Magnitude();
}
