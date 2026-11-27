#include "StdInc.h"

#include "Plane.h"
#include "CarCtrl.h"
#include "FireManager.h"
#include <reversiblebugfixes/Bugs.hpp>

auto& HARRIER_NOZZLE_ROTATERATE = StaticRef<float>(0x8D33DC);       // 25.0f
auto& PLANE_DAMAGE_WAVE_COUNTER_VAR = StaticRef<float>(0x8D33E0);   // 0.75f
auto& PLANE_DAMAGE_THRESHHOLD = StaticRef<float>(0x8D33E4);         // 500.0f
auto& PLANE_DAMAGE_SCALE_MASS = StaticRef<float>(0x8D33E8);         // 10000.0f
auto& PLANE_DAMAGE_DESTROY_THRESHHOLD = StaticRef<float>(0x8D33EC); // 5000.0f
auto& vecRCBaronGunPos = StaticRef<CVector>(0x8D33F0);            // <0.0f, 0.45f, 0.0f>

void CPlane::InjectHooks() {
    RH_ScopedVirtualClass(CPlane, 0x871948, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6C8E20);
    RH_ScopedInstall(InitPlaneGenerationAndRemoval, 0x6CAD90);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6C9140);
    RH_ScopedVMTInstall(BurstTyre, 0x6C9150);
    RH_ScopedVMTInstall(PreRender, 0x6C94A0, { .Reversed = false });
    RH_ScopedVMTInstall(Render, 0x6CAB70);
    RH_ScopedInstall(IsAlreadyFlying, 0x6CAB90);
    RH_ScopedVMTInstall(Fix, 0x6CABB0);
    RH_ScopedVMTInstall(SetupDamageAfterLoad, 0x6CAC10);
    RH_ScopedInstall(SetGearUp, 0x6CAC20);
    RH_ScopedInstall(SetGearDown, 0x6CAC70);
    RH_ScopedVMTInstall(OpenDoor, 0x6CACB0);
    RH_ScopedVMTInstall(ProcessControl, 0x6C9260);
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6CADD0);
    RH_ScopedVMTInstall(ProcessFlyingCarStuff, 0x6CB7C0, { .Reversed = false });
    RH_ScopedVMTInstall(VehicleDamage, 0x6CC4B0);
    RH_ScopedInstall(CountPlanesAndHelis, 0x6CCA50);
    RH_ScopedInstall(AreWeInNoPlaneZone, 0x6CCAA0);
    RH_ScopedInstall(AreWeInNoBigPlaneZone, 0x6CCBB0);
    RH_ScopedInstall(SwitchAmbientPlanes, 0x6CCC50, { .Reversed = false });
    RH_ScopedVMTInstall(BlowUpCar, 0x6CCCF0);
    RH_ScopedInstall(FindPlaneCreationCoors, 0x6CD090, { .Reversed = false });
    RH_ScopedInstall(DoPlaneGenerationAndRemoval, 0x6CD2F0, { .Reversed = false });
}

