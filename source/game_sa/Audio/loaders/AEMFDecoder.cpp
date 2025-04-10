/* MikuAuahDark: This was meant to be CAEQTDecoder
 * but since it's very hard to find documentation on QuickTime
 * and Apple itself no longer support QuickTime, MediaFoundation
 * is used instead!
 */

#include "StdInc.h" // TODO: Remove

#include "AEMFDecoder.h"

#include <propvarutil.h>    // todo: cross-platform

#include <mfidl.h>
#include <mfapi.h>

#include "oswrapper.h"

// These GUIDs only available when we link with MediaFoundation
// libraries. We don't really want to link with it tho.
namespace MFID {
// 73647561-0000-0010-8000-00aa00389b71
const GUID MediaType_Audio = {0x73647561, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};
// 00000001-0000-0010-8000-00aa00389b71
const GUID AudioFormat_PCM = {0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
// 48eba18e-f8c9-4687-bf11-0a74c9f96a8f
const GUID MT_MAJOR_TYPE = {0x48eba18e, 0xf8c9, 0x4687, {0xbf, 0x11, 0x0a, 0x74, 0xc9, 0xf9, 0x6a, 0x8f}};
// f7e34c9a-42e8-4714-b74b-cb29d72c35e5
const GUID MT_SUBTYPE = {0xf7e34c9a, 0x42e8, 0x4714, {0xb7, 0x4b, 0xcb, 0x29, 0xd7, 0x2c, 0x35, 0xe5}};
// 37e48bf5-645e-4c5b-89de-ada9e29b696a
const GUID MT_AUDIO_NUM_CHANNELS = {0x37e48bf5, 0x645e, 0x4c5b, {0x89, 0xde, 0xad, 0xa9, 0xe2, 0x9b, 0x69, 0x6a}};
// 5faeeae7-0290-4c31-9e8a-c534f68d9dba
const GUID MT_AUDIO_SAMPLES_PER_SECOND = {0x5faeeae7, 0x0290, 0x4c31, {0x9e, 0x8a, 0xc5, 0x34, 0xf6, 0x8d, 0x9d, 0xba}};
// f2deb57f-40fa-4764-aa33-ed4f2d1ff669
const GUID MT_AUDIO_BITS_PER_SAMPLE = {0xf2deb57f, 0x40fa, 0x4764, {0xaa, 0x33, 0xed, 0x4f, 0x2d, 0x1f, 0xf6, 0x69}};
// fc358289-3cb6-460c-a424-b6681260375a
const GUID BYTESTREAM_CONTENT_TYPE = {0xfc358289, 0x3cb6, 0x460c, {0xa4, 0x24, 0xb6, 0x68, 0x12, 0x60, 0x37, 0x5a}};
// 2cd2d921-c447-44a7-a13c-4adabfc247e3
const IID MFAttributes = {0x2cd2d921, 0xc447, 0x44a7, {0xa1, 0x3c, 0x4a, 0xda, 0xbf, 0xc2, 0x47, 0xe3}};
} // namespace MFID

// QuickTime is replaced with MediaFoundation
bool& CAEMFDecoder::quickTimeInitialized = *(bool*)0xb61d70;

// MediaFoundation pointers
HMODULE CAEMFDecoder::mfPlatModule = nullptr;
HMODULE CAEMFDecoder::mfReadWriteModule = nullptr;
HRESULT(__stdcall* CAEMFDecoder::MFCreateSourceReaderFromURL)(LPCWSTR, IMFAttributes*, IMFSourceReader**) = nullptr;
HRESULT(__stdcall* CAEMFDecoder::MFCreateMediaType)(IMFMediaType**) = nullptr;

// 0x4e7770
CAEMFDecoder::CAEMFDecoder(char* filename, int32 trackID)
    : CAEStreamingDecoder(nullptr), m_pFilename(filename), m_bInitialized(false), m_pSourceReader(nullptr), m_pPcmAudio(nullptr), m_wpTempBuffer(nullptr), m_nTempBufferUsed(0), m_nPlayTime(0),
      m_nLength(0), m_nTrackID(trackID), m_nLengthMs(0), m_nSampleRate(0), m_fTimeDivider(0.0f) {}

// 0x4e77c0
CAEMFDecoder::~CAEMFDecoder() {
    if (m_pPcmAudio) {
        m_pPcmAudio->Release();
        m_pPcmAudio = nullptr;
    }

    if (m_pSourceReader) {
        m_pSourceReader->Release();
        m_pSourceReader = nullptr;
    }

    if (m_wpTempBuffer) {
        delete[] m_wpTempBuffer;
        m_wpTempBuffer = nullptr;
    }

    if (m_pFilename) {
        delete[] m_pFilename;
        m_pFilename = nullptr;
    }
}

// 0x4e7cb0
bool CAEMFDecoder::Initialise() {
    if (!CAEMFDecoder::quickTimeInitialized)
        return false;

    // Create source reader
    if (FAILED(CAEMFDecoder::MFCreateSourceReaderFromURL(UTF8ToUnicode(m_pFilename).c_str(), nullptr, &m_pSourceReader)))
        return false;

    // Select first audio stream
    if (FAILED(m_pSourceReader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE)) || FAILED(m_pSourceReader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE))) {
        m_pSourceReader->Release();
        m_pSourceReader = nullptr;
        return false;
    }

    // Create partial type
    IMFMediaType* partialType = nullptr;
    if (FAILED(CAEMFDecoder::MFCreateMediaType(&partialType))) {
        m_pSourceReader->Release();
        m_pSourceReader = nullptr;
        return false;
    }
    // Setup partial type to match what GTASA expect
    if (FAILED(partialType->SetGUID(MFID::MT_MAJOR_TYPE, MFID::MediaType_Audio)) || FAILED(partialType->SetGUID(MFID::MT_SUBTYPE, MFID::AudioFormat_PCM)) ||
        FAILED(partialType->SetUINT32(MFID::MT_AUDIO_NUM_CHANNELS, 2)) || FAILED(partialType->SetUINT32(MFID::MT_AUDIO_BITS_PER_SAMPLE, 16))) {
        partialType->Release();
        m_pSourceReader->Release();
        m_pSourceReader = nullptr;
        return false;
    }

    // Set source reader output to PCM
    if (FAILED(m_pSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, partialType))) {
        partialType->Release();
        m_pSourceReader->Release();
        m_pSourceReader = nullptr;
        return false;
    }
    partialType->Release();

    // Get PCM MediaType
    if (FAILED(m_pSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &m_pPcmAudio))) {
        m_pSourceReader->Release();
        m_pSourceReader = nullptr;
        return false;
    }
    // Just to make sure
    if (FAILED(m_pSourceReader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE))) {
        m_pPcmAudio->Release();
        m_pPcmAudio = nullptr;
        m_pSourceReader->Release();
        m_pSourceReader = nullptr;
        return false;
    }

    UINT32 temp = 0;

    // Get sample rate
    if (FAILED(m_pPcmAudio->GetUINT32(MFID::MT_AUDIO_SAMPLES_PER_SECOND, &temp))) {
        m_pPcmAudio->Release();
        m_pPcmAudio = nullptr;
        m_pSourceReader->Release();
        m_pSourceReader = nullptr;
        return false;
    }
    m_nSampleRate = temp;

    // Get duration
    PROPVARIANT prop;
    if (FAILED(m_pSourceReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &prop))) {
        m_pPcmAudio->Release();
        m_pPcmAudio = nullptr;
        m_pSourceReader->Release();
        m_pSourceReader = nullptr;
        return false;
    }
    m_nLengthMs = (long)(prop.uhVal.QuadPart / 10000);
    if (m_nLengthMs < 7000) {
        m_pPcmAudio->Release();
        m_pPcmAudio = nullptr;
        m_pSourceReader->Release();
        m_pSourceReader = nullptr;
        return false;
    }

    m_wpTempBuffer = new int16[TEMPBUFFRAME * 2];
    m_bInitialized = true;
    return true;
}

