#include "StdInc.h"

#include "CAEAudioChannel.h"

#include "CAEAudioUtility.h"
#include "CAESmoothFadeThread.h"

uint32& g_numSoundChannelsUsed = *(uint32*)0xB5F898;

void CAEAudioChannel::InjectHooks() {
    ReversibleHooks::Install("CAEAudioChannel", "SetPosition", 0x4D7950, &CAEAudioChannel::SetPosition);
    ReversibleHooks::Install("CAEAudioChannel", "UpdateStatus", 0x4D7BD0, &CAEAudioChannel::UpdateStatus);
    ReversibleHooks::Install("CAEAudioChannel", "Lost", 0x4D7A10, &CAEAudioChannel::Lost);
    ReversibleHooks::Install("CAEAudioChannel", "ConvertFromBytesToMS", 0x4D79D0, &CAEAudioChannel::ConvertFromBytesToMS);
    ReversibleHooks::Install("CAEAudioChannel", "ConvertFromMsToBytes", 0x4D79F0, &CAEAudioChannel::ConvertFromMsToBytes);
    ReversibleHooks::Install("CAEAudioChannel", "SetFrequency", 0x4D7A50, &CAEAudioChannel::SetFrequency);
    ReversibleHooks::Install("CAEAudioChannel", "SetVolume", 0x4D7C60, &CAEAudioChannel::SetVolume);
    ReversibleHooks::Install("CAEAudioChannel", "SetOriginalFrequency", 0x4D7A70, &CAEAudioChannel::SetOriginalFrequency);
    ReversibleHooks::Install("CAEAudioChannel", "SetFrequencyScalingFactor", 0x4D7D00, &CAEAudioChannel::SetFrequencyScalingFactor_Reversed);
    ReversibleHooks::Install("CAEAudioChannel", "GetCurrentPlaybackPosition", 0x4D79A0, &CAEAudioChannel::GetCurrentPlaybackPosition);
}

CAEAudioChannel::CAEAudioChannel(IDirectSound* directSound, uint16 channelId, uint32 samplesPerSec, uint16 bitsPerSample) {
    m_wBitsPerSample      = bitsPerSample;
    m_pDirectSound        = directSound;
    m_nChannelId          = channelId;
    m_nBytesPerSec        = samplesPerSec * (bitsPerSample / 8);
    m_nFlags              = 0;
    m_nBufferStatus       = 0;
    m_nFrequency          = samplesPerSec;
    field_57              = 0;
    field_53              = 2;
    m_wFrequencyMult      = 1;
    m_nBufferFrequency    = samplesPerSec;
    m_nOriginalFrequency  = samplesPerSec;
    field_47              = 1;
    m_fVolume             = -100.0f;
    m_pDirectSoundBuffer  = nullptr;
    m_pDirectSound3DBuffer= nullptr;
    m_bNoScalingFactor    = false;
    m_bLooped             = false;
    field_45              = 0;
}

CAEAudioChannel::~CAEAudioChannel() {
    if (m_pDirectSoundBuffer) {
        --g_numSoundChannelsUsed;
        m_pDirectSoundBuffer->Release();
        m_pDirectSoundBuffer = nullptr;
    }

    if (m_pDirectSound3DBuffer) {
        m_pDirectSound3DBuffer->Release();
        m_pDirectSound3DBuffer = nullptr;
    }
}

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

    const auto newFreq = static_cast<uint32>(m_nOriginalFrequency * factor);
    SetFrequency(newFreq);

    if (m_bNoScalingFactor) {
        const auto curPos = GetCurrentPlaybackPosition();
        if (curPos != 0)
            m_pDirectSoundBuffer->SetVolume(-10000);

        if (m_pDirectSoundBuffer)
            m_pDirectSoundBuffer->Play(0, 0, m_bLooped);

        if (curPos != 0 && !AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, m_fVolume, -2, false)) {
            const auto volume = static_cast<LONG>(m_fVolume * 100.0F);
            m_pDirectSoundBuffer->SetVolume(volume);
        }

        m_bNoScalingFactor = false;
    }
}
void CAEAudioChannel::SetFrequencyScalingFactor_Reversed(float factor) {
    CAEAudioChannel::SetFrequencyScalingFactor(factor);
}

// 0x4D7950
void CAEAudioChannel::SetPosition(CVector* vecPos) {
    if (!m_pDirectSoundBuffer)
        return;

    if (!m_pDirectSound3DBuffer)
        return;

    m_pDirectSound3DBuffer->SetPosition(vecPos->x, vecPos->y, vecPos->z, DS3D_DEFERRED);
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
int32 CAEAudioChannel::GetCurrentPlaybackPosition() {
    if (!m_pDirectSoundBuffer)
        return 0;

    uint32 outPos;
    m_pDirectSoundBuffer->GetCurrentPosition(reinterpret_cast<LPDWORD>(&outPos), nullptr);
    return outPos;
}

// 0x4D79D0
uint32 CAEAudioChannel::ConvertFromBytesToMS(uint32 bytes) {
    return CAEAudioUtility::ConvertFromBytesToMS(bytes, m_nBufferFrequency, m_wFrequencyMult);
}

// 0x4D79F0
uint32 CAEAudioChannel::ConvertFromMsToBytes(uint32 ms) {
    return CAEAudioUtility::ConvertFromMSToBytes(ms, m_nBufferFrequency, m_wFrequencyMult);
}

// 0x4D7A50
void CAEAudioChannel::SetFrequency(uint32 freq) {
    if (m_nFrequency == freq)
        return;

    m_nFrequency = freq;
    if (m_pDirectSoundBuffer)
        m_pDirectSoundBuffer->SetFrequency(freq);
}

// 0x4D7A70
void CAEAudioChannel::SetOriginalFrequency(uint32 freq) {
    SetFrequency(freq);
    m_nOriginalFrequency = freq;
}

// 0x4D7BD0
void CAEAudioChannel::UpdateStatus() {
    m_pDirectSoundBuffer->GetStatus(reinterpret_cast<LPDWORD>(&m_nBufferStatus));
    if (m_nBufferStatus & DSBSTATUS_BUFFERLOST)
        Lost();
}

// 0x4D7A10
bool CAEAudioChannel::Lost() {
    while (m_pDirectSoundBuffer->Restore() == DSERR_BUFFERLOST) // BUG: Infinite loop if we don't restore
        Sleep(10);

    return true;
}
