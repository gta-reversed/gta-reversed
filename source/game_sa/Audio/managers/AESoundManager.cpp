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

    m_AllocatedPhysicalChannels = availChannels >= MAX_NUM_SOUNDS ? MAX_NUM_SOUNDS : availChannels;
    m_AudioHardwareHandle = AEAudioHardware.AllocateChannels(m_AllocatedPhysicalChannels);
    if (m_AudioHardwareHandle == -1)
        return false;

    // BUG? There's some int16 weird logic in the original code, I simplified it to what's actually used i think
    m_PhysicallyPlayingSoundList = new int16[m_AllocatedPhysicalChannels];
    m_ChannelPosition = new int16[m_AllocatedPhysicalChannels];
    m_PrioritisedSoundList = new int16[m_AllocatedPhysicalChannels];

    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        sound.m_nIsUsed = 0;
        sound.m_nHasStarted = 0;
    }

    std::fill_n(m_PhysicallyPlayingSoundList, m_AllocatedPhysicalChannels, -1);

    m_TimeLastCalled = CTimer::GetTimeInMS();
    m_WasGamePausedLastFrame = false;
    m_TimeLastCalledUnpaused = 0;
    m_IsManuallyPaused = false;

    NOTSA_LOG_DEBUG("Initialised SoundManager");
    return true;
}

// 0x4EFAA0
void CAESoundManager::Terminate() {
    delete[] m_PhysicallyPlayingSoundList;
    delete[] m_ChannelPosition;
    delete[] m_PrioritisedSoundList;

    m_PhysicallyPlayingSoundList = nullptr;
    m_ChannelPosition = nullptr;
    m_PrioritisedSoundList = nullptr;
}

// 0x4EF4D0
void CAESoundManager::Reset() {
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed())
            continue;

        sound.StopSound();
    }
}

// 0x4EF510
void CAESoundManager::PauseManually(uint8 bPause) {
    m_IsManuallyPaused = bPause;
}