// 0x6C8E20
CPlane::CPlane(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile(modelIndex, createdBy, true) {
    m_nVehicleSubType = VEHICLE_TYPE_PLANE;

    m_fLeftRightSkid               = 0.0f;
    m_fSteeringUpDown              = 0.0f;
    m_fSteeringLeftRight           = 0.0f;
    m_fAccelerationBreakStatus     = 0.0f;
    m_fAccelerationBreakStatusPrev = 1.0f;
    m_fPropSpeed                   = 0.0f;
    field_9C8                      = 0.0f;
    m_LandingGearAngle             = 0.0f;
    m_StallCounter                 = 0;
    m_planeCreationHeading         = 0.0f;
    m_planeHeading                 = 0.0f;
    m_planeHeadingPrev             = 0.0f;
    m_maxAltitude                  = 15.0f;
    m_altitude                     = 25.0f;
    m_minAltitude                  = 20.0f;
    m_forwardZ                     = 0;
    m_nStartedFlyingTime           = 0;
    m_fSteeringFactor              = 0.0f;

    if (m_nModelIndex != MODEL_VORTEX)
        physicalFlags.bDontCollideWithFlyers = true;

    m_nExtendedRemovalRange = 255;
    vehicleFlags.bNeverUseSmallerRemovalRange = true;
    vehicleFlags.bIsBig = true;

    auto& leftDoor = m_doors[DOOR_LEFT_FRONT];
    switch (modelIndex) {
    case MODEL_HYDRA:
    case MODEL_RUSTLER:
    case MODEL_CROPDUST:
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        leftDoor.Init((3.0f * PI) / 5.0f, 0.0f, DOOR_AXIS_NEG_X, DOOR_AXIS_Y, DOOR_EXTRA_BASED);
        break;
    case MODEL_SHAMAL:
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        leftDoor.Init(-((3.0f * PI) / 4.0f), 0.0f, DOOR_AXIS_Z, DOOR_AXIS_Y, DOOR_EXTRA_BASED);
        rwObjectSetFlags(GetFirstObject(m_aCarNodes[PLANE_WHEEL_LF]), 0);
        break;
    case MODEL_NEVADA:
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        leftDoor.Init(-TWO_PI / 5.0f, 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
        break;
    case MODEL_VORTEX:
        if (m_panels[FRONT_LEFT_PANEL].m_nFrameId == (uint16)-1)
            m_panels[FRONT_LEFT_PANEL].SetPanel(PLANE_GEAR_L, 1, -0.25f);
        break;
    case MODEL_STUNT:
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        leftDoor.Init((3.0f * PI) / 5.0f, 0.0f, DOOR_AXIS_NEG_X, DOOR_AXIS_Y, DOOR_EXTRA_BASED);
        rwObjectSetFlags(GetFirstObject(m_aCarNodes[PLANE_WHEEL_LB]), 0);
        rwObjectSetFlags(GetFirstObject(m_aCarNodes[PLANE_WHEEL_RB]), 0);
        break;
    }

    CVector modelPos, localPos;
    for (auto wheelId = 0; wheelId < 4; wheelId++) {
        GetVehicleModelInfo()->GetWheelPosn(wheelId, modelPos, false);
        GetVehicleModelInfo()->GetWheelPosn(wheelId, localPos, true);
        m_wheelPosition[wheelId] = m_wheelPosition[wheelId] - modelPos.z + localPos.z;
    }

    m_planeDamageWave = 0;
    m_pGunParticles = nullptr;
    m_nFiringMultiplier = 16;
    m_FireMissilePressedTime = 0u;
    m_LastMissileTarget = nullptr;
    m_LastHSMissileStruct = 0u; // originally not initialized
    m_apJettrusParticles.fill(nullptr);

    m_pSmokeParticle = nullptr;

    if (m_nModelIndex == MODEL_HYDRA)
        m_wMiscComponentAngle = HARRIER_NOZZLE_ROTATE_LIMIT;

    m_bSmokeEjectorEnabled = false;
}

// 0x6C9160
CPlane::~CPlane() {
    if (m_pGunParticles) {
        for (auto i = 0; i < CVehicle::GetPlaneNumGuns(); i++) {
            if (auto& particle = m_pGunParticles[i]) {
                particle->Kill();
                g_fxMan.DestroyFxSystem(particle);
            }
        }
        delete[] m_pGunParticles;
        m_pGunParticles = nullptr;
    }

    for (auto particle : m_apJettrusParticles) {
        if (particle) {
            FxSystem_c::KillAndClear(particle);
        }
    }

    FxSystem_c::SafeKillAndClear(m_pSmokeParticle);

    m_vehicleAudio.Terminate();
}

// 0x6CAD90
void CPlane::InitPlaneGenerationAndRemoval() {
    GenPlane_Status = 0;
    GenPlane_LastTimeGenerated = 0;
    GenPlane_Active = true;
}

// 0x6CCCF0
void CPlane::BlowUpCar(CEntity* damager, bool bHideExplosion) {
    if (!vehicleFlags.bCanBeDamaged) {
        return;
    }

    // 0x6CCD03 - An AI plane crashes first, and blows up when it hits something
    if (GetStatus() != STATUS_PLAYER && m_autoPilot.m_nCarMission != MISSION_PLANE_CRASH_AND_BURN && m_nModelIndex != MODEL_RCBARON) {
        m_autoPilot.SetCarMission(MISSION_PLANE_CRASH_AND_BURN);
        m_fHealth = 0.0f;
        return;
    }

    if (damager == FindPlayerPed() || damager == FindPlayerVehicle()) {
        auto& playerInfo = FindPlayerInfo();
        playerInfo.m_nHavocCaused += 20;
        playerInfo.m_fCurrentChaseValue += 10.0f;
        // NOTE: `CGeneral::GetRandomNumberInRange` isn't used, the original is `rand() % 6000 + 4000` here
        CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, (float)(CGeneral::GetRandomNumber() % 6'000 + 4'000));
    }

    if (GetStatus() == STATUS_PLAYER) { // 0x6CCDA6
        if (m_pDriver) {
            m_pDriver->bDontRender = true;
        }
        for (auto* const passenger : m_apPassengers) {
            if (passenger) {
                passenger->bDontRender = true;
            }
        }
        SetUsesCollision(false);
        SetIsVisible(false);
        ResetMoveSpeed();
        ResetTurnSpeed();
    }

    SetStatus(STATUS_WRECKED);
    physicalFlags.bRenderScorched = true;
    m_nTimeWhenBlowedUp           = CTimer::GetTimeInMS();

    CVisibilityPlugins::SetClumpForAllAtomicsFlag(GetRpClump(), eAtomicComponentFlag::ATOMIC_PIPE_NO_EXTRA_PASSES);
    m_damageManager.FuckCarCompletely(false);

    if (m_nModelIndex != MODEL_RCBARON) { // 0x6CCE99
        for (const auto bumper : { FRONT_BUMPER, REAR_BUMPER }) {
            SetBumperDamage(bumper, false);
        }
        for (const auto door : { DOOR_BONNET, DOOR_BOOT, DOOR_LEFT_FRONT, DOOR_RIGHT_FRONT, DOOR_LEFT_REAR, DOOR_RIGHT_REAR }) {
            SetDoorDamage(door, false);
        }
        SpawnFlyingComponent(CAR_WHEEL_LF, 1);
        if (auto* const wheel = m_aCarNodes[PLANE_WHEEL_LF]) {
            if (auto* const atomic = GetCurrentAtomicObject(wheel)) {
                RpAtomicSetFlags(atomic, 0);
            }
        }
    }

    m_nBombOnBoard = 0;
    m_fHealth      = 0.0f;
    m_wBombTimer   = 0;

    TheCamera.CamShake(0.4f, GetPosition());
    KillPedsInVehicle();

    vehicleFlags.bLightsOn     = false;
    vehicleFlags.bEngineOn     = false;
    vehicleFlags.bSirenOrAlarm = false;
    m_nOverrideLights          = NO_CAR_LIGHT_OVERRIDE;
    autoFlags.bTaxiLight       = false;
    m_bSmokeEjectorEnabled     = false;

    if (vehicleFlags.bIsAmbulanceOnDuty) { // 0x6CCFB9
        vehicleFlags.bIsAmbulanceOnDuty = false;
        CCarCtrl::NumAmbulancesOnDuty--;
    }
    if (vehicleFlags.bIsFireTruckOnDuty) {
        vehicleFlags.bIsFireTruckOnDuty = false;
        CCarCtrl::NumFireTrucksOnDuty--;
    }

    ChangeLawEnforcerState(false);
    gFireManager.StartFire(this, damager);
    CDarkel::RegisterCarBlownUpByPlayer(*this, 0);
    CExplosion::AddExplosion(
        this,
        damager,
        m_nModelIndex == MODEL_RCBARON ? EXPLOSION_RC_VEHICLE : EXPLOSION_AIRCRAFT,
        GetPosition(),
        0,
        true,
        -1.0f,
        false
    );
}

// 0x6CABB0
void CPlane::Fix() {
    m_damageManager.ResetDamageStatus();
    if (m_pHandlingData->m_bNoDoors) {
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_NOTPRESENT);
        m_damageManager.SetDoorStatus(DOOR_RIGHT_FRONT, DAMSTATE_NOTPRESENT);
        m_damageManager.SetDoorStatus(DOOR_LEFT_REAR, DAMSTATE_NOTPRESENT);
        m_damageManager.SetDoorStatus(DOOR_RIGHT_REAR, DAMSTATE_NOTPRESENT);
    }
    SetupDamageAfterLoad();
}

