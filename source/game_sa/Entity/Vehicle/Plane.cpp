#include "StdInc.h"

#include "Plane.h"
#include "CarCtrl.h"
#include "FireManager.h"

int32& CPlane::GenPlane_ModelIndex = *(int32*)0xC1CAD8;
uint32& CPlane::GenPlane_Status = *(uint32*)0xC1CADC;
uint32& CPlane::GenPlane_LastTimeGenerated = *(uint32*)0xC1CAE0;

bool& CPlane::GenPlane_Active = *(bool*)0x8D33BC;                 // true
float& CPlane::ANDROM_COL_ANGLE_MULT = *(float*)0x8D33C0;         // 0.00015f
uint16& CPlane::HARRIER_NOZZLE_ROTATE_LIMIT = *(uint16*)0x8D33C4; // 5000
uint16& CPlane::HARRIER_NOZZLE_SWITCH_LIMIT = *(uint16*)0x8D33C8; // 3000
float& CPlane::PLANE_MIN_PROP_SPEED = *(float*)0x8D33CC;          // 0.05f
float& CPlane::PLANE_STD_PROP_SPEED = *(float*)0x8D33D0;          // 0.18f
float& CPlane::PLANE_MAX_PROP_SPEED = *(float*)0x8D33D4;          // 0.34f
float& CPlane::PLANE_ROC_PROP_SPEED = *(float*)0x8D33D8;          // 0.01f

