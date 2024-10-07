#include "StdInc.h"

#include "AEAudioChannel.h"

#include "AEAudioUtility.h"
#include "AESmoothFadeThread.h"

uint32& g_numSoundChannelsUsed = *(uint32*)0xB5F898;

void CAEAudioChannel::InjectHooks() {
    RH_ScopedVirtualClass(CAEAudioChannel, 0x85F03C, 9);
    RH_ScopedCategory("Audio/Hardware");

    RH_ScopedInstall(SetPosition, 0x4D7950);
    RH_ScopedInstall(UpdateStatus, 0x4D7BD0);
    RH_ScopedInstall(Lost, 0x4D7A10);
    RH_ScopedInstall(ConvertFromBytesToMS, 0x4D79D0);
    RH_ScopedInstall(ConvertFromMsToBytes, 0x4D79F0);
    RH_ScopedInstall(SetFrequency, 0x4D7A50);
    RH_ScopedInstall(SetVolume, 0x4D7C60);
    RH_ScopedInstall(SetOriginalFrequency, 0x4D7A70);
    RH_ScopedVMTInstall(SetFrequencyScalingFactor, 0x4D7D00);
    RH_ScopedInstall(GetCurrentPlaybackPosition, 0x4D79A0);
}

// 0x4D7890
CAEAudioChannel::CAEAudioChannel(IDirectSound* directSound, uint16 channelId, uint32 samplesPerSec, uint16 bitsPerSample) {
    m_pDirectSound        = directSound;
    m_nChannelId          = channelId;
    m_nFlags              = 0;
    m_nBufferStatus       = 0;
    m_nFrequency          = samplesPerSec;
    m_nOriginalFrequency  = samplesPerSec;
    m_fVolume             = -100.0f;
    m_pDirectSoundBuffer  = nullptr;
    m_pDirectSound3DBuffer= nullptr;
    m_bNoScalingFactor    = false;
    m_bLooped             = false;
    field_45              = 0;
    m_WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    m_WaveFormat.nChannels = 1;
    m_WaveFormat.nSamplesPerSec = samplesPerSec;
    m_WaveFormat.nAvgBytesPerSec = samplesPerSec * (bitsPerSample / 8);
    m_WaveFormat.nBlockAlign = 2;
    m_WaveFormat.wBitsPerSample = bitsPerSample;
    m_WaveFormat.cbSize = 0;
}

// 0x4D7910
CAEAudioChannel::~CAEAudioChannel() {
    if (m_pDirectSoundBuffer) {
        --g_numSoundChannelsUsed;
        std::exchange(m_pDirectSoundBuffer, nullptr)->Release();
    }

    if (m_pDirectSound3DBuffer) {
        std::exchange(m_pDirectSound3DBuffer, nullptr)->Release();
    }
}

// 0x4D7D00
void CAEAudioChannel::SetFrequencyScalingFactor(float factor) {
    if (factor == 0.0F) {
        if (m_pDirectSoundBuffer &&
            !m_bNoScalingFactor &&
            IsBufferPlaying() &&
            !AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, -100.0F, -1, true)
        ) {
            m_pDirectSoundBuffer->Stop();
        }

        m_bNoScalingFactor = true;
        return;
    }

    const auto newFreq = static_cast<uint32>(float(m_nOriginalFrequency) * factor);
    SetFrequency(newFreq);

    if (m_bNoScalingFactor) {
        if (m_pDirectSoundBuffer) {
            const auto curPos = GetCurrentPlaybackPosition();
            if (curPos != 0) {
                m_pDirectSoundBuffer->SetVolume(-10000);
            }

            m_pDirectSoundBuffer->Play(0, 0, m_bLooped);

            if (curPos != 0 && !AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, m_fVolume, -2, false)) {
                const auto volume = static_cast<LONG>(m_fVolume * 100.0F);
                m_pDirectSoundBuffer->SetVolume(volume);
            }
        }
        m_bNoScalingFactor = false;
    }
}

// 0x4D7950
void CAEAudioChannel::SetPosition(const CVector& vecPos) const {
    if (!m_pDirectSoundBuffer)
        return;

    if (!m_pDirectSound3DBuffer)
        return;

#ifdef USE_DSOUND
    m_pDirectSound3DBuffer->SetPosition(vecPos.x, vecPos.y, vecPos.z, DS3D_DEFERRED);
#endif
}

// 0x4D7C60
void CAEAudioChannel::SetVolume(float volume) {
    if (!m_pDirectSoundBuffer)
        return;

    if (IsBufferPlaying() && fabs(volume - m_fVolume) > 60.0F) {
        if (volume <= m_fVolume) {
            if (AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, volume, -1, false)) {
                m_fVolume = volume;
                return;
            }
        } else if (AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, volume, -2, false)) {
            m_fVolume = volume;
            return;
        }
    }

    AESmoothFadeThread.SetBufferVolume(m_pDirectSoundBuffer, volume);
    m_fVolume = volume;
}

// 0x4D79A0
uint32 CAEAudioChannel::GetCurrentPlaybackPosition() const {
    if (!m_pDirectSoundBuffer)
        return 0;

    uint32 outPos = 0;
    m_pDirectSoundBuffer->GetCurrentPosition(reinterpret_cast<LPDWORD>(&outPos), nullptr);
    return outPos;
}

// 0x4D79D0
uint32 CAEAudioChannel::ConvertFromBytesToMS(uint32 bytes) const {
    return CAEAudioUtility::ConvertFromBytesToMS(bytes, m_WaveFormat.nSamplesPerSec, m_WaveFormat.nChannels);
}

// 0x4D79F0
uint32 CAEAudioChannel::ConvertFromMsToBytes(uint32 ms) const {
    return CAEAudioUtility::ConvertFromMSToBytes(ms, m_WaveFormat.nSamplesPerSec, m_WaveFormat.nChannels);
}

// 0x4D7A50
void CAEAudioChannel::SetFrequency(uint32 freq) {
    if (m_nFrequency == freq)
        return;

    m_nFrequency = freq;

    #ifdef USE_DSOUND
    if (m_pDirectSoundBuffer) {
        VERIFY_TODO_FIX(SUCCEEDED(m_pDirectSoundBuffer->SetFrequency(freq)));
    }
    #endif
}

// 0x4D7A70
void CAEAudioChannel::SetOriginalFrequency(uint32 freq) {
    SetFrequency(freq);
    m_nOriginalFrequency = freq;
}

// 0x4D7BD0
void CAEAudioChannel::UpdateStatus() {
#ifdef USE_DSOUND
    m_pDirectSoundBuffer->GetStatus(reinterpret_cast<LPDWORD>(&m_nBufferStatus));
    if (m_nBufferStatus & DSBSTATUS_BUFFERLOST) {
        Lost();
    }
#endif
}

// 0x4D7A10
bool CAEAudioChannel::Lost() const {
#ifdef USE_DSOUND
    while (m_pDirectSoundBuffer->Restore() == DSERR_BUFFERLOST) { // BUG: Infinite loop if we don't restore
        OS_ThreadSleep(10);
    }
#endif

    return true;
}

bool CAEAudioChannel::SetReverbAndDepth(uint32 reverb, uint32 depth) {
    return plugin::CallMethodAndReturn<bool, 0x4D7AA0>(this, reverb, depth);
}