// 0x6CACB0
void CPlane::OpenDoor(CPed* ped, int32 componentId, eDoors door, float doorOpenRatio, bool playSound) {
    CAutomobile::OpenDoor(ped, componentId, door, doorOpenRatio, playSound);

    if (m_nModelIndex != MODEL_STUNT)
        return;

    // Unfinished code R*, which removed in Android
    if (false) // byte_C1CAFC
    {
        CMatrix matrix(RwFrameGetMatrix(m_aCarNodes[componentId]), false);
        const auto y = m_doors[door].m_angle - m_doors[door].m_prevAngle + matrix.GetPosition().y;
        matrix.SetTranslate({matrix.GetPosition().x, y, matrix.GetPosition().z});
        matrix.UpdateRW();
    }
}

// 0x6CAC10
void CPlane::SetupDamageAfterLoad() {
    vehicleFlags.bIsDamaged = false;
}

// 0x6CC4B0
void CPlane::VehicleDamage(float damageIntensity, eVehicleCollisionComponent component, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) {
    if (m_nModelIndex == MODEL_VORTEX) {
        return CAutomobile::VehicleDamage(damageIntensity, component, damager, vecCollisionCoors, vecCollisionDirection, weapon);
    }

    if (!vehicleFlags.bCanBeDamaged) {
        return;
    }

    // 0x6CC516 - Only the player can damage an invulnerable plane
    if (GetStatus() != STATUS_PLAYER && physicalFlags.bInvulnerable && m_pDamageEntity != FindPlayerPed() && m_pDamageEntity != FindPlayerVehicle()) {
        return;
    }

    const bool isCollision = damageIntensity == 0.0f;
    if (isCollision) { // 0x6CC569
        if (m_fDamageIntensity <= 0.0f || physicalFlags.bCollisionProof) {
            return;
        }
        if (m_pDamageEntity && m_pDamageEntity->GetIsTypePed()) {
            return;
        }
    } else { // 0x6CC5F7
        bool damagedDueToFireOrExplosionOrBullet = false;
        if (!CanVehicleBeDamaged(damager, weapon, damagedDueToFireOrExplosionOrBullet)) {
            return;
        }
        switch (weapon) {
        case WEAPON_ROCKET:
        case WEAPON_ROCKET_HS:
        case WEAPON_FREEFALL_BOMB:
        case WEAPON_EXPLOSION:
            break;
        default:
            return;
        }
    }

    // NOTE: The intensity of a weapon's damage isn't used (0x6CC634), it only damages the components
    const float          massFactor    = m_fMass / PLANE_DAMAGE_SCALE_MASS;
    const float          intensity     = isCollision ? m_fDamageIntensity : 0.0f;
    const float          minIntensity  = isCollision ? std::max(massFactor, 1.0f) * PLANE_DAMAGE_THRESHHOLD : PLANE_DAMAGE_THRESHHOLD;
    const float          blowIntensity = isCollision ? massFactor * PLANE_DAMAGE_DESTROY_THRESHHOLD : PLANE_DAMAGE_DESTROY_THRESHHOLD;
    const CVector* const collisionPos  = isCollision ? &m_vecLastCollisionPosn : vecCollisionCoors;

    // 0x6CC63C
    const float prevHealth = m_fHealth;
    const float damageMult = this == FindPlayerVehicle()
        ? (vehicleFlags.bTakeLessDamage ? 1.0f / 6.0f : 0.5f)
        : vehicleFlags.bTakeLessDamage
        ? 1.0f / 12.0f
        : m_pDamageEntity && m_pDamageEntity == FindPlayerVehicle()
        ? 2.0f / 3.0f
        : 0.25f;
    m_fHealth -= intensity * m_pHandlingData->m_fCollisionDamageMultiplier * damageMult;
    if (m_fHealth <= 0.0f && prevHealth > 0.0f) {
        m_fHealth = 1.0f;
    }

    // 0x6CC702
    if (intensity > blowIntensity || (m_fHealth <= 1.0f && prevHealth > 0.0f)) {
        return BlowUpCar(this, false);
    }

    // 0x6CC73D
    if (isCollision && intensity <= minIntensity) {
        return;
    }
    if (GetStatus() == STATUS_WRECKED || m_fMass <= 1000.0f) {
        return;
    }

    // 0x6CC778 - Find the component closest to the collision
    const CVector localPos    = GetMatrix().InverseTransformPoint(*collisionPos);
    float         closestDist = 1000.0f;
    int32         closest     = -1;
    for (int32 i = PLANE_STATIC_PROP; i < PLANE_NUM_NODES; i++) {
        if (!m_aCarNodes[i]) {
            continue;
        }
        const float distSq = (CVector{ RwFrameGetMatrix(m_aCarNodes[i])->pos } - localPos).SquaredMagnitude();
        if (distSq < sq(closestDist)) {
            closestDist = std::sqrt(distSq);
            closest     = i;
        }
    }
    if (closest == -1) {
        return;
    }

    // 0x6CC850 - A hit on the rudder can damage one of the elevators instead
    // NOTE: Not `CGeneral::DoCoinFlip()`, the original checks the lowest bit of `rand()`
    const int32 node = [&] {
        if (closest == PLANE_RUDDER && (CGeneral::GetRandomNumber() & 1)) {
            if (m_aCarNodes[PLANE_ELEVATOR_L] && (CGeneral::GetRandomNumber() & 1)) {
                return +PLANE_ELEVATOR_L;
            }
            if (m_aCarNodes[PLANE_ELEVATOR_R]) {
                return +PLANE_ELEVATOR_R;
            }
        }
        return closest;
    }();

    // 0x6CC887
    if (m_damageManager.ProgressAeroplaneDamage((uint8)node)) {
        if (m_damageManager.GetAeroplaneCompStatus((uint8)node) == DAMSTATE_OPENED && node >= PLANE_RUDDER && node <= PLANE_GEAR_R) {
            // NOTE/BUG: The original checks 4 panels, there are only 3 (The 4th one is the start of `m_swingingChassis`)
            const size_t numPanels = notsa::bugfixes::GenericOOB
                ? m_panels.size()
                : m_panels.size() + 1;
            for (size_t i = 0; i < numPanels; i++) {
                auto& panel = m_panels.data()[i];
                if (panel.m_nFrameId == (uint16)-1) {
                    panel.SetPanel((int16)node, 0, -0.02f);
                    break;
                }
                if ((int16)panel.m_nFrameId == node) {
                    break;
                }
            }
        } else if (m_damageManager.GetAeroplaneCompStatus((uint8)node) == DAMSTATE_DAMAGED) { // 0x6CC8F4
            SetComponentVisibility(m_aCarNodes[node], ATOMIC_DAMAGED);
        }
    }

    // 0x6CC911
    dmgDrawCarCollidingParticles(*collisionPos, intensity * (isCollision ? 1.0f : 0.333f), weapon);

    // 0x6CC92E - The damaged component starts to smoke
    const CVector nodePos = RwFrameGetMatrix(m_aCarNodes[node])->pos;
    if (m_pSmokeParticle) {
        m_pSmokeParticle->Kill();
        m_pSmokeParticle = nullptr;
    }
    auto* const modellingMat = GetModellingMatrix();
    if (!modellingMat) {
        return;
    }
    m_pSmokeParticle = g_fxMan.CreateFxSystem("fire_med", nodePos, modellingMat, false);
    if (!m_pSmokeParticle) {
        return;
    }
    m_pSmokeParticle->Play();
    m_pSmokeParticle->SetVelAdd(-m_vecMoveSpeed * 5.0f);
    m_pSmokeParticle->SetLocalParticles(true);
    m_nSmokeTimer = CGeneral::GetRandomNumberInRange(2'000, 4'000);
}

