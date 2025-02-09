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

enum eAEVehicleSoundType : int8 {
    AE_CAR                 = 0x0, // 0
    AE_BIKE                = 0x1, // 1
    AE_BMX                 = 0x2, // 2
    AE_BOAT                = 0x3, // 3
    AE_AIRCRAFT_HELICOPTER = 0x4, // 4
    AE_AIRCRAFT_PLANE      = 0x5, // 5
    AE_AIRCRAFT_SEAPLANE   = 0x6, // 6
    AE_ONE_GEAR            = 0x7, // 7
    AE_TRAIN               = 0x8, // 8
    AE_SPECIAL             = 0x9, // 9
    AE_NO_VEHICLE          = 0xA, // 10
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
    eAEVehicleSoundType VehicleAudioType;
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
    [[nodiscard]] bool IsHeli()          const { return VehicleAudioType == eAEVehicleSoundType::AE_AIRCRAFT_HELICOPTER; }
    [[nodiscard]] bool IsPlane()         const { return VehicleAudioType == eAEVehicleSoundType::AE_AIRCRAFT_PLANE; }
    [[nodiscard]] bool IsFlyingVehicle() const { return IsPlane() || IsHeli(); }
    [[nodiscard]] bool IsNonVeh()        const { return VehicleAudioType == eAEVehicleSoundType::AE_AIRCRAFT_SEAPLANE; }
    [[nodiscard]] bool IsCar()           const { return VehicleAudioType == eAEVehicleSoundType::AE_CAR; }
    [[nodiscard]] bool IsMotorcycle()    const { return VehicleAudioType == eAEVehicleSoundType::AE_BIKE; }
    [[nodiscard]] bool IsBicycle()       const { return VehicleAudioType == eAEVehicleSoundType::AE_BMX; }
};
VALIDATE_SIZE(tVehicleAudioSettings, 0x24);

class CPed;

namespace notsa {
namespace debugmodules {
    class VehicleAudioEntityDebugModule;
};
};

class NOTSA_EXPORT_VTABLE CAEVehicleAudioEntity : public CAEAudioEntity {
    friend notsa::debugmodules::VehicleAudioEntityDebugModule;

protected:
    // Config struct - Obviously this is notsa, but it's necessary for the debug module
    static inline struct Config {
        // Dummy engine constants:
        struct {
            float VolumeUnderwaterOffset = 6.f; // 0x8CBC44
            float VolumeTrailerOffset    = 6.f; // 0x8CBC40

            float FreqUnderwaterFactor   = 0.7f; // 0x8CBC48

            // ...Idle:
            struct {
                float Ratio      = 0.2f; // 0x8CBBF0

                float VolumeBase = -3.f; // 0x8CBC00
                float VolumeMax  = 0.f;  // 0xB6B9CC

                float FreqBase   = 0.85f; // 0x8CBBF8
                float FreqMax    = 1.2f;  // 0x8CBBFC
            } Idle{};

            // ...Rev:
            struct {
                float Ratio      = 0.15f; // 0x8CBBF4

                float VolumeBase = -4.5f; // 0xB6BA2C
                float VolumeMax  = 0.f;   // 0xB6B9D0

                float FreqBase   = 0.9f; // 0x8CBC0C
                float FreqMax    = 1.5f; // 0x8CBC10
            } Rev{};
        } DummyEngine{};

        struct {
            float AccelWheelSpinThreshold{ 150.f / 255.f }; // 0x8CBC54
            float SpeedOffsetCrz{ 0.001f };                 // 0x8CBD34
            float SpeedOffsetAc{ 0.0015f };                 // 0x8CBD30
            float ZMoveSpeedThreshold{ 0.2f };              // 0x8CBD38
            int32 MaxAuGear{ 5 };                           // 0xdeadbeef
            int32 MaxCrzCnt{ 150 };                         // 0x8CBC8C
            float NitroFactor{ 3.f };                       // 0x8CBC4C
            float SingleGearVolume{ -2.f };                 // 0xB6BA3C

            struct {
                float VolMin{ -4.f }, VolMax{}; // 0x8CBCA8, 0xB6B9DC
            } ST1;
            struct {
                float VolMin{ -3.5f }, VolMax{ -1.5f }; // 0x8CBC78, 0x8CBC7C
            } ST2;
            struct {
                float VolMin{ 0.f }, VolMax{ 2.f }; // 0xB6B9D8, 0x8CBC88
            } ST3;
            struct {
                float VolMin{ -2.f }, VolMax{ 0.f }; // 0x8CBC68, 0xB6B9D4
            } ST4;
        } PlayerEngine;
    } s_Config{};
    static inline Config s_DefaultConfig{};

public:
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

