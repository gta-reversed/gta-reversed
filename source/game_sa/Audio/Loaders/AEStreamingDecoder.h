#pragma once

#include "AEDataStream.h"

class CAEStreamingDecoder {
public:
    CAEStreamingDecoder(CAEDataStream* dataStream);

#ifdef USE_DEFAULT_FUNCTIONS
    void operator delete(void* mem);
#endif

    virtual bool   Initialise() = 0;
    virtual size_t FillBuffer(void* dest, size_t size) = 0;
    virtual long   GetStreamLengthMs() = 0;
    virtual long   GetStreamPlayTimeMs() = 0;
    virtual void   SetCursor(unsigned long pos) = 0;
    virtual int32  GetSampleRate() = 0;
    virtual ~CAEStreamingDecoder();
    virtual int32 GetStreamID() = 0;

protected:
    CAEDataStream* m_dataStream;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEStreamingDecoder* Constructor(CAEDataStream* dataStream);
    CAEStreamingDecoder* Destructor();
};

VALIDATE_SIZE(CAEStreamingDecoder, 0x8);