// 0x6CAB90
void CPlane::IsAlreadyFlying() {
    m_nStartedFlyingTime = CTimer::GetTimeInMS() - 20000;
}

// 0x6CAC20
void CPlane::SetGearUp() {
    m_LandingGearAngle = 1.0f;
    m_fAirResistance = m_pHandlingData->m_fDragMult / 1000.0f / 2.0f * m_pFlyingHandlingData->m_fGearUpR;
    m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_LEFT,  WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_LEFT,   WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_RIGHT, WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_RIGHT,  WHEEL_STATUS_MISSING);
}

// 0x6CAC70
void CPlane::SetGearDown() {
    m_LandingGearAngle = 0.0f;
    m_fAirResistance = m_pHandlingData->m_fDragMult / 1000.0f / 2.0f;
    m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_LEFT,  WHEEL_STATUS_OK);
    m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_LEFT,   WHEEL_STATUS_OK);
    m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_RIGHT, WHEEL_STATUS_OK);
    m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_RIGHT,  WHEEL_STATUS_OK);
}

// 0x6CCA50
uint32 CPlane::CountPlanesAndHelis() {
    uint32 counter = 0;
    for (auto& vehicle : GetVehiclePool()->GetAllValid()) {
        if (vehicle.IsSubHeli() || vehicle.IsSubPlane()) {
            counter++;
        }
    }
    return counter;
}