    using eVehicleEngineSoundType = int16;

    enum eCarEngineSoundType : eVehicleEngineSoundType { // For automobiles
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

    enum : eVehicleEngineSoundType { // For trains
        AE_SOUND_TRAIN_ENGINE  = 1,
        AE_SOUND_TRAIN_TRACK   = 2,
        AE_SOUND_TRAIN_DISTANT = 3,
    };

    enum : eVehicleEngineSoundType { // For heli
        AE_PLAYER_HELI_FRONT = 0,
        AE_PLAYER_HELI_REAR  = 1,
        AE_PLAYER_HELI_START = 2,
        AE_PLAYER_HELI_TAIL  = 3,
    };

    enum : eVehicleEngineSoundType { // For bicycle
        AE_SOUND_BICYCLE_TYRE        = 1,
        AE_SOUND_BICYCLE_SPROCKET_1  = 2,
        AE_SOUND_BICYCLE_CHAIN_CLANG = 3,
    };

    enum : eVehicleEngineSoundType { // For boat
        AE_SOUND_BOAT_IDLE       = 1,
        AE_SOUND_BOAT_ENGINE     = 2,
        AE_SOUND_BOAT_DISTANT    = 3,
        AE_SOUND_BOAT_PADDING1   = 4,
        AE_SOUND_BOAT_PADDING2   = 5,
        AE_SOUND_BOAT_WATER_SKIM = 6,
    };

    enum class eAEState : uint8 {
        CAR_OFF              = 0,

        // Used for vehicles the player is *not* inside of
        DUMMY_ID             = 1, // Idle
        DUMMY_CRZ            = 2, // Cruising (?)

        // Used for vehicles the player is in
        PLAYER_AC_FULL       = 3, // Acceleration (?)
        PLAYER_WHEEL_SPIN    = 4,
        PLAYER_CRZ           = 5, // Cruising (?)
        PLAYER_ID            = 6, // Idle (?)
        PLAYER_REVERSE       = 7,
        PLAYER_REVERSE_OFF   = 8,
        PLAYER_FAILING_TO_AC = 9, // AC = accelerate (?)

        // Keep this at the bottom
        NUM_STATES
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
    std::array<tEngineSound, AE_SOUND_ENGINE_MAX> m_EngineSounds;

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

    static constexpr int32 NUM_DUMMY_ENGINE_SLOTS = (int32)(eAEState::NUM_STATES);
    static tDummyEngineSlot (&s_DummyEngineSlots)[NUM_DUMMY_ENGINE_SLOTS];

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
    void CancelAllVehicleEngineSounds(std::optional<size_t> except = std::nullopt); // notsa
    void RequestNewPlayerCarEngineSound(int16 vehicleSoundId, float speed = 1.f, float changeSound = -100.f);
    float GetFreqForPlayerEngineSound(cVehicleParams& params, int16 engineState_QuestionMark);
    float GetVolForPlayerEngineSound(cVehicleParams& params, int16 gear);

    void UpdateVehicleEngineSound(int16, float, float);
    static void UpdateGasPedalAudio(CVehicle* vehicle, int32 vehType);
    void UpdateBoatSound(int16 engineState, int16 bankSlotId, int16 sfxId, float speed, float volume);
    void UpdateTrainSound(int16 engineState, int16 bankSlotId, int16 sfxId, float speed, float volume);
    void UpdateGenericVehicleSound(int16 soundId, int16 bankSlotId, int16 bankId, int16 sfxId, float speed, float volume, float distance);

    bool HasVehicleEngineSound(eVehicleEngineSoundType st) const noexcept;
    void UpdateOrRequestVehicleEngineSound(eVehicleEngineSoundType st, float freq, float volume);

    static void EnableHelicoptors();
    static void DisableHelicoptors();
    void EnableHelicoptor();
    void DisableHelicoptor();

    static constexpr float GetDummyRevRatioProgress(float ratio);
    float GetVolumeForDummyIdle(float fGearRevProgress, float fRevProgressBaseline) const;
    float GetFrequencyForDummyIdle(float fGearRevProgress, float fRevProgressBaseline) const;
    [[nodiscard]] float GetFreqForIdle(float fRatio) const;

    static constexpr float GetDummyIdleRatioProgress(float ratio);
    float GetVolumeForDummyRev(float fRatio, float fFadeRatio) const;
    float GetFrequencyForDummyRev(float fRatio, float fFadeRatio) const;

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
    void ProcessDummyStateTransition(eAEState newState, float fRatio, cVehicleParams& params);
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

