/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "PluginBase.h"
#include "CVehicle.h"
#include "CDoor.h"
#include "tBoatHandlingData.h"

enum eBoatNodes {
    BOAT_NODE_NONE = 0,
    BOAT_MOVING = 1,
    BOAT_WINDSCREEN = 2,
    BOAT_RUDDER = 3,
    BOAT_FLAP_LEFT = 4,
    BOAT_FLAP_RIGHT = 5,
    BOAT_REARFLAP_LEFT = 6,
    BOAT_REARFLAP_RIGHT = 7,
    BOAT_STATIC_PROP = 8,
    BOAT_MOVING_PROP = 9,
    BOAT_STATIC_PROP_2 = 10,
    BOAT_MOVING_PROP_2 = 11,
    BOAT_NUM_NODES
};

class CBoat : public CVehicle {
public:
    CBoat(int modelIndex, eVehicleCreatedBy createdBy);
    ~CBoat() override;
public:
    float              m_fMovingHiRotation; // works as counter also
    float              m_fPropSpeed; // propeller speed
    float              m_fPropRotation; // propeller rotation (radians)
    struct {
        unsigned char bOnWater : 1; // is placed on water
        unsigned char bMovingOnWater : 1;
        unsigned char bAnchored : 1; // is anchored
    } m_nBoatFlags;
private:
    char _pad5AD[3];
public:
    RwFrame           *m_aBoatNodes[BOAT_NUM_NODES];
    CDoor              m_boatFlap; // for marquis model
    tBoatHandlingData *m_pBoatHandling;
    float              m_fAnchoredAngle; // radians, initialised with -9999.99
    uint32_t           m_nAttackPlayerTime;
    int field_604; // initialised with 0, not used
    float              m_fBurningTimer; // starts when vehicle health is lower than 250.0, boat blows up when it hits 5000.0
    CEntity           *m_pWhoDestroyedMe;
    CVector            m_vecBoatMoveForce; // m_vecBoatMoveForce = m_vecMoveForce + m_vecFrictionMoveForce
    CVector            m_vecBoatTurnForce; // m_vecBoatTurnForce = m_vecTurnForce + m_vecFrictionTurnForce
    FxSystem_c        *m_apPropSplashFx[2];
    CVector            m_vecWaterDamping; // { 0.0f, 0.0f, DampingPower }
    char field_63C; // initialised with 0, maybe boat handling type (@CBoat::DebugCode), possibly a leftover
    unsigned char      m_nPadNumber; // 0 - 3
private:
    char _pad63E[2];
public:
    float              m_fLastWaterImmersionDepth; // initialised with 7.0f, 0.0f - not in water
    short              m_nNumWaterTrailPoints;
private:
    char _pad646[2];
public:
    CVector2D          m_avecWakePoints[32];
    float              m_afWakePointLifeTime[32];
    unsigned char      m_anWakePointIntensity[32]; // m_anWakePointIntensity[i] = boat->m_vecMoveForce.Magnitude() * 100.0f;

    static constexpr int32_t NUM_WAKE_GEN_BOATS = 4;
    static CBoat*(&apFrameWakeGeneratingBoats)[NUM_WAKE_GEN_BOATS]; // static CBoat *apFrameWakeGeneratingBoats[4]
    static float& MAX_WAKE_LENGTH; // 50.0
    static float& MIN_WAKE_INTERVAL; // 2.0
    static float& WAKE_LIFETIME; // 150.0
    static float& fShapeLength; // 0.4
    static float& fShapeTime; // 0.05
    static float& fRangeMult; // 0.6

    static short* waUnknArr;  // static CBoat::waUnknArr[4]
    static short* waUnknArr2; // static CBoat::waUnknArr2[4]

    static const constexpr uint32_t uiNumVertices = 4;
    static RxObjSpace3DVertex* aRenderVertices;

    static const constexpr uint32_t uiNumIndices = 6;
    static RxVertexIndex* auRenderIndices;

    static void InjectHooks();
    //funcs

    // Virtual methods
    void SetModelIndex(unsigned int index) override;
    void ProcessControl() override;
    void Teleport(CVector destination, bool resetRotation) override;
    void PreRender() override;
    void Render() override;
    void ProcessControlInputs(unsigned char playerNum) override;
    void GetComponentWorldPosition(int componentId, CVector& posnOut) override;
    void ProcessOpenDoor(CPed* ped, unsigned int doorComponentId, unsigned int arg2, unsigned int arg3, float arg4) override;
    void BlowUpCar(CEntity* damager, unsigned char bHideExplosion) override;

    // Reversed virtual methods
private:
    void SetModelIndex_Reversed(unsigned int index);
    void ProcessControl_Reversed();
    void Teleport_Reversed(CVector destination, bool resetRotation);
    void PreRender_Reversed();
    void inline ProcessBoatNodeRendering(eBoatNodes eNode, float fRotation, RwUInt8 ucAlpha);
    void Render_Reversed();
    void ProcessControlInputs_Reversed(unsigned char ucPadNum);
    void GetComponentWorldPosition_Reversed(int componentId, CVector& posnOut);
    void ProcessOpenDoor_Reversed(CPed* ped, unsigned int doorComponentId, unsigned int arg2, unsigned int arg3, float arg4);
    void BlowUpCar_Reversed(CEntity* damager, unsigned char bHideExplosion);

public:
    inline void SetupModelNodes(); // fill m_aBoatNodes array
    void DebugCode();
    void PrintThrustAndRudderInfo(); // uses debug printing
    void ModifyHandlingValue(bool const& bIncrement);
    void PruneWakeTrail();
    void AddWakePoint(CVector posn);

    static bool IsSectorAffectedByWake(CVector2D vecPos, float fOffset, CBoat** ppBoats);
    static float IsVertexAffectedByWake(CVector vecPos, CBoat* pBoat, short wIndex, bool bUnkn);
    static void CheckForSkippingCalculations();
    static void FillBoatList();
};

VALIDATE_SIZE(CBoat, 0x7E8);

RwObject* GetBoatAtomicObjectCB(RwObject* object, void* data);
