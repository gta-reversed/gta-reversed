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

using tSoundID = int16;

enum eSoundEnvironment : uint16 {
    SOUND_DEFAULT                          = 0x0,
    SOUND_FRONT_END                        = 0x1,
    SOUND_UNCANCELLABLE                    = 0x2,
    SOUND_REQUEST_UPDATES                  = 0x4,
    SOUND_PLAY_PHYSICALLY                  = 0x8,
    SOUND_UNPAUSABLE                       = 0x10,
    SOUND_START_PERCENTAGE                 = 0x20,
    SOUND_MUSIC_MASTERED                   = 0x40,
    SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY = 0x80,
    SOUND_UNDUCKABLE                       = 0x100,
    SOUND_UNCOMPRESSABLE                   = 0x200,
    SOUND_ROLLED_OFF                       = 0x400,
    SOUND_SMOOTH_DUCKING                   = 0x800,
    SOUND_FORCED_FRONT                     = 0x1000
};

enum eSoundState : int16 {
    SOUND_ACTIVE  = 0,
    SOUND_STOPPED = 1,
};

class CAESound {
public:
    eSoundBankSlot  m_BankSlot;
    eSoundID        m_SoundID;
    CAEAudioEntity* m_pBaseAudio;
    CEntity*        m_pPhysicalEntity;
    int32           m_Event; // Not necessarily `eAudioEvents`, for ex. see `CAEWeaponAudioEntity`
    float           m_ClientVariable;
    float           m_Volume;
    float           m_RollOffFactor;
    float           m_Speed;
    float           m_SpeedVariance;
    CVector         m_CurrPos;
    CVector         m_PrevPos;
    int32           m_LastFrameUpdateMs;
    int32           m_CurrTimeUpdateMs;
    int32           m_PrevTimeUpdateMs;
    float           m_CurrCamDist;
    float           m_PrevCamDist;
    float           m_Doppler; // AKA TimeScale
    uint8           m_FrameDelay; // Seemingly never used, but CAESoundManager::Service still checks for that
    char            __pad;
    union {
        uint16 m_nEnvironmentFlags;
        struct {
            uint16 m_IsFrontEnd : 1;
            uint16 m_CanBeCancelled : 1;
            uint16 m_RequestUpdates : 1;
            uint16 m_PlayPhysically : 1;
            uint16 m_CanBePaused : 1;
            uint16 m_PlayTimeIsPercentage : 1;
            uint16 m_IsMusicMastered : 1;
            uint16 m_IsLifespanTiedToPhysicalEntity : 1;

            uint16 m_CanBeDucked : 1;
            uint16 m_CanBeCompressed : 1;
            uint16 m_IsRolledOff : 1;
            uint16 m_HasSmoothDucking : 1;
            uint16 m_IsForcedFront : 1;
        };
    };
    uint16 m_IsInUse;
    int16  m_IsAudioHardwareAware;
    int16  m_PlayTime;
    int16  m_IsPhysicallyPlaying;
    float  m_ListenerVolume;
    float  m_ListenerSpeed;
    int16  m_HasRequestedStopped; // see eSoundState
    float  m_Headroom;
    int16  m_Length;

    static constexpr float fSlowMoFrequencyScalingFactor = 0.5F;

public:
    static void InjectHooks();

    CAESound() { m_pPhysicalEntity = nullptr; }
    CAESound(CAESound& sound);
    CAESound(eSoundBankSlot bankSlotId, eSoundID sfxId, CAEAudioEntity* baseAudio, CVector posn, float volume, float fDistance, float speed, float timeScale, uint8 ignoredServiceCycles, eSoundEnvironment environmentFlags, float speedVariability);
    ~CAESound();

    CAESound& operator=(const CAESound& sound);

    void Initialise(
        eSoundBankSlot  bankSlotId,
        eSoundID        soundID,
        CAEAudioEntity* audioEntity,
        CVector         pos,
        float           volume,
        float           rollOffFactor     = 1.f,
        float           relativeFrequency = 1.f, // Speed
        float           doppler           = 1.f,
        uint8           frameDelay        = 0,
        uint32          flags             = 0,
        float           frequencyVariance = 0.f,
        int16           playTime          = 0
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

    bool GetUncancellable() const { return m_CanBeCancelled; }
    bool GetFrontEnd() const { return m_IsFrontEnd; }
    bool GetRequestUpdates() const { return m_RequestUpdates; }
    bool GetUnpausable() const { return m_CanBePaused; }
    bool GetPlayPhysically() const { return m_PlayPhysically; };
    bool GetStartPercentage() const { return m_PlayTimeIsPercentage; }
    bool GetMusicMastered() const { return m_IsMusicMastered; }
    bool GetLifespanTiedToPhysicalEntity() const { return m_IsLifespanTiedToPhysicalEntity; }
    bool GetUnduckable() const { return m_CanBeDucked; }
    bool GetUncompressable() const { return m_CanBeCompressed; }
    bool GetRolledOff() const { return m_IsRolledOff; }
    bool GetSmoothDucking() const { return m_HasSmoothDucking; }
    bool GetForcedFront() const { return m_IsForcedFront; }
    bool IsActive() const { return m_IsInUse; }
    bool WasServiced() const { return m_IsAudioHardwareAware; }

};
VALIDATE_SIZE(CAESound, 0x74);
