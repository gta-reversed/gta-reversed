/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CAEAudioEntity;
class CEntity;

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
    int16           m_nBankSlotId;
    int16           m_nSoundIdInSlot;
    CAEAudioEntity* m_pBaseAudio;
    CEntity*        m_pPhysicalEntity;
    eAudioEvents    m_nEvent;
    float           m_fMaxVolume;
    float           m_fVolume;
    float           m_fSoundDistance;
    float           m_fSpeed;
    float           m_fSpeedVariability;
    CVector         m_vecCurrPosn;
    CVector         m_vecPrevPosn;
    int32           m_nLastFrameUpdate;
    int32           m_nCurrTimeUpdate;
    int32           m_nPrevTimeUpdate;
    float           m_fCurrCamDist;
    float           m_fPrevCamDist;
    float           m_fTimeScale;
    uint8           m_nIgnoredServiceCycles; // Seemingly never used, but CAESoundManager::Service still checks for that
    char            field_55;
    union {
        uint16 m_nEnvironmentFlags;
        struct {
            uint16 m_bFrontEnd : 1;
            uint16 m_bUncancellable : 1;
            uint16 m_bRequestUpdates : 1;
            uint16 m_bPlayPhysically : 1;
            uint16 m_bUnpausable : 1;
            uint16 m_bStartPercentage : 1;
            uint16 m_bMusicMastered : 1;
            uint16 m_bLifespanTiedToPhysicalEntity : 1;

            uint16 m_bUnduckable : 1;
            uint16 m_bUncompressable : 1;
            uint16 m_bRolledOff : 1;
            uint16 m_bSmoothDucking : 1;
            uint16 m_bForcedFront : 1;
        };
    };
    uint16 m_nIsUsed;
    int16  m_bWasServiced;
    int16  m_nCurrentPlayPosition;
    int16  m_nHasStarted;
    float  m_fFinalVolume;
    float  m_fFrequency;
    int16  m_nPlayingState; // see eSoundState
    char   field_6A[2];
    float  m_fSoundHeadRoom;
    int16  m_nSoundLength;
    int16  field_72;

    static constexpr float fSlowMoFrequencyScalingFactor = 0.5F;

public:
    static void InjectHooks();

    CAESound() { m_pPhysicalEntity = nullptr; }
    CAESound(CAESound& sound);
    CAESound(int16 bankSlotId, int16 sfxId, CAEAudioEntity* baseAudio, CVector posn, float volume, float fDistance, float speed, float timeScale, uint8 ignoredServiceCycles, eSoundEnvironment environmentFlags, float speedVariability);
    ~CAESound();

    CAESound& operator=(const CAESound& sound);

    void Initialise(int16 bankSlotId, int16 sfxId, CAEAudioEntity* baseAudio, CVector posn, float volume,
                    float maxDistance = 1.0f,
                    float speed = 1.0f,
                    float timeScale = 1.0f,
                    uint8 ignoredServiceCycles = 0,
                    eSoundEnvironment environmentFlags = static_cast<eSoundEnvironment>(0),
                    float speedVariability = 0,
                    int16 currPlayPosn = 0);

    void  UnregisterWithPhysicalEntity();
    void  StopSound();
    bool  GetUncancellable() const { return m_bUncancellable; }
    bool  GetFrontEnd() const { return m_bFrontEnd; }
    bool  GetRequestUpdates() const { return m_bRequestUpdates; }
    bool  GetUnpausable() const { return m_bUnpausable; }
    bool  GetPlayPhysically() const { return m_bPlayPhysically; };
    bool  GetStartPercentage() const { return m_bStartPercentage; }
    bool  GetMusicMastered() const { return m_bMusicMastered; }
    bool  GetLifespanTiedToPhysicalEntity() const { return m_bLifespanTiedToPhysicalEntity; }
    bool  GetUnduckable() const { return m_bUnduckable; }
    bool  GetUncompressable() const { return m_bUncompressable; }
    bool  GetRolledOff() const { return m_bRolledOff; }
    bool  GetSmoothDucking() const { return m_bSmoothDucking; }
    bool  GetForcedFront() const { return m_bForcedFront; }
    void  SetIndividualEnvironment(uint16 envFlag, uint16 bEnabled); // pass eSoundEnvironment as envFlag
    void  UpdatePlayTime(int16 soundLength, int16 loopStartTime, int16 playProgress);
    CVector GetRelativePosition() const;
    void  CalculateFrequency();
    void  UpdateFrequency();
    float GetRelativePlaybackFrequencyWithDoppler() const;
    float GetSlowMoFrequencyScalingFactor() const;
    void  NewVPSLentry();
    void  RegisterWithPhysicalEntity(CEntity* entity);
    void  StopSoundAndForget();
    void  SetPosition(CVector vecPos);
    void  CalculateVolume();
    void  UpdateParameters(int16 curPlayPos);
    void  SoundHasFinished();

public:
    bool IsUsed() const { return m_nIsUsed; }
    bool WasServiced() const { return m_bWasServiced; }

};
VALIDATE_SIZE(CAESound, 0x74);