// 0x6CCAA0
bool CPlane::AreWeInNoPlaneZone() {
    const auto& camPos = TheCamera.GetPosition();
    constexpr CVector vec1 = { -1073.0f, -675.0f, 50.0f };

    return DistanceBetweenPoints(vec1, camPos) < 200.0f ||
           camPos.x > -2743.0f && camPos.x < -2626.0f && camPos.y > 1300.0f && camPos.y < 2200.0f || // todo: Is point inside
           camPos.x > -1668.0f && camPos.x < -1122.0f && camPos.y > 541.0f && camPos.y < 1118.0f;
}

// 0x6CCBB0
bool CPlane::AreWeInNoBigPlaneZone() {
    // untested
    const auto& camPos = TheCamera.GetPosition();
    return DistanceBetweenPoints2D({ +1522.0f, -1237.0f }, camPos) < 800.0f ||
           DistanceBetweenPoints2D({ -1836.0f, +659.0f }, camPos) < 800.0f;
}

// 0x6CCC50
void CPlane::SwitchAmbientPlanes(bool enable) {
    plugin::Call<0x6CCC50, bool>(enable);
}

// 0x6CD090
void CPlane::FindPlaneCreationCoors(CVector* outCoors, CVector* outTargetCoors, float* outPlaneOrientation, float* outFlightHeight, bool isBigPlane) {
    plugin::Call<0x6CD090, CVector*, CVector*, float*, float*, bool>(outCoors, outTargetCoors, outPlaneOrientation, outFlightHeight, isBigPlane);
}

