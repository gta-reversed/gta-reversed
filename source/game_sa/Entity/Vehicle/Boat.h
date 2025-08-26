/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vehicle.h"
#include "Door.h"
#include "tBoatHandlingData.h"

enum eBoatNodes {
    BOAT_NODE_NONE      = 0,
    BOAT_MOVING         = 1,
    BOAT_WINDSCREEN     = 2,
    BOAT_RUDDER         = 3,
    BOAT_FLAP_LEFT      = 4,
    BOAT_FLAP_RIGHT     = 5,
    BOAT_REARFLAP_LEFT  = 6,
    BOAT_REARFLAP_RIGHT = 7,
    BOAT_STATIC_PROP    = 8,
    BOAT_MOVING_PROP    = 9,
    BOAT_STATIC_PROP_2  = 10,
    BOAT_MOVING_PROP_2  = 11,

    BOAT_NUM_NODES
};

class NOTSA_EXPORT_VTABLE CBoat : public CVehicle {
public:
    float m_fMovingHiRotation; // works as counter also
    float m_fPropSpeed;        // propeller speed
    float m_fPropRotation;     // propeller rotation (radians)

    struct {
        uint8 bOnWater : 1; // is placed on water
        uint8 bMovingOnWater : 1;
        uint8 bAnchored : 1; // is anchored
    } m_nBoatFlags;

    RwFrame* m_boatNodes[BOAT_NUM_NODES];
    CDoor m_boatDoor;

    tBoatHandlingData* m_boatHandling;

    float m_fAnchoredAngle; // radians

    uint32 m_nAttackPlayerTime;
    uint32 m_timeOfLastParticle; // unused

    float m_burningTimer; // starts when vehicle health is lower than 250.0, boat blows up when it hits 5000.0
    CEntity* m_pWhoDestroyedMe;

    CVector m_boatMoveForce; // m_boatMoveForce = m_vecMoveForce + m_vecFrictionMoveForce
    CVector m_boatTurnForce; // m_boatTurnForce = m_vecTurnForce + m_vecFrictionTurnForce

    FxSystem_c* m_apPropSplashFx[2];
    CVector m_waterDamping; // { 0.0f, 0.0f, DampingPower }

    uint8 m_currentField; // unused, maybe boat handling type (@CBoat::DebugCode)

    uint8 m_padNum; // 0 - 3

    float m_prevVolume; // initialised with 7.0f, 0.0f - not in water

    uint16 m_countWakePoints;
    CVector2D m_wakePoints[32];
    float m_wakePtCounters[32];
    uint8 m_wakeBoatSpeed[32]; // m_wakeBoatSpeed[i] = boat->m_vecMoveForce.Magnitude() * 100.0f;

    static constexpr int32 NUM_WAKE_GEN_BOATS = 4;

    static inline std::array<CBoat*, NUM_WAKE_GEN_BOATS> apFrameWakeGeneratingBoats{}; // 0xC27994
    static constexpr float MAX_WAKE_LENGTH = 50.0f; // 0x8D3938
    static constexpr float MIN_WAKE_INTERVAL = 2.0f; // 0x8D393C
    static constexpr float WAKE_LIFETIME = 150.0f; // 0x8D3940

    static constexpr auto Type = VEHICLE_TYPE_BOAT;

public:
    CBoat(int32 modelIndex, eVehicleCreatedBy createdBy);
    ~CBoat() override;

    void ProcessControl() override;
    void Teleport(CVector destination, bool resetRotation) override;
    void ProcessControlInputs(uint8 playerNum) override;

    void PreRender() override;
    void Render() override;

    void SetModelIndex(uint32 index) override;
    void SetupModelNodes(); // fill m_boatNodes array
    void GetComponentWorldPosition(int32 componentId, CVector& outPos) override;
    bool IsComponentPresent(int32 component) const override;
    void BlowUpCar(CEntity* damager, bool bHideExplosion) override;

    void DisplayHandlingData();
    void ModifyHandlingValue(const bool& bIncrement);

    void DebugCode();

    void PruneWakeTrail();
    void AddWakePoint(CVector pos);

    void ProcessOpenDoor(CPed* ped, uint32 doorComponentId, uint32 animGroup, uint32 animId, float fTime) override;

    static void FillBoatList();
    static bool IsSectorAffectedByWake(CVector2D pos, float size, CBoat** ppBoats);
    static float IsVertexAffectedByWake(CVector pos, CBoat* boat, int16 wIndex, bool bUnkn);
    static void CheckForSkippingCalculations();

    // NOTSA
    static void RenderAllWakePointBoats();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    void inline ProcessBoatNodeRendering(eBoatNodes eNode, float fRotation, RwUInt8 ucAlpha);
    void RenderWakePoints();

private: // Wrappers for hooks
    // 0x6F2940
    CBoat* Constructor(int32 modelId, eVehicleCreatedBy createdBy) {
        this->CBoat::CBoat(modelId, createdBy);
        return this;
    }

    // 0x6F00F0
    CBoat* Destructor() {
        this->CBoat::~CBoat();
        return this;
    }
};

VALIDATE_SIZE(CBoat, 0x7E8);

RwObject* GetBoatAtomicObjectCB(RwObject* object, void* data);