// 0x4e7860
size_t CAEMFDecoder::FillBuffer(void* dest, size_t size) {
    constexpr size_t PCM_DIVIDER = sizeof(int16) * 2;

    bool   playTimeSet = false;
    char*  buf = (char*)dest;
    size_t pcmFrames = size / PCM_DIVIDER;
    size_t writtenFrame = 0;

    // If there's some buffers in temporary buffer, use it
    if (m_nTempBufferUsed > 0) {
        size_t framesToCopy = m_nTempBufferUsed > pcmFrames ? pcmFrames : m_nTempBufferUsed;
        size_t bytesToCopy = framesToCopy * PCM_DIVIDER;
        memcpy(buf, m_wpTempBuffer, bytesToCopy);

        m_nTempBufferUsed -= framesToCopy;
        pcmFrames -= framesToCopy;
        size -= bytesToCopy;
        buf += bytesToCopy;
        writtenFrame += framesToCopy;

        if (m_nTempBufferUsed > 0)
            // Push buffer
            memmove(m_wpTempBuffer, m_wpTempBuffer + framesToCopy * 2, m_nTempBufferUsed * PCM_DIVIDER);
    }

    while (pcmFrames > 0) {
        DWORD           flags = 0, maxBufLen = 0;
        IMFSample*      sample = nullptr;
        IMFMediaBuffer* mediaBuffer = nullptr;
        BYTE*           rawBuffer = nullptr;
        LONGLONG        timestamp;

        if (FAILED(m_pSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &flags, &timestamp, &sample)))
            break;
        if (flags & (MF_SOURCE_READERF_ENDOFSTREAM | MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED))
            break;
        if (sample == nullptr)
            break;
        if (FAILED(sample->ConvertToContiguousBuffer(&mediaBuffer))) {
            sample->Release();
            break;
        }
        if (FAILED(mediaBuffer->Lock(&rawBuffer, nullptr, &maxBufLen))) {
            mediaBuffer->Release();
            sample->Release();
            break;
        }

        // Copy buffer
        size_t pcmFrameCount = maxBufLen / PCM_DIVIDER;
        size_t framesToCopy = pcmFrames > pcmFrameCount ? pcmFrameCount : pcmFrames;
        memcpy(buf, rawBuffer, framesToCopy * PCM_DIVIDER);
        buf += framesToCopy * PCM_DIVIDER;
        writtenFrame += framesToCopy;
        pcmFrames -= framesToCopy;

        // If there's buffer left, store it to temporary buffer
        if (pcmFrames == 0 && pcmFrameCount > framesToCopy) {
            // Store to temporary buffer
            size_t remain = pcmFrameCount - framesToCopy;
            size_t framesToCopyToTemp = remain > TEMPBUFFRAME ? TEMPBUFFRAME : remain;
            memcpy(m_wpTempBuffer, rawBuffer + framesToCopy * PCM_DIVIDER, remain * PCM_DIVIDER);
            m_nTempBufferUsed += framesToCopyToTemp;
        }

        mediaBuffer->Unlock();
        mediaBuffer->Release();
        sample->Release();

        if (playTimeSet == false) {
            m_nPlayTime = (long)(timestamp / 10000);
            playTimeSet = true;
        }
    }

    return writtenFrame * PCM_DIVIDER;
}

