#include "StdInc.h" // TODO: Remove

#include "CAEVorbisDecoder.h"

ov_callbacks CAEVorbisDecoder::staticCallbacks = {
    &CAEVorbisDecoder::ReadCallback,
    &CAEVorbisDecoder::SeekCallback,
    &CAEVorbisDecoder::CloseCallback,
    &CAEVorbisDecoder::TellCallback
};

// 0x5026b0
CAEVorbisDecoder::CAEVorbisDecoder(CAEDataStream* dataStream, int32 userTrack) : CAEStreamingDecoder(dataStream) {
    m_sCallbacks        = staticCallbacks;
    m_pVorbisFileHandle = nullptr;
    m_bInitialized      = false;
    m_bIsUserTrack      = static_cast<bool>(userTrack);
    m_pDataStreamCopy   = dataStream;
    m_pVorbisInfo       = nullptr;
}

// 0x5023f0
CAEVorbisDecoder::~CAEVorbisDecoder() {
    if (m_pVorbisFileHandle) {
        ov_clear(m_pVorbisFileHandle);
        delete m_pVorbisFileHandle;
        m_pVorbisFileHandle = nullptr;
    }
}

// 0x5024d0
bool CAEVorbisDecoder::Initialise() {
    m_pVorbisFileHandle = new OggVorbis_File();
    if (ov_open_callbacks(m_dataStream, m_pVorbisFileHandle, nullptr, 0, m_sCallbacks) != 0)
        return false;

    m_pVorbisInfo = ov_info(m_pVorbisFileHandle, -1);

    // Not support channels > 2
    if (m_pVorbisInfo->channels > 2)
        return false;

    m_bInitialized = true;

    if (m_bIsUserTrack) {
        if (GetStreamLengthMs() < 7000)
            return m_bInitialized = false;
    }

    return true;
}

// 0x502470
size_t CAEVorbisDecoder::FillBuffer(void* dest, size_t size) {
    size_t readedTotal = 0;
    char*  buffer = reinterpret_cast<char*>(dest);

    // In case of mono stream, only read half of it
    if (m_pVorbisInfo->channels == 1)
        size /= 2;

    if (size > 0) {
        int32 dummy;

        while (readedTotal < size) {
            long readed = ov_read(m_pVorbisFileHandle, buffer + readedTotal, size - readedTotal, 0, sizeof(int16), 1, &dummy);
            if (readed <= 0)
                break;

            readedTotal += static_cast<size_t>(readed);
        }
    }

    if (m_pVorbisInfo->channels == 1) {
        // Duplicate channel data
        int16* bufShort = reinterpret_cast<int16*>(buffer);

        for (int32 i = static_cast<int32>(readedTotal / sizeof(int16)) - 1; i >= 0; i--)
            bufShort[i * 2] = bufShort[i * 2 + 1] = bufShort[i];

        readedTotal *= 2;
    }

    return readedTotal;
}

// 0x502610
long CAEVorbisDecoder::GetStreamLengthMs() {
    if (m_bInitialized) {
        double length = ov_time_total(m_pVorbisFileHandle, -1);
        return (long)(length * 1000.0f);
    }

    return -1;
}

// 0x502640
long CAEVorbisDecoder::GetStreamPlayTimeMs() {
    if (m_bInitialized) {
        double length = ov_time_tell(m_pVorbisFileHandle);
        return (long)(length * 1000.0f);
    }

    return -1;
}

// 0x502670
void CAEVorbisDecoder::SetCursor(unsigned long pos) {
    if (m_bInitialized)
        ov_time_seek(m_pVorbisFileHandle, pos * 0.001f);
}

// 0x502560
int32 CAEVorbisDecoder::GetSampleRate() {
    return m_bInitialized ? static_cast<int32>(m_pVorbisInfo->rate) : -1;
}

// 0x502460
int32 CAEVorbisDecoder::GetStreamID() {
    return m_dataStream->m_nTrackId;
}

