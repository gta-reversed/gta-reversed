/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Automobile.h"

enum ePlaneNodes {
    PLANE_NODE_NONE = 0,
    PLANE_CHASSIS = 1,
    PLANE_WHEEL_RF = 2,
    PLANE_WHEEL_RM = 3,
    PLANE_WHEEL_RB = 4,
    PLANE_WHEEL_LF = 5,
    PLANE_WHEEL_LM = 6,
    PLANE_WHEEL_LB = 7,
    PLANE_DOOR_RF = 8,
    PLANE_DOOR_RR = 9,
    PLANE_DOOR_LF = 10,
    PLANE_DOOR_LR = 11,
    PLANE_STATIC_PROP = 12,
    PLANE_MOVING_PROP = 13,
    PLANE_STATIC_PROP2 = 14,
    PLANE_MOVING_PROP2 = 15,
    PLANE_RUDDER = 16,
    PLANE_ELEVATOR_L = 17,
    PLANE_ELEVATOR_R = 18,
    PLANE_AILERON_L = 19,
    PLANE_AILERON_R = 20,
    PLANE_GEAR_L = 21,
    PLANE_GEAR_R = 22,
    PLANE_MISC_A = 23,
    PLANE_MISC_B = 24,
    PLANE_NUM_NODES
};

class NOTSA_EXPORT_VTABLE CPlane : public CAutomobile {
public:
    float        m_fLeftRightSkid;
    float        m_fSteeringUpDown;
    float        m_fSteeringLeftRight;
    float        m_fAccelerationBreakStatus;
    float        m_fAccelerationBreakStatusPrev;
    float        m_fSteeringFactor;
    float        field_9A0;
    float        m_planeCreationHeading; // The heading when plane is created or placed on road properly
    float        m_maxAltitude;
    float        m_altitude;
    float        m_minAltitude;
    float        m_planeHeading;
    float        m_planeHeadingPrev;
    float        m_forwardZ;
    uint32       m_nStartedFlyingTime;
    float        m_fPropSpeed;
    float        field_9C8;
    float        m_fLandingGearStatus;
    int32        m_planeDamageWave;
    FxSystem_c** m_pGunParticles;
    uint8        m_nFiringMultiplier;
    int32        field_9DC;
    int32        field_9E0;
    int32        field_9E4;
    std::array<FxSystem_c*, 4> m_apJettrusParticles;
    FxSystem_c*  m_pSmokeParticle;
    uint32       m_nSmokeTimer;
    bool         m_bSmokeEjectorEnabled;

    static constexpr auto Type = VEHICLE_TYPE_PLANE;

public:
    static int32& GenPlane_ModelIndex;
    static uint32& GenPlane_Status;
    static uint32& GenPlane_LastTimeGenerated;

    static bool& GenPlane_Active;
    static float& ANDROM_COL_ANGLE_MULT;
    static uint16& HARRIER_NOZZLE_ROTATE_LIMIT;
    static uint16& HARRIER_NOZZLE_SWITCH_LIMIT;
    static float& PLANE_MIN_PROP_SPEED;
    static float& PLANE_STD_PROP_SPEED;
    static float& PLANE_MAX_PROP_SPEED;
    static float& PLANE_ROC_PROP_SPEED;

public:
    CPlane(int32 modelIndex, eVehicleCreatedBy createdBy);
    ~CPlane() override;

    bool SetUpWheelColModel(CColModel* wheelCol) override;
    bool BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) override;
    void ProcessControl() override;
    void ProcessControlInputs(uint8 playerNum) override;
    void ProcessFlyingCarStuff() override;
    void PreRender() override;
    void Render() override;
    void BlowUpCar(CEntity* damager, bool bHideExplosion) override;
    void Fix() override;
    void OpenDoor(CPed* ped, int32 componentId, eDoors door, float doorOpenRatio, bool playSound) override;
    void SetupDamageAfterLoad() override;
    void VehicleDamage(float damageIntensity, eVehicleCollisionComponent component, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) override;

    static void InitPlaneGenerationAndRemoval();

    void IsAlreadyFlying();
    void SetGearUp();
    void SetGearDown();

    static uint32 CountPlanesAndHelis();
    static bool AreWeInNoPlaneZone();
    static bool AreWeInNoBigPlaneZone();
    static void SwitchAmbientPlanes(bool enable);
    static void FindPlaneCreationCoors(CVector* center, CVector* playerCoords, float* outHeading, float* outHeight, bool arg4);
    static void DoPlaneGenerationAndRemoval();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CPlane* Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) { this->CPlane::CPlane(modelIndex, createdBy); return this; }
    CPlane* Destroy() { this->CPlane::~CPlane(); return this; }

};

VALIDATE_SIZE(CPlane, 0xA04);
