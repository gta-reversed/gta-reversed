#include "StdInc.h" // TODO: Remove

#include "AEWMADecoder.h"

#include "MemoryMgr.h"

// WMA functions
HRESULT(__stdcall*& CAEWMADecoder::WMCreateSyncReader)(IUnknown*, DWORD, IWMSyncReader**) = *(HRESULT(__stdcall**)(IUnknown*, DWORD, IWMSyncReader**))0xb6bad8;

// 0x502720
CAEWMADecoder::CAEWMADecoder(CAEDataStream* dataStream) : CAEStreamingDecoder(dataStream) {
    m_bInitialized = false;
    m_nTempBufferUsed = 0;
}

// 0x502760
CAEWMADecoder::~CAEWMADecoder() {
    if (m_bInitialized) {
        syncReader->Close();
        syncReader->Release();
        m_bInitialized = false;
    }

    CoUninitialize();
}

// 0x502c60
bool CAEWMADecoder::Initialise() {
    WMT_STREAM_SELECTION activateStream = WMT_ON;

    VERIFY(SUCCEEDED(CoInitialize(nullptr)));

    if (!WMCreateSyncReader)
        return m_bInitialized;

    if (!SUCCEEDED(WMCreateSyncReader(nullptr, 0, &syncReader)))
        return m_bInitialized;

    if (SUCCEEDED(syncReader->OpenStream(m_dataStream))) {
        IWMProfile* profile;

        if (!SUCCEEDED(syncReader->QueryInterface(IID_IWMProfile, (void**)&profile)))
            return m_bInitialized;

        HRESULT selectResult = SelectStreamIndex(profile);

        if (profile) {
            profile->Release();
            profile = nullptr;
        }

        if (!SUCCEEDED(selectResult))
            return m_bInitialized;

        DWORD                outputNumber;
        IWMOutputMediaProps* mediaProps;

        syncReader->SetStreamsSelected(1, &m_wStreamNumber, &activateStream);
        syncReader->SetReadStreamSamples(m_wStreamNumber, FALSE);
        syncReader->SetRange(0ULL, 0LL);
        syncReader->GetOutputNumberForStream(m_wStreamNumber, &outputNumber);

        if (!SUCCEEDED(syncReader->GetOutputProps(outputNumber, &mediaProps)))
            return m_bInitialized;

        DWORD mediaTypeSize = 0;
        mediaProps->GetMediaType(nullptr, &mediaTypeSize);

        WM_MEDIA_TYPE* mediaTypeBase = (WM_MEDIA_TYPE*)CMemoryMgr::Malloc(mediaTypeSize);
        mediaProps->GetMediaType(mediaTypeBase, &mediaTypeSize);
        WAVEFORMATEX* mediaMetadata = (WAVEFORMATEX*)mediaTypeBase->pbFormat;

        if (mediaMetadata && mediaMetadata->nChannels == 2 && mediaMetadata->wBitsPerSample == 16) {
            IWMHeaderInfo3* headerInfo;
            m_nSampleRate = mediaMetadata->nSamplesPerSec;

            if (SUCCEEDED(syncReader->QueryInterface(IID_IWMHeaderInfo3, (void**)&headerInfo))) {
                WORD count;
                headerInfo->GetAttributeIndices(0, L"Duration", nullptr, nullptr, &count);

                WORD* indices = new WORD[count];
                headerInfo->GetAttributeIndices(0, L"Duration", nullptr, indices, &count);

                WORD              dummyNameLen;
                DWORD             dummyBufLen = 8;
                QWORD             duration;
                WMT_ATTR_DATATYPE dataType;
                headerInfo->GetAttributeByIndexEx(0, indices[0], nullptr, &dummyNameLen, &dataType, nullptr, (BYTE*)&duration, &dummyBufLen);

                m_nLengthMs = (long)(duration / 10000ULL);
                m_bInitialized = m_nLengthMs >= 7000;

                delete[] indices;
                headerInfo->Release();
            }
        }

        CMemoryMgr::Free(mediaTypeBase);
        mediaProps->Release();
    } else if (syncReader) {
        syncReader->Release();
        syncReader = nullptr;
    }

    return m_bInitialized;
}

// 0x5027d0
size_t CAEWMADecoder::FillBuffer(void* dest, size_t size) {
    char*  buf = (char*)dest;
    size_t written = 0;

    if (m_nTempBufferUsed > 0) {
        size_t sizeToCopy = m_nTempBufferUsed > size ? size : m_nTempBufferUsed;
        memcpy(buf, m_szBuffer, sizeToCopy);

        m_nTempBufferUsed -= sizeToCopy;
        size -= sizeToCopy;
        buf += sizeToCopy;
        written += sizeToCopy;

        if (m_nTempBufferUsed > 0)
            // Push buffer
            memmove(m_szBuffer, m_szBuffer + sizeToCopy, m_nTempBufferUsed);
    }

    while (size > 0) {
        INSSBuffer* nssBuffer = nullptr;
        DWORD       dummy1 = 0, dummy2 = 0;
        WORD        stream = 0;

        if (FAILED(syncReader->GetNextSample(m_wStreamNumber, &nssBuffer, &m_nSampleTime, &m_nSampleDuration, &dummy1, &dummy2, &stream)))
            return written;

        if (stream == m_wStreamNumber) {
            BYTE* byteBuffer;
            DWORD bufferLen = 0;

            if (FAILED(nssBuffer->GetBufferAndLength(&byteBuffer, &bufferLen))) {
                nssBuffer->Release();
                return written;
            }

            size_t sizeToCopy = size > bufferLen ? bufferLen : size;
            memcpy(buf, byteBuffer, sizeToCopy);
            bufferLen -= sizeToCopy;
            size -= sizeToCopy;
            written += sizeToCopy;
            buf += sizeToCopy;

            if (size == 0) {
                // Store into temporary buffer
                size_t freeTempBufSize = TEMPBUFSIZE - m_nTempBufferUsed;
                byteBuffer += sizeToCopy;
                sizeToCopy = bufferLen > freeTempBufSize ? freeTempBufSize : bufferLen;
                memcpy(m_szBuffer + m_nTempBufferUsed, byteBuffer, sizeToCopy);
                m_nTempBufferUsed += sizeToCopy;
            }
        }

        nssBuffer->Release();
    }

    return written;
}

