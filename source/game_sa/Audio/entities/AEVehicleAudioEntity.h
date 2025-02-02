/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AEAudioEntity.h"
#include "cTransmission.h"
#include "AETwinLoopSoundEntity.h"
#include <extensions/FixedFloat.hpp>
#include "eAudioEvents.h"
#include "eRadioID.h"
#include "eVehicleType.h"
enum tWheelState : int32;

class CVehicle;
class CPlane;

enum eVehicleSoundType : int8 {
    VEHICLE_SOUND_CAR              = 0x0,
    VEHICLE_SOUND_MOTORCYCLE       = 0x1,
    VEHICLE_SOUND_BICYCLE          = 0x2,
    VEHICLE_SOUND_BOAT             = 0x3,
    VEHICLE_SOUND_HELI             = 0x4,
    VEHICLE_SOUND_PLANE            = 0x5,
    VEHICLE_SOUND_NON_VEH          = 0x6, // SEA_PLANE
    VEHICLE_SOUND_USED_BY_NONE_VEH = 0x7,
    VEHICLE_SOUND_TRAIN            = 0x8,
    VEHICLE_SOUND_TRAILER          = 0x9,
    VEHICLE_SOUND_SPECIAL          = 0xA,
};

enum eRadioType : int8 {
    RADIO_CIVILIAN  = 0,
    RADIO_SPECIAL   = 1,
    RADIO_UNKNOWN   = 2,
    RADIO_EMERGENCY = 3,
    RADIO_DISABLED  = -1,
};

struct tDummyEngineSlot {
    int16 BankID;
    int16 RefCnt;

    void Reset() {
        BankID = -1;
        RefCnt = 0;
    }
};
VALIDATE_SIZE(tDummyEngineSlot, 0x4);

struct cVehicleParams {
    int32                    SpecificVehicleType{ VEHICLE_TYPE_IGNORE };
    int32                    BaseVehicleType{ VEHICLE_TYPE_IGNORE };
    bool                     IsDistCalculated{ false };
    float                    DistSq{ 0.0f };
    CVehicle*                Vehicle{ nullptr };
    cTransmission*           Transmission{ nullptr };
    uint32                   ModelIndexMinusOffset{ 0 }; // Offset is `400`
    float                    Speed{ 0.0f };
    FixedFloat<int16, 255.f> ThisAccel{ 0 };
    FixedFloat<int16, 255.f> ThisBrake{ 0 };
    float                    AbsSpeed{ 0.0f };
    float                    ZOverSpeed{};
    float                    SpeedRatio{ 0.0f };
    float*                   GasPedalAudioRevs{ nullptr };
    float                    PrevSpeed{ 0.0f };
    uint8                    RealGear{ 0 };
    bool                     IsHandbrakeOn{ false };
    float                    RealRevsRatio{ 0.0f };
    float                    WheelSpin{ 0.0f };
    int16                    NumGears{};
    uint8                    NumDriveWheelsOnGround{};
    uint8                    NumDriveWheelsOnGroundLastFrame{};
    tWheelState*             WheelState{};
};
VALIDATE_SIZE(cVehicleParams, 0x4C);

class NOTSA_EXPORT_VTABLE tEngineSound {
public:
    void Init(uint32 index) {
        EngineSoundType = index;
    }

    uint32    EngineSoundType{};
    CAESound* Sound{};

};
VALIDATE_SIZE(tEngineSound, 0x8);

struct tVehicleAudioSettings {
    eVehicleSoundType VehicleAudioType;
    int16             PlayerBank;
    int16             DummyBank;
    int8              BassSetting; // m_nStereo
    float             BassFactor; // m_fBassFactor
    float             EnginePitch;
    int8              HornType; // sfx id
    float             HornPitch;
    char              DoorType;
    char              EngineUpgrade;
    eRadioID          RadioStation;
    eRadioType        RadioType;
    int8              VehicleAudioTypeForName;
    float             EngineVolumeOffset;

public:
    [[nodiscard]] bool IsHeli()          const { return VehicleAudioType == eVehicleSoundType::VEHICLE_SOUND_HELI; }
    [[nodiscard]] bool IsPlane()         const { return VehicleAudioType == eVehicleSoundType::VEHICLE_SOUND_PLANE; }
    [[nodiscard]] bool IsFlyingVehicle() const { return IsPlane() || IsHeli(); }
    [[nodiscard]] bool IsNonVeh()        const { return VehicleAudioType == eVehicleSoundType::VEHICLE_SOUND_NON_VEH; }
    [[nodiscard]] bool IsCar()           const { return VehicleAudioType == eVehicleSoundType::VEHICLE_SOUND_CAR; }
    [[nodiscard]] bool IsMotorcycle()    const { return VehicleAudioType == eVehicleSoundType::VEHICLE_SOUND_MOTORCYCLE; }
    [[nodiscard]] bool IsBicycle()       const { return VehicleAudioType == eVehicleSoundType::VEHICLE_SOUND_BICYCLE; }
};
VALIDATE_SIZE(tVehicleAudioSettings, 0x24);

