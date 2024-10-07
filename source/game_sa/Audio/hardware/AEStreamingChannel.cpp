#include "StdInc.h"

#include <extensions/utility.hpp>

#include "AEStreamingChannel.h"
#include "AEAudioUtility.h"

//! Stereo channel FX Param EQ presets [0x8CBA70]
static inline DSFXParamEq s_FXParamEqPresets[3][2]{
    {{ 0.0f, 0.0f,  0.0f }, { 0.f,   0.0f,  0.0f }}, // Preset 1
    {{ 80.f, 30.f,  4.0f }, { 180.f, 30.f,  1.5f }}, // Preset 2
    {{ 80.f, 36.f, -15.f }, { 80.f,  36.f, -15.f }}, // Preset 3
};

// 0x4F2200
CAEStreamingChannel::~CAEStreamingChannel() {
    DirectSoundBufferFadeToSilence();

    m_nState = StreamingChannelState::UNK_MINUS_5;
    field_60088 = 0;

    if (m_pDirectSoundBuffer8)
        m_pDirectSoundBuffer8->SetFX(0, nullptr, nullptr);

    if (m_pSilenceBuffer)
        std::exchange(m_pSilenceBuffer, nullptr)->Release();

    if (m_pDirectSoundBuffer)
        std::exchange(m_pDirectSoundBuffer, nullptr)->Release();

    if (m_pStreamingDecoder)
        delete std::exchange(m_pStreamingDecoder, nullptr);

    if (m_pNextStreamingDecoder)
        delete std::exchange(m_pNextStreamingDecoder, nullptr);
}

// 0x4F22F0
void CAEStreamingChannel::Initialise() {
    VERIFY(SUCCEEDED(CoInitialize(nullptr)));

    DSBUFFERDESC bufferDesc{};
    bufferDesc.guid3DAlgorithm = GUID_NULL;
    bufferDesc.lpwfxFormat = &m_WaveFormat;
    bufferDesc.dwSize = 36;
    bufferDesc.dwBufferBytes = 0xC0000;
    bufferDesc.dwReserved = 0;
    bufferDesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLFX | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCSOFTWARE;

    m_WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    m_WaveFormat.cbSize = 0;
    m_WaveFormat.nChannels = 2;
    m_WaveFormat.wBitsPerSample = 16;
    m_WaveFormat.nSamplesPerSec = 48000;
    m_WaveFormat.nAvgBytesPerSec = 192000;
    m_WaveFormat.nBlockAlign = 4;

    if (SUCCEEDED(m_pDirectSound->CreateSoundBuffer(
        &bufferDesc,
        &m_pDirectSoundBuffer,
        0
    ))) {
        m_bInitialized = true;
        SetOriginalFrequency(m_WaveFormat.nSamplesPerSec);
        m_pBuffer = m_aBuffer;
        m_bLooped = true;
    }
    InitialiseSilence();
}

// 0x4F1C70
void CAEStreamingChannel::InitialiseSilence() {
    DSBUFFERDESC bufferDesc{};
    bufferDesc.dwSize = 36;
    bufferDesc.lpwfxFormat = &m_WaveFormat;
    bufferDesc.dwBufferBytes = 0x8000;
    bufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCSOFTWARE;

    if (SUCCEEDED(m_pDirectSound->CreateSoundBuffer(&bufferDesc, &m_pSilenceBuffer, 0))) {
        void *audioPtr;
        DWORD audioPtrBytes;

        m_pSilenceBuffer->Lock(0, 0, &audioPtr, &audioPtrBytes, nullptr, 0, DSBLOCK_ENTIREBUFFER);
        memset(audioPtr, 0, audioPtrBytes);
        m_pSilenceBuffer->Unlock(audioPtr, audioPtrBytes, nullptr, 0);
        m_pSilenceBuffer->Play(0, 0, DSBPLAY_LOOPING);

    } else {
        NOTSA_LOG_WARN("Creating silence buffer failed.");
    }
}

// 0x4F1FF0
void CAEStreamingChannel::SetReady() {
    switch (m_nState) {
    case StreamingChannelState::UNK_MINUS_5:
        AESmoothFadeThread.CancelFade(m_pDirectSoundBuffer);
        m_pDirectSoundBuffer->Stop();
        m_nState = StreamingChannelState::UNK_MINUS_6;

        break;
    case StreamingChannelState::UNK_MINUS_6:
        if (m_pStreamingDecoder)
            m_nState = StreamingChannelState::UNK_MINUS_2;
        break;
    }
}

