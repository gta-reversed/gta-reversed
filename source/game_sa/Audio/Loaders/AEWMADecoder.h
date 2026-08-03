#pragma once

// WMA
#include <wmsdkidl.h>
#include <wmsdk.h>

#include "AEStreamingDecoder.h"

class CAEWMADecoder : public CAEStreamingDecoder {
private:
    static constexpr size_t TEMPBUFSIZE = 131072;

    bool           m_bInitialized;
    int32          _unknown;
    uint64         m_nSampleTime;
    uint64         m_nSampleDuration;
    char           m_szBuffer[TEMPBUFSIZE];
    size_t         m_nTempBufferUsed;
    uint16         m_wStreamNumber;
    int32          m_nSampleRate;
    long           m_nLengthMs;
    IWMSyncReader* syncReader;
    int32          _unknown2;

    HRESULT SelectStreamIndex(IWMProfile* profile);

    static HRESULT(__stdcall*& WMCreateSyncReader)(IUnknown*, DWORD, IWMSyncReader**);
    static inline auto& wmvCoreModule = StaticRef<HMODULE>(0xb6bad4);

public:
    CAEWMADecoder(CAEDataStream* dataStream);
    ~CAEWMADecoder() override;

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

    CAEWMADecoder* Constructor(CAEDataStream* dataStream);
    CAEWMADecoder* Destructor();
};

VALIDATE_SIZE(CAEWMADecoder, 0x20038);
