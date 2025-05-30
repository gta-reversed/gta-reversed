#include "StdInc.h"

#include "AESoundManager.h"

#include "AEAudioEnvironment.h"
#include "AEAudioHardware.h"

#include <DebugModules/Audio/SoundManagerDebugModule.hpp>
#include <UIRenderer.h>

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
    if (availChannels <= 10) {
        return false;
    }

    m_NumAllocatedPhysicalChannels = availChannels >= MAX_NUM_SOUNDS ? MAX_NUM_SOUNDS : availChannels;
    m_AudioHardwareHandle       = AEAudioHardware.AllocateChannels(m_NumAllocatedPhysicalChannels);
    if (m_AudioHardwareHandle == -1) {
        return false;
    }

    // BUG? There's some int16 weird logic in the original code, I simplified it to what's actually used i think
    m_PhysicallyPlayingSoundList = new tSoundReference[m_NumAllocatedPhysicalChannels];
    m_ChannelPosition            = new int16[m_NumAllocatedPhysicalChannels];
    m_PrioritisedSoundList       = new tSoundReference[m_NumAllocatedPhysicalChannels];

    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        sound.m_nIsUsed             = 0;
        sound.m_IsPhysicallyPlaying = 0;
    }

    std::fill_n(m_PhysicallyPlayingSoundList, m_NumAllocatedPhysicalChannels, -1);

    m_TimeLastCalled         = CTimer::GetTimeInMS();
    m_WasGamePausedLastFrame = false;
    m_TimeLastCalledUnpaused = 0;
    m_IsManuallyPaused       = false;

    NOTSA_LOG_DEBUG("Initialised SoundManager");
    return true;
}

// 0x4EFAA0
void CAESoundManager::Terminate() {
    delete[] m_PhysicallyPlayingSoundList;
    delete[] m_ChannelPosition;
    delete[] m_PrioritisedSoundList;

    m_PhysicallyPlayingSoundList = nullptr;
    m_ChannelPosition            = nullptr;
    m_PrioritisedSoundList       = nullptr;
}