// 0x4F1F30
void CAEStreamingChannel::SetBassEQ(uint8 mode, float gain) {
    if (mode == 0) {
        if (m_bFxEnabled)
            RemoveFX();
        return;
    }

    if (!m_bFxEnabled && !AddFX()) {
        return;
    }

    for (auto i = 0; i < 2; i++) {
        IDirectSoundFXParamEq* fxEqParam;
        if (FAILED(m_pDirectSoundBuffer8->GetObjectInPath(
            GUID_All_Objects,
            i,
            IID_IDirectSoundFXParamEq,
            reinterpret_cast<LPVOID*>(&fxEqParam)
        ))) {
            continue;
        }

        DSFXParamEq savedFxExParam{ s_FXParamEqPresets[mode][i] };
        savedFxExParam.fGain *= gain;
        VERIFY(SUCCEEDED(fxEqParam->SetAllParameters(&savedFxExParam)));
        fxEqParam->Release();
    }
}

// 0x4F2060
void CAEStreamingChannel::SetFrequencyScalingFactor(float factor) {
    if (factor == 0.0f) {
        if (!m_pDirectSoundBuffer || m_nState == StreamingChannelState::UNK_MINUS_7 || !IsBufferPlaying())
            return;

        DirectSoundBufferFadeToSilence();
        m_nState = StreamingChannelState::UNK_MINUS_7;
    } else {
        SetFrequency(static_cast<uint32>((float)m_nOriginalFrequency * factor));

        if (m_nState != StreamingChannelState::UNK_MINUS_7)
            return;

        if (!m_pDirectSoundBuffer)
            return;

        m_pDirectSoundBuffer->SetVolume(-10'000);
        m_pDirectSoundBuffer->Play(0, 0, m_bLooped ? DSBPLAY_LOOPING : 0);

        if (!AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, m_fVolume, 35, true))
            m_pDirectSoundBuffer->SetVolume(static_cast<int32>(m_fVolume * 100.0f));

        m_nState = StreamingChannelState::UNK_MINUS_1;
    }
}

// 0x4F1870
void CAEStreamingChannel::SynchPlayback() {
    // NOP
}

// 0x4F1E20
uint32 CAEStreamingChannel::FillBuffer(void* buffer, uint32 size) {
    auto filled = m_pStreamingDecoder->FillBuffer(buffer, size);
    if (filled < size) {
        if (field_61) {
            while (filled < size) {
                m_pStreamingDecoder->SetCursor(1);
                filled += m_pStreamingDecoder->FillBuffer((uint8*)buffer + filled, size - filled);
            }
        } else {
            if (m_pNextStreamingDecoder) {
                if (m_pNextStreamingDecoder->GetSampleRate() == m_pStreamingDecoder->GetSampleRate()) {
                    if (m_pStreamingDecoder)
                        delete m_pStreamingDecoder;

                    m_pStreamingDecoder = m_pNextStreamingDecoder;
                    m_pNextStreamingDecoder = nullptr;

                    filled += m_pStreamingDecoder->FillBuffer((uint8*)buffer + filled, size - filled);
                } else {
                    memset((uint8*)buffer + filled, 0, size - filled);
                    filled = size;
                    m_bWrongSampleRate = true;
                }
            } else {
                m_nState = StreamingChannelState::UNK_MINUS_4;
            }
        }
    }

    m_nStreamPlayTimeMs = m_pStreamingDecoder->GetStreamLengthMs();
    return filled;
}

