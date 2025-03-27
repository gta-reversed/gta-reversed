#include "StdInc.h"
#include "OALBuffer.h"

#ifdef USE_OPENAL
OALBuffer::OALBuffer(void* data, uint32 size, uint32 frequency, uint32 channels)
    : OALBase()
{
    alGenBuffers(1, &m_bufferId);
    alBufferData(m_bufferId, channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, data, size, frequency);
    m_bufferSize = size;
}

OALBuffer::OALBuffer(void* preloopData, uint32 preloopSize, void* loopData, uint32 loopSize, uint32 frequency, uint32 channels)
    : OALBase()
{
    ALuint buffers[2]{};
    alGenBuffers(2, buffers);
    m_bufferId = buffers[0];
    m_preloopBufferId = buffers[1];

    const auto format = channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

    alBufferData(m_preloopBufferId, format, preloopData, preloopSize, frequency);

    if (loopSize > 0x18F) {
        alBufferData(m_bufferId, format, loopData, loopSize, frequency);
    } else {
        auto* data = new uint8[4 * loopSize];
        for (auto i = 0; i < 4; i++) {
            memcpy(&data[i * loopSize], loopData, loopSize);
        }
        alBufferData(m_bufferId, format, data, 4 * loopSize, frequency);
        delete[] data;
    }

    m_bufferSize = loopSize + preloopSize;
}

OALBuffer::~OALBuffer() {
    alDeleteBuffers(1, &m_bufferId);

    if (m_preloopBufferId) {
        alDeleteBuffers(1, &m_preloopBufferId);
    }
}
#endif
