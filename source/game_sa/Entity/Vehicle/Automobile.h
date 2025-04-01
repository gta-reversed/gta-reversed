/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vehicle.h"
#include "Door.h"
#include "BouncingPanel.h"
#include "DamageManager.h"
#include "ColPoint.h"

#include "SurfaceInfo_c.h"
#include "SurfaceInfos_c.h"
#include "eSurfaceType.h"
#include "eCarWheel.h"
#include "eCarNodes.h"

enum class eSkidmarkType : uint32;

class CVehicleModelInfo;

using eWheels = eCarWheel; // todo: maybe wrong, "eWheels" is original enum.

class CObject;

enum eExtraHandlingFlags : uint32 {
    EXTRA_HANDLING_PERFECT              = 0x1,
    EXTRA_HANDLING_NITROS               = 0x2,
    EXTRA_HANDLING_WHEELS_TOUCHING_SAND = 0x8,
    EXTRA_HANDLING_TAXI_BOOST           = 0x10
};

class FxSystem_c;

class NOTSA_EXPORT_VTABLE CAutomobile : public CVehicle {
public:
    CDamageManager                      m_damageManager;
    std::array<CDoor, 6>                m_doors;
    std::array<RwFrame*, CAR_NUM_NODES> m_aCarNodes;
    std::array<CBouncingPanel, 3>       m_panels;
    CDoor                               m_swingingChassis;
    std::array<CColPoint, 4>            m_wheelColPoint;                    // 0x724
    std::array<float, 4>                m_fWheelsSuspensionCompression;     // 0x7D4 - [0-1] with 0 being suspension fully compressed, and 1 being completely relaxed - Filled with 1.f in the ctor
    std::array<float, 4>                m_fWheelsSuspensionCompressionPrev; // 0x7E4 - Filled with 1.f in the ctor
    std::array<float, 4>                m_aWheelTimer;

    float field_804;
    float m_fIntertiaValue1; //  m_anWheelSurfaceType[2]
    float m_fIntertiaValue2;

    std::array<eSkidmarkType, 4> m_wheelSkidmarkType;       // 0x810
    std::array<bool,          4> m_wheelSkidmarkBloodState; // 0x820
    std::array<bool,          4> m_wheelSkidmarkMuddy;      // 0x824
    std::array<float,         4> m_wheelRotation;           // 0x828
    std::array<float,         4> m_wheelPosition;           // 0x838
    union {                                                 // 0x848
        std::array<float, 4> m_wheelSpeed;
        struct {
            float m_fHeliWheelSpeed1;
            float m_fHeliRotorSpeed;
            float m_fHeliWheelSpeed3;
            float m_fHeliWheelSpeed4;
        };
    };
    std::array<float, 4> m_fWheelBurnoutSpeed; // 0x858 - Passed to CVehicle::ProcessWheel as last 3rd parameter, but it's not used
    struct {
        bool bTaxiLight : 1 { m_sAllTaxiLights }; // AKA `bTaxiLightOn`
        bool bShouldNotChangeColour : 1 {}; // AKA `ucNPCFlagPad2`
        bool bWaterTight : 1 {}; // AKA `bIgnoreWater`
        bool bDoesNotGetDamagedUpsideDown : 1 {}; // AKA `bDontDamageOnRoof`
        bool bCanBeVisiblyDamaged : 1 { true }; // AKA `bTakePanelDamage`
        bool bTankExplodesCars : 1 { true }; // AKA `ucTaxiUnkn6`
        bool bIsBoggedDownInSand : 1 {}; // AKA `bLostTraction`
        bool bIsMonsterTruck : 1 {}; // AKA `bSoftSuspension`
    } autoFlags;
    int8   _align;
    bool   m_bDoingBurnout;                         // 0x86A
    uint16 m_wMiscComponentAngle;                   // 0x86C
    uint16 m_wMiscComponentAnglePrev;               // 0x86E
    uint32 m_nBusDoorTimerEnd;                      // 0x870
    uint32 m_nBusDoorTimerStart;                    // 0x874
    std::array<float, 4> m_aSuspensionSpringLength; // 0x878 // By default SuspensionUpperLimit - SuspensionLowerLimit
    std::array<float, 4> m_aSuspensionLineLength;   // 0x888 // By default SuspensionUpperLimit - SuspensionLowerLimit + mi.GetSizeOfWheel(<corresponding wheel>) / 2.f - So I assume line is always longer than the spring
    float m_fHeightAboveRoad;
    float m_fRearHeightAboveRoad;
    float m_fCarTraction;
    float m_fTireTemperature;
    float m_fAircraftGoToHeading;
    float m_fRotationBalance; // Controls destroyed helicopter rotation
    float m_fMoveDirection;
    CVector m_moveForce;
    CVector m_turnForce;
    std::array<float, 6> DoorRotation; // Inited in ctor with random values, but seemingly unused.

