#include "StdInc.h"

#include "AEStaticChannel.h"

#include "AEAudioUtility.h"

#include "AESmoothFadeThread.h"

void CAEStaticChannel::InjectHooks() {
    RH_ScopedVirtualClass(CAEStaticChannel, 0x85F3CC, 9);
    RH_ScopedCategory("Audio/Hardware");

    RH_ScopedVMTInstall(Service, 0x4F10D0);
    RH_ScopedVMTInstall(IsSoundPlaying, 0x4F0F40);
    RH_ScopedVMTInstall(GetPlayTime, 0x4F0F70);
    RH_ScopedVMTInstall(GetLength, 0x4F0FA0);
    RH_ScopedVMTInstall(Play, 0x4F0BD0);
    RH_ScopedVMTInstall(SynchPlayback, 0x4F1040);
    RH_ScopedVMTInstall(Stop, 0x4F0FB0);

    RH_ScopedInstall(SetAudioBuffer, 0x4F0C40, {.reversed = false}); // check
}

CAEStaticChannel::CAEStaticChannel(IDirectSound* pDirectSound, uint16 channelId, bool hardwareMixAvailable, uint32 samplesPerSec, uint16 bitsPerSample) :
    CAEAudioChannel(pDirectSound, channelId, samplesPerSec, bitsPerSample),
    m_bNeedData(false),
    m_bNeedsSynch(false),
    m_IsHardwareMixAvailable(hardwareMixAvailable)
{
}

// 0x4F10D0
void CAEStaticChannel::Service() {
    if (!m_pDirectSoundBuffer) {
        m_nBufferStatus = 0;
        return;
    }

    if (m_bNeedData && (int32)(CTimer::GetTimeInMS() - m_nSyncTime) > m_CurrentBufferOffsetMs) {
        uint8* ppvAudioPtr1{};
        DWORD pdwAudioBytes{};

        VERIFY(SUCCEEDED(m_pDirectSoundBuffer->Lock(
            m_dwLockOffset,
            m_nNumLockBytes,
            reinterpret_cast<LPVOID*>(&ppvAudioPtr1),
            &pdwAudioBytes,
            nullptr,
            0,
            0
        )));

        for (auto i = 0u; i < m_nNumLoops; i++) {
            memcpy(
                &ppvAudioPtr1[i * m_nNumLockBytes],
                (uint8*)m_pBuffer + m_nCurrentBufferOffset,
                m_nNumLockBytes
            );
        }
        VERIFY(SUCCEEDED(m_pDirectSoundBuffer->Unlock(ppvAudioPtr1, pdwAudioBytes, nullptr, 0)));
        m_bNeedData = false;
    }

    UpdateStatus();

    if (!m_bPaused && !bufferStatus.Bit0x1) {
        if (const auto buf = std::exchange(m_pDirectSoundBuffer, nullptr)) {
            --g_numSoundChannelsUsed;
            buf->Release();
        }
    }
}

// 0x4F0F40
bool CAEStaticChannel::IsSoundPlaying() {
    if (!m_pDirectSoundBuffer)
        return false;

    if (m_bPaused || m_bNeedsSynch)
        return true;

    return CAEAudioChannel::IsBufferPlaying();
}

// 0x4F0F70
int16 CAEStaticChannel::GetPlayTime() {
    if (!IsSoundPlaying())
        return -1;

    const auto curPos = CAEAudioChannel::GetCurrentPlaybackPosition();
    return CAEAudioChannel::ConvertFromBytesToMS(curPos);
}

// 0x4F0FA0
uint16 CAEStaticChannel::GetLength() {
    return CAEAudioChannel::ConvertFromBytesToMS(m_nLengthInBytes);
}

// 0x4F0BD0
void CAEStaticChannel::Play(int16 timeInMs, int8 unused, float scalingFactor) {
    if (m_bLooped && m_nCurrentBufferOffset != 0 || !timeInMs) {
        m_bUnkn2 = false;
    } else {
        m_pDirectSoundBuffer->SetCurrentPosition(ConvertFromMsToBytes(timeInMs));
        m_bUnkn2 = true;
    }
    m_bNeedsSynch = true;
    m_bPaused = scalingFactor == 0.0f;
}

// 0x4F1040
void CAEStaticChannel::SynchPlayback() {
    if (!m_pDirectSoundBuffer || !m_bNeedsSynch || m_bPaused)
        return;

    if (m_bUnkn2) {
        m_pDirectSoundBuffer->SetVolume(-10000);
        if (!AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, m_Volume, -2, false)) {
            const auto dwVolume = static_cast<LONG>(m_Volume * 100.0F);
            m_pDirectSoundBuffer->SetVolume(dwVolume);
        }
    }

    m_pDirectSoundBuffer->Play(0, 0, m_bLooped);
    m_nSyncTime = CTimer::GetTimeInMS();
    m_bNeedsSynch = false;
}

// 0x4F0FB0
void CAEStaticChannel::Stop() {
    if (m_pDirectSoundBuffer &&
        CAEAudioChannel::IsBufferPlaying() &&
        !AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, VOLUME_SILENCE, -1, true)
    ) {
        m_pDirectSoundBuffer->Stop();
    }

    { // todo: Same as CAEAudioChannel::~CAEAudioChannel
        if (m_pDirectSoundBuffer) {
            --g_numSoundChannelsUsed;
            SAFE_RELEASE(m_pDirectSoundBuffer);
        }
        SAFE_RELEASE(m_pDirectSound3DBuffer);
    }
}

