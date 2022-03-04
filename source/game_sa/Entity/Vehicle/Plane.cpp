#include "StdInc.h"

#include "Plane.h"

int32& CPlane::GenPlane_ModelIndex = *(int32*)0xC1CAD8;
uint32& CPlane::GenPlane_Status = *(uint32*)0xC1CADC;
uint32& CPlane::GenPlane_LastTimeGenerated = *(uint32*)0xC1CAE0;

bool& CPlane::GenPlane_Active = *(bool*)0x8D33BC;               // true
float& CPlane::ANDROM_COL_ANGLE_MULT = *(float*)0x8D33C0;       // 0.00015f
float& CPlane::HARRIER_NOZZLE_ROTATE_LIMIT = *(float*)0x8D33C4; // 5000.0f
float& CPlane::HARRIER_NOZZLE_SWITCH_LIMIT = *(float*)0x8D33C8; // 3000.0f
float& CPlane::PLANE_MIN_PROP_SPEED = *(float*)0x8D33CC;        // 0.05f
float& CPlane::PLANE_STD_PROP_SPEED = *(float*)0x8D33D0;        // 0.18f
float& CPlane::PLANE_MAX_PROP_SPEED = *(float*)0x8D33D4;        // 0.34f
float& CPlane::PLANE_ROC_PROP_SPEED = *(float*)0x8D33D8;        // 0.01f

float& HARRIER_NOZZLE_ROTATERATE = *(float*)0x8D33DC;       // 25.0f
float& PLANE_DAMAGE_WAVE_COUNTER_VAR = *(float*)0x8D33E0;   // 0.75f
float& PLANE_DAMAGE_THRESHHOLD = *(float*)0x8D33E4;         // 500.0f
float& PLANE_DAMAGE_SCALE_MASS = *(float*)0x8D33E8;         // 10000.0f
float& PLANE_DAMAGE_DESTROY_THRESHHOLD = *(float*)0x8D33EC; // 5000.0f
CVector& vecRCBaronGunPos = *(CVector*)0x8D33F0;            // <0.0f, 0.45f, 0.0f>

void CPlane::InjectHooks() {
    RH_ScopedClass(CPlane);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(InitPlaneGenerationAndRemoval, 0x6CAD90);
    RH_ScopedInstall(SetUpWheelColModel_Reversed, 0x6C9140);
    RH_ScopedInstall(BurstTyre_Reversed, 0x6C9150);
    // RH_ScopedInstall(PreRender_Reversed, 0x6C94A0);
    RH_ScopedInstall(Render_Reversed, 0x6CAB70);
    RH_ScopedInstall(IsAlreadyFlying, 0x6CAB90);
    RH_ScopedInstall(Fix_Reversed, 0x6CABB0);
    RH_ScopedInstall(SetupDamageAfterLoad_Reversed, 0x6CAC10);
    RH_ScopedInstall(SetGearUp, 0x6CAC20);
    RH_ScopedInstall(SetGearDown, 0x6CAC70);
    RH_ScopedInstall(OpenDoor_Reversed, 0x6CACB0);
    // RH_ScopedInstall(ProcessControl_Reversed, 0x6C9260);
    // RH_ScopedInstall(ProcessControlInputs_Reversed, 0x6CADD0);
    // RH_ScopedInstall(ProcessFlyingCarStuff_Reversed, 0x6CB7C0);
    // RH_ScopedInstall(VehicleDamage_Reversed, 0x6CC4B0);
    RH_ScopedInstall(CountPlanesAndHelis, 0x6CCA50);
    // RH_ScopedInstall(AreWeInNoPlaneZone, 0x6CCAA0);
    // RH_ScopedInstall(AreWeInNoBigPlaneZone, 0x6CCBB0);
    // RH_ScopedInstall(SwitchAmbientPlanes, 0x6CCC50);
    // RH_ScopedInstall(BlowUpCar_Reversed, 0x6CCCF0);
    // RH_ScopedInstall(FindPlaneCreationCoors, 0x6CD090);
    // RH_ScopedInstall(DoPlaneGenerationAndRemoval, 0x6CD2F0);
}

