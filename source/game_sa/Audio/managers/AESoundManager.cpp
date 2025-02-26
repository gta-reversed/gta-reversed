#include "StdInc.h"

#include "AESoundManager.h"

#include "AEAudioEnvironment.h"
#include "AEAudioHardware.h"

CAESoundManager& AESoundManager = *(CAESoundManager*)0xB62CB0;

void CAESoundManager::InjectHooks() {
    RH_ScopedClass(CAESoundManager);
    RH_ScopedCategory("Audio/Managers");

    RH_ScopedInstall(Service, 0x4F0000);
    RH_ScopedInstall(Initialise, 0x5B9690);
    RH_ScopedInstall(Terminate, 0x4EFAA0);
    RH_ScopedInstall(Reset, 0x4EF4D0);
    RH_ScopedInstall(PauseManually, 0x4EF510);
    RH_ScopedInstall(RequestNewSound, 0x4EFB10);
    RH_ScopedInstall(AreSoundsPlayingInBankSlot, 0x4EF520);
    RH_ScopedInstall(AreSoundsOfThisEventPlayingForThisEntity, 0x4EF570);
    RH_ScopedInstall(AreSoundsOfThisEventPlayingForThisEntityAndPhysical, 0x4EF5D0);
    RH_ScopedInstall(GetVirtualChannelForPhysicalChannel, 0x4EF630);
    RH_ScopedInstall(CancelSoundsInBankSlot, 0x4EFC60);
    RH_ScopedInstall(CancelSoundsOwnedByAudioEntity, 0x4EFCD0);
    RH_ScopedInstall(CancelSoundsOfThisEventPlayingForThisEntity, 0x4EFB90);
    RH_ScopedInstall(CancelSoundsOfThisEventPlayingForThisEntityAndPhysical, 0x4EFBF0);
}

// 0x5B9690
bool CAESoundManager::Initialise() {
    const auto availChannels = AEAudioHardware.GetNumAvailableChannels();
    if (availChannels <= 10)
        return false;

    m_nNumAvailableChannels = availChannels >= MAX_NUM_SOUNDS ? MAX_NUM_SOUNDS : availChannels;
    m_nChannel = AEAudioHardware.AllocateChannels(m_nNumAvailableChannels);
    if (m_nChannel == -1)
        return false;

    // BUG? There's some int16 weird logic in the original code, I simplified it to what's actually used i think
    m_aChannelSoundTable = new int16[m_nNumAvailableChannels];
    m_aChannelSoundPlayTimes = new int16[m_nNumAvailableChannels];
    m_aChannelSoundUncancellable = new int16[m_nNumAvailableChannels];

    for (CAESound& sound : m_aSounds) {
        sound.m_IsInUse = 0;
        sound.m_IsPhysicallyPlaying = 0;
    }

    std::fill_n(m_aChannelSoundTable, m_nNumAvailableChannels, -1);

    m_nUpdateTime = CTimer::GetTimeInMS();
    m_bPauseTimeInUse = false;
    m_nPauseUpdateTime = 0;
    m_bManuallyPaused = false;

    DEV_LOG("Initialised SoundManager");
    return true;
}

// 0x4EFAA0
void CAESoundManager::Terminate() {
    delete[] m_aChannelSoundTable;
    delete[] m_aChannelSoundPlayTimes;
    delete[] m_aChannelSoundUncancellable;

    m_aChannelSoundTable = nullptr;
    m_aChannelSoundPlayTimes = nullptr;
    m_aChannelSoundUncancellable = nullptr;
}

// 0x4EF4D0
void CAESoundManager::Reset() {
    for (CAESound& sound : m_aSounds) {
        if (!sound.IsUsed())
            continue;

        sound.StopSound();
    }
}

// 0x4EF510
void CAESoundManager::PauseManually(uint8 bPause) {
    m_bManuallyPaused = bPause;
}