    float m_fBurnTimer;

    std::array<CPhysical*, 4> m_apWheelCollisionEntity{};
    std::array<CVector, 4>    m_vWheelCollisionPos{}; // Bike::m_avTouchPointsLocalSpace

    CPed* m_pExplosionVictim;
    std::array<char, 24> field_928;

    float LeftDoorOpenForDriveBys;
    float RightDoorOpenForDriveBys;
    float m_fDoomVerticalRotation;
    float m_fDoomHorizontalRotation;
    float m_fForcedOrientation;
    std::array<float, 2> m_fUpDownLightAngle;
    uint8 m_nNoOfContactWheels;
    uint8 m_nWheelsOnGround;
    uint8 m_wheelsOnGrounPrev;
    float m_fGasPedalAudio; // [0; 1] adjusts the speed of playback of the skiding sound

    std::array<tWheelState, 4> m_aWheelState;
    std::array<FxSystem_c*, 2> m_exhaustNitroFxSystem;

    uint8 m_harvesterParticleCounter;
    uint8 m_fireParticleCounter;
    int16 __pad_982;
    float m_heliDustFxTimeConst;

    // variables
    static constexpr float PACKER_COL_ANGLE_MULT   = -0.0001f;
    static constexpr float FORKLIFT_COL_ANGLE_MULT = 0.0006f;
    static constexpr float DOZER_COL_ANGLE_MULT    = 0.0002f;
    static constexpr float ROLL_ONTO_WHEELS_FORCE  = 0.0025f;
    static bool&           m_sAllTaxiLights;
    static CVector&        vecHunterGunPos; // { 0.0f, 4.8f, -1.3f }
    static CMatrix*        matW2B;

    static constexpr auto Type = VEHICLE_TYPE_AUTOMOBILE;

public:
    CAutomobile(int32 modelIndex, eVehicleCreatedBy createdBy, bool setupSuspensionLines);
    ~CAutomobile() override;

    // CEntity
    void Teleport(CVector destination, bool resetRotation) override;

    // CPhysical
    void ProcessControl() override;

    // CVehicle
    void ProcessControlCollisionCheck(bool applySpeed) override;
    void ProcessControlInputs(uint8 playerNum) override;
    void GetComponentWorldPosition(int32 componentId, CVector& outPos) override;
    bool IsComponentPresent(int32 componentId) override;
    void OpenDoor(CPed* ped, int32 componentId, eDoors door, float doorOpenRatio, bool playSound) override; // eCarNodes = componentId

    //!!!!!!!!!!!!!!!!!!!
    // PAY CLOSE ATTENTION TO WHICH VERSION OF THE FUNCTIONS DOWN BELOW YOU'RE CALLING!
    //!!!!!!!!!!!!!!!!!!!
    float GetDooorAngleOpenRatioU32(uint32 door) override;
    float GetDooorAngleOpenRatio(eDoors door) override;
    bool IsDoorReadyU32(uint32 door) override;
    bool IsDoorReady(eDoors door) override;
    bool IsDoorFullyOpenU32(uint32 door) override;
    bool IsDoorFullyOpen(eDoors door) override;
    bool IsDoorClosedU32(uint32 door) override;
    bool IsDoorClosed(eDoors door) override;
    bool IsDoorMissingU32(uint32 door) override;
    bool IsDoorMissing(eDoors door) override;