// 0x4F0000
void CAESoundManager::Service() {
    // Clear sounds uncancellable status for this frame
    std::fill_n(m_PrioritisedSoundList, m_AllocatedPhysicalChannels, -1);

    // Calculate time diff from last update
    uint32 timeSinceLastUpdate;
    if (CAESoundManager::IsPaused()) {
        if (m_WasGamePausedLastFrame)
            timeSinceLastUpdate = CTimer::GetTimeInMSPauseMode() - m_TimeLastCalled;
        else {
            m_TimeLastCalledUnpaused = m_TimeLastCalled;
            timeSinceLastUpdate = 0;
        }

        m_TimeLastCalled = CTimer::GetTimeInMSPauseMode();
        m_WasGamePausedLastFrame = true;
    } else {
        timeSinceLastUpdate = CTimer::GetTimeInMS() - (m_WasGamePausedLastFrame ? m_TimeLastCalledUnpaused : m_TimeLastCalled);

        m_TimeLastCalled = CTimer::GetTimeInMS();
        m_WasGamePausedLastFrame = false;
    }

    // Get current frame sounds infos
    AEAudioHardware.GetChannelPlayTimes(m_AudioHardwareHandle, m_ChannelPosition);
    AEAudioHardware.GetVirtualChannelSoundLengths(m_VirtualChannelSoundLengths);
    AEAudioHardware.GetVirtualChannelSoundLoopStartTimes(m_VirtualChannelSoundLoopStartTimes);

    // Initialize sounds that are using percentage specified start positions 0x4F011C
    for (auto&& [i, sound] : rngv::enumerate(m_VirtuallyPlayingSoundList)) {
        if (!sound.IsUsed() || !sound.WasServiced() || !sound.GetStartPercentage())
            continue;

        sound.SetIndividualEnvironment(eSoundEnvironment::SOUND_START_PERCENTAGE, false);
        if (sound.m_nHasStarted)
            continue;

        //sound.m_nCurrentPlayPosition *= uint16(static_cast<float>(m_aSoundLengths[i]) / 100.0F);
        sound.m_nCurrentPlayPosition = static_cast<uint16>((float)(sound.m_nCurrentPlayPosition * m_VirtualChannelSoundLengths[i]) / 100.0f);
    }

    // Stop sounds that turned inactive
    for (auto i = 0; i < m_AllocatedPhysicalChannels; ++i) {
        const auto channelSound = m_PhysicallyPlayingSoundList[i];
        if (channelSound == -1)
            continue;

        auto& sound = m_VirtuallyPlayingSoundList[channelSound];
        sound.m_nCurrentPlayPosition = m_ChannelPosition[i];
        if (sound.m_nPlayingState != eSoundState::SOUND_ACTIVE)
            AEAudioHardware.StopSound(m_AudioHardwareHandle, i);
    }

    // Update sounds playtime
    for (auto&& [i, sound] : rngv::enumerate(m_VirtuallyPlayingSoundList)) {
        if (!sound.IsUsed() || !sound.WasServiced() || sound.m_nIgnoredServiceCycles)
            continue;

        auto usedProgress = CAESoundManager::IsSoundPaused(sound) ? 0 : timeSinceLastUpdate;
        sound.UpdatePlayTime(m_VirtualChannelSoundLengths[i], m_VirtualChannelSoundLoopStartTimes[i], usedProgress);
    }

    // Remove songs that ended from sounds table
    for (auto i = 0; i < m_AllocatedPhysicalChannels; ++i)
        if (m_ChannelPosition[i] == -1)
            m_PhysicallyPlayingSoundList[i] = -1;

    // Mark songs that ended as finished
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed() || !sound.WasServiced() || sound.m_nCurrentPlayPosition != -1)
            continue;

        sound.SoundHasFinished();
    }

    // Update song positions and volumes
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (sound.IsUsed()) {
            sound.UpdateParameters(sound.m_nCurrentPlayPosition);
            sound.CalculateVolume();
        }
    }

    // Mark uncancellable songs as so for this frame
    auto numUncancellableSoundsThisFrame = 0;
    for (auto i = 0; i < m_AllocatedPhysicalChannels; ++i) {
        const auto channelSound = m_PhysicallyPlayingSoundList[i];
        if (channelSound == -1 || !m_VirtuallyPlayingSoundList[channelSound].GetUncancellable())
            continue;

        m_PrioritisedSoundList[numUncancellableSoundsThisFrame] = channelSound;
        ++numUncancellableSoundsThisFrame;
    }

    // Mark some more songs as uncancellable under specific conditions
    for (auto&& [i, sound] : rngv::enumerate(m_VirtuallyPlayingSoundList)) {
        if (!sound.IsUsed() || (sound.m_nHasStarted && sound.GetUncancellable()) || sound.m_nIgnoredServiceCycles)
            continue;

        int32 iCurUncancell;
        for (iCurUncancell = m_AllocatedPhysicalChannels - 1; iCurUncancell >= numUncancellableSoundsThisFrame; --iCurUncancell) {
            if (m_PrioritisedSoundList[iCurUncancell] == -1)
                continue;

            auto& uncancellSound = m_VirtuallyPlayingSoundList[m_PrioritisedSoundList[iCurUncancell]];
            if (sound.m_fFinalVolume < uncancellSound.m_fFinalVolume && sound.GetPlayPhysically() <= uncancellSound.GetPlayPhysically()) {
                break;
            }
        }

        auto iFreeUncancellInd = iCurUncancell + 1;
        if (iFreeUncancellInd != m_AllocatedPhysicalChannels) {
            for (auto ind = m_AllocatedPhysicalChannels - 1; ind > iFreeUncancellInd; --ind)
                m_PrioritisedSoundList[ind] = m_PrioritisedSoundList[ind - 1];

            m_PrioritisedSoundList[iFreeUncancellInd] = i;
        }
    }

    // Stop songs that aren't marked as uncancellable in this frame
    for (auto i = 0; i < m_AllocatedPhysicalChannels; ++i) {
        const auto channelSound = m_PhysicallyPlayingSoundList[i];
        if (channelSound == -1)
            continue;

        int32 uncancellIndex;
        for (uncancellIndex = 0; uncancellIndex < m_AllocatedPhysicalChannels; ++uncancellIndex)
            if (channelSound == m_PrioritisedSoundList[uncancellIndex])
                break;

        if (uncancellIndex == m_AllocatedPhysicalChannels) {
            m_VirtuallyPlayingSoundList[channelSound].m_nHasStarted = false;
            m_PhysicallyPlayingSoundList[i] = -1;
            AEAudioHardware.StopSound(m_AudioHardwareHandle, i);
        } else
            m_PrioritisedSoundList[uncancellIndex] = -1;
    }

    // 0x4F0628 - Play sounds that require that
    for (auto i = 0, chN = 0; i < m_AllocatedPhysicalChannels; ++i, ++chN) {
        const auto ref = m_PrioritisedSoundList[i];
        if (ref == -1) {
            continue;
        }

        while (m_PhysicallyPlayingSoundList[chN] != -1 && ++chN < m_AllocatedPhysicalChannels);
        if (chN >= m_AllocatedPhysicalChannels) {
            continue; // TODO: Probably want to `break` here instead?
        }

        m_PhysicallyPlayingSoundList[chN] = ref;
        auto& sound               = m_VirtuallyPlayingSoundList[ref];
        sound.m_nHasStarted       = true;

        auto freq        = sound.GetRelativePlaybackFrequencyWithDoppler();
        auto slomoFactor = sound.GetSlowMoFrequencyScalingFactor();

        CAEAudioHardwarePlayFlags flags{};
        flags.CopyFromAESound(sound);

        AEAudioHardware.PlaySound(m_AudioHardwareHandle, chN, sound.m_nSoundIdInSlot, sound.m_nBankSlotId, sound.m_nCurrentPlayPosition, flags.m_nFlags, sound.m_fSpeed);
        AEAudioHardware.SetChannelVolume(m_AudioHardwareHandle, chN, sound.m_fFinalVolume, 0);
        AEAudioHardware.SetChannelPosition(m_AudioHardwareHandle, chN, sound.GetRelativePosition(), 0);
        AEAudioHardware.SetChannelFrequencyScalingFactor(m_AudioHardwareHandle, chN, freq * slomoFactor);
    }

    // 0x4F0894
    for (auto i = 0; i < m_AllocatedPhysicalChannels; ++i) {
        const auto channelSound = m_PhysicallyPlayingSoundList[i];
        if (channelSound == -1)
            continue;

        auto& sound = m_VirtuallyPlayingSoundList[channelSound];
        if (!sound.IsUsed())
            continue;

        if (!CAESoundManager::IsSoundPaused(sound)) {
            AEAudioHardware.SetChannelVolume(m_AudioHardwareHandle, i, sound.m_fFinalVolume, 0);
            auto freq = sound.GetRelativePlaybackFrequencyWithDoppler();
            auto slomoFactor = sound.GetSlowMoFrequencyScalingFactor();
            AEAudioHardware.SetChannelFrequencyScalingFactor(m_AudioHardwareHandle, i, freq * slomoFactor);
        } else {
            AEAudioHardware.SetChannelVolume(m_AudioHardwareHandle, i, -100.0F, 0);
            AEAudioHardware.SetChannelFrequencyScalingFactor(m_AudioHardwareHandle, i, 0.0F);
        }
        AEAudioHardware.SetChannelPosition(m_AudioHardwareHandle, i, sound.GetRelativePosition(), 0);
    }

    // 0x4F0AB8
    AEAudioHardware.Service();

    // 0x4F0B05
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed()) {
            continue;
        }

        sound.m_bWasServiced = true;
        if (sound.m_nIgnoredServiceCycles > 0 && !CAESoundManager::IsSoundPaused(sound))
            --sound.m_nIgnoredServiceCycles;
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
        AEAudioHardware.RequestVirtualChannelSoundInfo((uint16)sidx, s->m_nSoundIdInSlot, s->m_nBankSlotId);
    }
    return s;
}

