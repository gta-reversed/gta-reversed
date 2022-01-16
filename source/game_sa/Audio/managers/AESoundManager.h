#pragma once

#include "AESound.h"
#include "Physical.h"

static constexpr int32 MAX_NUM_SOUNDS = 300;
static constexpr int32 MAX_NUM_AUDIO_CHANNELS = 64;

enum eSoundPlayingStatus : int16 {
    SOUND_NOT_PLAYING = 0,
    SOUND_PLAYING = 1,
    SOUND_HAS_STARTED = 2,
};

class CAESoundManager {
public:
    uint16   m_nNumAvailableChannels;
    int16    m_nChannel;
    CAESound m_aSounds[MAX_NUM_SOUNDS];
    int16*   m_aChannelSoundTable;
    int16*   m_aChannelSoundPlayTimes;
    int16*   m_aChannelSoundUncancellable;
    int16    m_aSoundLengths[MAX_NUM_SOUNDS];
    int16    m_aSoundLoopStartTimes[MAX_NUM_SOUNDS];
    uint32   m_nUpdateTime;
    bool     m_bPauseTimeInUse;
    bool     m_bManuallyPaused;
    uint8    field_8CB6[2];
    uint32   m_nPauseUpdateTime;

public:
    static void InjectHooks();

    CAESoundManager();
    ~CAESoundManager();

    bool      Initialise();
    void      Terminate();
    void      Reset();
    void      PauseManually(uint8 bPause);
    void      Service();
    CAESound* RequestNewSound(CAESound* pSound);
    int16     AreSoundsPlayingInBankSlot(int16 bankSlot);
    int16     AreSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity);
    int16     AreSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical);
    void      CancelSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity);
    void      CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical);
    void      CancelSoundsInBankSlot(int16 bankSlot, uint8 bFullStop);
    void      CancelSoundsOwnedByAudioEntity(CAEAudioEntity* audioEntity, uint8 bFullStop);
    int16     GetVirtualChannelForPhysicalChannel(int16 physicalChannel);

public:
    bool IsPaused() const { return CTimer::GetIsPaused() || m_bManuallyPaused; }
    bool IsSoundPaused(CAESound const& sound) const { return CAESoundManager::IsPaused() && !sound.GetUnpausable(); }
};
VALIDATE_SIZE(CAESoundManager, 0x8CBC);

extern CAESoundManager& AESoundManager;
