/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
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

class CPlane : public CAutomobile {
protected:
    CPlane(plugin::dummy_func_t) : CAutomobile(plugin::dummy) {}
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
    float        m_propSpeed;
    float        field_9C8;
    float        m_fLandingGearStatus;
    int32        m_planeDamageWave;
    FxSystem_c** m_pGunParticles;
    uint8        m_nFiringMultiplier;
    int32        field_9DC;
    int32        field_9E0;
    int32        field_9E4;
    FxSystem_c*  m_apJettrusParticles[4];
    FxSystem_c*  m_pSmokeParticle;
    uint32       m_nSmokeTimer;
    bool         m_bSmokeEjectorEnabled;
    char         _pad[3];

public:
    static int32& GenPlane_ModelIndex;
    static uint32& GenPlane_Status;
    static uint32& GenPlane_LastTimeGenerated;

    static bool& GenPlane_Active;               // true
    static float& ANDROM_COL_ANGLE_MULT;        // 0.00015
    static float& HARRIER_NOZZLE_ROTATE_LIMIT;  // 5000.0
    static float& HARRIER_NOZZLE_SWITCH_LIMIT;  // 3000.0
    static float& PLANE_MIN_PROP_SPEED;         // 0.05
    static float& PLANE_STD_PROP_SPEED;         // 0.18
    static float& PLANE_MAX_PROP_SPEED;         // 0.34
    static float& PLANE_ROC_PROP_SPEED;         // 0.01

public:
    static void InjectHooks();

    CPlane(int32 modelIndex, eVehicleCreatedBy createdBy);

    void BlowUpCar(CEntity* damager, uint8 bHideExplosion) override;
    void Fix() override;

    void IsAlreadyFlying();
    void SetGearUp();
    void SetGearDown();

    static void InitPlaneGenerationAndRemoval();
    static uint32 CountPlanesAndHelis();
    static bool AreWeInNoPlaneZone();
    static bool AreWeInNoBigPlaneZone();
    static void SwitchAmbientPlanes(bool enable);
    static void FindPlaneCreationCoors(CVector* arg0, CVector* arg1, float* arg2, float* arg3, bool arg4);
    static void DoPlaneGenerationAndRemoval();

private:
    void BlowUpCar_Reversed(CEntity* damager, uint8 bHideExplosion);
    void Fix_Reversed();
};

VALIDATE_SIZE(CPlane, 0xA04);

extern float &HARRIER_NOZZLE_ROTATERATE;        // 25.0
extern float& PLANE_DAMAGE_WAVE_COUNTER_VAR;    // 0.75
extern float& PLANE_DAMAGE_THRESHHOLD;          // 500.0
extern float& PLANE_DAMAGE_SCALE_MASS;          // 10000.0
extern float& PLANE_DAMAGE_DESTROY_THRESHHOLD;  // 5000.0
extern CVector& vecRCBaronGunPos;               // <0.0, 0.45, 0.0>