CAESound* CAESoundManager::PlaySound(tSoundPlayParams p) {
    if (p.RegisterWithEntity) {
        p.Flags |= SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY;
    }
    CAESound s;
    s.Initialise(
        p.BankSlotID,
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
    s.m_nEvent = p.EventID;
    s.m_ClientVariable = p.ClientVariable;
    if (p.Flags & SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY) {
        s.RegisterWithPhysicalEntity(p.RegisterWithEntity);
    }
    return RequestNewSound(&s);
}

// 0x4EF520
int16 CAESoundManager::AreSoundsPlayingInBankSlot(int16 bankSlot) {
    auto nPlaying = eSoundPlayingStatus::SOUND_NOT_PLAYING;
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed() || sound.m_nBankSlotId != bankSlot) {
            continue; 
        }
        if (sound.m_nHasStarted) {
            return eSoundPlayingStatus::SOUND_HAS_STARTED;
        }
        nPlaying = eSoundPlayingStatus::SOUND_PLAYING;
    }
    return nPlaying;
}

// 0x4EF570
int16 CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity) {
    auto nPlaying = eSoundPlayingStatus::SOUND_NOT_PLAYING;
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed() || sound.m_nEvent != eventId || sound.m_pBaseAudio != audioEntity) {
            continue;
        }
        if (sound.m_nHasStarted) {
            return eSoundPlayingStatus::SOUND_HAS_STARTED;
        }
        nPlaying = eSoundPlayingStatus::SOUND_PLAYING;
    }

    return nPlaying;
}

