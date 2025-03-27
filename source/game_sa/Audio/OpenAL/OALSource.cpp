#include "StdInc.h"
#include "OALSource.h"
#include "OALBuffer.h"

#ifdef USE_OPENAL
OALSource::~OALSource() {
    if (m_oalBuffer && m_type != OALSourceType::OST_Preloop) {
        if (m_sourceId)
            alSourcei(m_sourceId, AL_BUFFER, 0);

        m_oalBuffer->Release();
        m_oalBuffer = nullptr;
    }

    if (m_sourceId) {
        switch (m_type) {
        case OALSourceType::OST_Preloop:
        case OALSourceType::OST_Streaming:
            UnqueueBuffers();
            break;
        }

        std::erase(activeSources, this);
        alDeleteSources(1, &m_sourceId);
        m_sourceId = 0;
    }
}

// Ensures that OpenAL source is created.
void OALSource::ObtainSource() {
    if (m_sourceId)
        return;

    alGenSources(1, &m_sourceId);
    activeSources.push_back(this);

    if (m_oalBuffer && m_type != OALSourceType::OST_Preloop) {
        alSourcei(m_sourceId, AL_BUFFER, m_oalBuffer->m_bufferId);
    }
}

void OALSource::SetZero(uint32 size, uint32 frequency) {
    auto* zeros = new uint8[size];
    std::memset(zeros, 0, size);
    SetData(zeros, size, frequency, 1);
    delete[] zeros;
}

void OALSource::SetStream() {
    m_type = OALSourceType::OST_Streaming;
}

void OALSource::SetData(void* data, uint32 size, uint32 frequency, uint32 channels) {
    const auto buffer = new OALBuffer(data, size, frequency, channels);
    SetBuffer(buffer);
    buffer->Release(); // ???
}

void OALSource::SetBuffer(OALBuffer* buffer) {
    assert(buffer);

    if (m_oalBuffer)
        m_oalBuffer->Release();

    m_type = buffer->m_preloopBufferId ? OALSourceType::OST_Preloop : OALSourceType::OST_Static;

    if (m_sourceId)
        alSourcei(m_sourceId, AL_BUFFER, buffer->m_bufferId);

    m_oalBuffer = buffer;
    buffer->AddRef();
}

// !!!ONLY PUT PTRS THAT ARE ALLOCATED WITH `NEW`!!!
void OALSource::QueueBuffer(OALBuffer* buffer) {
    ObtainSource();
    UnqueueBuffers();

    alSourceQueueBuffers(m_sourceId, 1, &buffer->m_bufferId);
    m_queuedBuffers.push_back(buffer);
    buffer->AddRef();
}

void OALSource::UnqueueBuffers() {
    ALint processed{};
    alGetSourcei(m_sourceId, AL_BUFFERS_PROCESSED, &processed);

    for (auto i = 0; i < processed; i++) {
        ALuint buffer{};
        alSourceUnqueueBuffers(m_sourceId, 1, &buffer);

        const auto queuedBufIt = rng::find_if(m_queuedBuffers, [&buffer](OALBuffer* buf) {
            return buf->m_bufferId == buffer || buf->m_preloopBufferId == buffer;
        });

        if (queuedBufIt == m_queuedBuffers.end())
            continue;
        auto queuedBuf = *queuedBufIt;

        m_posOffset += queuedBuf->m_bufferSize;
        std::erase(m_queuedBuffers, queuedBuf);
        queuedBuf->Release();
    }
}

void OALSource::Play() {
    ObtainSource();

    if (m_type == OALSourceType::OST_Preloop) {
        UnqueueBuffers();

        alSourceQueueBuffers(m_sourceId, 1, &m_oalBuffer->m_preloopBufferId);
        for (auto i = 0; i < 11; i++) { // todo: why 11?
            alSourceQueueBuffers(m_sourceId, 1, &m_oalBuffer->m_bufferId);
        }
    }

    alSourcePlay(m_sourceId);
    m_currentState = AL_PLAYING;
    m_wasStopped = false;
}

void OALSource::Pause() {
    ObtainSource();
    alSourcePause(m_sourceId);
}

float OALSource::GetVolume() {
    return std::log(1.0f / m_currentVolume) * -8.6562f; // todo: constant
}

void OALSource::SetVolume(float volume)
{
    ObtainSource();

    if (const auto vol = std::min(1.0f / std::exp2f(volume / -6.0f), 1.0f); m_currentVolume != vol) {
        m_currentVolume = vol;
        alSourcef(m_sourceId, AL_GAIN, vol);
    }
}

void OALSource::Update() {
    m_currentState = 0;

    if (m_type != OALSourceType::OST_Preloop)
        return;

    ALint processed{};
    alGetSourcei(m_sourceId, AL_BUFFERS_PROCESSED, &processed);

    for (auto i = 0; i < processed; i++) {
        ALuint buffer{};
        alSourceUnqueueBuffers(m_sourceId, 1, &buffer);
        alSourceQueueBuffers(m_sourceId, 1, &m_oalBuffer->m_bufferId);
    }
}
#endif
