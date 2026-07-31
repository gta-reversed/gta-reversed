#pragma once
#include "OALBase.h"

class OALBuffer final : public OALBase {
public:
    OALBuffer(void* data, uint32 size, uint32 frequency, uint32 channels);
    OALBuffer(void* preloopData, uint32 preloopSize, void* loopData, uint32 loopSize, uint32 frequency, uint32 channels);
    virtual ~OALBuffer() override;

    uint32 GetSize() const { return m_bufferSize; }

    // NOTSA
    OALBuffer(const OALBuffer&) = delete;

private:
    friend class OALSource;
    ALuint m_bufferId{};
    ALuint m_preloopBufferId{ 0 };
    uint32 m_bufferSize{};
};
