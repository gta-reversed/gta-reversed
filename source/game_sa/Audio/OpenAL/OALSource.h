#pragma once
#include "OALBase.h"

class OALBuffer;

enum class OALSourceType {
    OST_Uninitialized,
    OST_Static,
    OST_Preloop,
    OST_Streaming
};

class OALSource final : public OALBase {
public:
    OALSource() : OALBase() {}
    virtual ~OALSource() override;

    void ObtainSource();
    void SetZero(uint32 size, uint32 frequency);
    void SetStream();
    void SetData(void* data, uint32 size, uint32 frequency, uint32 channels);

    void SetBuffer(OALBuffer* buffer);
    void QueueBuffer(OALBuffer* buffer);
    void UnqueueBuffers();

    void Play();
    void Pause();

    float GetVolume();
    void SetVolume(float volume);

    void Update();

private:
    static inline HeapPtrArray<OALSource> activeSources{};

    ALuint                  m_sourceId{ 0 };
    OALSourceType           m_type{ OALSourceType::OST_Uninitialized };
    uint32                  __pad;
    OALBuffer*              m_oalBuffer{ nullptr };
    HeapPtrArray<OALBuffer> m_queuedBuffers{};
    uint32                  m_posOffset{ 0 };
    float                   m_currentVolume{ 1.0f };
    ALint                   m_currentState{};
    bool                    m_wasStopped{ false };
};