// 0x4F23D0, broken af
void CAEStreamingChannel::PrepareStream(CAEStreamingDecoder* newDecoder, int8 arg2, uint32 audioBytes) {
    if (!newDecoder || !m_pDirectSoundBuffer)
        return;

    if (audioBytes != 0) {
        switch (m_nState) {
        case StreamingChannelState::UNK_MINUS_3:
        case StreamingChannelState::UNK_MINUS_4:
        case StreamingChannelState::UNK_MINUS_1:
            DirectSoundBufferFadeToSilence();
            break;
        default:
            break;
        }

        m_nState = StreamingChannelState::UNK_MINUS_6;
    }

    if (m_pStreamingDecoder)
        delete m_pStreamingDecoder;

    field_61 = arg2;
    m_pStreamingDecoder = newDecoder;
    if (SUCCEEDED(m_pDirectSoundBuffer->Lock(
        0,
        0,
        (LPVOID*)(&newDecoder),
        (DWORD*)&audioBytes,
        nullptr,
        0,
        DSBLOCK_ENTIREBUFFER
    ))) {
        const auto written = FillBuffer(newDecoder, audioBytes);
        if (written == audioBytes) {
            field_64 = 0;
        } else {
            memset(reinterpret_cast<uint8*>(newDecoder) + written, 0, audioBytes - written);
            field_64 = 1;
        }

        // ?
        if (audioBytes & 0xFFFFFFFC) {
            for (auto i = 0u; i < audioBytes >> 2; i++) {
                // *((uint32*)&stream->_vftable + i) |= 0x10001u;
            }
        }

        m_pDirectSoundBuffer->Unlock(newDecoder, audioBytes, nullptr, 0);
    }

    switch (m_nState) {
    case StreamingChannelState::UNK_MINUS_3:
    case StreamingChannelState::UNK_MINUS_1:
    case StreamingChannelState::UNK_MINUS_7:
        m_nState = StreamingChannelState::UNK_MINUS_2;
        break;
    default:
        break;
    }

    m_nCurrentlyLoadedChunk = 0;
    SetOriginalFrequency(newDecoder->GetSampleRate());
    m_bWrongSampleRate = false;
    m_bPrepareNewStream = false;
    field_45 = 0;
    m_pDirectSoundBuffer->SetCurrentPosition(0);
}

// 0x4F1DE0
void CAEStreamingChannel::SetNextStream(CAEStreamingDecoder* decoder) {
    if (m_pNextStreamingDecoder) {
        delete m_pNextStreamingDecoder;
    }
    m_pNextStreamingDecoder = decoder;
}

// 0x4F1A60
int32 CAEStreamingChannel::GetPlayingTrackID() {
    switch (m_nState) {
    case StreamingChannelState::UNK_MINUS_3:
    case StreamingChannelState::UNK_MINUS_4:
    case StreamingChannelState::UNK_MINUS_1:
    case StreamingChannelState::UNK_MINUS_5:
        if (m_pStreamingDecoder)
            return m_pStreamingDecoder->GetStreamID();

        [[fallthrough]];
    default:
        return -1;
    }
}

// 0x4F1A40
int32 CAEStreamingChannel::GetActiveTrackID() {
    return m_pStreamingDecoder ? m_pStreamingDecoder->GetStreamID() : -1;
}

// 0x4F18A0
int32 CAEStreamingChannel::UpdatePlayTime() {
    return plugin::CallMethodAndReturn<int32, 0x4F18A0, CAEStreamingChannel*>(this);
}

// 0x4F1AE0
bool CAEStreamingChannel::AddFX() {
    if (m_bFxEnabled)
        return true;

    DSEFFECTDESC effectDesc[2]{};
    for (auto& desc : effectDesc) {
        desc.guidDSFXClass = GUID_DSFX_STANDARD_PARAMEQ;
        desc.dwFlags = 0;
        desc.dwReserved1 = desc.dwReserved2 = 0;
        desc.dwSize = 32;
    }

    bool wasStopped = false;
    if (bufferStatus.Bit0x1) {
        m_pDirectSoundBuffer->Stop();
        wasStopped = true;
    }

    if (FAILED(m_pDirectSoundBuffer8->SetFX(
        std::size(effectDesc),
        effectDesc,
        nullptr
    )))
        return false;

    for (auto i = 0; i < 2; i++) { // Set both channels
        IDirectSoundFXParamEq* fxParamEq{};
        m_pDirectSoundBuffer8->GetObjectInPath(
            GUID_All_Objects,
            i,
            IID_IDirectSoundFXParamEq,
            reinterpret_cast<LPVOID*>(&fxParamEq)
        );
        fxParamEq->SetAllParameters(&s_FXParamEqPresets[0][i]);
        fxParamEq->Release();
    }

    if (wasStopped)
        m_pDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);

    m_bFxEnabled = true;
    return true;
}

// 0x4F1C20
void CAEStreamingChannel::RemoveFX() {
    bool stopped{false};
    if (bufferStatus.Bit0x1) {
        m_pDirectSoundBuffer->Stop();
        stopped = true;
    }

    m_pDirectSoundBuffer8->SetFX(0, nullptr, nullptr);

    if (stopped)
        m_pDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);

    m_bFxEnabled = false;
}

// 0x4F2040
bool CAEStreamingChannel::IsSoundPlaying() {
    switch (m_nState) {
    case StreamingChannelState::UNK_MINUS_3:
    case StreamingChannelState::UNK_MINUS_4:
    case StreamingChannelState::UNK_MINUS_5:
    case StreamingChannelState::UNK_MINUS_1:
        return true;
    default:
        return false;
    }
}