// 0x6CD2F0
void CPlane::DoPlaneGenerationAndRemoval() {
    plugin::Call<0x6CD2F0>();
}

// 0x6C9140
bool CPlane::SetUpWheelColModel(CColModel* wheelCol) {
    return false;
}

// 0x6C9150
bool CPlane::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    return false;
}

// 0x6C94A0
void CPlane::PreRender() {
    plugin::CallMethod<0x6C94A0, CPlane*>(this);
}

// 0x6CAB70
void CPlane::Render() {
    m_nTimeTillWeNeedThisCar = CTimer::GetTimeInMS() + 3000;
    CVehicle::Render();
}

// 0x6C9260
void CPlane::ProcessControl() {
    if (GetStatus() == STATUS_PLAYER && notsa::contains({ MODEL_CROPDUST, MODEL_STUNT }, GetModelId())) {
        if (CPad::GetPad(m_pDriver && m_pDriver->m_nPedType == PED_TYPE_PLAYER2 ? 1 : 0)->IsRightShockPressed()) {
            m_bSmokeEjectorEnabled = !m_bSmokeEjectorEnabled;
        }
    }
    if (m_bSmokeEjectorEnabled && (!vehicleFlags.bEngineOn || vehicleFlags.bIsDrowning || !m_pDriver)) { // 0x6C92BF
        m_bSmokeEjectorEnabled = false;
    }

    if (m_nModelIndex == MODEL_SKIMMER) { // 0x6C92EC
        m_damageManager.SetAllWheelsState(WHEEL_STATUS_MISSING);
    }

    CAutomobile::ProcessControl();

    m_vehicleAudio.m_DoCountStalls = (int16)m_StallCounter;
    m_StallCounter                 = 0;

    ProcessWeapons();

    if (m_nModelIndex == MODEL_VORTEX) { // 0x6C934D
        m_WheelStates.fill(WHEEL_STATE_NORMAL);
    }

    // 0x6C936F - Smoke of a damaged component
    if (m_pSmokeParticle) {
        m_nSmokeTimer += (int32)(CTimer::GetTimeStep() * 0.02f * -1000.0f);

        RwMatrix smokeMat;
        m_pSmokeParticle->GetCompositeMatrix(&smokeMat);
        const CVector     smokeVel = -m_vecMoveSpeed * 5.0f;
        const FxPrtMult_c smokeMult{ 0.0f, 0.0f, 0.0f, 0.2f, 1.0f, 1.0f, 0.1f };
        g_fx.m_SmokeHuge->AddParticle(smokeMat.pos, smokeVel, 0.0f, smokeMult);
        g_fx.m_SmokeHuge->AddParticle(smokeMat.pos, smokeVel, 0.05f, smokeMult);

        if (m_nSmokeTimer <= 0 || vehicleFlags.bIsDrowning) {
            m_pSmokeParticle->Kill();
            m_pSmokeParticle = nullptr;
        }
    }
}