class CPed;

class NOTSA_EXPORT_VTABLE CAEVehicleAudioEntity : public CAEAudioEntity {
private:
    // Indices for `EngineSound[]` (?) depending on the vehicle type:
    enum eAircraftSoundType { // For planes (aircrafts)
        AE_SOUND_AIRCRAFT_DISTANT     = 1,
        AE_SOUND_AIRCRAFT_FRONT       = 2,
        AE_SOUND_AIRCRAFT_NEAR        = 3,
        AE_SOUND_AIRCRAFT_REAR        = 4,
        AE_SOUND_AIRCRAFT_THRUST      = 5,
        AE_SOUND_PLANE_WATER_SKIM     = 6,
        AE_SOUND_AIRCRAFT_JET_DISTANT = 7,
    };

    enum { // For automobiles
        AE_SOUND_ENGINE_OFF     = 0,

        AE_SOUND_CAR_REV        = 1,
        AE_SOUND_CAR_ID         = 2,

        AE_SOUND_PLAYER_CRZ     = 3,
        AE_SOUND_PLAYER_AC      = 4,
        AE_SOUND_PLAYER_OFF     = 5,
        AE_SOUND_PLAYER_REVERSE = 6,

        AE_SOUND_NITRO1         = 7,
        AE_SOUND_NITRO2         = 8,

        AE_SOUND_STEAM          = 9,

        AE_SOUND_FUCKED         = 10,

        AE_SOUND_MOVING_PARTS   = 11,

        AE_SOUND_ENGINE_MAX     = 12,
    };

    enum { // For trains
        AE_SOUND_TRAIN_ENGINE  = 1,
        AE_SOUND_TRAIN_TRACK   = 2,
        AE_SOUND_TRAIN_DISTANT = 3,
    };

    enum { // For heli
        AE_PLAYER_HELI_FRONT = 0,
        AE_PLAYER_HELI_REAR  = 1,
        AE_PLAYER_HELI_START = 2,
        AE_PLAYER_HELI_TAIL  = 3,
    };

    enum { // For bicycle
        AE_SOUND_BICYCLE_TYRE        = 1,
        AE_SOUND_BICYCLE_SPROCKET_1  = 2,
        AE_SOUND_BICYCLE_CHAIN_CLANG = 3,
    };

    enum { // For boat
        AE_SOUND_BOAT_IDLE       = 1,
        AE_SOUND_BOAT_ENGINE     = 2,
        AE_SOUND_BOAT_DISTANT    = 3,
        AE_SOUND_BOAT_PADDING1   = 4,
        AE_SOUND_BOAT_PADDING2   = 5,
        AE_SOUND_BOAT_WATER_SKIM = 6,
    };

    enum  { // AE Vehicle types (?)
        AE_CAR                 = 0,
        AE_BIKE                = 1,
        AE_BMX                 = 2,
        AE_BOAT                = 3,
        AE_AIRCRAFT_HELICOPTOR = 4,
        AE_AIRCRAFT_PLANE      = 5,
        AE_AIRCRAFT_SEAPLANE   = 6,
        AE_ONE_GEAR            = 7,
        AE_TRAIN               = 8,
        AE_SPECIAL             = 9,
        AE_NO_VEHICLE          = 10,
    };

    enum eAEState : uint8 { // Enums for `m_State` (?)
        AE_STATE_CAR_OFF              = 0,

        AE_STATE_DUMMY_ID             = 1, // Idle?
        AE_STATE_DUMMY_CRZ            = 2, // crz = cruising?

        AE_STATE_PLAYER_AC_FULL       = 3, // AC = Acceleration?
        AE_STATE_PLAYER_WHEEL_SPIN    = 4,
        AE_STATE_PLAYER_CRZ           = 5, // Cruising?
        AE_STATE_PLAYER_ID            = 6, // ???
        AE_STATE_PLAYER_REVERSE       = 7,
        AE_STATE_PLAYER_REVERSE_OFF   = 8,
        AE_STATE_PLAYER_FAILING_TO_AC = 9, // Failing to accelerate? 

