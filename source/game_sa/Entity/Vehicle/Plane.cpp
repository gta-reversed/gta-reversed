#include "StdInc.h"

#include "Plane.h"
#include "CarCtrl.h"
#include "FireManager.h"

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
    RH_ScopedVMTInstall(PreRender, 0x6C94A0, { .reversed = false });
    RH_ScopedVMTInstall(Render, 0x6CAB70);
    RH_ScopedInstall(IsAlreadyFlying, 0x6CAB90);
    RH_ScopedVMTInstall(Fix, 0x6CABB0);
    RH_ScopedVMTInstall(SetupDamageAfterLoad, 0x6CAC10);
    RH_ScopedInstall(SetGearUp, 0x6CAC20);
    RH_ScopedInstall(SetGearDown, 0x6CAC70);
    RH_ScopedVMTInstall(OpenDoor, 0x6CACB0);
    RH_ScopedVMTInstall(ProcessControl, 0x6C9260);
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6CADD0, { .reversed = false });
    RH_ScopedVMTInstall(ProcessFlyingCarStuff, 0x6CB7C0, { .reversed = false });
    RH_ScopedVMTInstall(VehicleDamage, 0x6CC4B0, { .reversed = false });
    RH_ScopedInstall(CountPlanesAndHelis, 0x6CCA50);
    RH_ScopedInstall(AreWeInNoPlaneZone, 0x6CCAA0);
    RH_ScopedInstall(AreWeInNoBigPlaneZone, 0x6CCBB0);
    RH_ScopedInstall(SwitchAmbientPlanes, 0x6CCC50);
    RH_ScopedVMTInstall(BlowUpCar, 0x6CCCF0);
    RH_ScopedInstall(FindPlaneCreationCoors, 0x6CD090);
    RH_ScopedInstall(DoPlaneGenerationAndRemoval, 0x6CD2F0, { .reversed = false });
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
    m_fLandingGearStatus           = 0.0f;
    field_9A0                      = 0;
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
    field_9DC = 0;
    field_9E0 = 0;
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
    if (!vehicleFlags.bCanBeDamaged)
        return;

    // Check if this is a mission plane that should crash-and-burn instead of exploding immediately.
    // (The actual binary checks raw bytes here: `TEST [ESI+0x36],0xF8` etc, which don't map cleanly
    // onto the `m_nStatus` bitfield - matching the observable behavior instead. See disasm @ 0x6CCD03.)
    if (GetStatus() != STATUS_PLAYER && m_autoPilot.m_nCarMission != MISSION_PLANE_CRASH_AND_BURN && m_nModelIndex != MODEL_RCBARON) {
        m_autoPilot.SetCarMission(MISSION_PLANE_CRASH_AND_BURN);
        m_fHealth = 0.0f;
        return;
    }

    if (damager == FindPlayerPed() || damager == FindPlayerVehicle()) {
        FindPlayerInfo().m_nHavocCaused += 20;
        FindPlayerInfo().m_fCurrentChaseValue += 10.0f;
        CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, (float)CGeneral::GetRandomNumberInRange(4000, 10'000));
    }

    if (GetStatus() == STATUS_PLAYER) {
        if (m_pDriver) {
            m_pDriver->bDontRender = true;
        }
        for (auto& passenger : m_apPassengers) {
            if (passenger) {
                passenger->bDontRender = true;
            }
        }
        // NOTSA: assembly `AND [ESI+0x1C],0xFFFFFF7E` - m_nFlags @ 0x1C
        *(uint32*)((uint8*)this + 0x1C) &= 0xFFFFFF7E;
        ResetMoveSpeed();
        ResetTurnSpeed();
    }

    // NOTSA: inlined `SetStatus(STATUS_WRECKED)` - assembly:
    //   MOV DL,[ESI+0x36] ; AND DL,0x7 ; OR DL,0x28 ; MOV [ESI+0x36],DL
    *(uint8*)((uint8*)this + 0x36) = (*(uint8*)((uint8*)this + 0x36) & 7) | 0x28;
    physicalFlags.bRenderScorched = true;
    m_nTimeWhenBlowedUp = CTimer::GetTimeInMS();
    CVisibilityPlugins::SetClumpForAllAtomicsFlag(GetRpClump(), eAtomicComponentFlag::ATOMIC_PIPE_NO_EXTRA_PASSES_LOD);
    m_damageManager.FuckCarCompletely(false);
    if (m_nModelIndex != MODEL_RCBARON) {
        CAutomobile::SetBumperDamage(FRONT_BUMPER, false);
        CAutomobile::SetBumperDamage(REAR_BUMPER, false);
        CAutomobile::SetDoorDamage(DOOR_BONNET, false);
        CAutomobile::SetDoorDamage(DOOR_BOOT, false);
        CAutomobile::SetDoorDamage(DOOR_LEFT_FRONT, false);
        CAutomobile::SetDoorDamage(DOOR_RIGHT_FRONT, false);
        CAutomobile::SetDoorDamage(DOOR_LEFT_REAR, false);
        CAutomobile::SetDoorDamage(DOOR_RIGHT_REAR, false);
        CAutomobile::SpawnFlyingComponent(static_cast<eCarNodes>(PLANE_WHEEL_LF), 1);

        if (auto node = m_aCarNodes[PLANE_WHEEL_LF]) {
            RwObject* atomic = nullptr;
            RwFrameForAllObjects(node, GetCurrentAtomicObjectCB, &atomic);
            if (atomic) {
                rwObjectSetFlags(atomic, 0);
            }
        }
    }
    // NOTSA: raw writes matching the binary (`MOV [ESI+0x4DE],BX`, `AND [ESI+0x4A8],0xF8`) -
    // these offsets overlap bitfields with no clean named mapping, so match the bytes exactly.
    *(uint16*)((uint8*)this + 0x4DE) = 0;
    *(uint8*)((uint8*)this + 0x4A8) &= 0xF8;
    m_fHealth = 0.0f;
    m_wBombTimer = 0;

    TheCamera.CamShake(0.4f, GetPosition());
    KillPedsInVehicle();
    // NOTSA: more raw flag writes from the disasm (`AND [ESI+0x428],0xAF`, `AND [ESI+0x4A8],0xE7`,
    // `AND [ESI+0x42D],0x7F`, `AND [ESI+0x868],0xFE`) - no clean named fields, matching bytes.
    *(uint8*)((uint8*)this + 0x428) &= 0xAF;
    *(uint8*)((uint8*)this + 0x4A8) &= 0xE7;
    *(uint8*)((uint8*)this + 0x42D) &= 0x7F;
    *(uint8*)((uint8*)this + 0x868) &= 0xFE;
    m_bSmokeEjectorEnabled = false;

    if (vehicleFlags.bIsAmbulanceOnDuty) {
        vehicleFlags.bIsAmbulanceOnDuty = false;
        --CCarCtrl::NumAmbulancesOnDuty;
    }

    if (vehicleFlags.bIsFireTruckOnDuty) {
        vehicleFlags.bIsFireTruckOnDuty = false;
        --CCarCtrl::NumFireTrucksOnDuty;
    }

    ChangeLawEnforcerState(false);
    gFireManager.StartFire(this, damager, 0.8f, 1, 7000, 0);
    CDarkel::RegisterCarBlownUpByPlayer(*this, 0);
    if (m_nModelIndex == MODEL_RCBARON) {
        CExplosion::AddExplosion(this, damager, EXPLOSION_RC_VEHICLE, GetPosition(), 0, 1, -1.0f, 0);
    } else {
        CExplosion::AddExplosion(this, damager, EXPLOSION_AIRCRAFT, GetPosition(), 0, 1, -1.0f, 0);
    }
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
    plugin::CallMethod<0x6CC4B0, CPlane*, float, eVehicleCollisionComponent, CEntity*, CVector*, CVector*, eWeaponType>(this, damageIntensity, component, damager, vecCollisionCoors, vecCollisionDirection, weapon);
}