float& HARRIER_NOZZLE_ROTATERATE = *(float*)0x8D33DC;       // 25.0f
float& PLANE_DAMAGE_WAVE_COUNTER_VAR = *(float*)0x8D33E0;   // 0.75f
float& PLANE_DAMAGE_THRESHHOLD = *(float*)0x8D33E4;         // 500.0f
float& PLANE_DAMAGE_SCALE_MASS = *(float*)0x8D33E8;         // 10000.0f
float& PLANE_DAMAGE_DESTROY_THRESHHOLD = *(float*)0x8D33EC; // 5000.0f
CVector& vecRCBaronGunPos = *(CVector*)0x8D33F0;            // <0.0f, 0.45f, 0.0f>

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
    RH_ScopedVMTInstall(ProcessControl, 0x6C9260, { .reversed = false });
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6CADD0, { .reversed = false });
    RH_ScopedVMTInstall(ProcessFlyingCarStuff, 0x6CB7C0, { .reversed = false });
    RH_ScopedVMTInstall(VehicleDamage, 0x6CC4B0, { .reversed = false });
    RH_ScopedInstall(CountPlanesAndHelis, 0x6CCA50);
    RH_ScopedInstall(AreWeInNoPlaneZone, 0x6CCAA0);
    RH_ScopedInstall(AreWeInNoBigPlaneZone, 0x6CCBB0);
    RH_ScopedInstall(SwitchAmbientPlanes, 0x6CCC50, { .reversed = false });
    RH_ScopedVMTInstall(BlowUpCar, 0x6CCCF0, { .reversed = false });
    RH_ScopedInstall(FindPlaneCreationCoors, 0x6CD090, { .reversed = false });
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
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK); // todo: add func(openAngle, closedAngle, axis, dir)
        leftDoor.m_fOpenAngle = (3.0f * PI) / 5.0f;
        leftDoor.m_fClosedAngle = 0.0f;
        leftDoor.m_nAxis = 1;
        leftDoor.m_nDirn = 19;
        break;
    case MODEL_SHAMAL:
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        leftDoor.m_fOpenAngle = -((3.0f * PI) / 4.0f);
        leftDoor.m_fClosedAngle = 0.0f;
        leftDoor.m_nAxis = 1;
        leftDoor.m_nDirn = 18;
        rwObjectSetFlags(GetFirstObject(m_aCarNodes[PLANE_WHEEL_LF]), 0);
        break;
    case MODEL_NEVADA:
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        leftDoor.m_fOpenAngle = -((2.0f * PI) / 5.0f);
        leftDoor.m_fClosedAngle = 0.0f;
        leftDoor.m_nAxis = 2;
        leftDoor.m_nDirn = 20;
        break;
    case MODEL_VORTEX:
        if (m_panels[FRONT_LEFT_PANEL].m_nFrameId == (uint16)-1)
            m_panels[FRONT_LEFT_PANEL].SetPanel(PLANE_GEAR_L, 1, -0.25f);
        break;
    case MODEL_STUNT:
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        leftDoor.m_fOpenAngle = (3.0f * PI) / 5.0f;
        leftDoor.m_fClosedAngle = 0.0f;
        leftDoor.m_nAxis = 1;
        leftDoor.m_nDirn = 19;
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
    return plugin::CallMethod<0x6CCCF0, CPlane*, CEntity*, uint8>(this, damager, bHideExplosion);

    // untested \ wip
    if (!vehicleFlags.bCanBeDamaged)
        return;

    if (m_nStatus == STATUS_PLAYER || m_autoPilot.m_nCarMission == MISSION_CRASH_PLANE_AND_BURN || m_nModelIndex == MODEL_RCBARON) {
        if (damager == FindPlayerPed() || damager == FindPlayerVehicle()) {
            FindPlayerInfo().m_nHavocCaused += 20;
            FindPlayerInfo().m_fCurrentChaseValue += 10.0f;
            CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, (float)CGeneral::GetRandomNumberInRange(4000, 10'000));
        }

        if (m_nStatus == STATUS_PLAYER) { // strange
            if (m_pDriver) {
                m_pDriver->bDontRender = true;
            }
            for (auto& passenger : m_apPassengers) {
                if (passenger) {
                    passenger->bDontRender = true;
                }
            }
            m_nFlags &= 0xFFFFFF7E;
            ResetMoveSpeed();
            ResetTurnSpeed();
        }

        // m_nType = m_nType & 7 | STATUS_WRECKED;
        physicalFlags.bDestroyed = true;
        m_nTimeWhenBlowedUp = CTimer::GetTimeInMS();
        CVisibilityPlugins::SetClumpForAllAtomicsFlag(m_pRwClump, ATOMIC_IS_BLOWN_UP);
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

            // todo: shit
            if (auto node = m_aCarNodes[PLANE_WHEEL_LF]) {
                damager = 0;
                RwFrameForAllObjects(node, GetCurrentAtomicObjectCB, &damager);
                if (damager) {
                    rwObjectSetFlags(damager, 0); // RpAtomic* damager
                }
            }
        }
        // this->m_nBombLightsWinchFlags &= 0xF8u;
        m_fHealth = 0.0f;
        m_wBombTimer = 0;

        TheCamera.CamShake(0.4f, GetPosition());
        KillPedsInVehicle();
        // auto v23 = this->m_nFlags1 & 0xAF;
        // this->m_nBombLightsWinchFlags &= 0xE7u;
        // this->m_nFlags1 = v23;
        m_bSmokeEjectorEnabled = false;
        // this->m_nFlags6 = m_nFlags6 & 0x7F; // vehicleFlags.bSirenOrAlarm = false;
        // this->ucNPCVehicleFlags = this->ucNPCVehicleFlags & 0xFE;

        if (vehicleFlags.bIsAmbulanceOnDuty) {
            vehicleFlags.bIsAmbulanceOnDuty = false;
            --CCarCtrl::NumAmbulancesOnDuty;
        }

        if (vehicleFlags.bIsFireTruckOnDuty) {
            vehicleFlags.bIsFireTruckOnDuty = false;
            --CCarCtrl::NumFireTrucksOnDuty;
        }

        ChangeLawEnforcerState(0);
        gFireManager.StartFire(this, damager, 0.8f, 1, 7000, 0);
        CDarkel::RegisterCarBlownUpByPlayer(*this, 0);
        if (m_nModelIndex == MODEL_RCBARON) {
            CExplosion::AddExplosion(this, damager, EXPLOSION_RC_VEHICLE, GetPosition(), 0, 1, -1.0f, 0);
        } else {

            CExplosion::AddExplosion(this, damager, EXPLOSION_AIRCRAFT, GetPosition(), 0, 1, -1.0f, 0);
        }
    } else {
        m_autoPilot.m_nCarMission = MISSION_CRASH_PLANE_AND_BURN;
        m_fHealth = 0.0f;
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
        const auto y = m_doors[door].m_fAngle - m_doors[door].m_fPrevAngle + matrix.GetPosition().y;
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
    for (auto i = 0; i < GetVehiclePool()->GetSize(); i++) {
        auto vehicle = GetVehiclePool()->GetAt(i);
        if (vehicle && (vehicle->IsSubHeli() || vehicle->IsSubPlane())) {
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
void CPlane::FindPlaneCreationCoors(CVector* center, CVector* playerCoords, float* outHeading, float* outHeight, bool arg4) {
    plugin::Call<0x6CD090, CVector*, CVector*, float*, float*, bool>(center, playerCoords, outHeading, outHeight, arg4);
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
    return plugin::CallMethod<0x6C9260, CPlane*>(this);

    // untested
    if (m_nStatus == STATUS_PLAYER) {
        if (m_nModelIndex == MODEL_CROPDUST || m_nModelIndex == MODEL_STUNT) {
            auto pad = CPad::GetPad(m_pDriver->GetPadNumber());
            if (pad->IsRightShockPressed()) {
                m_bSmokeEjectorEnabled = m_bSmokeEjectorEnabled == 0;
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

    m_vehicleAudio.field_7C = static_cast<int16>(field_9A0);
    if (field_9A0) {
        field_9A0 = 0;
    }

    CVehicle::ProcessWeapons();
    if (m_nModelIndex == MODEL_VORTEX) {
        m_aWheelState[0] = WHEEL_STATE_NORMAL;
        m_aWheelState[1] = WHEEL_STATE_NORMAL;
        m_aWheelState[2] = WHEEL_STATE_NORMAL;
        m_aWheelState[3] = WHEEL_STATE_NORMAL;
    }

    if (m_pSmokeParticle) {
        RwMatrix out;
        m_nSmokeTimer += (uint32)(-CTimer::GetTimeStepInMS());
        m_pSmokeParticle->GetCompositeMatrix(&out);
        CVector velocity = -m_vecMoveSpeed * 5.0f;
        auto particleData = FxPrtMult_c(0.0f, 0.0f, 0.0f, 0.2f, 1.0f, 1.0f, 0.1f);
        g_fx.m_SmokeHuge->AddParticle((CVector*)&out.pos, &velocity, 0.00f, &particleData, -1.0f, 1.2f, 0.6f, false);
        g_fx.m_SmokeHuge->AddParticle((CVector*)&out.pos, &velocity, 0.05f, &particleData, -1.0f, 1.2f, 0.6f, false);
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