// 0x4F0000
void CAESoundManager::Service() {
    // Clear sounds uncancellable status for this frame
    std::fill_n(m_aChannelSoundUncancellable, m_nNumAvailableChannels, -1);

    // Calculate time diff from last update
    uint32 timeSinceLastUpdate;
    if (CAESoundManager::IsPaused()) {
        if (m_bPauseTimeInUse)
            timeSinceLastUpdate = CTimer::GetTimeInMSPauseMode() - m_nUpdateTime;
        else {
            m_nPauseUpdateTime = m_nUpdateTime;
            timeSinceLastUpdate = 0;
        }

        m_nUpdateTime = CTimer::GetTimeInMSPauseMode();
        m_bPauseTimeInUse = true;
    } else {
        timeSinceLastUpdate = CTimer::GetTimeInMS() - (m_bPauseTimeInUse ? m_nPauseUpdateTime : m_nUpdateTime);

        m_nUpdateTime = CTimer::GetTimeInMS();
        m_bPauseTimeInUse = false;
    }

    // Get current frame sounds infos
    AEAudioHardware.GetChannelPlayTimes(m_nChannel, m_aChannelSoundPlayTimes);
    AEAudioHardware.GetVirtualChannelSoundLengths(m_aSoundLengths);
    AEAudioHardware.GetVirtualChannelSoundLoopStartTimes(m_aSoundLoopStartTimes);

    // Initialize sounds that are using percentage specified start positions 0x4F011C
    for (auto&& [i, sound] : notsa::enumerate(m_aSounds)) {
        if (!sound.IsUsed() || !sound.WasServiced() || !sound.GetStartPercentage())
            continue;

        sound.SetIndividualEnvironment(eSoundEnvironment::SOUND_START_PERCENTAGE, false);
        if (sound.m_IsPhysicallyPlaying)
            continue;

        //sound.m_PlayTime *= uint16(static_cast<float>(m_aSoundLengths[i]) / 100.0F);
        sound.m_PlayTime = static_cast<uint16>((float)(sound.m_PlayTime * m_aSoundLengths[i]) / 100.0f);
    }

    // Stop sounds that turned inactive
    for (auto i = 0; i < m_nNumAvailableChannels; ++i) {
        const auto channelSound = m_aChannelSoundTable[i];
        if (channelSound == -1)
            continue;

        auto& sound = m_aSounds[channelSound];
        sound.m_PlayTime = m_aChannelSoundPlayTimes[i];
        if (sound.m_HasRequestedStopped != eSoundState::SOUND_ACTIVE)
            AEAudioHardware.StopSound(m_nChannel, i);
    }

    // Update sounds playtime
    for (auto&& [i, sound] : notsa::enumerate(m_aSounds)) {
        if (!sound.IsUsed() || !sound.WasServiced() || sound.m_FrameDelay)
            continue;

        auto usedProgress = CAESoundManager::IsSoundPaused(sound) ? 0 : timeSinceLastUpdate;
        sound.UpdatePlayTime(m_aSoundLengths[i], m_aSoundLoopStartTimes[i], usedProgress);
    }

    // Remove songs that ended from sounds table
    for (auto i = 0; i < m_nNumAvailableChannels; ++i)
        if (m_aChannelSoundPlayTimes[i] == -1)
            m_aChannelSoundTable[i] = -1;

    // Mark songs that ended as finished
    for (CAESound& sound : m_aSounds) {
        if (!sound.IsUsed() || !sound.WasServiced() || sound.m_PlayTime != -1)
            continue;

        sound.SoundHasFinished();
    }

    // Update song positions and volumes
    for (CAESound& sound : m_aSounds) {
        if (sound.IsUsed()) {
            sound.UpdateParameters(sound.m_PlayTime);
            sound.CalculateVolume();
        }
    }

    // Mark uncancellable songs as so for this frame
    auto numUncancellableSoundsThisFrame = 0;
    for (auto i = 0; i < m_nNumAvailableChannels; ++i) {
        const auto channelSound = m_aChannelSoundTable[i];
        if (channelSound == -1 || !m_aSounds[channelSound].GetUncancellable())
            continue;

        m_aChannelSoundUncancellable[numUncancellableSoundsThisFrame] = channelSound;
        ++numUncancellableSoundsThisFrame;
    }

    // Mark some more songs as uncancellable under specific conditions
    for (auto&& [i, sound] : notsa::enumerate(m_aSounds)) {
        if (!sound.IsUsed() || (sound.m_IsPhysicallyPlaying && sound.GetUncancellable()) || sound.m_FrameDelay)
            continue;

        int32 iCurUncancell;
        for (iCurUncancell = m_nNumAvailableChannels - 1; iCurUncancell >= numUncancellableSoundsThisFrame; --iCurUncancell) {
            if (m_aChannelSoundUncancellable[iCurUncancell] == -1)
                continue;

            auto& uncancellSound = m_aSounds[m_aChannelSoundUncancellable[iCurUncancell]];
            if (sound.m_ListenerVolume < uncancellSound.m_ListenerVolume && sound.GetPlayPhysically() <= uncancellSound.GetPlayPhysically()) {
                break;
            }
        }

        auto iFreeUncancellInd = iCurUncancell + 1;
        if (iFreeUncancellInd != m_nNumAvailableChannels) {
            for (auto ind = m_nNumAvailableChannels - 1; ind > iFreeUncancellInd; --ind)
                m_aChannelSoundUncancellable[ind] = m_aChannelSoundUncancellable[ind - 1];

            m_aChannelSoundUncancellable[iFreeUncancellInd] = i;
        }
    }

    // Stop songs that aren't marked as uncancellable in this frame
    for (auto i = 0; i < m_nNumAvailableChannels; ++i) {
        const auto channelSound = m_aChannelSoundTable[i];
        if (channelSound == -1)
            continue;

        int32 uncancellIndex;
        for (uncancellIndex = 0; uncancellIndex < m_nNumAvailableChannels; ++uncancellIndex)
            if (channelSound == m_aChannelSoundUncancellable[uncancellIndex])
                break;

        if (uncancellIndex == m_nNumAvailableChannels) {
            m_aSounds[channelSound].m_IsPhysicallyPlaying = false;
            m_aChannelSoundTable[i] = -1;
            AEAudioHardware.StopSound(m_nChannel, i);
        } else
            m_aChannelSoundUncancellable[uncancellIndex] = -1;
    }

    // Play sounds that require that
    for (auto i = 0, curSound = 0; i < m_nNumAvailableChannels; ++i) {
        const auto uncancell = m_aChannelSoundUncancellable[i];
        if (uncancell == -1)
            continue;

        while (m_aChannelSoundTable[curSound] != -1 && curSound < m_nNumAvailableChannels)
            ++curSound;

        if (curSound >= m_nNumAvailableChannels)
            continue;

        m_aChannelSoundTable[curSound] = uncancell;
        auto& sound = m_aSounds[uncancell];
        sound.m_IsPhysicallyPlaying = 1;

        auto freq = sound.GetRelativePlaybackFrequencyWithDoppler();
        auto slomoFactor = sound.GetSlowMoFrequencyScalingFactor();

        CAEAudioHardwarePlayFlags flags{};
        flags.CopyFromAESound(sound);

        AEAudioHardware.PlaySound(m_nChannel, curSound, sound.m_SoundID, sound.m_BankSlot, sound.m_PlayTime, flags.m_nFlags, sound.m_Speed);
        AEAudioHardware.SetChannelVolume(m_nChannel, curSound, sound.m_ListenerVolume, 0);

        AEAudioHardware.SetChannelPosition(m_nChannel, curSound, sound.GetRelativePosition(), 0);
        AEAudioHardware.SetChannelFrequencyScalingFactor(m_nChannel, curSound, freq * slomoFactor);
    }

    for (auto i = 0; i < m_nNumAvailableChannels; ++i) {
        const auto channelSound = m_aChannelSoundTable[i];
        if (channelSound == -1)
            continue;

        auto& sound = m_aSounds[channelSound];
        if (!sound.IsUsed())
            continue;

        if (!CAESoundManager::IsSoundPaused(sound)) {
            AEAudioHardware.SetChannelVolume(m_nChannel, i, sound.m_ListenerVolume, 0);
            auto freq = sound.GetRelativePlaybackFrequencyWithDoppler();
            auto slomoFactor = sound.GetSlowMoFrequencyScalingFactor();
            AEAudioHardware.SetChannelFrequencyScalingFactor(m_nChannel, i, freq * slomoFactor);
        } else {
            AEAudioHardware.SetChannelVolume(m_nChannel, i, -100.0F, 0);
            AEAudioHardware.SetChannelFrequencyScalingFactor(m_nChannel, i, 0.0F);
        }
        AEAudioHardware.SetChannelPosition(m_nChannel, i, sound.GetRelativePosition(), 0);
    }

    AEAudioHardware.Service();

    for (CAESound& sound : m_aSounds) {
        if (!sound.IsUsed()) {
            continue;
        }

        sound.m_IsAudioHardwareAware = true;
        if (sound.m_FrameDelay > 0 && !CAESoundManager::IsSoundPaused(sound))
            --sound.m_FrameDelay;
    }
}

