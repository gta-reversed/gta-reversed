/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vehicle.h"
#include "Skidmark.h"

struct tBikeHandlingData;

enum eBikeNodes {
    BIKE_NODE_NONE   = 0,
    BIKE_CHASSIS     = 1,
    BIKE_FORKS_FRONT = 2,
    BIKE_FORKS_REAR  = 3,
    BIKE_WHEEL_FRONT = 4,
    BIKE_WHEEL_REAR  = 5,
    BIKE_MUDGUARD    = 6,
    BIKE_HANDLEBARS  = 7,
    BIKE_MISC_A      = 8,
    BIKE_MISC_B      = 9,

    BIKE_NUM_NODES
};

class NOTSA_EXPORT_VTABLE CBike : public CVehicle {
    static constexpr auto NUM_SUSP_LINES = 4;
public:
    std::array<RwFrame*, BIKE_NUM_NODES>           m_aBikeNodes;
    bool     m_bLeanMatrixCalculated;
    CMatrix  m_mLeanMatrix;
    union {
        struct {
            uint8 bShouldNotChangeColour : 1;
            uint8 bPanelsAreThougher : 1;
            uint8 bWaterTight : 1; // Ignores water
            uint8 bGettingPickedUp : 1;
            uint8 bOnSideStand : 1; // Is Standing
            uint8 bPlayerBoost : 1;
            uint8 bEngineOnFire : 1;
            uint8 bWheelieForCamera : 1;
        } bikeFlags;
        uint8 nBikeFlags;
    };
    CVector m_vecAveGroundNormal;
    CVector m_vecGroundRight;
    CVector m_vecOldSpeedForPlayback;
    tBikeHandlingData* m_BikeHandling;
    CRideAnimData m_RideAnimData;
    std::array<uint8, 2>          m_nWheelStatus;
    std::array<CColPoint, NUM_SUSP_LINES> m_aWheelColPoints;
    std::array<float, NUM_SUSP_LINES>     m_aWheelRatios;
    std::array<float, NUM_SUSP_LINES>     m_aRatioHistory;
    std::array<float, NUM_SUSP_LINES>     m_WheelCounts;
    float m_fBrakeCount;
    std::array<eSkidmarkType, 2>  m_aWheelSkidmarkType;
    std::array<bool, 2>           m_bWheelBloody;
    std::array<bool, 2>           m_bMoreSkidMarks;
    std::array<float, 2>          m_aWheelPitchAngles;
    std::array<float, 2>          m_aWheelAngularVelocity;
    std::array<float, 2>          m_aWheelSuspensionHeights;
    std::array<float, 2>          m_aWheelOrigHeights;
    std::array<float, NUM_SUSP_LINES> m_fSuspensionLength;
    std::array<float, NUM_SUSP_LINES> m_fLineLength;
    float m_fHeightAboveRoad;
    float m_fExtraTractionMult;
    float m_fSwingArmLength;
    float m_fForkYOffset;
    float m_fForkZOffset;
    float m_fSteerAngleTan;
    uint16 m_nBrakesOn;
    float m_fTyreTemp;
    float m_fBrakingSlide;
    uint8 m_nFixLeftHand;
    uint8 m_nFixRightHand;
    uint8 m_nTestPedCollision;
    float m_PrevSpeed;
    float m_BlowUpTimer;
    std::array<CPhysical*, 4>     m_aGroundPhysicalPtrs;
    std::array<CVector, 4>        m_aGroundOffsets;
    CEntity* m_Damager; // Entity That Set Us On Fire
    uint8 m_nNoOfContactWheels;
    uint8 m_NumDriveWheelsOnGround;
    uint8 m_NumDriveWheelsOnGroundLastFrame;
    float m_GasPedalAudioRevs;
    std::array<tWheelState, 2>    m_WheelStates;

    static constexpr auto Type = VEHICLE_TYPE_BIKE;

public:
    CBike(int32 modelIndex, eVehicleCreatedBy createdBy); // 0x6BF430
    ~CBike() override;