        AE_CAR_ENGINE_STATE_MAX       = 10,
    };

    enum { // ????
        AE_DUMMY_CRZ = 0x0,
        AE_DUMMY_ID = 0x1,
    };

public:
    int16                  m_DoCountStalls;
    tVehicleAudioSettings  m_AuSettings;
    bool                   m_IsInitialized;
    bool                   m_IsPlayerDriver;
    bool                   m_IsPlayerPassenger;
    bool                   m_IsPlayerDriverAboutToExit;
    bool                   m_IsWreckedVehicle;
    eAEState               m_State; // Engine state?
    uint8                  m_AuGear; // AudioGear
    float                  m_CrzCount; // Crz = Cruise ?
    bool                   m_IsSingleGear;
    int16                  m_RainDropCounter;
    int16                  m_StalledCount;
    uint32                 m_SwapStalledTime;
    bool                   m_IsSilentStalled; // ProcessPlayerProp
    bool                   m_IsHelicopterDisabled;

    bool                   m_IsHornOn;
    bool                   m_IsSirenOn;
    bool                   m_IsFastSirenOn;
    float                  m_HornVolume;
    bool                   m_HasSiren;

    uint32                 m_TimeSplashLastTriggered;
    uint32                 m_TimeBeforeAllowAccelerate;
    uint32                 m_TimeBeforeAllowCruise;

    float                  m_EventVolume;

    int16                  m_DummyEngineBank;
    int16                  m_PlayerEngineBank;
    int16                  m_DummySlot;
    tEngineSound           m_EngineSounds[AE_SOUND_ENGINE_MAX];

    int32                  m_TimeLastServiced;

    int16                  m_ACPlayPositionThisFrame;
    int16                  m_ACPlayPositionLastFrame;
    int16                  m_FramesAgoACLooped;
    int16                  m_ACPlayPercentWhenStopped;  // [0, 100]
    uint32                 m_TimeACStopped;
    int16                  m_ACPlayPositionWhenStopped;

    int16                  m_SurfaceSoundType;
    CAESound*              m_SurfaceSound;

    int16                  m_RoadNoiseSoundType;
    CAESound*              m_RoadNoiseSound;

    int16                  m_FlatTireSoundType;
    CAESound*              m_FlatTireSound;

    int16                  m_ReverseGearSoundType;
    CAESound*              m_ReverseGearSound;

    int16                  m_HornSoundType;
    CAESound*              m_HornSound;
    CAESound*              m_SirenSound;
    CAESound*              m_FastSirenSound;

    CAETwinLoopSoundEntity m_SkidSound;

    float                  m_CurrentRotorFrequency;

    float                  m_CurrentDummyEngineVolume;
    float                  m_CurrentDummyEngineFrequency; // speed ?
    float                  m_MovingPartSmoothedSpeed; // volume ?

    float                  m_FadeIn;
    float                  m_FadeOut;

    bool                   m_bNitroOnLastFrame;
    float                  m_CurrentNitroRatio;

public:
    static CPed*&                  s_pPlayerAttachedForRadio;
    static CPed*&                  s_pPlayerDriver;
    static bool&                   s_HelicoptorsDisabled;
    static int16&                  s_NextDummyEngineSlot;
    static tVehicleAudioSettings*& s_pVehicleAudioSettingsForRadio;

    static constexpr int32 NUM_DUMMY_ENGINE_SLOTS = AE_CAR_ENGINE_STATE_MAX;
    static tDummyEngineSlot (&s_DummyEngineSlots)[AE_CAR_ENGINE_STATE_MAX];

public:
    CAEVehicleAudioEntity();
    ~CAEVehicleAudioEntity();

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    void Initialise(CEntity* entity);
    static void StaticInitialise();

    void Terminate();

    static tVehicleAudioSettings* StaticGetPlayerVehicleAudioSettingsForRadio();
    static bool DoesBankSlotContainThisBank(int16 bankSlot, int16 bankId);
    static int16 DemandBankSlot(int16 bankId);
    static int16 RequestBankSlot(int16 bankId);
    static void StoppedUsingBankSlot(int16 bankSlot);
    static tVehicleAudioSettings GetVehicleAudioSettings(int16 vehId);

    void AddAudioEvent(eAudioEvents event, float fVolume);
    void AddAudioEvent(eAudioEvents event, CVehicle* vehicle);

    void Service();
    static void StaticService();

    int16 GetVehicleTypeForAudio() const;