// 0x4e7920
long CAEMFDecoder::GetStreamLengthMs() {
    if (m_bInitialized)
        return m_nLengthMs;

    return -1;
}

// 0x4e7940
long CAEMFDecoder::GetStreamPlayTimeMs() {
    if (m_bInitialized)
        return m_nPlayTime;

    return -1;
}

// 0x4e78e0
void CAEMFDecoder::SetCursor(unsigned long pos) {
    if (m_bInitialized) {
        PROPVARIANT seekDest;
        InitPropVariantFromInt64(pos * 10000ULL, &seekDest);
        m_pSourceReader->Flush(MF_SOURCE_READER_FIRST_AUDIO_STREAM);
        m_pSourceReader->SetCurrentPosition(GUID_NULL, seekDest);
    }
}

// 0x4e78c0
int32 CAEMFDecoder::GetSampleRate() {
    if (m_bInitialized)
        return m_nSampleRate;

    return -1;
}

// 0x4e77b0
int32 CAEMFDecoder::GetStreamID() {
    return m_nTrackID;
}

// 0x4e7c70
bool CAEMFDecoder::InitLibrary() {
    /*
    // MikuAuahDark: This is the original function
    // but I replace it with MediaFoundation
    if (quickTimeInitialized == false)
    {
        if (CQuickTime::Initialise_sub_8195c0(0) != 0)
            return false;
        if (CQuickTime::Initialise_sub_819820() != 0)
            return false;
    }

    quickTimeInitialized = true;
    return true;
    */

    if (quickTimeInitialized)
        return true;

    // If user is running < Windows 7, don't bother.
    // Despite all APIs used are supported in Vista,
    // AAC decoding is only supported in Windows 7 or later
    if (IsWin7OrGreater()) {
        // Load MediaFoundation libraries
        mfPlatModule = LoadLibraryA("mfplat.dll");
        if (mfPlatModule == nullptr)
            return false;

        mfReadWriteModule = LoadLibraryA("mfreadwrite.dll");
        if (mfReadWriteModule == nullptr) {
            FreeLibrary(mfPlatModule);
            mfPlatModule = nullptr;
            return false;
        }

        // Call MFStartup()
        using MFStartupFunc = HRESULT(__stdcall*)(ULONG, DWORD);
        MFStartupFunc MFStartup = (MFStartupFunc)GetProcAddress(mfPlatModule, "MFStartup");

        if (FAILED(MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET))) {
            FreeLibrary(mfPlatModule);
            mfPlatModule = nullptr;
            FreeLibrary(mfReadWriteModule);
            mfReadWriteModule = nullptr;
            return false;
        }

        // Load address
        using MFCreateSourceReaderFromURLFunc = HRESULT(__stdcall*)(LPCWSTR, IMFAttributes*, IMFSourceReader**);
        CAEMFDecoder::MFCreateSourceReaderFromURL = (MFCreateSourceReaderFromURLFunc)GetProcAddress(mfReadWriteModule, "MFCreateSourceReaderFromURL");
        using MFCreateMediaTypeFunc = HRESULT(__stdcall*)(IMFMediaType * *ppMFType);
        CAEMFDecoder::MFCreateMediaType = (MFCreateMediaTypeFunc)GetProcAddress(mfPlatModule, "MFCreateMediaType");

        // More like MediaFoundationInitialized
        quickTimeInitialized = true;
        return true;
    }

    return false;
}

