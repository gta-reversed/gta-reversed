#include "StdInc.h"

#include "D3DIndexDataBuffer.h"

// NOTSA (Inlined in `D3DResourceSystem::Init`)
void D3DIndexDataBuffer::Setup(D3DFORMAT format, uint32 maxNumIndices, uint32 capacity) {
    m_Format                 = format;
    m_MaxNumIndices          = maxNumIndices;
    m_IndexBuffers           = new IDirect3DIndexBuffer9*[capacity];
    m_Capacity               = capacity;
    m_NumIndexBuffers        = 0;
    m_NumIndexBuffersCounter = 0;
}

// 0x7301D0
void D3DIndexDataBuffer::Clear() {
    for (auto* const indexBuffer : std::span{ m_IndexBuffers, m_NumIndexBuffers }) {
        indexBuffer->Release();
    }
    m_NumIndexBuffers        = 0;
    m_NumIndexBuffersCounter = 0;
}

// 0x730200
void D3DIndexDataBuffer::Destroy() {
    Clear();
    delete[] std::exchange(m_IndexBuffers, nullptr);
}

// 0x730330
void D3DIndexDataBuffer::Resize(uint32 newCapacity) {
    if (newCapacity == m_Capacity) {
        return;
    }
    auto* const indexBuffers = new IDirect3DIndexBuffer9*[newCapacity];
    for (auto i = 0u; i < m_NumIndexBuffers && i < newCapacity; i++) {
        indexBuffers[i] = m_IndexBuffers[i];
    }
    for (auto i = newCapacity; i < m_NumIndexBuffers; i++) {
        m_IndexBuffers[i]->Release();
    }
    // NOTE: The original keeps `m_NumIndexBuffers` when shrinking - it's only ever called to grow
    delete[] m_IndexBuffers;
    m_IndexBuffers = indexBuffers;
    m_Capacity     = newCapacity;
}

// 0x7303B0
uint32 D3DIndexDataBuffer::GetTotalDataSize() const {
    const auto GetSize = [](IDirect3DIndexBuffer9* indexBuffer) {
        D3DINDEXBUFFER_DESC desc;
        indexBuffer->GetDesc(&desc);
        return desc.Size;
    };
    if (m_Format != D3DFMT_UNKNOWN) { // NOTE: Always the case, so this is only an estimate for the buffer of the big ones
        return m_NumIndexBuffers
            ? m_NumIndexBuffers * GetSize(m_IndexBuffers[0])
            : 0;
    }
    uint32 total = 0;
    for (auto* const indexBuffer : std::span{ m_IndexBuffers, m_NumIndexBuffers }) {
        total += GetSize(indexBuffer);
    }
    return total;
}

// 0x730270
IDirect3DIndexBuffer9* D3DIndexDataBuffer::Pop(uint32 numIndices) {
    // Find the smallest one that's big enough
    auto   bestIdx  = -1;
    uint32 bestSize = INT32_MAX;
    for (auto i = (int32)m_NumIndexBuffers; i != 0;) {
        D3DINDEXBUFFER_DESC desc;
        m_IndexBuffers[--i]->GetDesc(&desc);
        if (desc.Size < bestSize && desc.Size >= numIndices * sizeof(uint16)) {
            bestSize = desc.Size;
            bestIdx  = i;
        }
    }
    if (bestIdx == -1) {
        return nullptr;
    }
    auto* const indexBuffer = m_IndexBuffers[bestIdx];
    m_IndexBuffers[bestIdx] = m_IndexBuffers[--m_NumIndexBuffers];
    return indexBuffer;
}

// NOTSA
IDirect3DIndexBuffer9* D3DIndexDataBuffer::PopLast() {
    if (!m_NumIndexBuffers) {
        return nullptr;
    }
    m_NumIndexBuffersCounter--;
    return m_IndexBuffers[--m_NumIndexBuffers];
}

// NOTSA (Inlined in `D3DResourceSystem::DestroyIndexBuffer`)
void D3DIndexDataBuffer::PushOrGrow(IDirect3DIndexBuffer9* indexBuffer) {
    m_NumIndexBuffersCounter++;
    if (m_NumIndexBuffers >= m_Capacity) {
        Resize(m_Capacity * 2);
        if (m_NumIndexBuffers >= m_Capacity) { // NOTE: Only with a capacity of 0 (never the case), the original drops (leaks) the buffer then
            return;
        }
    }
    m_IndexBuffers[m_NumIndexBuffers++] = indexBuffer;
}
