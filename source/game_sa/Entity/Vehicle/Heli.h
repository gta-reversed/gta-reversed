/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Automobile.h"

constexpr float HELI_ROTOR_DOTPROD_LIMIT = 0.5f;

enum
{
	ROTOR_TOP = -3,
	ROTOR_FRONT = -2,
	ROTOR_RIGHT = -1,
	ROTOR_LEFT = 1,
	ROTOR_BACK = 2,
	ROTOR_BOTTOM = 3,
};

enum eHeliNodes {
    HELI_NODE_NONE     = 0,
    HELI_CHASSIS       = 1,
    HELI_WHEEL_RF      = 2,
    HELI_WHEEL_RM      = 3,
    HELI_WHEEL_RB      = 4,
    HELI_WHEEL_LF      = 5,
    HELI_WHEEL_LM      = 6,
    HELI_WHEEL_LB      = 7,
    HELI_DOOR_RF       = 8,
    HELI_DOOR_RR       = 9,
    HELI_DOOR_LF       = 10,
    HELI_DOOR_LR       = 11,
    HELI_STATIC_ROTOR  = 12,
    HELI_MOVING_ROTOR  = 13,
    HELI_STATIC_ROTOR2 = 14,
    HELI_MOVING_ROTOR2 = 15,
    HELI_RUDDER        = 16,
    HELI_ELEVATORS     = 17,
    HELI_MISC_A        = 18,
    HELI_MISC_B        = 19,
    HELI_MISC_C        = 20,
    HELI_MISC_D        = 21,

    HELI_NUM_NODES
};

struct tHeliLight {
    CVector m_vecOrigin;
    CVector m_vecTarget;
    float   m_fTargetRadius;
    float   m_fPower;
    uint32  m_nCoronaIndex;
    bool    field_24; // unknown flag
    bool    m_bDrawShadow;
    CVector m_vecUseless[3]; // m_aSearchLightHistory

    void Init() {
        m_vecOrigin = CVector{};
        m_vecTarget = CVector{};
        m_fTargetRadius = 0.0f;
        m_fPower = 0.0f;
    }
};

VALIDATE_SIZE(tHeliLight, 0x4C);

class FxSystem_c;

class NOTSA_EXPORT_VTABLE CHeli : public CAutomobile {
public:
    uint8        m_nHeliFlags;
    float        m_fLeftRightSkid;
    float        m_fSteeringUpDown;
    float        m_fSteeringLeftRight;
    float        m_fAccelerationBreakStatus;
    int32        field_99C;
    int32        m_fRotorZ;
    int32        m_fSecondRotorZ;
    float        m_fMaxAltitude;
    float        field_9AC;
    float        m_fMinAltitude;
    float        field_9B4; // m_fHeading related
    char         field_9B8;
    int8         m_nNumSwatOccupants;
    std::array<uint8, 4> m_aSwatState;
    CVector      field_9C0;
    int32        f9CC; // unused?
    int32        f9D0; // unused?
    FxSystem_c** m_pParticlesList;
    std::array<float, 3> m_aSearchLightHistoryX;
    std::array<float, 3> m_aSearchLightHistoryY;
    uint32       m_nSearchLightTimer;
    CVector      m_vecSearchLightTarget;
    float        m_fSearchLightIntensity;
    uint32       m_nShootTimer;
    uint32       m_nPoliceShoutTimer;
    FxSystem_c** m_ppGunflashFx;
    uint8        m_nFiringMultiplier;
    bool         m_bSearchLightEnabled;
    float        field_A14;

    static inline bool& bPoliceHelisAllowed                   = *(bool*)0x8D338C; // 1
    static inline uint32& TestForNewRandomHelisTimer          = *(uint32*)0xC1C960;
    static inline std::array<CHeli*, 2>& pHelis               = *(std::array<CHeli*, 2>*)0xC1C964;
    static inline uint32& NumberOfSearchLights                = *(uint32*)0xC1C96C;
    static inline bool& bHeliControlsCheat                    = *(bool*)0xC1C970;
    static inline std::array<tHeliLight, 4>& HeliSearchLights = *(std::array<tHeliLight, 4>*)0xC1C990;

    static constexpr auto Type = VEHICLE_TYPE_HELI;

public:
    CHeli(int32 modelIndex, eVehicleCreatedBy createdBy);
    ~CHeli() override; // 0x6C4340, 0x6C4810

    void BlowUpCar(CEntity* damager, bool bHideExplosion) override;
    void Fix() override;
    bool BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) override;
    bool SetUpWheelColModel(CColModel* wheelCol) override;
    void ProcessControlInputs(uint8 playerNum) override;
    void Render() override;
    void SetupDamageAfterLoad() override;
    void ProcessFlyingCarStuff() override;
    void PreRender() override;
    void ProcessControl() override;

    void PreRenderAlways();
    CVector FindSwatPositionRelativeToHeli(int32 swatNumber);
    bool SendDownSwat();

    inline uint32 GetRopeId() { return reinterpret_cast<int32>(this + m_nNumSwatOccupants - 1); }

    static void InitHelis();
    static void AddHeliSearchLight(const CVector& origin, const CVector& target, float targetRadius, float power, uint32 coronaIndex, uint8 unknownFlag, uint8 drawShadow);
    static void Pre_SearchLightCone();
    static void Post_SearchLightCone();
    static void SpecialHeliPreRender(); // dummy function
    static void SwitchPoliceHelis(bool enable);
    static void SearchLightCone(int32 coronaIndex,
                                CVector origin,
                                CVector target,
                                float targetRadius,
                                float power,
                                uint8 clipIfColliding,
                                uint8 drawShadow,
                                CVector& useless0,
                                CVector& useless1,
                                CVector& useless2,
                                bool a11,
                                float baseRadius,
                                float a13,
                                float a14,
                                float a15);
    static CHeli* GenerateHeli(CPed* target, bool newsHeli);
    static void TestSniperCollision(CVector* origin, CVector* target);
    static void UpdateHelis();
    static void RenderAllHeliSearchLights();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CHeli* Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) { this->CHeli::CHeli(modelIndex, createdBy); return this;}
};

VALIDATE_SIZE(CHeli, 0xA18);