    bool IsOpenTopCar() override;
    void RemoveRefsToVehicle(CEntity* entity) override;
    void BlowUpCar(CEntity* damager, bool bHideExplosion) override;
    void BlowUpCarCutSceneNoExtras(bool bDontShakeCam, bool bDontSpawnStuff, bool bNoExplosion, bool bMakeSound) override;
    bool SetUpWheelColModel(CColModel* wheelCol) override;
    bool BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) override;
    bool IsRoomForPedToLeaveCar(uint32 arg0, CVector* arg1) override;
    void SetupSuspensionLines() override;
    CVector AddMovingCollisionSpeed(CVector& point) override;
    void Fix() override;
    void SetupDamageAfterLoad() override;
    void DoBurstAndSoftGroundRatios() override;
    float GetHeightAboveRoad() override;
    void PlayCarHorn() override;
    int32 GetNumContactWheels() override;
    void VehicleDamage(float damageIntensity, eVehicleCollisionComponent collisionComponent, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) override;
    bool GetTowHitchPos(CVector& outPos, bool bCheckModelInfo, CVehicle* veh) override;
    bool GetTowBarPos(CVector& outPos, bool bCheckModelInfo, CVehicle* veh) override;
    bool SetTowLink(CVehicle* tractor, bool setMyPosToTowBar) override;
    bool BreakTowLink() override;
    float FindWheelWidth(bool bRear) override;
    bool Save() override;
    bool Load() override;

    virtual bool ProcessAI(uint32& extraHandlingFlags);
    virtual void ResetSuspension();
    virtual void ProcessFlyingCarStuff();
    virtual void DoHoverSuspensionRatios();
    virtual void ProcessSuspension();
    int32 ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) override;

    void PreRender() override;
    void Render() override;

    // Find and save components ptrs (RwFrame) to m_modelNodes array
    void SetupModelNodes();
    // Process vehicle hydraulics
    void HydraulicControl();
    // Sets the angle of a vehicles extra. Called at 08A4 opcode (CONTROL_MOVABLE_VEHICLE_PART)
    bool UpdateMovingCollision(float angle);
    // Called at 098D opcode (GET_CAR_MOVING_COMPONENT_OFFSET)
    float GetMovingCollisionOffset();
    // Makes the helicopter fly to the specified location, keeping a specific Z height/altitude. This must be called for helis only.
    void TellHeliToGoToCoors(float x, float y, float z, float altitudeMin, float altitudeMax);
    // Force orientation for heli to specified angle (radians)
    void SetHeliOrientation(float angle);
    // Cancel orientation forcing (m_fForcedOrientation = -1.0f)
    void ClearHeliOrientation();
    // Makes the plane fly to the specified location, keeping a specific Z height/altitude.
    void TellPlaneToGoToCoors(float x, float y, float z, float altitudeMin, float altitudeMax);
    // Empty function
    void HideAllComps();
    // Empty function
    void ShowAllComps();
    // Set random damage to vehicle. Called when generating a vehicle @CCarCtrl::GenerateOneRandomCar
    void SetRandomDamage(bool arg0);
    // Make a vehicle fully damaged
    void SetTotalDamage(bool component);
    void ReduceHornCounter();
    // Apply custom car plate texture to vehicle
    void CustomCarPlate_BeforeRenderingStart(const CVehicleModelInfo& model);
    // Reset car plate texture after rendering
    void CustomCarPlate_AfterRenderingStop(CVehicleModelInfo* model);
    bool GetAllWheelsOffGround() const;
    // Some debug function
    void DebugCode();
    // Repair vehicle's tyre
    void FixTyre(eWheels wheel);
    // Repair vehicle's door. "nodeIndex" is an index of component in m_modelNodes array
    void FixDoor(int32 nodeIndex, eDoors door);
    // Repair vehicle's panel. "nodeIndex" is an index of component in m_modelNodes array
    void FixPanel(eCarNodes nodeIndex, ePanels panel);
    // Enable/disable taxi light for taxi
    void SetTaxiLight(bool enable);
    // Enable taxi light for all taxis (CAutomobile::m_sAllTaxiLights = true;)
    static void SetAllTaxiLights(bool enable);
    // Play horn for NPC vehicle (called @CAutomobile::ProcessAI)
    void PlayHornIfNecessary();
    void SetBusDoorTimer(uint32 time, bool setAsStartedInPast);
    void ProcessAutoBusDoors();
    // Make player vehicle jumps when pressing horn
    void BoostJumpControl();
    // Creates/updates nitro particle
    void DoNitroEffect(float power);
    // Remove nitro particle
    void StopNitroEffect();
    void NitrousControl(int8);
    void TowTruckControl();
    CPed* KnockPedOutCar(eWeaponType type, uint16 a2, CPed* ped);
    void PopBootUsingPhysics();
    // Close all doors
    void CloseAllDoors();
    void DoSoftGroundResistance(uint32& extraHandlingFlags);
    void ProcessCarWheelPair(eCarWheel leftWheel, eCarWheel rightWheel, float steerAngle, CVector* contactSpeeds, CVector* contactPoints, float traction, float acceleration, float brake, bool bFront);
    float GetCarRoll();
    float GetCarPitch();
    bool IsInAir();
    // Create colliding particles
    void dmgDrawCarCollidingParticles(const CVector&, float force, eWeaponType weapon);
    void ProcessCarOnFireAndExplode(bool bExplodeImmediately);
    CObject* SpawnFlyingComponent(eCarNodes nodeIndex, uint32 collisionType);
    void ProcessBuoyancy();
    void inline ProcessPedInVehicleBuoyancy(CPed* ped, bool bIsDriver);
    // Process combine
    void ProcessHarvester();
    void ProcessSwingingDoor(eCarNodes nodeIndex, eDoors door);
    // Returns spawned flying component?
    CObject* RemoveBonnetInPedCollision();
    void UpdateWheelMatrix(int32 nodeIndex, int32 flags);
    void PopDoor(eCarNodes nodeIndex, eDoors door, bool showVisualEffect);
    void PopPanel(eCarNodes nodeIndex, ePanels panel, bool showVisualEffect);
    void ScanForCrimes();
    void TankControl();
    // Makes a vehicles acts like a tank on a road - blows up collided vehicles. Must be called in a loop
    void BlowUpCarsInPath();
    void PlaceOnRoadProperly();
    void PopBoot();
    void CloseBoot();
    void DoHeliDustEffect(float arg0, float arg1);
    void SetBumperDamage(ePanels panel, bool withoutVisualEffect);
    void SetPanelDamage(ePanels panel, bool createWindowGlass);
    void SetDoorDamage(eDoors door, bool withoutVisualEffect);
    bool RcbanditCheck1CarWheels(CPtrListDoubleLink<CVehicle*>& ptrList);
    bool RcbanditCheckHitWheels();
    void FireTruckControl(CFire* fire);
    bool HasCarStoppedBecauseOfLight();

    // NOTSA section

    CBouncingPanel* CheckIfExistsGetFree(eCarNodes nodeIdx);
    CDoor& GetDoor(eDoors door) { return m_doors[(unsigned)door]; }
    CDamageManager& GetDamageManager() { return m_damageManager; }

    void SetEngineState(bool state) {
        if (vehicleFlags.bEngineBroken)
            vehicleFlags.bEngineOn = false;
        else
            vehicleFlags.bEngineOn = state;
    }

    [[nodiscard]] bool AreAllWheelsNotTouchingGround() const {
        return std::ranges::all_of(m_fWheelsSuspensionCompression, [](float v) {return v >= 1.f; });
    }

    bool IsAnyWheelMakingContactWithGround() {
        return m_fWheelsSuspensionCompression[0] != 1.0F
               || m_fWheelsSuspensionCompression[1] != 1.0F
               || m_fWheelsSuspensionCompression[2] != 1.0F
               || m_fWheelsSuspensionCompression[3] != 1.0F;
    };

    bool IsAnyWheelNotMakingContactWithGround() {
        return m_fWheelsSuspensionCompression[0] == 1.0F
               || m_fWheelsSuspensionCompression[1] == 1.0F
               || m_fWheelsSuspensionCompression[2] == 1.0F
               || m_fWheelsSuspensionCompression[3] == 1.0F;
    };

    bool IsAnyWheelTouchingSand() {
        for (int32 i = 0; i < 4; i++) {
            if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                if (g_surfaceInfos.GetAdhesionGroup(m_wheelColPoint[i].m_nSurfaceTypeB) == ADHESION_GROUP_SAND)
                    return true;
            }
        }
        return false;
    }

    bool IsAnyWheelTouchingRailTrack() {
        for (int32 i = 0; i < 4; i++) {
            if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                if (m_wheelColPoint[i].m_nSurfaceTypeB == SURFACE_RAILTRACK)
                    return true;
            }
        }
        return false;
    }

    bool IsAnyWheelTouchingShallowWaterGround() {
        for (int32 i = 0; i < 4; i++) {
            if (m_fWheelsSuspensionCompression[i] < 1.0f && m_wheelColPoint[i].m_nSurfaceTypeB == SURFACE_WATER_SHALLOW)
                return true;
        }
        return false;
    }

    bool IsAnyFrontAndRearWheelTouchingGround() {
        if (m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_LEFT] < 1.0f  || m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_RIGHT] < 1.0f) {
            if (m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_LEFT] < 1.0f || m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_RIGHT] < 1.0f)
                return true;
        }
        return false;
    }

    [[nodiscard]] bool AreFrontWheelsNotTouchingGround() const {
        return m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_LEFT] >= 1.0f && m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_RIGHT];
    }

    [[nodiscard]] bool AreRearWheelsNotTouchingGround() const {
        return m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_LEFT] >= 1.0f && m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_RIGHT];
    }

    // check the previous compression state using m_fWheelsSuspensionCompressionPrev
    bool DidAnyWheelTouchShallowWaterGroundPrev() {
        for (int32 i = 0; i < 4; i++) {
            if (m_fWheelsSuspensionCompressionPrev[i] < 1.0f && m_wheelColPoint[i].m_nSurfaceTypeB == SURFACE_WATER_SHALLOW)
                return true;
        }
        return false;
    }
    bool DidAnyWheelTouchGroundPrev() {
        for (float prevSuspension : m_fWheelsSuspensionCompressionPrev) {
            if (prevSuspension < 1.0f)
                return true;
        }
        return false;
    }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    void BlowUpCar_Impl(CEntity* dmgr, bool bDontShakeCam, bool bDontSpawnStuff, bool bNoExplosion, bool bHideExplosionFx, bool bIsForCutScene, bool bMakeSound);

    auto Constructor(int32 modelIndex, eVehicleCreatedBy createdBy, bool setupSuspensionLines) {
        this->CAutomobile::CAutomobile(modelIndex, createdBy, setupSuspensionLines);
        return this;
    }

    auto Destructor() {
        this->CAutomobile::~CAutomobile();
        return this;
    }
};

VALIDATE_SIZE(CAutomobile, 0x988);
VALIDATE_OFFSET(CAutomobile, m_damageManager, 0x5A0);
VALIDATE_OFFSET(CAutomobile, m_wheelColPoint, 0x724);
VALIDATE_OFFSET(CAutomobile, autoFlags, 0x868);
VALIDATE_OFFSET(CAutomobile, m_bDoingBurnout, 0x86A);
VALIDATE_OFFSET(CAutomobile, m_wMiscComponentAngle, 0x86C);
VALIDATE_OFFSET(CAutomobile, m_fGasPedalAudio, 0x964);

// Disable matfx (material effects) for material (callback), "data" parameter is unused
RpMaterial *DisableMatFx(RpMaterial* material, void* data);

RwObject* GetCurrentAtomicObjectCB(RwObject* object, void* data);
RwObject* GetCurrentAtomicObject(RwFrame* frame);