    void InhibitAccForTime(uint32 time);
    bool IsAccInhibited(cVehicleParams& params) const;
    bool IsAccInhibitedBackwards(cVehicleParams& params) const;
    bool IsAccInhibitedForLowSpeed(cVehicleParams& params) const;
    [[nodiscard]] bool IsAccInhibitedForTime() const;

    void InhibitCrzForTime(uint32 time);
    [[nodiscard]] bool IsCrzInhibitedForTime() const;

    void JustGotInVehicleAsDriver();
    void TurnOnRadioForVehicle();
    void TurnOffRadioForVehicle();
    void PlayerAboutToExitVehicleAsDriver();
    bool CopHeli();
    bool JustFinishedAccelerationLoop();
    void PlaySkidSound(int16 soundType, float speed = 1.0f, float volume = -100.0f);
    void JustWreckedVehicle();
    CVector GetAircraftNearPosition();
    float GetFlyingMetalVolume(CPhysical* physical);
    void GetSirenState(bool& bSirenOrAlarm, bool& bHorn, cVehicleParams& params);
    void PlayTrainBrakeSound(int16 soundType, float speed = 1.0f, float volume = -100.0f);
    void JustGotOutOfVehicleAsDriver();

    void StartVehicleEngineSound(int16, float, float);
    void CancelVehicleEngineSound(size_t engineSoundStateId);
    void CancelAllVehicleEngineSounds(); // notsa
    void RequestNewPlayerCarEngineSound(int16 vehicleSoundId, float speed, float changeSound);
    float GetFreqForPlayerEngineSound(cVehicleParams& params, int16 engineState_QuestionMark);
    float GetVolForPlayerEngineSound(cVehicleParams& params, int16 gear);

    void UpdateVehicleEngineSound(int16, float, float);
    static void UpdateGasPedalAudio(CVehicle* vehicle, int32 vehType);
    void UpdateBoatSound(int16 engineState, int16 bankSlotId, int16 sfxId, float speed, float volume);
    void UpdateTrainSound(int16 engineState, int16 bankSlotId, int16 sfxId, float speed, float volume);
    void UpdateGenericVehicleSound(int16 soundId, int16 bankSlotId, int16 bankId, int16 sfxId, float speed, float volume, float distance);

    static void EnableHelicoptors();
    static void DisableHelicoptors();
    void EnableHelicoptor();
    void DisableHelicoptor();

    float GetVolumeForDummyIdle(float fGearRevProgress, float fRevProgressBaseline);
    float GetFrequencyForDummyIdle(float fGearRevProgress, float fRevProgressBaseline);
    [[nodiscard]] float GetFreqForIdle(float fRatio) const;

    float GetVolumeForDummyRev(float fRatio, float fFadeRatio);
    float GetFrequencyForDummyRev(float fRatio, float fFadeRatio);

    float GetVehicleDriveWheelSkidValue(CVehicle* vehicle, int32 wheelState, float fUnk, cTransmission& transmission, float fVelocity);
    float GetVehicleNonDriveWheelSkidValue(CVehicle* vehicle, int32 wheelState, cTransmission& transmission, float velocity);

    [[nodiscard]] float GetBaseVolumeForBicycleTyre(float fGearVelocityProgress) const;
    void GetHornState(bool* out, cVehicleParams& params);
    void GetAccelAndBrake(cVehicleParams& vp) const;

    void PlayAircraftSound(eAircraftSoundType engineState, int16 bankSlotId, int16 sfxId, float volume = -100.0f, float speed = 1.0f);
    void PlayRoadNoiseSound(int16 newRoadNoiseSoundType, float speed = 1.0f, float volume = -100.0f);
    void PlayFlatTyreSound(int16 soundType, float speed = 1.0f, float volume = -100.0f);
    void PlayReverseSound(int16 soundType, float speed = 1.0f, float volume = -100.0f);
    void PlayHornOrSiren(bool bPlayHornTone, bool bPlaySirenOrAlarm, bool bPlayHorn, cVehicleParams& params);
    void PlayBicycleSound(int16 engineState, int16 bankSlotId, int16 sfxId, float volume = -100.0f, float speed = 1.0f);