// 0x4EF4D0
void CAESoundManager::Reset() {
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed()) {
            continue;
        }

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
    std::fill_n(m_PrioritisedSoundList, m_NumAllocatedPhysicalChannels, -1);

    // Calculate time diff from last update
    uint32 timeSinceLastUpdate;
    if (CAESoundManager::IsPaused()) {
        if (m_WasGamePausedLastFrame) {
            timeSinceLastUpdate = CTimer::GetTimeInMSPauseMode() - m_TimeLastCalled;
        } else {
            m_TimeLastCalledUnpaused = m_TimeLastCalled;
            timeSinceLastUpdate      = 0;
        }

        m_TimeLastCalled         = CTimer::GetTimeInMSPauseMode();
        m_WasGamePausedLastFrame = true;
    } else {
        timeSinceLastUpdate = CTimer::GetTimeInMS() - (m_WasGamePausedLastFrame ? m_TimeLastCalledUnpaused : m_TimeLastCalled);

        m_TimeLastCalled         = CTimer::GetTimeInMS();
        m_WasGamePausedLastFrame = false;
    }

    // Get current frame sounds infos
    AEAudioHardware.GetChannelPlayTimes(m_AudioHardwareHandle, m_ChannelPosition);
    AEAudioHardware.GetVirtualChannelSoundLengths(m_VirtualChannelSoundLengths);
    AEAudioHardware.GetVirtualChannelSoundLoopStartTimes(m_VirtualChannelSoundLoopStartTimes);

    // 0x4F011C - Initialize sounds that are using percentage specified start positions
    for (auto&& [i, sound] : rngv::enumerate(m_VirtuallyPlayingSoundList)) {
        if (!sound.IsUsed() || !sound.WasServiced() || !sound.GetStartPercentage()) {
            continue;
        }

        sound.SetIndividualEnvironment(eSoundEnvironment::SOUND_START_PERCENTAGE, false);
        if (sound.m_IsPhysicallyPlaying) {
            continue;
        }

        //sound.m_nCurrentPlayPosition *= uint16(static_cast<float>(m_aSoundLengths[i]) / 100.0F);
        sound.m_nCurrentPlayPosition = static_cast<uint16>((float)(sound.m_nCurrentPlayPosition * m_VirtualChannelSoundLengths[i]) / 100.0f);
    }

    // 0x4F016D - Stop sounds that turned inactive
    for (auto&& [i, ref] : rngv::enumerate(GetPhysicallyPlayingSoundList())) {
        if (ref == -1) {
            continue;
        }
        auto& sound                  = m_VirtuallyPlayingSoundList[ref];
        sound.m_nCurrentPlayPosition = m_ChannelPosition[i];
        if (sound.GetPlayingState() != eSoundState::SOUND_ACTIVE) {
            AEAudioHardware.StopSound(m_AudioHardwareHandle, i);
        }
    }

    // 0x4F01E4 - Update sounds playtime
    for (auto&& [i, sound] : rngv::enumerate(m_VirtuallyPlayingSoundList)) {
        if (!sound.IsUsed() || !sound.WasServiced() || sound.m_FrameDelay) {
            continue;
        }
        auto usedProgress = CAESoundManager::IsSoundPaused(sound) ? 0 : timeSinceLastUpdate;
        sound.UpdatePlayTime(m_VirtualChannelSoundLengths[i], m_VirtualChannelSoundLoopStartTimes[i], usedProgress);
    }

    // 0x4F02FB - Remove sounds that ended from sounds table
    for (auto&& [chN, ref] : rngv::enumerate(GetChannelPositions())) {
        if (ref == -1) {
            GetPhysicallyPlayingSoundList()[chN] = -1;
        }
    }

    // 0x4F0329 - Mark sounds that ended as finished
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed() || !sound.WasServiced() || sound.m_nCurrentPlayPosition != -1) {
            continue;
        }
        sound.SoundHasFinished();
    }

    // 0x4F03E5, 0x4F040D - Update sounds positions and volumes
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed()) {
            continue;
        }
        sound.UpdateParameters(sound.m_nCurrentPlayPosition);
        sound.CalculateVolume();
    }

    // 0x4F042C - Find prioritized sounds
    auto numPrioritisedSounds = 0;
    for (auto&& [i, ref] : rngv::enumerate(GetPhysicallyPlayingSoundList())) {
        if (ref == -1) {
            continue;
        }
        if (!m_VirtuallyPlayingSoundList[ref].GetUncancellable()) {
            continue;
        }
        m_PrioritisedSoundList[numPrioritisedSounds++] = ref;
    }

    // Check if we need to insert any of the already playing sounds into the list
    for (auto&& [i, sound] : rngv::enumerate(m_VirtuallyPlayingSoundList)) {
        if (!sound.IsUsed()) {
            continue;
        }
        if (sound.m_IsPhysicallyPlaying && sound.GetUncancellable()) {
            continue;
        }
        if (sound.m_FrameDelay != 0) {
            continue;
        }

        int16 chN = m_NumAllocatedPhysicalChannels - 1;

        // 0x4F04CE - Find last slot in use
        for (; chN > numPrioritisedSounds; chN--) {
            if (GetPrioritisedSoundList()[chN] != -1) {
                break;
            }
        }

        // 0x4F04EB - Find where to insert
        for (; chN >= numPrioritisedSounds; chN--) {
            const auto& soundB = m_VirtuallyPlayingSoundList[m_PrioritisedSoundList[chN]];
            if (sound.m_ListenerVolume >= soundB.m_ListenerVolume) {
                continue;
            }
            if (sound.GetPlayPhysically() <= soundB.GetPlayPhysically()) {
                continue;
            }
            break;
        }

        // 0x4F0529 - Insert at given index
        if (chN != m_NumAllocatedPhysicalChannels - 1) {
            // Shift to right
            for (auto i = m_NumAllocatedPhysicalChannels - 1; i > chN + 1; --i) {
                m_PrioritisedSoundList[i] = m_PrioritisedSoundList[i - 1];
            }

            // Insert
            m_PrioritisedSoundList[chN + 1] = i;
        }
    }

    // 0x4F0585 - Stop songs that aren't marked as uncancellable in this frame
    for (auto i = 0; i < m_NumAllocatedPhysicalChannels; ++i) {
        const auto ref = m_PhysicallyPlayingSoundList[i];
        if (ref == -1) {
            continue;
        }

        int32 chN;
        for (chN = 0; chN < m_NumAllocatedPhysicalChannels; ++chN) {
            if (ref == m_PrioritisedSoundList[chN]) {
                break;
            }
        }

        if (chN == m_NumAllocatedPhysicalChannels) {
            m_VirtuallyPlayingSoundList[ref].m_IsPhysicallyPlaying = false;
            m_PhysicallyPlayingSoundList[i]                        = -1;
            AEAudioHardware.StopSound(m_AudioHardwareHandle, i);
        } else {
            m_PrioritisedSoundList[chN] = -1;
        }
    }

    // 0x4F0628 - Play sounds that require that
    for (auto i = 0, chN = 0; i < m_NumAllocatedPhysicalChannels; ++i) {
        const auto ref = m_PrioritisedSoundList[i];
        if (ref == -1) {
            continue;
        }

        // Find a free channel to use
        while (chN < m_NumAllocatedPhysicalChannels && m_PhysicallyPlayingSoundList[chN] != -1) {
            chN++;
        }
        if (chN >= m_NumAllocatedPhysicalChannels) {
            break;
        }

        m_PhysicallyPlayingSoundList[chN] = ref;
        auto& sound                       = m_VirtuallyPlayingSoundList[ref];
        sound.m_IsPhysicallyPlaying               = true;

        const auto freqFactor = sound.GetRelativePlaybackFrequencyWithDoppler() * sound.GetSlowMoFrequencyScalingFactor();

        CAEAudioHardwarePlayFlags flags{};
        flags.CopyFromAESound(sound);

        AEAudioHardware.PlaySound(m_AudioHardwareHandle, chN, sound.m_nSoundIdInSlot, sound.m_nBankSlotId, sound.m_nCurrentPlayPosition, flags.m_nFlags, sound.m_fSpeed);
        AEAudioHardware.SetChannelVolume(m_AudioHardwareHandle, chN, sound.m_ListenerVolume, 0);
        AEAudioHardware.SetChannelPosition(m_AudioHardwareHandle, chN, sound.GetRelativePosition(), 0);
        AEAudioHardware.SetChannelFrequencyScalingFactor(m_AudioHardwareHandle, chN, freqFactor);
    }

    // 0x4F0894
    for (auto i = 0; i < m_NumAllocatedPhysicalChannels; ++i) {
        const auto ref = m_PhysicallyPlayingSoundList[i];
        if (ref == -1) {
            continue;
        }

        auto& sound = m_VirtuallyPlayingSoundList[ref];
        if (!sound.IsUsed()) {
            continue;
        }

        if (!CAESoundManager::IsSoundPaused(sound)) {
            AEAudioHardware.SetChannelVolume(m_AudioHardwareHandle, i, sound.m_ListenerVolume, 0);
            auto freq        = sound.GetRelativePlaybackFrequencyWithDoppler();
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
        if (sound.m_FrameDelay > 0 && !CAESoundManager::IsSoundPaused(sound)) {
            --sound.m_FrameDelay;
        }
    }
}

// 0x4EFB10
CAESound* CAESoundManager::RequestNewSound(CAESound* pSound) {
    size_t sidx;
    auto   s = GetFreeSound(&sidx);
    if (s) {
        *s = *pSound;
        pSound->UnregisterWithPhysicalEntity();
        s->NewVPSLentry();
        AEAudioHardware.RequestVirtualChannelSoundInfo((uint16)sidx, s->m_nSoundIdInSlot, s->m_nBankSlotId);
    }

#ifdef NOTSA_DEBUG
    notsa::ui::UIRenderer::GetSingleton().GetDebugModules().GetModule<notsa::debugmodules::SoundManagerDebugModule>()->SetSoundInfo(sidx, {
        .StackTrace = std::stacktrace::current()
    });
#endif

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
    s.m_nEvent         = p.EventID;
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
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed() || sound.m_nEvent != eventId || sound.m_pBaseAudio != audioEntity) {
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
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsUsed() || sound.m_nEvent != eventId || sound.m_pBaseAudio != audioEntity || sound.m_pPhysicalEntity != physical) {
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
    if (chIdx < 0 || chIdx >= m_NumAllocatedPhysicalChannels) {
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