// 0x6CADD0
void CPlane::ProcessControlInputs(uint8 playerNum) {
    constexpr float PAD_STEER_RATE          = 0.2f;
    constexpr float MOUSE_STEER_SCALE       = 0.0015f;
    constexpr float MOUSE_STEER_DAMP_BELOW  = 0.15f;
    constexpr float MOUSE_STEER_DAMP        = 0.99f;
    constexpr float GEAR_MOVE_RATE          = 0.02f;
    constexpr float HARRIER_NOZZLE_DEADZONE = 10.0f;
    constexpr float DISABLED_MAX_SPEED      = 0.28f;

    auto* const   pad                       = CPad::GetPad(playerNum);
    const float   timeStep                  = CTimer::GetTimeStep();
    const CVector forward                   = GetForward();
    // NOTE: Not `Dot()`, the original sums in this order
    const float fwdSpeed = m_vecMoveSpeed.z * forward.z + m_vecMoveSpeed.y * forward.y + m_vecMoveSpeed.x * forward.x;

    // 0x6CADF4 - Roll and pitch
    const int16 padLeftRight = pad->GetSteeringLeftRight();
    const int16 padUpDown    = pad->GetSteeringUpDown();
    const auto  SteerWithPad = [&] {
        CVehicle::m_nLastControlInput = eControllerType::KEYBOARD;
        m_fSteeringLeftRight += ((float)padLeftRight / 128.0f - m_fSteeringLeftRight) * timeStep * PAD_STEER_RATE;
        m_fSteeringUpDown += ((float)padUpDown / 128.0f - m_fSteeringUpDown) * timeStep * PAD_STEER_RATE;
    };
    if (CCamera::m_bUseMouse3rdPerson && CVehicle::m_bEnableMouseFlying) {
        const CVector2D mouseMoved   = CPad::NewMouseControllerState.GetAmountMouseMoved();
        const bool      isPadSteered = padLeftRight != 0 || padUpDown != 0;
        const bool      wasMouseLast = CVehicle::m_nLastControlInput == eControllerType::MOUSE;
        const bool      isSteering   = std::fabs(m_fSteeringLeftRight) > 0.0f || std::fabs(m_fSteeringUpDown) > 0.0f;
        if (mouseMoved.x != 0.0f || mouseMoved.y != 0.0f || (isSteering && wasMouseLast && !isPadSteered)) { // 0x6CAF6B
            CVehicle::m_nLastControlInput = eControllerType::MOUSE;
            if (!pad->NewState.m_bVehicleMouseLook) {
                m_fSteeringLeftRight += mouseMoved.x * MOUSE_STEER_SCALE;
                m_fSteeringUpDown += mouseMoved.y * MOUSE_STEER_SCALE;
            }
            // NOTE: The original multiplies by the unrounded (x87) result of `pow`, so it's done in double precision here
            const double damping = std::pow((double)MOUSE_STEER_DAMP, (double)timeStep);
            if (std::fabs(m_fSteeringLeftRight) < MOUSE_STEER_DAMP_BELOW) {
                m_fSteeringLeftRight = (float)(damping * m_fSteeringLeftRight);
            }
            if (std::fabs(m_fSteeringUpDown) < MOUSE_STEER_DAMP_BELOW) {
                m_fSteeringUpDown = (float)(damping * m_fSteeringUpDown);
            }
        } else if (isPadSteered || !wasMouseLast) { // 0x6CAEF1
            SteerWithPad();
        }
    } else { // 0x6CB02B
        SteerWithPad();
    }
    m_fSteeringLeftRight = std::clamp(m_fSteeringLeftRight, -1.0f, 1.0f);
    m_fSteeringUpDown    = std::clamp(m_fSteeringUpDown, -1.0f, 1.0f);

    // 0x6CB16B - Yaw
    const float yawInput = m_nModelIndex == MODEL_VORTEX
        ? (float)padLeftRight / 128.0f
        : pad->GetLookLeft()
        ? -1.0f
        : (float)pad->GetLookRight();
    m_fLeftRightSkid     = std::clamp(m_fLeftRightSkid + (yawInput - m_fLeftRightSkid) * timeStep * PAD_STEER_RATE, -1.0f, 1.0f);

    // 0x6CB260
    // NOTE: Not `DegreesToRadians()`, the original multiplies in this order
    m_fSteerAngle = -(m_pHandlingData->m_fSteeringLock * DEG_TO_RAD * m_fLeftRightSkid * std::max(1.0f - fwdSpeed, 0.1f));
    if (m_nModelIndex == MODEL_VORTEX && pad->GetHandBrake()) { // 0x6CB2A9
        m_fSteerAngle *= 1.5f;
        m_fLeftRightSkid *= 1.5f;
    }

    m_fAccelerationBreakStatus = (float)(pad->GetAccelerate() - pad->GetBrake()) * (1.0f / 255.0f);

    // 0x6CB310 - Landing gear (`m_LandingGearAngle`: 0 = down, 1 = up, below 0 = moving down, between = moving up)
    const bool canToggleGear = pad->IsRightShockPressed()
        && !IsAnyWheelMakingContactWithGround()
        && m_pFlyingHandlingData->m_fGearUpR < 1.0f
        && m_nModelIndex != MODEL_RCBARON;
    if (canToggleGear) {
        if (m_LandingGearAngle == 0.0f) { // 0x6CB3AD
            m_damageManager.SetAllWheelsState(WHEEL_STATUS_MISSING);
            m_LandingGearAngle += timeStep * GEAR_MOVE_RATE;
        } else if (m_LandingGearAngle == 1.0f) { // 0x6CB3EE
            m_LandingGearAngle = timeStep * GEAR_MOVE_RATE - 1.0f;
        } else { // 0x6CB40B
            m_LandingGearAngle *= -1.0f;
        }
    } else if (m_LandingGearAngle < 0.0f) { // 0x6CB435
        m_LandingGearAngle += timeStep * GEAR_MOVE_RATE;
        if (m_LandingGearAngle >= 0.0f) {
            SetGearDown();
        }
    } else if (m_LandingGearAngle > 0.0f && m_LandingGearAngle < 1.0f) { // 0x6CB4C7
        m_LandingGearAngle += timeStep * GEAR_MOVE_RATE;
        if (m_LandingGearAngle >= 1.0f) {
            SetGearUp();
        }
    }

    // 0x6CB4F3 - Hydra's thrust nozzles
    if (m_nModelIndex == MODEL_HYDRA) {
        if (const int16 nozzleInput = pad->GetCarGunUpDown(); std::fabs((float)nozzleInput) > HARRIER_NOZZLE_DEADZONE) {
            m_wMiscComponentAnglePrev = m_wMiscComponentAngle;
            const int32 angle         = (int16)(int32)((float)nozzleInput * timeStep * HARRIER_NOZZLE_ROTATERATE / 128.0f) + (int32)m_wMiscComponentAngle;
            m_wMiscComponentAngle     = angle < 0 ? 0 : (uint16)angle;
            if ((int32)m_wMiscComponentAngle > (int16)HARRIER_NOZZLE_ROTATE_LIMIT) {
                m_wMiscComponentAngle = HARRIER_NOZZLE_ROTATE_LIMIT;
            }
        }
    }

    // 0x6CB5EA - Brakes (Only on the ground, the thrust uses `m_fAccelerationBreakStatus`)
    vehicleFlags.bIsHandbrakeOn = false;
    m_GasPedal                  = 0.0f;
    if (fwdSpeed > 0.0f && pad->GetBrake() != 0) {
        m_BrakePedal = (float)pad->GetBrake() * (fwdSpeed > 0.35f ? 0.5f / 255.0f : 1.0f / 255.0f);
    } else if (m_vecMoveSpeed.Magnitude() < 0.1f && pad->GetBrake() < 10 && pad->GetAccelerate() < 10) { // 0x6CB68D
        m_BrakePedal = 0.5f;
    } else {
        m_BrakePedal = 0.0f;
    }

    // 0x6CB6FC
    if (pad->DisablePlayerControls) {
        vehicleFlags.bIsHandbrakeOn = true;
        m_BrakePedal                = 1.0f;
        m_GasPedal                  = 0.0f;
        FindPlayerPed()->KeepAreaAroundPlayerClear();
        if (const float speed = m_vecMoveSpeed.Magnitude(); speed > DISABLED_MAX_SPEED) {
            m_vecMoveSpeed *= DISABLED_MAX_SPEED / speed;
        }
    }
}

// 0x6CB7C0
void CPlane::ProcessFlyingCarStuff() {
    plugin::CallMethod<0x6CB7C0, CPlane*>(this);
}