// 0x502580
size_t CAEVorbisDecoder::ReadCallback(void* ptr, size_t size, size_t nmemb, void* opaque) {
    if (opaque == nullptr)
        return 0;

    CAEDataStream* dataStream = reinterpret_cast<CAEDataStream*>(opaque);
    return dataStream->FillBuffer(ptr, size * nmemb) / size;
}

// 0x5025d0
int32 CAEVorbisDecoder::CloseCallback(void* opaque) {
    if (opaque == nullptr)
        return -1;

    CAEDataStream* dataStream = reinterpret_cast<CAEDataStream*>(opaque);
    return dataStream->Close();
}

// 0x5025b0
int32 CAEVorbisDecoder::SeekCallback(void* opaque, ogg_int64_t offset, int32 whence) {
    if (opaque == nullptr)
        return -1;

    CAEDataStream* dataStream = reinterpret_cast<CAEDataStream*>(opaque);
    unsigned long  result = dataStream->Seek(static_cast<long>(offset), whence);

    return static_cast<int32>(result);
}

// 0x5025f0
long CAEVorbisDecoder::TellCallback(void* opaque) {
    if (opaque == nullptr)
        return -1;

    CAEDataStream* dataStream = reinterpret_cast<CAEDataStream*>(opaque);

    return static_cast<int32>(dataStream->GetCurrentPosition());
}

void CAEVorbisDecoder::InjectHooks() {
    ReversibleHooks::Install("CAEVorbisDecoder", "CAEVorbisDecoder", 0x5026b0, &CAEVorbisDecoder::Constructor);
    ReversibleHooks::Install("CAEVorbisDecoder", "~CAEVorbisDecoder", 0x5023f0, &CAEVorbisDecoder::Destructor);
    ReversibleHooks::Install("CAEVorbisDecoder", "Initialise", 0x5024d0, &CAEVorbisDecoder::Initialise);
    ReversibleHooks::Install("CAEVorbisDecoder", "GetStreamID", 0x502460, &CAEVorbisDecoder::GetStreamID);
    ReversibleHooks::Install("CAEVorbisDecoder", "FillBuffer", 0x502470, &CAEVorbisDecoder::FillBuffer);
    ReversibleHooks::Install("CAEVorbisDecoder", "GetSampleRate", 0x502560, &CAEVorbisDecoder::GetSampleRate);
    ReversibleHooks::Install("CAEVorbisDecoder", "ReadCallback", 0x502580, &CAEVorbisDecoder::ReadCallback);
    ReversibleHooks::Install("CAEVorbisDecoder", "CloseCallback", 0x5025d0, &CAEVorbisDecoder::CloseCallback);
    ReversibleHooks::Install("CAEVorbisDecoder", "SeekCallback", 0x5025b0, &CAEVorbisDecoder::SeekCallback);
    ReversibleHooks::Install("CAEVorbisDecoder", "TellCallback", 0x5025f0, &CAEVorbisDecoder::TellCallback);
    ReversibleHooks::Install("CAEVorbisDecoder", "GetStreamLengthMs", 0x502610, &CAEVorbisDecoder::GetStreamLengthMs);
    ReversibleHooks::Install("CAEVorbisDecoder", "GetStreamPlayTimeMs", 0x502640, &CAEVorbisDecoder::GetStreamPlayTimeMs);
    ReversibleHooks::Install("CAEVorbisDecoder", "SetCursor", 0x502670, &CAEVorbisDecoder::SetCursor);
}

CAEVorbisDecoder* CAEVorbisDecoder::Constructor(CAEDataStream* dataStream, int32 unknown) {
    this->CAEVorbisDecoder::CAEVorbisDecoder(dataStream, unknown);
    return this;
}

CAEVorbisDecoder* CAEVorbisDecoder::Destructor() {
    this->CAEVorbisDecoder::~CAEVorbisDecoder();
    return this;
}