    void Fix() override;
    void BlowUpCar(CEntity* damager, bool bHideExplosion) override;
    void ProcessDrivingAnims(CPed* driver, bool blend) override;
    bool BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) override;
    void ProcessControlInputs(uint8 playerNum) override;
    int32 ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) override;
    void Render() override;
    void PreRender() override;
    void Teleport(CVector destination, bool resetRotation) override;
    void ProcessControl() override;
    void VehicleDamage(float damageIntensity, eVehicleCollisionComponent component, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) override;
    void SetupSuspensionLines() override;
    void SetModelIndex(uint32 index) override;
    void PlayCarHorn() override;
    void SetupDamageAfterLoad() override;
    void DoBurstAndSoftGroundRatios() override;
    bool SetUpWheelColModel(CColModel* wheelCol) override;
    void RemoveRefsToVehicle(CEntity* entityToRemove) override;
    void ProcessControlCollisionCheck(bool applySpeed) override;
    void GetComponentWorldPosition(int32 componentId, CVector& outPos) override;
    void ProcessOpenDoor(CPed* ped, uint32 doorComponentId, uint32 animGroup, uint32 animId, float fTime) override;

    bool IsDoorReady(eDoors door) override { return true; }      // 0x6B58E0
    bool IsDoorFullyOpen(eDoors door) override { return false; } // 0x6B58F0
    bool IsDoorClosed(eDoors door) override { return false; }    // 0x6B5900
    bool IsDoorMissing(eDoors door) override { return true; }    // 0x6B5910
    bool IsDoorReadyU32(uint32 door) override { return true; }      // 0x6B5920
    bool IsDoorFullyOpenU32(uint32 door) override { return false; } // 0x6B5930
    bool IsDoorClosedU32(uint32 door) override { return false; }    // 0x6B5940
    bool IsDoorMissingU32(uint32 door) override { return true; }    // 0x6B5950

    bool IsRoomForPedToLeaveCar(uint32 door, CVector* pvecCarJackOffset) override { return true; }                    // 0x6B7270
    inline bool IsComponentPresent(int32 componentId) const override { return m_aBikeNodes[componentId] != nullptr; } // 0x6B59E0
    CRideAnimData* GetRideAnimData() override { return &m_RideAnimData; }                                             // 0x6B58C0
    float GetHeightAboveRoad() override { return m_fHeightAboveRoad; }                                                // 0x6B58B0
    int32 GetNumContactWheels() override { return m_nNoOfContactWheels; }                                             // 0x6B58A0
    float FindWheelWidth(bool bRear) override { return 0.15f; }                                                       // 0x6B8940

    virtual bool ProcessAI(uint32& extraHandlingFlags);

    void SetupModelNodes();
    void dmgDrawCarCollidingParticles(const CVector& position, float power, eWeaponType weaponType);
    static bool DamageKnockOffRider(CVehicle* pVehicle, float fIntensity, uint16 arg2, CEntity* arg3, CVector& arg4, CVector& arg5);
    CPed* KnockOffRider(eWeaponType arg0, uint8 arg1, CPed* ped, bool arg3);
    void SetRemoveAnimFlags(CPed* ped);
    void ReduceHornCounter();
    void ProcessBuoyancy();
    void inline ProcessPedInVehicleBuoyancy(CPed* ped, bool bIsDriver);
    void ResetSuspension();
    [[nodiscard]] bool GetAllWheelsOffGround() const;
    void DebugCode();
    void DoSoftGroundResistance(uint32& arg0);
    void PlayHornIfNecessary();
    void CalculateLeanMatrix();
    static void ProcessRiderAnims(CPed* rider, CVehicle* vehicle, CRideAnimData* rideData, tBikeHandlingData* handling, int16 a5);
    void FixHandsToBars(CPed* rider);
    void PlaceOnRoadProperly();
    void GetCorrectedWorldDoorPosition(CVector& out, CVector arg1, CVector arg2);

public: // NOTSA
    [[nodiscard]] bool IsAnyWheelMakingContactWithGround()  const { return std::ranges::any_of(m_aWheelRatios, [](const auto& r) { return r != 1.0F; }); };
    [[nodiscard]] bool IsAnyWheelNotMakingContactWithGround() const { return std::ranges::any_of(m_aWheelRatios, [](const auto& r) { return r == 1.0F; }); };

    static void InjectHooks();
    CBike* Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) { this->CBike::CBike(modelIndex, createdBy); return this; }
    CBike* Destructor() {this->CBike::~CBike(); return this; }
                                                                                    };
VALIDATE_SIZE(CBike, 0x814);
VALIDATE_OFFSET(CBike, m_GasPedalAudioRevs, 0x808);