    void ProcessVehicleFlatTyre(cVehicleParams& params);
    void ProcessVehicleRoadNoise(cVehicleParams& params);
    void ProcessReverseGear(cVehicleParams& params);
    void ProcessVehicleSkidding(cVehicleParams& params);
    void ProcessRainOnVehicle(cVehicleParams& params);
    void ProcessGenericJet(bool bEngineOn, cVehicleParams& params, float fEngineSpeed, float fAccelRatio, float fBrakeRatio, float fStalledVolume, float fStalledFrequency);
    void ProcessDummyJet(cVehicleParams& params);
    void ProcessPlayerJet(cVehicleParams& params);
    void ProcessDummySeaPlane(cVehicleParams& params);
    void ProcessPlayerSeaPlane(cVehicleParams& params);
    void ProcessAIHeli(cVehicleParams& params);
    void ProcessDummyHeli(cVehicleParams& params);
    void ProcessPlayerHeli(cVehicleParams& params);
    void ProcessAIProp(cVehicleParams& params);
    void ProcessProp(cVehicleParams& vp, bool isProp); // notsa
    void ProcessDummyProp(cVehicleParams& params);
    void ProcessPlayerProp(cVehicleParams& params);
    void ProcessAircraft(cVehicleParams& params);
    void ProcessPlayerBicycle(cVehicleParams& params);
    void ProcessDummyBicycle(cVehicleParams& params);
    void ProcessDummyStateTransition(int16 newState, float fRatio, cVehicleParams& params);
    void ProcessDummyVehicleEngine(cVehicleParams& params);
    void ProcessPlayerVehicleEngine(cVehicleParams& params);
    void ProcessVehicleSirenAlarmHorn(cVehicleParams& params);
    void ProcessBoatEngine(cVehicleParams& params);
    void ProcessBoatMovingOverWater(cVehicleParams& params);
    void ProcessDummyTrainEngine(cVehicleParams& params);
    void ProcessTrainTrackSound(cVehicleParams& params);
    void ProcessPlayerTrainEngine(cVehicleParams& params);
    void ProcessPlayerTrainBrakes(cVehicleParams& params);
    void ProcessDummyRCPlane(cVehicleParams& params);
    void ProcessPlayerRCPlane(cVehicleParams& params);
    void ProcessDummyRCHeli(cVehicleParams& params);
    void ProcessPlayerRCHeli(cVehicleParams& params);
    void ProcessPlayerRCCar(cVehicleParams& params);
    void ProcessPlayerHovercraft(cVehicleParams& params);
    void ProcessPlayerGolfCart(cVehicleParams& params);
    void ProcessDummyGolfCart(cVehicleParams& params);
    void ProcessDummyHovercraft(cVehicleParams& params);
    void ProcessDummyRCCar(cVehicleParams& params);
    void ProcessPlayerCombine(cVehicleParams& params);
    void ProcessEngineDamage(cVehicleParams& params);
    void ProcessNitro(cVehicleParams& params);
    void ProcessMovingParts(cVehicleParams& params);
    void ProcessVehicle(CPhysical* vehicle);
    void ProcessSpecialVehicle(cVehicleParams& params);

    // Seems to be inlined, so whenever you see something similar, replace it with a call
    void StopGenericEngineSound(int16 index);

    bool UpdateGenericEngineSound(int16 index, float fVolume = 1.0f, float fSpeed = 1.0f);
    bool PlayGenericEngineSound(int16 index, int16 bank, int16 slotInBank,
                                float fVolume = 1.0f, float fSpeed = 1.0f, float fSoundDistance = 1.0f,
                                float fTimeScale = 1.0f, eSoundEnvironment individualEnvironment = SOUND_REQUEST_UPDATES, int16 playPos = 0);
#undef PlaySound
    CAESound* PlaySound(int16 bank, int16 slotInBank,
                        float fVolume = 1.0f, float fSpeed = 1.0f, float fSoundDistance = 1.0f,
                        float fTimeScale = 1.0f, eSoundEnvironment individualEnvironment = SOUND_REQUEST_UPDATES, int16 playPos = 0);

private:
    void ProcessPropStall(CPlane* plane, float& outVolume, float& outFreq);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEVehicleAudioEntity* Constructor() { this->CAEVehicleAudioEntity::CAEVehicleAudioEntity(); return this; }
    CAEVehicleAudioEntity* Destructor() { this->CAEVehicleAudioEntity::~CAEVehicleAudioEntity(); return this; }
};
VALIDATE_SIZE(CAEVehicleAudioEntity, 0x24C);

// indexes = (Vehicles modelId - 400)
static constexpr int32 NUM_VEH_AUDIO_SETTINGS = 232;
extern tVehicleAudioSettings const (&gVehicleAudioSettings)[NUM_VEH_AUDIO_SETTINGS];

// OG debug stuff
static bool& s_bVehicleDriveWheelSkidEnabled = *(bool*)0x8CBD80; // true
static bool& s_bVehicleNonDriveWheelSkidValueEnabled = *(bool*)0x8CBD81; // true
