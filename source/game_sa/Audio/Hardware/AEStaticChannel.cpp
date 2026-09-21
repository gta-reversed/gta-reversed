#include "StdInc.h"

#include "AEStaticChannel.h"

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

    RH_ScopedInstall(SetAudioBuffer, 0x4F0C40);
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

    if (m_bNeedData && (int32)(CTimer::GetTimeInMS() - m_nSyncTime) > field_74) {
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
        !AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, -100.0F, -1, true)
    ) {
        m_pDirectSoundBuffer->Stop();
    }

    { // todo: Same as CAEAudioChannel::~CAEAudioChannel
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
}

// 0x4F0C40
bool CAEStaticChannel::SetAudioBuffer(IDirectSound3DBuffer* buffer, uint16 size, int16 field88, int16 field8C, int16 loopOffset, uint16 frequency) {
    if (size == 0 || frequency == 0) {
        return false;
    }

    if (m_pDirectSoundBuffer) {
        --g_numSoundChannelsUsed;
    }
    if (m_pDirectSoundBuffer) {
        std::exchange(m_pDirectSoundBuffer, nullptr)->Release();
    }
    if (m_pDirectSound3DBuffer) {
        std::exchange(m_pDirectSound3DBuffer, nullptr)->Release();
    }

    // Original also mirrors the total buffer size into a spare word at +0x24 (inside `CAEAudioChannel::_pad10`);
    // nothing else reads it, but keep the write for faithfulness.
    auto& sizeMirror = *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this) + 0x24);

    m_pBuffer                = buffer;
    m_pDirectSound3DBuffer   = nullptr;
    field_8C                 = 0;
    m_nCurrentBufferOffset   = 0;
    m_bUnkn2                 = false;
    m_bNeedData              = false;
    m_bUnkn4                 = false;
    field_6C                 = 0;
    m_nLengthInBytes         = size;
    field_88                 = field88;
    field_8B                 = 0;
    field_8C                 = field8C;
    m_bPaused                = false;
    if (loopOffset != -1) {
        m_bLooped              = true;
        m_nCurrentBufferOffset = (loopOffset << 4) >> 3;
        field_68               = size;
    }
    uint32 bufferBytes{};
    if (!m_bLooped || m_nCurrentBufferOffset == 0) {
        bufferBytes = size;
        sizeMirror  = size;
    } else {
        const auto loopedPart = field_68 - m_nCurrentBufferOffset;
        m_nNumLockBytes = loopedPart;
        const uint32 total = field_68 < 24'000 ? 24'000 : field_68;
        const auto   loops = total / loopedPart + 1;
        field_6C    = loops;
        bufferBytes = loops * loopedPart;
        sizeMirror  = bufferBytes;
    }

    // Original builds 0x80B4/0x80B8 via arithmetic; the bits are GLOBALFOCUS | CTRLVOLUME | CTRLFREQUENCY | CTRL3D | LOC{HARDWARE,SOFTWARE}.
    static_assert((DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D | DSBCAPS_LOCHARDWARE) == 0x80B4);
    static_assert((DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D | DSBCAPS_LOCSOFTWARE) == 0x80B8);
    DSBUFFERDESC bufferDesc{};
    bufferDesc.dwSize          = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags         = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D |
                                 (m_IsHardwareMixAvailable ? DSBCAPS_LOCHARDWARE : DSBCAPS_LOCSOFTWARE);
    bufferDesc.dwBufferBytes   = bufferBytes;
    bufferDesc.dwReserved      = 0;
    bufferDesc.lpwfxFormat     = &m_WaveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;
    m_WaveFormat.wFormatTag      = WAVE_FORMAT_PCM;
    m_WaveFormat.nChannels       = 1;
    m_WaveFormat.nSamplesPerSec  = frequency;
    m_WaveFormat.nAvgBytesPerSec = frequency * 2u;
    m_WaveFormat.nBlockAlign     = 2;
    m_WaveFormat.wBitsPerSample  = 16;
    m_WaveFormat.cbSize          = 0;
    m_nFrequency                 = frequency;
    m_nOriginalFrequency         = frequency;
    if (FAILED(m_pDirectSound->CreateSoundBuffer(&bufferDesc, &m_pDirectSoundBuffer, nullptr))) {
        return false;
    }
    ++g_numSoundChannelsUsed;

    void* audioPtr1{};
    DWORD audioBytes1{};
    void* audioPtr2{};
    DWORD audioBytes2{};
    if (FAILED(m_pDirectSoundBuffer->Lock(0, m_nLengthInBytes, &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, 0))) {
        std::exchange(m_pDirectSoundBuffer, nullptr)->Release();
        return false;
    }
    if (m_nCurrentBufferOffset == 0) {
        memcpy(audioPtr1, m_pBuffer, size);
        if (audioBytes1 < m_nLengthInBytes) {
            memset((uint8*)audioPtr1 + audioBytes1, 0, m_nLengthInBytes - audioBytes1);
        }
        m_bNeedData = false;
    } else {
        memcpy((uint8*)audioPtr1 + sizeMirror - m_nCurrentBufferOffset, m_pBuffer, m_nCurrentBufferOffset);
        m_nNumLoops    = (uint16)(m_nCurrentBufferOffset / m_nNumLockBytes + 1);
        m_dwLockOffset = sizeMirror - (uint32)m_nNumLoops * m_nNumLockBytes;
        for (uint32 i = 0; i < (uint32)field_6C - m_nNumLoops; i++) {
            memcpy((uint8*)audioPtr1 + i * m_nNumLockBytes, (uint8*)m_pBuffer + m_nCurrentBufferOffset, m_nNumLockBytes);
        }
        m_nSyncTime = CAEAudioChannel::ConvertFromBytesToMS(m_nCurrentBufferOffset);
        m_bNeedData = true;
    }
    VERIFY(SUCCEEDED(m_pDirectSoundBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2)));
    VERIFY(SUCCEEDED(m_pDirectSoundBuffer->QueryInterface(IID_IDirectSound3DBuffer, (void**)&m_pDirectSound3DBuffer)));
    m_Volume = -100.0f;
    VERIFY(SUCCEEDED(m_pDirectSoundBuffer->SetVolume(-10'000)));
    return true;
}