// 0x4EFB10
CAESound* CAESoundManager::RequestNewSound(CAESound* pSound) {
    size_t sidx;
    auto s = GetFreeSound(&sidx);
    if (s) {
        *s = *pSound;
        pSound->UnregisterWithPhysicalEntity();
        s->NewVPSLentry();
        AEAudioHardware.RequestVirtualChannelSoundInfo((uint16)sidx, s->m_SoundID, s->m_BankSlot);
    }
    return s;
}

CAESound* CAESoundManager::PlaySound(tSoundPlayParams p) {
    if (p.RegisterWithEntity) {
        p.Flags |= SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY;
    }
    CAESound s;
    s.Initialise(
        p.BankSlot,
        p.SoundID,
        p.AudioEntity,
        p.Pos,
        p.Volume,
        p.RollOffFactor,
        p.Speed,
        p.Doppler,
        p.FrameDelay,
        p.Flags,
        p.FrequencyVariance,
        p.PlayTime
    );
    s.m_Event = p.EventID;
    s.m_ClientVariable = p.ClientVariable;
    if (p.Flags & SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY) {
        s.RegisterWithPhysicalEntity(p.RegisterWithEntity);
    }
    return RequestNewSound(&s);
}

// 0x4EF520
int16 CAESoundManager::AreSoundsPlayingInBankSlot(int16 bankSlot) {
    auto nPlaying = eSoundPlayingStatus::SOUND_NOT_PLAYING;
    for (CAESound& sound : m_aSounds) {
        if (!sound.IsUsed() || sound.m_BankSlot != bankSlot) {
            continue; 
        }
        if (sound.m_IsPhysicallyPlaying) {
            return eSoundPlayingStatus::SOUND_HAS_STARTED;
        }
        nPlaying = eSoundPlayingStatus::SOUND_PLAYING;
    }
    return nPlaying;
}