// TODO: 0x
void CAEMFDecoder::Shutdown() {
    // Free Media Foundation
    if (quickTimeInitialized) {
        using MFShutdownFunc = HRESULT(__stdcall*)();
        MFShutdownFunc MFShutdown = (MFShutdownFunc)GetProcAddress(mfPlatModule, "MFShutdown");

        MFShutdown();
        FreeLibrary(mfPlatModule);
        FreeLibrary(mfReadWriteModule);

        mfPlatModule = nullptr;
        mfReadWriteModule = nullptr;
        CAEMFDecoder::MFCreateSourceReaderFromURL = nullptr;
        CAEMFDecoder::MFCreateMediaType = nullptr;
        quickTimeInitialized = false;
    }
}

void CAEMFDecoder::InjectHooks() {
    RH_ScopedClass(CAEMFDecoder);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x4e7770);
    RH_ScopedInstall(Destructor, 0x4e77c0);
    RH_ScopedInstall(InitLibrary, 0x4e7c70);
    RH_ScopedInstall(Initialise, 0x4e7cb0);
    RH_ScopedInstall(FillBuffer, 0x4e7860);
    RH_ScopedInstall(GetStreamLengthMs, 0x4e7920);
    RH_ScopedInstall(GetStreamPlayTimeMs, 0x4e7940);
    RH_ScopedInstall(SetCursor, 0x4e78e0);
    RH_ScopedInstall(GetSampleRate, 0x4e78c0);
    RH_ScopedInstall(GetStreamID, 0x4e77b0);
}

CAEMFDecoder* CAEMFDecoder::Constructor(char* filename, int32 trackID) {
    this->CAEMFDecoder::CAEMFDecoder(filename, trackID);
    return this;
}

CAEMFDecoder* CAEMFDecoder::Destructor() {
    this->CAEMFDecoder::~CAEMFDecoder();
    return this;
}