// 0x4EF5D0
int16 CAESoundManager::AreSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical) {
    bool nPlaying = eSoundPlayingStatus::SOUND_NOT_PLAYING;
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed() || sound.m_nEvent != eventId || sound.m_pBaseAudio != audioEntity || sound.m_pPhysicalEntity != physical) {
            continue;
        }
        if (sound.m_nHasStarted) {
            return eSoundPlayingStatus::SOUND_HAS_STARTED;
        }
        nPlaying = eSoundPlayingStatus::SOUND_PLAYING;
    }

    return nPlaying;
}

// 0x4EFB90
void CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity) {
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed() || sound.m_nEvent != eventId || sound.m_pBaseAudio != audioEntity) {
            continue;
        }
        sound.StopSoundAndForget();
    }
}

// 0x4EFBF0
void CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical) {
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed() || sound.m_nEvent != eventId || sound.m_pBaseAudio != audioEntity || sound.m_pPhysicalEntity != physical) {
            continue;
        }
        sound.StopSoundAndForget();
    }
}

// 0x4EFC60
void CAESoundManager::CancelSoundsInBankSlot(int16 bankSlot, bool bFullStop) {
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed() || sound.m_nBankSlotId != bankSlot) {
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
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
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
    const auto chIdx = physicalChannel - m_AudioHardwareHandle;
    if (chIdx < 0 || chIdx >= m_AllocatedPhysicalChannels) {
        return -1;
    }
    return m_PhysicallyPlayingSoundList[chIdx];
}

// NOTSA
CAESound* CAESoundManager::GetFreeSound(size_t* outIdx) {
    for (auto&& [i, s] : rngv::enumerate(m_VirtuallyPlayingSoundList)) {
        if (!s.IsUsed()) {
            if (outIdx) {
                *outIdx = (size_t)i;
            }
            return &s;
        }
    }
    return nullptr;
}
