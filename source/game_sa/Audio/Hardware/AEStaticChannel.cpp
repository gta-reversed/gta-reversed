#include "StdInc.h"

#include "AEStaticChannel.h"

#include "AEAudioUtility.h"

#include "AESmoothFadeThread.h"
#include "app_debug.h"

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

    RH_ScopedInstall(SetAudioBuffer, 0x4F0C40, {.reversed = true}); // check
}

CAEStaticChannel::CAEStaticChannel(IDirectSound* pDirectSound, uint16 channelId, bool hardwareMixAvailable, uint32 samplesPerSec, uint16 bitsPerSample) :
    CAEAudioChannel(pDirectSound, channelId, samplesPerSec, bitsPerSample),
    m_OverwriteIntroWhenWrapped(false),
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

    // `SetAudioBuffer` (refer for its impl.) fills looped audio with offsets like this:
    //
    // 0                                                                  end
    // [loop][loop] ... [loop] [intro (samples before the m_LoopStartOffset)]
    //                        ^~~ m_IntroOverwriteOffset
    //
    // Since the "intro" samples are at the end, DirectSound automatically wrapping the buffer to zero
    // means that "intro" samples are finished. We will overwrite the intro section with loop samples.
    //
    // m_IntroOverwriteOffset is calculated (refer to `SetAudioBuffer`) so it will overwrite the previous sample if
    // necessary, it won't divide a sample in half, so no pops and clicks and shit will happen.
    if (m_OverwriteIntroWhenWrapped && (int32)(CTimer::GetTimeInMS() - m_nSyncTime) > m_CurrentBufferOffsetMs) {
        uint8* ppvAudioPtr1{};
        DWORD pdwAudioBytes{};

        VERIFY(SUCCEEDED(m_pDirectSoundBuffer->Lock(
            m_IntroOverwriteOffset,
            m_LoopedBytes,
            reinterpret_cast<LPVOID*>(&ppvAudioPtr1),
            &pdwAudioBytes,
            nullptr,
            0,
            0
        )));

        // BUG: We lock for m_LoopedBytes but copy m_NumLoopsCoverIntro * m_LoopedBytes.
        for (auto i = 0u; i < m_NumLoopsCoverIntro; i++) {
            memcpy(
                &ppvAudioPtr1[i * m_LoopedBytes],
                (uint8*)m_pBuffer + m_LoopStartOffset,
                m_LoopedBytes
            );
        }
        VERIFY(SUCCEEDED(m_pDirectSoundBuffer->Unlock(ppvAudioPtr1, pdwAudioBytes, nullptr, 0)));
        m_OverwriteIntroWhenWrapped = false;
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
    if (m_bLooped && m_LoopStartOffset != 0 || !timeInMs) {
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
        m_pDirectSoundBuffer->SetVolume(DSBVOLUME_MIN);
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
bool CAEStaticChannel::SetAudioBuffer(void* buffer, uint16 size, int16 f88, int16 f8c, int16 loopOffsetInSamples, uint16 frequency) {
    if (!size || !frequency) {
        NOTSA_LOG_ERR("Invalid params for SetAudioBuffer: size={}, freq={}", size, frequency);
        assert(false);
        return false;
    }

    if (m_pDirectSoundBuffer) {
        --g_numSoundChannelsUsed;
        SAFE_RELEASE(m_pDirectSoundBuffer);
    }
    SAFE_RELEASE(m_pDirectSound3DBuffer);

    m_pBuffer                   = buffer;
    m_LoopStartOffset           = 0;
    m_LoopEndOffset             = 0;
    m_bLooped                   = false;
    m_OverwriteIntroWhenWrapped = false;
    m_bPaused                   = false;
    m_TotalLoops                = 0;
    m_nLengthInBytes            = size;
    field_88                    = f88;
    field_8C                    = f8c;

    m_WaveFormat.nAvgBytesPerSec = sizeof(int16) * frequency;
    m_WaveFormat.nSamplesPerSec  = frequency;
    m_WaveFormat.cbSize          = 0;
    m_WaveFormat.nChannels       = 1;
    m_WaveFormat.wBitsPerSample  = sizeof(int16) * 8;
    m_WaveFormat.nBlockAlign     = sizeof(int16);
    m_WaveFormat.wFormatTag      = WAVE_FORMAT_PCM;

    if (loopOffsetInSamples != -1) {
        m_bLooped         = true;
        m_LoopStartOffset = loopOffsetInSamples * sizeof(int16);
        m_LoopEndOffset   = size;
    }

    DSBUFFERDESC dsBufferDesc{ .dwSize = sizeof(DSBUFFERDESC) }; // guid already set to zero
    if (m_bLooped && m_LoopStartOffset) {
        m_LoopedBytes              = m_LoopEndOffset - m_LoopStartOffset;
        m_TotalLoops               = std::max(m_LoopEndOffset, 24'000u) / m_LoopedBytes + 1;
        m_TotalBufferSize          = m_LoopedBytes * m_TotalLoops;
        dsBufferDesc.dwBufferBytes = m_TotalBufferSize;
    } else {
        dsBufferDesc.dwBufferBytes = m_TotalBufferSize = size;
    }
    dsBufferDesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_TRUEPLAYPOSITION
        | (m_IsHardwareMixAvailable ? DSBCAPS_LOCHARDWARE : DSBCAPS_LOCSOFTWARE);
    dsBufferDesc.dwReserved  = 0;
    dsBufferDesc.lpwfxFormat = &m_WaveFormat;

    m_nOriginalFrequency     = frequency;
    m_nFrequency             = frequency;

    if (const auto r = m_pDirectSound->CreateSoundBuffer(&dsBufferDesc, &m_pDirectSoundBuffer, nullptr); FAILED(r)) {
        NOTSA_LOG_ERR("Couldn't create DSOUND buffer ec={:08X}", (uint32)r);
        return false;
    }
    ++g_numSoundChannelsUsed;

    void* audioPtr{};
    DWORD audioBytes{};
    if (const auto r = m_pDirectSoundBuffer->Lock(0, m_nLengthInBytes, &audioPtr, &audioBytes, nullptr, nullptr, DSBLOCK_ENTIREBUFFER); FAILED(r)) {
        SAFE_RELEASE(m_pDirectSoundBuffer);
        NOTSA_LOG_ERR("Locking the DSOUND buffer failed ec={:08X}", (uint32)r);
        return false;
    }

    DWORD newPosition{};
    if (m_LoopStartOffset) {
        // SA fills the looped audio with offset as this:
        //
        // 0                                                    m_TotalBufferSize
        // [loop][loop] ... [loop] [intro (samples before the m_LoopStartOffset)]
        //
        // And sets the current position into the beginning of intro. (m_TotalBufferSize - m_LoopStartOffset)
        // After the intro finishes, `Service` (refer for impl. info) overwrites its place with extra looped samples.
        //
        // The reason for this approach is DSound doesn't support looping a specific portion of a static buffer. Other
        // manual approaches (e.g. two seperate buffers and playing the loop after intro ends) are not viable because
        // we would had CPU deal with audio thread all the time.
        std::memcpy((uint8*)audioPtr + m_TotalBufferSize - m_LoopStartOffset, m_pBuffer, m_LoopStartOffset);
        m_NumLoopsCoverIntro = m_LoopStartOffset / m_LoopedBytes + 1;

        // This points to where `Service` should start to overwrite the looped samples.
        // Why not just use m_TotalBufferSize - m_LoopStartOffset?
        // Because a loop may not fill an intro evenly, this way we fit exact amount of loop samples.
        // We need to be safe so we neither;
        // 1.) End up playing the remnants of the intro samples, the reason of adding 1 above.
        // 2.) Overshooting the DSound buffer and crashing the game, the reason of this calculation.
        m_IntroOverwriteOffset = m_TotalBufferSize - m_LoopedBytes * m_NumLoopsCoverIntro;
        newPosition            = m_TotalBufferSize - m_LoopStartOffset;

        if (m_NumLoopsCoverIntro != m_TotalLoops) {
            for (auto i = 0; i < m_TotalLoops - m_NumLoopsCoverIntro; i++) {
                std::memcpy(
                    (uint8*)audioPtr + m_LoopedBytes * i,
                    (uint8*)m_pBuffer + m_LoopStartOffset,
                    m_LoopedBytes
                );
            }
        }
        m_CurrentBufferOffsetMs     = ConvertFromBytesToMS(m_LoopStartOffset);
        m_OverwriteIntroWhenWrapped = true;
    } else {
        std::memcpy(audioPtr, m_pBuffer, size);
        if (size < m_nLengthInBytes) {
            std::memset((uint8*)audioPtr + size, 0, m_nLengthInBytes - size);
        }
        m_OverwriteIntroWhenWrapped = false;
    }

    m_pDirectSoundBuffer->Unlock(audioPtr, audioBytes, nullptr, 0);
    m_pDirectSoundBuffer->SetCurrentPosition(newPosition);
    m_pDirectSoundBuffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&m_pDirectSound3DBuffer);
    m_Volume = VOLUME_SILENCE;
    m_pDirectSoundBuffer->SetVolume(DSBVOLUME_MIN);
    return true;
}
