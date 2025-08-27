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
    RwFrame* m_aBikeNodes[BIKE_NUM_NODES];

    bool m_bLeanMatrixCalculated;
    CMatrix m_mLeanMatrix;

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

    uint8 m_nWheelStatus[2];

    CColPoint m_aWheelColPoints[NUM_SUSP_LINES];
    float m_aWheelRatios[NUM_SUSP_LINES];
    float m_aRatioHistory[NUM_SUSP_LINES];

    float m_WheelCounts[NUM_SUSP_LINES];
    float m_fBrakeCount;

    eSkidmarkType m_aWheelSkidmarkType[2];
    bool m_bWheelBloody[2];
    bool m_bMoreSkidMarks[2];

    float m_aWheelPitchAngles[2];
    float m_aWheelAngularVelocity[2];
    float m_aWheelSuspensionHeights[2];
    float m_aWheelOrigHeights[2];

    float m_fSuspensionLength[NUM_SUSP_LINES];
    float m_fLineLength[NUM_SUSP_LINES];
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

    CPhysical* m_aGroundPhysicalPtrs[NUM_SUSP_LINES];
    CVector m_aGroundOffsets[NUM_SUSP_LINES];

    CEntity* m_Damager; // Entity That Set Us On Fire

    uint8 m_nNoOfContactWheels;
    uint8 m_NumDriveWheelsOnGround;
    uint8 m_NumDriveWheelsOnGroundLastFrame;
    float m_GasPedalAudioRevs;
    tWheelState m_WheelStates[2];

    static constexpr auto Type = VEHICLE_TYPE_BIKE;

public:
    [[nodiscard]] bool GetAllWheelsOffGround() const;

    void ReduceHornCounter();

    void PlayHornIfNecessary();
    void PlayCarHorn() override;
    int32 GetNumContactWheels() override { return m_nNoOfContactWheels; } // 0x6B58A0

    CBike(int32 modelIndex, eVehicleCreatedBy createdBy); // 0x6BF430
    ~CBike() override;

    void SetupSuspensionLines() override;
    float GetHeightAboveRoad() const override { return m_fHeightAboveRoad; } // 0x6B58B0

    void ProcessBuoyancy();
    void ProcessControl() override;
    virtual bool ProcessAI(uint32& extraHandlingFlags);
    void ProcessControlCollisionCheck(bool applySpeed) override;

    void ProcessControlInputs(uint8 playerNum) override;
    void Teleport(CVector destination, bool resetRotation) override;

    void PreRender() override;
    void Render() override;

    void ResetSuspension();
    void GetComponentWorldPosition(int32 componentId, CVector& outPos) override;
    bool IsComponentPresent(int32 componentId) const override { return m_aBikeNodes[componentId] != nullptr; } // 0x6B59E0


    void RemoveRefsToVehicle(CEntity* entityToRemove) override;

    void BlowUpCar(CEntity* damager, bool bHideExplosion) override;
    bool SetUpWheelColModel(CColModel* wheelCol) override;
    bool BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) override;
    void DoBurstAndSoftGroundRatios() override;
    void DoSoftGroundResistance(uint32& arg0);

    bool IsRoomForPedToLeaveCar(uint32 door, CVector* pvecCarJackOffset) override { return true; } // 0x6B7270
    void ProcessDrivingAnims(CPed* driver, bool blend) override;
    CRideAnimData* GetRideAnimData() override { return &m_RideAnimData; } // 0x6B58C0
    static void ProcessRiderAnims(CPed* rider, CVehicle* vehicle, CRideAnimData* rideData, tBikeHandlingData* handling, int16 a5);
    void FixHandsToBars(CPed* rider);

    void ProcessOpenDoor(CPed* ped, uint32 doorComponentId, uint32 animGroup, uint32 animId, float fTime) override;

    bool IsDoorReady(eDoors door) const override { return true; } // 0x6B58E0
    bool IsDoorFullyOpen(eDoors door) const override { return false; } // 0x6B58F0
    bool IsDoorClosed(eDoors door) const override { return false; } // 0x6B5900
    bool IsDoorMissing(eDoors door) const override { return true; } // 0x6B5910

    bool IsDoorReadyU32(uint32 door) const override { return true; } // 0x6B5920
    bool IsDoorFullyOpenU32(uint32 door) const override { return false; } // 0x6B5930
    bool IsDoorClosedU32(uint32 door) const override { return false; } // 0x6B5940
    bool IsDoorMissingU32(uint32 door) const override { return true; } // 0x6B5950

    void Fix() override;
    void SetupDamageAfterLoad() override;

    void CalculateLeanMatrix();
    CPed* KnockOffRider(eWeaponType hitType, uint8 localDirn, CPed* ped, bool beingJacked);
    void SetRemoveAnimFlags(CPed* ped);

    void PlaceOnRoadProperly();
    void GetCorrectedWorldDoorPosition(CVector& out, CVector arg1, CVector arg2);

    float FindWheelWidth(bool bRear) override { return 0.15f; } // 0x6B8940
    void VehicleDamage(float damageIntensity, eVehicleCollisionComponent component, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) override;
    static bool DamageKnockOffRider(CVehicle* arg0, float arg1, uint16 arg2, CEntity* arg3, CVector& arg4, CVector& arg5);

protected:
    int32 ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) override;

    void SetupModelNodes();
    void SetModelIndex(uint32 index) override;
    void DebugCode();

    void dmgDrawCarCollidingParticles(const CVector& position, float power, eWeaponType weaponType);

public: // NOTSA
    void ProcessPedInVehicleBuoyancy(CPed* ped, bool bIsDriver);
    [[nodiscard]] bool IsAnyWheelMakingContactWithGround() const {
        return std::ranges::any_of(m_aWheelRatios, [](const auto& r) {
            return r != BILLS_EXTENSION_LIMIT;
        });
    }

    [[nodiscard]] bool IsAnyWheelNotMakingContactWithGround() const {
        return std::ranges::any_of(m_aWheelRatios, [](const auto& r) {
            return r == BILLS_EXTENSION_LIMIT;
        });
    }

    static void InjectHooks();

    CBike* Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) {
        this->CBike::CBike(modelIndex, createdBy);
        return this;
    }

    CBike* Destructor() {
        this->CBike::~CBike();
        return this;
    }
};

VALIDATE_SIZE(CBike, 0x814);
VALIDATE_OFFSET(CBike, m_GasPedalAudioRevs, 0x808);
