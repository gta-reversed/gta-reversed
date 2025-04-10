#pragma once
#include "OALBase.h"

#ifdef USE_OPENAL
class OALBuffer final : public OALBase {
public:
    ALuint m_bufferId{};
    ALuint m_preloopBufferId{0};
    uint32 m_bufferSize{};

public:
    OALBuffer(void* data, uint32 size, uint32 frequency, uint32 channels);
    OALBuffer(void* preloopData, uint32 preloopSize, void* loopData, uint32 loopSize, uint32 frequency, uint32 channels);
    virtual ~OALBuffer() override;

    // NOTSA
    OALBuffer(const OALBuffer&) = delete;
};
#endif