// 0x502ad0
long CAEWMADecoder::GetStreamLengthMs() {
    if (m_bInitialized)
        return m_nLengthMs;

    return -1;
}

// 0x502af0
long CAEWMADecoder::GetStreamPlayTimeMs() {
    if (m_bInitialized)
        return (long)((m_nSampleTime + m_nSampleDuration) / 10000.0f);

    return -1;
}

// 0x502b50
void CAEWMADecoder::SetCursor(unsigned long pos) {
    if (m_bInitialized)
        syncReader->SetRange(pos * 10000ULL, 0LL);
}

// 0x502ab0
int32 CAEWMADecoder::GetSampleRate() {
    if (m_bInitialized)
        return m_nSampleRate;

    return -1;
}

// 0x502750
int32 CAEWMADecoder::GetStreamID() {
    return m_dataStream->m_nTrackId;
}

// 0x502b80
bool CAEWMADecoder::InitLibrary() {
    if (WMCreateSyncReader != nullptr)
        return true;

    wmvCoreModule = LoadLibraryA("wmvcore.dll");
    if (wmvCoreModule != nullptr) {
        using WMCreateSyncReaderFunc = HRESULT(__stdcall*)(IUnknown*, DWORD, IWMSyncReader**);
        WMCreateSyncReader = (WMCreateSyncReaderFunc)GetProcAddress(wmvCoreModule, "WMCreateSyncReader");

        if (WMCreateSyncReader) {
            IWMSyncReader* reader = nullptr;

            // MikuAuahDark: GTASA tests if WMCreateSyncReader actually works
            if (SUCCEEDED(WMCreateSyncReader(nullptr, 0, &reader))) {
                reader->Release();
                return true;
            }

            WMCreateSyncReader = nullptr;
        }

        FreeLibrary(wmvCoreModule);
        wmvCoreModule = nullptr;
    }

    return false;
}

void CAEWMADecoder::Shutdown() {
    // Free WMV core
    if (WMCreateSyncReader) {
        FreeLibrary(wmvCoreModule);
        WMCreateSyncReader = nullptr;
        wmvCoreModule = nullptr;
    }
}

// 0x502990
HRESULT CAEWMADecoder::SelectStreamIndex(IWMProfile* profile) {
    if (profile == nullptr)
        return E_INVALIDARG;

    DWORD streams = 0;
    if (SUCCEEDED(profile->GetStreamCount(&streams))) {
        IWMStreamConfig* streamConfig = nullptr;
        GUID             streamGUID;

        m_wStreamNumber = 0;

        for (DWORD i = 0; i < streams; i++) {
            WORD index;

            if (FAILED(profile->GetStream(i, &streamConfig)))
                break;

            if (FAILED(streamConfig->GetStreamNumber(&index)))
                break;

            if (FAILED(streamConfig->GetStreamType(&streamGUID)))
                break;

            streamConfig->Release();
            streamConfig = nullptr;

            if (streamGUID == WMMEDIATYPE_Audio) {
                m_wStreamNumber = index;
                break;
            }
        }

        if (streamConfig) {
            streamConfig->Release();
            streamConfig = nullptr;
        }
    }

    if (m_wStreamNumber == 0)
        return E_INVALIDARG;

    return S_OK;
}

void CAEWMADecoder::InjectHooks() {
    RH_ScopedClass(CAEWMADecoder);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x502720);
    RH_ScopedInstall(Destructor, 0x502760);
    RH_ScopedInstall(Initialise, 0x502c60);
    RH_ScopedInstall(InitLibrary, 0x502b80);
    RH_ScopedInstall(FillBuffer, 0x5027d0);
    RH_ScopedInstall(GetStreamLengthMs, 0x502ad0);
    RH_ScopedInstall(GetStreamPlayTimeMs, 0x502af0);
    RH_ScopedInstall(SetCursor, 0x502b50);
    RH_ScopedInstall(GetSampleRate, 0x502ab0);
    RH_ScopedInstall(GetStreamID, 0x502750);
    RH_ScopedInstall(SelectStreamIndex, 0x502990);
}

CAEWMADecoder* CAEWMADecoder::Constructor(CAEDataStream* dataStream) {
    this->CAEWMADecoder::CAEWMADecoder(dataStream);
    return this;
}

CAEWMADecoder* CAEWMADecoder::Destructor() {
    this->CAEWMADecoder::~CAEWMADecoder();
    return this;
}