// 0x6CAB90
void CPlane::IsAlreadyFlying() {
    m_nStartedFlyingTime = CTimer::GetTimeInMS() - 20000;
}

// 0x6CAC20
void CPlane::SetGearUp() {
    m_fLandingGearStatus = 1.0f;
    m_fAirResistance = m_pHandlingData->m_fDragMult / 1000.0f / 2.0f * m_pFlyingHandlingData->m_fGearUpR;
    m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_LEFT,  WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_LEFT,   WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_RIGHT, WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_RIGHT,  WHEEL_STATUS_MISSING);
}

// 0x6CAC70
void CPlane::SetGearDown() {
    m_fLandingGearStatus = 0.0f;
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
    if (!GenPlane_Active) {
        GenPlane_Active = enable;
        return;
    }
    if (!enable) {
        for (auto& vehicle : GetVehiclePool()->GetAllValid()) {
            if (vehicle.IsSubHeli() || vehicle.IsSubPlane()) {
                if (vehicle.m_autoPilot.m_nCarMission == MISSION_PLANE_CRASH_AND_BURN) {
                    CWorld::Remove(&vehicle);
                    delete &vehicle;
                }
            }
        }
    }
    GenPlane_Active = enable;
}

// 0x6CD090
void CPlane::FindPlaneCreationCoors(CVector* outCoors, CVector* outTargetCoors, float* outPlaneOrientation, float* outFlightHeight, bool isBigPlane) {
    float spawnRadius, baseHeight;
    if (!isBigPlane) {
        spawnRadius = 140.0f;
        baseHeight = 25.0f;
    } else {
        spawnRadius = 340.0f;
        baseHeight = 200.0f;
    }
    int32 attempt = 0;
    for (; attempt < 500; attempt += 10) {
        const float angle = (float)(CGeneral::GetRandomNumber() % 360) * DEG_TO_RAD;
        *outCoors = FindPlayerCoors();
        *outFlightHeight = (float)(CGeneral::GetRandomNumber() & 0xF) + (float)attempt + baseHeight;
        outCoors->x += std::cos(angle) * spawnRadius;
        outCoors->y += std::sin(angle) * spawnRadius;
        outCoors->z += *outFlightHeight;
        *outTargetCoors = FindPlayerCoors();
        const float targetDist = (float)(CGeneral::GetRandomNumber() & 0x1F) + 20.0f;
        // NOTSA: raw static @ 0xB6F9AC - camera/plane target direction vector, no clean named mapping
        const auto dir = *reinterpret_cast<CVector*>(0xB6F9AC);
        outTargetCoors->x += dir.x * targetDist;
        outTargetCoors->y += dir.y * targetDist;
        outTargetCoors->z += dir.z * targetDist + *outFlightHeight;
        *outPlaneOrientation = CGeneral::GetATanOfXY(outTargetCoors->x - outCoors->x, outTargetCoors->y - outCoors->y);
        CVector lineEnd = *outTargetCoors + (*outTargetCoors - *outCoors);
        CWorld::AdvanceCurrentScanCode();
        CColSphere sphere(*outCoors, 16.0f);
        if (CWorld::GetIsLineOfSightClear(*outCoors, lineEnd, true, false, false, false, false, false, false)) {
            // NOTSA: the binary builds raw sphere/box structs on the stack and calls
            // `CCollision::CheckCameraCollisionBuildings` with them - no clean high-level mapping,
            // matching the observable behavior (break iff no building blocks the spawn line).
            CColSphere spS(lineEnd, 16.0f), spA(lineEnd, 16.0f), spB(lineEnd, 16.0f);
            CColBox box(spA.m_vecCenter - CVector(16.0f, 16.0f, 16.0f), spA.m_vecCenter + CVector(16.0f, 16.0f, 16.0f));
            if (!CCollision::CheckCameraCollisionBuildings(0, 0, box, spS, spA, spB)) {
                break;
            }
        }
    }
    *outFlightHeight += 20.0f;
    outTargetCoors->z += 20.0f;
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
    if (GetStatus() == STATUS_PLAYER) {
        if (m_nModelIndex == MODEL_CROPDUST || m_nModelIndex == MODEL_STUNT) {
            const auto pad = CPad::GetPad(m_pDriver->GetPadNumber());
            if (pad->IsRightShockPressed()) {
                m_bSmokeEjectorEnabled = !m_bSmokeEjectorEnabled;
            }
        }
    }

    if (m_bSmokeEjectorEnabled) {
        if (!vehicleFlags.bEngineOn || vehicleFlags.bIsDrowning || !m_pDriver) {
            m_bSmokeEjectorEnabled = false;
        }
    }

    if (m_nModelIndex == MODEL_SKIMMER) {
        m_damageManager.SetAllWheelsState(WHEEL_STATUS_MISSING);
    }

    CAutomobile::ProcessControl();

    m_vehicleAudio.m_DoCountStalls = static_cast<int16>(field_9A0);
    if (field_9A0) {
        field_9A0 = 0;
    }

    CVehicle::ProcessWeapons();
    if (m_nModelIndex == MODEL_VORTEX) {
        m_WheelStates[0] = WHEEL_STATE_NORMAL;
        m_WheelStates[1] = WHEEL_STATE_NORMAL;
        m_WheelStates[2] = WHEEL_STATE_NORMAL;
        m_WheelStates[3] = WHEEL_STATE_NORMAL;
    }

    if (m_pSmokeParticle) {
        RwMatrix out;
        m_nSmokeTimer += static_cast<uint32>(-CTimer::GetTimeStepInMS());
        m_pSmokeParticle->GetCompositeMatrix(&out);
        const CVector velocity = -m_vecMoveSpeed * 5.0f;
        const auto particleData = FxPrtMult_c(0.0f, 0.0f, 0.0f, 0.2f, 1.0f, 1.0f, 0.1f);
        g_fx.m_SmokeHuge->AddParticle(out.pos, velocity, 0.0f, particleData);
        g_fx.m_SmokeHuge->AddParticle(out.pos, velocity, 0.05f, particleData);
        if (m_nSmokeTimer <= 0 || vehicleFlags.bIsDrowning) {
            m_pSmokeParticle->Kill();
            m_pSmokeParticle = nullptr;
        }
    }
}

// 0x6CADD0
void CPlane::ProcessControlInputs(uint8 playerNum) {
    plugin::CallMethod<0x6CADD0, CPlane*, uint8>(this, playerNum);
}

// 0x6CB7C0
void CPlane::ProcessFlyingCarStuff() {
    plugin::CallMethod<0x6CB7C0, CPlane*>(this);
}