// 0x6C8E20
CPlane::CPlane(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile({}) /* CAutomobile(modelIndex, createdBy, true) */ {
    plugin::CallMethod<0x6C8E20, CPlane*, int32, eVehicleCreatedBy>(this, modelIndex, createdBy);
}

// 0x6C9160
CPlane::~CPlane() {
    if (m_pGunParticles) {
        for (auto i = 0; i < CVehicle::GetPlaneNumGuns(); i++) {
            auto& particle = m_pGunParticles[i];
            if (particle) {
                particle->Kill();
                g_fxMan.DestroyFxSystem(particle);
            }
        }
        delete[] m_pGunParticles;
        m_pGunParticles = nullptr;
    }

    for (auto& particle : m_apJettrusParticles) {
        particle->Kill();
        particle = nullptr;
    }

    if (m_pSmokeParticle) {
        m_pSmokeParticle->Kill();
        m_pSmokeParticle = nullptr;
    }

    m_vehicleAudio.Terminate();
    CAutomobile::~CAutomobile();
}

// 0x6CAD90
void CPlane::InitPlaneGenerationAndRemoval() {
    GenPlane_Status = 0;
    GenPlane_LastTimeGenerated = 0;
    GenPlane_Active = true;
}

// 0x6CCCF0
void CPlane::BlowUpCar(CEntity* damager, uint8 bHideExplosion) {
    plugin::CallMethod<0x6CCCF0, CPlane*, CEntity*, uint8>(this, damager, bHideExplosion);
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
        CMatrix matrix(&m_aCarNodes[componentId]->modelling, false);
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
    m_fAirResistance = m_pHandlingData->m_fDragMult / 1000.0f * 0.5f * m_pFlyingHandlingData->m_fGearUpR;
    m_damageManager.SetWheelStatus(CARWHEEL_FRONT_LEFT,  WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CARWHEEL_REAR_LEFT,   WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CARWHEEL_FRONT_RIGHT, WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CARWHEEL_REAR_RIGHT,  WHEEL_STATUS_MISSING);
}

// 0x6CAC70
void CPlane::SetGearDown() {
    m_fLandingGearStatus = 0.0f;
    m_fAirResistance = m_pHandlingData->m_fDragMult / 1000.0f * 0.5f;
    m_damageManager.SetWheelStatus(CARWHEEL_FRONT_LEFT,  WHEEL_STATUS_OK);
    m_damageManager.SetWheelStatus(CARWHEEL_REAR_LEFT,   WHEEL_STATUS_OK);
    m_damageManager.SetWheelStatus(CARWHEEL_FRONT_RIGHT, WHEEL_STATUS_OK);
    m_damageManager.SetWheelStatus(CARWHEEL_REAR_RIGHT,  WHEEL_STATUS_OK);
}

// 0x6CCA50
uint32 CPlane::CountPlanesAndHelis() {
    auto counter = 0;

    for (auto i = 0; i < GetVehiclePool()->GetSize(); i++) {
        if (auto vehicle = GetVehiclePool()->GetAt(i)) {
            if (vehicle->IsSubHeli() || vehicle->IsSubPlane())
                counter++;
        }
    }

    return counter;
}

// 0x6CCAA0
bool CPlane::AreWeInNoPlaneZone() {
    return plugin::CallAndReturn<bool, 0x6CCAA0>();

    const auto camPos = TheCamera.GetPosition();
    return sqrt((camPos.z - 50.0f) * (camPos.z - 50.0f) + (camPos.y - -675.0f) * (camPos.y - -675.0f) + (camPos.x - -1073.0f) * (camPos.x - -1073.0)) < 200.0f
           || camPos.x > -2743.0f && camPos.x < -2626.0f && camPos.y > 1300.0f && camPos.y < 2200.0f
           || camPos.x > -1668.0f && camPos.x < -1122.0f && camPos.y > 541.0f && camPos.y < 1118.0f;
}

// 0x6CCBB0
bool CPlane::AreWeInNoBigPlaneZone() {
    return plugin::CallAndReturn<bool, 0x6CCBB0>();

    const auto camPos = TheCamera.GetPosition();
    return sqrt((camPos.y - -1237.0f) * (camPos.y - -1237.0f) + (camPos.x - 1522.0f) * (camPos.x - 1522.0f)) < 800.0f
           || sqrt((camPos.y - 659.0f) * (camPos.y - 659.0f) + (camPos.x - -1836.0f) * (camPos.x - -1836.0f)) < 800.0f;
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
    plugin::CallMethod<0x6C9260, CPlane*>(this);
}

// 0x6CADD0
void CPlane::ProcessControlInputs(uint8 playerNum) {
    plugin::CallMethod<0x6CADD0, CPlane*, uint8>(this, playerNum);
}

// 0x6CB7C0
void CPlane::ProcessFlyingCarStuff() {
    plugin::CallMethod<0x6CB7C0, CPlane*>(this);
}
