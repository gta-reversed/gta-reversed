#pragma once

#include "AESound.h"
#include "Physical.h"
#include "Timer.h"
#include <Enums/eAudioEvents.h>

#undef PlaySound

static constexpr int32 MAX_NUM_SOUNDS = 300;
static constexpr int32 MAX_NUM_AUDIO_CHANNELS = 64;

enum eSoundPlayingStatus : int16 {
    SOUND_NOT_PLAYING = 0,
    SOUND_PLAYING = 1,
    SOUND_HAS_STARTED = 2,
};

using tSoundReference = int16;

class CAESoundManager {
public:
    uint16           m_NumAllocatedPhysicalChannels;
    int16            m_AudioHardwareHandle;
    CAESound         m_VirtuallyPlayingSoundList[MAX_NUM_SOUNDS];
    tSoundReference* m_PhysicallyPlayingSoundList;                        //!< List of sounds that are currently playing (Size: `m_AllocatedPhysicalChannels`)
    int16*           m_ChannelPosition;                                   //!< Sound positions (Size: `m_AllocatedPhysicalChannels`)
    tSoundReference* m_PrioritisedSoundList;                              //!< List of sounds that are currently playing and are uncancellable (Size: `m_AllocatedPhysicalChannels`)
    int16            m_VirtualChannelSoundLengths[MAX_NUM_SOUNDS];        //!< Sound lengths in ms (Size: `m_AllocatedPhysicalChannels`)
    int16            m_VirtualChannelSoundLoopStartTimes[MAX_NUM_SOUNDS]; //!< Sound loop start times in ms (Size: `m_AllocatedPhysicalChannels`)
    uint32           m_TimeLastCalled;
    bool             m_WasGamePausedLastFrame;
    bool             m_IsManuallyPaused;
    uint32           m_TimeLastCalledUnpaused;

public:
    static void InjectHooks();

    CAESoundManager() = default;  // 0x4EFD30
    ~CAESoundManager() = default; // 0x4EFD60

    bool      Initialise();
    void      Terminate();
    void      Reset();
    void      PauseManually(uint8 bPause);
    void      Service();
    CAESound* RequestNewSound(CAESound* pSound);

    struct tSoundPlayParams {
        eSoundBankSlot  BankSlotID;               //!< The slot to play the sound from
        eSoundID        SoundID;                  //!< The sound ID to play (From the specified bank slot)
        CAEAudioEntity* AudioEntity;              //!< The audio entity that requested this sound
        CVector         Pos;                      //!< The position
        float           Volume;                   //!< The volume (in dB)
        float           RollOffFactor{ 1.f };     //!< ??
        float           Speed{ 1.f };             //!< Speed
        float           Doppler{ 1.f };           //!< Scales the factor of doppler shift effect, see `CAEAudioEnvironment::GetDopplerRelativeFrequency`
        uint8           FrameDelay{ 0 };          //!< After how many frames the sound will be played
        uint32          Flags{ 0 };               //!< See `eSoundEnvironment`
        float           FrequencyVariance{ 0.f }; //!< Random speed variance (?)
        int16           PlayTime{ 0 };            //!< Where to start the sound (AKA `CurrentPlayPosition`)

        // NOTSA Args //
        CEntity*        RegisterWithEntity{};     //!< The entity to register this sound with (Automatically adds the `SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY` flag)
        int32           EventID{ AE_UNDEFINED };  //!< Event ID
        float           ClientVariable{ 0.f };    //!< Custom value that is just stored
    };
    /*!
     * @brief Play a new sound (Works like calling `RequestNewSound` with an initialized `CAESound` object)
     * @return The sound object, or null if it can't be played
     */
    CAESound* PlaySound(tSoundPlayParams p);
    int16     AreSoundsPlayingInBankSlot(int16 bankSlot);
    int16     AreSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity);
    int16     AreSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical);
    void      CancelSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity);
    void      CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical);
    void      CancelSoundsInBankSlot(int16 bankSlot, bool bFullStop);
    void      CancelSoundsOwnedByAudioEntity(CAEAudioEntity* audioEntity, bool bFullStop);
    int16     GetVirtualChannelForPhysicalChannel(int16 physicalChannel) const;

    auto GetPhysicallyPlayingSoundList() const { return std::span{ m_PhysicallyPlayingSoundList, m_NumAllocatedPhysicalChannels }; }
    auto GetChannelPositions() const { return std::span{ m_ChannelPosition, m_NumAllocatedPhysicalChannels }; }
    auto GetPrioritisedSoundList() const { return std::span{ m_PrioritisedSoundList, m_NumAllocatedPhysicalChannels }; }

private:
    CAESound* GetFreeSound(size_t* outIdx);

public:
    bool IsPaused() const { return CTimer::GetIsPaused() || m_IsManuallyPaused; }
    bool IsSoundPaused(const CAESound& sound) const { return CAESoundManager::IsPaused() && !sound.IsUnpausable(); }
};
VALIDATE_SIZE(CAESoundManager, 0x8CBC);

extern CAESoundManager& AESoundManager;