    auto GetVehicle() const { return m_Entity->AsVehicle(); }
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

inline std::string_view EnumToString(CAEVehicleAudioEntity::eAEState s) {
    using enum CAEVehicleAudioEntity::eAEState;
    using namespace std::string_view_literals;
    switch (s) {
    case CAR_OFF:               return "CAR_OFF"sv;
    case DUMMY_ID:              return "DUMMY_ID"sv;
    case DUMMY_CRZ:             return "DUMMY_CRZ"sv;
    case PLAYER_AC_FULL:        return "PLAYER_AC_FULL"sv;
    case PLAYER_WHEEL_SPIN:     return "PLAYER_WHEEL_SPIN"sv;
    case PLAYER_CRZ:            return "PLAYER_CRZ"sv;
    case PLAYER_ID:             return "PLAYER_ID"sv;
    case PLAYER_REVERSE:        return "PLAYER_REVERSE"sv;
    case PLAYER_REVERSE_OFF:    return "PLAYER_REVERSE_OFF"sv;
    case PLAYER_FAILING_TO_AC:  return "PLAYER_FAILING_TO_AC"sv;
    default:                    return "<invalid>";
    }
}

inline std::string_view EnumToString(CAEVehicleAudioEntity::eCarEngineSoundType st) {
    using namespace std::string_view_literals;
    switch (st) {
    case CAEVehicleAudioEntity::AE_SOUND_ENGINE_OFF:    return "ENGINE_OFF"sv;
    case CAEVehicleAudioEntity::AE_SOUND_CAR_REV:       return "CAR_REV"sv;
    case CAEVehicleAudioEntity::AE_SOUND_CAR_ID:        return "CAR_ID"sv;
    case CAEVehicleAudioEntity::AE_SOUND_PLAYER_CRZ:    return "PLAYER_CRZ"sv;
    case CAEVehicleAudioEntity::AE_SOUND_PLAYER_AC:     return "PLAYER_AC"sv;
    case CAEVehicleAudioEntity::AE_SOUND_PLAYER_OFF:    return "PLAYER_OFF"sv;
    case CAEVehicleAudioEntity::AE_SOUND_PLAYER_REVERSE:return "PLAYER_REVERSE"sv;
    case CAEVehicleAudioEntity::AE_SOUND_NITRO1:        return "NITRO1"sv;
    case CAEVehicleAudioEntity::AE_SOUND_NITRO2:        return "NITRO2"sv;
    case CAEVehicleAudioEntity::AE_SOUND_STEAM:         return "STEAM"sv;
    case CAEVehicleAudioEntity::AE_SOUND_FUCKED:        return "FUCKED"sv;
    case CAEVehicleAudioEntity::AE_SOUND_MOVING_PARTS:  return "MOVING_PARTS"sv;
    case CAEVehicleAudioEntity::AE_SOUND_ENGINE_MAX:    return "ENGINE_MAX"sv;
    default:                                            return "<invalid>";
    }
}

inline std::string_view EnumToString(eRadioType rt) {
    using namespace std::string_view_literals;
    switch (rt) {
    case RADIO_CIVILIAN:  return "CIVILIAN"sv;
    case RADIO_SPECIAL:   return "SPECIAL"sv;
    case RADIO_UNKNOWN:   return "UNKNOWN"sv;
    case RADIO_EMERGENCY: return "EMERGENCY"sv;
    case RADIO_DISABLED:  return "DISABLED"sv;
    default:              return "<invalid>";
    }
}

inline std::string_view EnumToString(eAEVehicleSoundType v) {
    using namespace std::string_view_literals;
    switch (v) {
    case AE_CAR:                    return "CAR"sv;
    case AE_BIKE:                   return "BIKE"sv;
    case AE_BMX:                    return "BMX"sv;
    case AE_BOAT:                   return "BOAT"sv;
    case AE_AIRCRAFT_HELICOPTER:    return "AIRCRAFT_HELICOPTER"sv;
    case AE_AIRCRAFT_PLANE:         return "AIRCRAFT_PLANE"sv;
    case AE_AIRCRAFT_SEAPLANE:      return "AIRCRAFT_SEAPLANE"sv;
    case AE_ONE_GEAR:               return "ONE_GEAR"sv;
    case AE_TRAIN:                  return "TRAIN"sv;
    case AE_SPECIAL:                return "SPECIAL"sv;
    case AE_NO_VEHICLE:             return "NO_VEHICLE"sv;
    default:                        return "<invalid>";
    }
}
