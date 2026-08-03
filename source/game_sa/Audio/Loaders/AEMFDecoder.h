#pragma once

#include <mfidl.h>
#include <mfreadwrite.h>

#include "AEStreamingDecoder.h"

class CAEMFDecoder : public CAEStreamingDecoder {
private:
    static constexpr size_t TEMPBUFFRAME = 65536;
    static inline auto& quickTimeInitialized = StaticRef<bool>(0xb61d70); // QuickTime is replaced with MediaFoundation
    static HMODULE mfPlatModule, mfReadWriteModule;
    static HRESULT(__stdcall* MFCreateSourceReaderFromURL)(LPCWSTR pwszURL, IMFAttributes* pAttributes, IMFSourceReader** ppSourceReader);
    static HRESULT(__stdcall* MFCreateMediaType)(IMFMediaType** ppMFType);

    char* m_pFilename;    // index 8
    bool  m_bInitialized; // index 12
    union {
        uint32 _unknown2[36];
        struct {
            IMFSourceReader* m_pSourceReader;
            IMFMediaType*    m_pPcmAudio;
            int16*         m_wpTempBuffer;
            size_t           m_nTempBufferUsed;
        };
    };
    long m_nPlayTime; // index 160
    long m_nLength;   // index 164
    union {
        uint32 _unknown3[3];
    };
    int32 m_nTrackID;     // index 180
    long  m_nLengthMs;    // index 184
    int32 m_nSampleRate;  // index 188
    float m_fTimeDivider; // index 192

public:
    CAEMFDecoder(char* filename, int32 trackID);
    ~CAEMFDecoder() override;

    bool   Initialise() override;
    size_t FillBuffer(void* dest, size_t size) override;
    long   GetStreamLengthMs() override;
    long   GetStreamPlayTimeMs() override;
    void   SetCursor(unsigned long pos) override;
    int32  GetSampleRate() override;
    int32  GetStreamID() override;

    static bool InitLibrary();
    static void Shutdown();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEMFDecoder* Constructor(char* filename, int32 trackID);
    CAEMFDecoder* Destructor();
};

VALIDATE_SIZE(CAEMFDecoder, 0xc4);