// 0x4EF570
int16 CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity) {
    auto nPlaying = eSoundPlayingStatus::SOUND_NOT_PLAYING;
    for (CAESound& sound : m_aSounds) {
        if (!sound.IsUsed() || sound.m_Event != eventId || sound.m_pBaseAudio != audioEntity) {
            continue;
        }
        if (sound.m_IsPhysicallyPlaying) {
            return eSoundPlayingStatus::SOUND_HAS_STARTED;
        }
        nPlaying = eSoundPlayingStatus::SOUND_PLAYING;
    }

    return nPlaying;
}

// 0x4EF5D0
int16 CAESoundManager::AreSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical) {
    bool nPlaying = eSoundPlayingStatus::SOUND_NOT_PLAYING;
    for (CAESound& sound : m_aSounds) {
        if (!sound.IsUsed() || sound.m_Event != eventId || sound.m_pBaseAudio != audioEntity || sound.m_pPhysicalEntity != physical) {
            continue;
        }
        if (sound.m_IsPhysicallyPlaying) {
            return eSoundPlayingStatus::SOUND_HAS_STARTED;
        }
        nPlaying = eSoundPlayingStatus::SOUND_PLAYING;
    }

    return nPlaying;
}

// 0x4EFB90
void CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity) {
    for (CAESound& sound : m_aSounds) {
        if (!sound.IsUsed() || sound.m_Event != eventId || sound.m_pBaseAudio != audioEntity) {
            continue;
        }
        sound.StopSoundAndForget();
    }
}

// 0x4EFBF0
void CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical) {
    for (CAESound& sound : m_aSounds) {
        if (!sound.IsUsed() || sound.m_Event != eventId || sound.m_pBaseAudio != audioEntity || sound.m_pPhysicalEntity != physical) {
            continue;
        }
        sound.StopSoundAndForget();
    }
}

// 0x4EFC60
void CAESoundManager::CancelSoundsInBankSlot(int16 bankSlot, bool bFullStop) {
    for (CAESound& sound : m_aSounds) {
        if (!sound.IsUsed() || sound.m_BankSlot != bankSlot) {
            continue;
        }
        if (bFullStop) {
            sound.StopSoundAndForget();
        } else {
            sound.StopSound();
        }
    }
}

// 0x4EFCD0
void CAESoundManager::CancelSoundsOwnedByAudioEntity(CAEAudioEntity* audioEntity, bool bFullStop) {
    for (CAESound& sound : m_aSounds) {
        if (!sound.IsUsed() || sound.m_pBaseAudio != audioEntity) {
            continue;
        }
        if (bFullStop) {
            sound.StopSoundAndForget();
        } else {
            sound.StopSound();
        }
    }
}

// 0x4EF630, unused
int16 CAESoundManager::GetVirtualChannelForPhysicalChannel(int16 physicalChannel) const {
    const auto chIdx = physicalChannel - m_nChannel;
    if (chIdx < 0 || chIdx >= m_nNumAvailableChannels) {
        return -1;
    }
    return m_aChannelSoundTable[chIdx];
}

// NOTSA
CAESound* CAESoundManager::GetFreeSound(size_t* outIdx) {
    for (auto&& [i, s] : notsa::enumerate(m_aSounds)) {
        if (!s.IsUsed()) {
            if (outIdx) {
                *outIdx = (size_t)i;
            }
            return &s;
        }
    }
    return nullptr;
}