// 0x4F19E0
int16 CAEStreamingChannel::GetPlayTime() {
    switch (m_nState) {
    case StreamingChannelState::UNK_MINUS_1:
    case StreamingChannelState::UNK_MINUS_3:
        if (CAEAudioUtility::GetCurrentTimeInMS() - m_nLastUpdateTime >= 1000u)
            return UpdatePlayTime();
        else
            return static_cast<int16>(m_nPlayTime - static_cast<uint32>(m_nLastUpdateTime) + CAEAudioUtility::GetCurrentTimeInMS());
        break;
    default:
        break;
    }

    return static_cast<int16>(m_nState); // possibly not intended.
}

// 0x4F1880
uint16 CAEStreamingChannel::GetLength() {
    return static_cast<uint16>(m_pStreamingDecoder ? m_pStreamingDecoder->GetStreamLengthMs() : -1);
}

// 0x4F1D40
void CAEStreamingChannel::Play(int16, int8 a3, float) {
    if (!m_pStreamingDecoder || !m_pDirectSoundBuffer)
        return;

    SetOriginalFrequency(m_pStreamingDecoder->GetSampleRate());

    if (m_nState != StreamingChannelState::UNK_MINUS_7)
        field_61 = a3;

    if (m_nState == StreamingChannelState::UNK_MINUS_5) {
        AESmoothFadeThread.CancelFade(m_pDirectSoundBuffer);
        m_pDirectSoundBuffer->Stop();

        m_nState = StreamingChannelState::UNK_MINUS_6;
    }

    m_nState = StreamingChannelState::UNK_MINUS_1;
    m_pDirectSoundBuffer->SetVolume(static_cast<int32>(m_fVolume * 100.0f));
    m_pDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

// 0x4F2170
void CAEStreamingChannel::Pause() {
    if (!m_pDirectSoundBuffer)
        return;

    DirectSoundBufferFadeToSilence();

    m_nState = StreamingChannelState::UNK_MINUS_7;
}

// 0x4F1A90
void CAEStreamingChannel::Stop() {
    DirectSoundBufferFadeToSilence();

    // SA: dead code
    if (false) {
        m_nState = StreamingChannelState::UNK_MINUS_5;
        field_60088 = 0;
    }
}

// 0x4F2550
void CAEStreamingChannel::Service() {
    plugin::CallMethod<0x4F2550, CAEStreamingChannel*>(this);
}

void CAEStreamingChannel::InjectHooks() {
    RH_ScopedVirtualClass(CAEStreamingChannel, 0x85F3F0, 9);
    RH_ScopedCategory("Audio/Hardware");

    RH_ScopedInstall(Constructor, 0x4F1800, { .reversed = false }); // makes game not load radio
    RH_ScopedInstall(Destructor, 0x4F2200);
    RH_ScopedInstall(SynchPlayback, 0x4F1870);
    RH_ScopedInstall(PrepareStream, 0x4F23D0, { .reversed = false });
    RH_ScopedInstall(Initialise, 0x4F22F0);
    RH_ScopedInstall(Pause, 0x4F2170);
    RH_ScopedInstall(SetReady, 0x4F1FF0);
    RH_ScopedInstall(SetBassEQ, 0x4F1F30);
    RH_ScopedInstall(FillBuffer, 0x4F1E20);
    RH_ScopedInstall(InitialiseSilence, 0x4F1C70);
    RH_ScopedInstall(SetNextStream, 0x4F1DE0);
    RH_ScopedInstall(AddFX, 0x4F1AE0);
    RH_ScopedInstall(Stop, 0x4F1A90);
    RH_ScopedInstall(GetPlayingTrackID, 0x4F1A60);
    RH_ScopedInstall(GetActiveTrackID, 0x4F1A40);
    RH_ScopedInstall(UpdatePlayTime, 0x4F18A0, { .reversed = false });
    RH_ScopedInstall(RemoveFX, 0x4F1C20);
    RH_ScopedVMTInstall(Service, 0x4F2550, { .reversed = false });
    RH_ScopedVMTInstall(IsSoundPlaying, 0x4F2040);
    RH_ScopedVMTInstall(GetPlayTime, 0x4F19E0);
    RH_ScopedVMTInstall(GetLength, 0x4F1880);
    RH_ScopedVMTInstall(Play, 0x4F1D40);
    // RH_ScopedVirtualOverloadedInstall(Stop, "", 0x4F21C0, int8(CAEStreamingChannel::*)()); <-- unused, maybe inlined?
    RH_ScopedVMTInstall(SetFrequencyScalingFactor, 0x4F2060);
}
