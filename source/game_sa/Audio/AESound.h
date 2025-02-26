/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "Enums/eSoundBankSlot.h"
#include "Enums/SoundIDs.h"

class CAEAudioEntity;
class CEntity;

enum eSoundEnvironment : uint16 {
    SOUND_DEFAULT                          = 0x0,
    SOUND_FRONT_END                        = 0x1,
    SOUND_IS_CANCELLABLE                   = 0x2,
    SOUND_REQUEST_UPDATES                  = 0x4,
    SOUND_PLAY_PHYSICALLY                  = 0x8,
    SOUND_IS_PAUSABLE                      = 0x10,
    SOUND_START_PERCENTAGE                 = 0x20,
    SOUND_MUSIC_MASTERED                   = 0x40,
    SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY = 0x80,
    SOUND_IS_DUCKABLE                      = 0x100,
    SOUND_IS_COMPRESSABLE                  = 0x200,
    SOUND_ROLLED_OFF                       = 0x400,
    SOUND_SMOOTH_DUCKING                   = 0x800,
    SOUND_FORCED_FRONT                     = 0x1000
};

class CAESound {
    static constexpr float fSlowMoFrequencyScalingFactor = 0.5F;
public:
    static void InjectHooks();

    CAESound() = default;
    CAESound(
        eSoundBankSlot  bankSlot,
        eSoundID        sfxId,
        CAEAudioEntity* audioEntity,
        CVector         pos,
        float           volume,
        float           rollOff,
        float           speed,
        float           doppler,
        uint8           frameDelay,
        uint16          flags, 
        float           speedVariance
    );
    ~CAESound();

    void Initialise(
        eSoundBankSlot  bankSlot,
        eSoundID        sfxId,
        CAEAudioEntity* audioEntity,
        CVector         pos,
        float           volume,
        float           rollOff       = 1.f,
        float           speed         = 1.f,
        float           doppler       = 1.f,
        uint8           frameDelay    = 0,
        uint16          flags         = 0,
        float           speedVariance = 0.f,
        int16           playTime      = 0
    );

    void  UnregisterWithPhysicalEntity();
    void  StopSound();
    void  SetIndividualEnvironment(uint16 envFlag, uint16 bEnabled); // pass eSoundEnvironment as envFlag
    void  UpdatePlayTime(int16 soundLength, int16 loopStartTime, int16 playProgress);
    CVector GetRelativePosition() const;
    void  GetRelativePosition(CVector* outVec) const;
    void  CalculateFrequency();
    void  UpdateFrequency();
    float GetRelativePlaybackFrequencyWithDoppler() const;
    float GetSlowMoFrequencyScalingFactor() const;
    void  NewVPSLEntry();
    void  RegisterWithPhysicalEntity(CEntity* entity);
    void  StopSoundAndForget();
    void  SetPosition(CVector vecPos);
    void  CalculateVolume();
    void  UpdateParameters(int16 curPlayPos);
    void  SoundHasFinished();

    void SetSpeed(float s) noexcept { m_Speed = s; }
    auto GetSpeed() const noexcept  { return m_Speed; }

    void SetVolume(float v) noexcept { m_Volume = v; }
    auto GetVolume() const noexcept  { return m_Volume; }

    auto GetSoundLength() const noexcept { return m_Length; }

    bool IsCancellable() const { return m_IsCancellable; }
    bool IsFrontEnd() const { return m_IsFrontEnd; }
    bool GetRequestUpdates() const { return m_RequestUpdates; }
    bool IsPausable() const { return m_IsPausable; }
    bool GetPlayPhysically() const { return m_PlayPhysically; };
    bool GetIsStartPercentage() const { return m_PlayTimeIsPercentage; }
    bool IsMusicMastered() const { return m_IsMusicMastered; }
    bool IsLifespanTiedToPhysicalEntity() const { return m_IsLifespanTiedToPhysicalEntity; }
    bool IsDuckablle() const { return m_IsDuckable; }
    bool IsCompressable() const { return m_IsCompressable; }
    bool GetRolledOff() const { return m_IsRolledOff; }
    bool GetSmoothDucking() const { return m_HasSmoothDucking; }
    bool IsForcedFront() const { return m_IsForcedFront; }
    bool IsActive() const { return m_IsInUse; }
    bool WasServiced() const { return m_IsAudioHardwareAware; }

public:
    eSoundBankSlot  m_BankSlot{};           //!< Slot to use for the sound
    eSoundID        m_SoundID{};            //!< Sound ID in the bank that's loaded into the slot
    CAEAudioEntity* m_AudioEntity{};        //!< The entity that's playing this sound
    CEntity::Ref    m_PhysicalEntity{};     //!< If set, the sound is tied to this entity
    int32           m_Event{AE_UNDEFINED};              //!< Not necessarily `eAudioEvents`, for ex. see `CAEWeaponAudioEntity`
    float           m_ClientVariable{-1.f};     //!< Custom variable set when playing the sound (Can be anything, not just a float)
    float           m_Volume{};             //!< Volume of the sound (Used to calculate the final volume, `ListenerVolume`)
    float           m_RollOffFactor{};      //!< Roll-off factor
    float           m_Speed{};              //!< Speed of the sound (Used to calculate the final frequency, `ListenerSpeed`)
    float           m_SpeedVariance{};      //!< Speed variability
    CVector         m_CurrPos{};            //!< Current position of the sound
    CVector         m_PrevPos{};            //!< Previous position of the sound the last time it was updated
    int32           m_LastFrameUpdatedAt{}; //!< Frame count when the sound was last updated (`CTimer::GetFrameCounter()`)
    int32           m_CurrTimeUpdateMs{};   //!< Time in milliseconds when the sound was updated (`CTimer::GetTimeInMS()`)
    int32           m_PrevTimeUpdateMs{};   //!< Time in milliseconds when the sound was last updated (`CTimer::GetTimeInMS()`)
    float           m_CurrCamDist{};        //!< Distance to the camera
    float           m_PrevCamDist{};        //!< Distance to the camera the last time the sound was updated
    float           m_Doppler{};            //!< Doppler effect
    uint8           m_FrameDelay{};         //!< Seemingly never used, but CAESoundManager::Service still checks for that
    char            __pad;
    union {
        uint16 m_Flags{};
        struct {
            uint16 m_IsFrontEnd : 1;
            uint16 m_IsCancellable : 1;
            uint16 m_RequestUpdates : 1;
            uint16 m_PlayPhysically : 1;
            uint16 m_IsPausable : 1;
            uint16 m_PlayTimeIsPercentage : 1;
            uint16 m_IsMusicMastered : 1;
            uint16 m_IsLifespanTiedToPhysicalEntity : 1;

            uint16 m_IsDuckable : 1;
            uint16 m_IsCompressable : 1;
            uint16 m_IsRolledOff : 1;
            uint16 m_HasSmoothDucking : 1;
            uint16 m_IsForcedFront : 1;
        };
    };
    uint16 m_IsInUse{true};
    int16  m_IsAudioHardwareAware{};
    int16  m_PlayTime{};
    int16  m_IsPhysicallyPlaying{};
    float  m_ListenerVolume{-100.f};
    float  m_ListenerSpeed{1.f};
    int16  m_HasRequestedStopped{};
    float  m_Headroom{};
    int16  m_Length{-1};
};
VALIDATE_SIZE(CAESound, 0x74);