// 0x4F0C40 -- broken, loops aren't correct. test with molotovs
// Mobile(OpenAL, 0x485360): bool(OALBuffer* buffer, uint16 soundID, int16 bankSlotID, int16 loopStartOffset, uint16 sampleRate);
bool CAEStaticChannel::SetAudioBuffer(void* buffer, uint16 size, int16 f88, int16 f8c, int16 loopOffset, uint16 frequency) {
    if (!size || !frequency) {
        return false;
    }

    if (m_pDirectSoundBuffer) {
        --g_numSoundChannelsUsed;
        SAFE_RELEASE(m_pDirectSoundBuffer);
    }
    SAFE_RELEASE(m_pDirectSound3DBuffer);

    m_pBuffer              = buffer;
    m_nCurrentBufferOffset = 0;
    field_68               = 0;
    m_bLooped              = false;
    m_bNeedData            = false;
    m_bPaused              = false;
    field_6C               = 0;
    m_nLengthInBytes       = size;
    field_88               = f88;
    field_8C               = f8c;

    if (loopOffset != -1) {
        m_bLooped              = true;
        m_nCurrentBufferOffset = (16 * loopOffset) >> 3;
        field_68               = size;
    }

    DSBUFFERDESC dsBufferDesc{ .dwSize = sizeof(DSBUFFERDESC) }; // guid already set to zero
    if (m_bLooped && m_nCurrentBufferOffset) {
        m_nNumLockBytes            = field_68 - m_nCurrentBufferOffset;
        field_6C                   = std::max(field_68, 24'000u) / m_nNumLockBytes + 1;
        m_TotalBufferSize          = m_nNumLockBytes * field_6C;
        dsBufferDesc.dwBufferBytes = m_TotalBufferSize;
    } else {
        dsBufferDesc.dwBufferBytes = m_TotalBufferSize = size;
    }
    dsBufferDesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS
        | (m_IsHardwareMixAvailable ? DSBCAPS_LOCHARDWARE : DSBCAPS_LOCSOFTWARE);
    dsBufferDesc.dwReserved  = 0;
    dsBufferDesc.lpwfxFormat = &m_WaveFormat;

    m_nOriginalFrequency         = frequency;
    m_nFrequency                 = frequency;

    m_WaveFormat.nAvgBytesPerSec = 2 * frequency;
    m_WaveFormat.nSamplesPerSec  = frequency;
    m_WaveFormat.cbSize          = 0;
    m_WaveFormat.nChannels       = 1;
    m_WaveFormat.wBitsPerSample  = 16;
    m_WaveFormat.nBlockAlign     = 2;
    m_WaveFormat.wFormatTag      = WAVE_FORMAT_PCM;

    if (const auto hr = m_pDirectSound->CreateSoundBuffer(&dsBufferDesc, &m_pDirectSoundBuffer, nullptr); FAILED(hr)) {
        NOTSA_LOG_ERR("Couldn't create DSOUND buffer ec={:08X}", (uint32)hr);
        return false;
    }
    ++g_numSoundChannelsUsed;

    void* audioPtr{};
    DWORD audioBytes{};
    if (FAILED(m_pDirectSoundBuffer->Lock(0, m_nLengthInBytes, &audioPtr, &audioBytes, nullptr, nullptr, DSBLOCK_ENTIREBUFFER))) {
        SAFE_RELEASE(m_pDirectSoundBuffer);
        return false;
    }

    DWORD newPosition{};
    if (m_nCurrentBufferOffset) {
        std::memcpy((uint8*)audioPtr + m_TotalBufferSize - m_nCurrentBufferOffset, m_pBuffer, m_nCurrentBufferOffset);
        m_nNumLoops = m_nCurrentBufferOffset / m_nNumLockBytes + 1;
        m_dwLockOffset = m_TotalBufferSize - m_nNumLockBytes * m_nNumLoops;
        newPosition    = m_TotalBufferSize - m_nCurrentBufferOffset;

        if (field_6C != m_nNumLoops) {
            for (auto i = 0; i < field_6C - m_nNumLoops; i++) {
                std::memcpy(
                    (uint8*)audioPtr + m_nNumLockBytes * i,
                    (uint8*)m_pBuffer + m_nCurrentBufferOffset,
                    m_nNumLockBytes
                );
            }
        }
        m_CurrentBufferOffsetMs = ConvertFromBytesToMS(m_nCurrentBufferOffset);
        m_bNeedData             = true;
    } else {
        std::memcpy(audioPtr, m_pBuffer, size);
        if (size < m_nLengthInBytes) {
            std::memset((uint8*)audioPtr + size, 0, m_nLengthInBytes - size);
        }
        m_bNeedData = false;
    }

    m_pDirectSoundBuffer->Unlock(audioPtr, audioBytes, nullptr, 0);
    m_pDirectSoundBuffer->SetCurrentPosition(newPosition);
    m_pDirectSoundBuffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&m_pDirectSound3DBuffer);
    m_Volume = VOLUME_SILENCE;
    m_pDirectSoundBuffer->SetVolume(DSBVOLUME_MIN);
    return true;
}
