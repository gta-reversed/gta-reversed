#include "StdInc.h"

#include "AnimBlendStaticAssociation.h"

void CAnimBlendStaticAssociation::InjectHooks() {
    RH_ScopedVirtualClass(CAnimBlendStaticAssociation, 0x85C6CC, 1);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Constructor1, 0x4CE940);
    RH_ScopedInstall(Constructor2, 0x4CEF60);
    RH_ScopedInstall(Destructor, 0x4CDF50);

    RH_ScopedInstall(Init, 0x4CEC20);
    RH_ScopedInstall(AllocateSequenceArray, 0x4CE960);
    RH_ScopedInstall(FreeSequenceArray, 0x4CE9A0);
}

// 0x4CEF60
CAnimBlendStaticAssociation::CAnimBlendStaticAssociation(RpClump* clump, CAnimBlendHierarchy* h) {
    Init(clump, h);
}

// 0x4CEC00 (We hook with the vtbl with a deleting destructor wrapper, not this fn directly)
CAnimBlendStaticAssociation::~CAnimBlendStaticAssociation() {
    FreeSequenceArray();
}

// 0x4CEC20
void CAnimBlendStaticAssociation::Init(RpClump* clump, CAnimBlendHierarchy* h) {
    m_BlendHier = h;

    if (!clump) {
        return;
    }

    const auto clumpAnimData = RpAnimBlendClumpGetData(clump);
    assert(clumpAnimData);

    m_NumBlendNodes = clumpAnimData->m_NumFrameData;
    AllocateSequenceArray(m_NumBlendNodes);

    // Initialize sequences from the data in the clump
    for (auto& seq : m_BlendHier->GetSequences()) {
        if (!seq.m_FramesNum) {
            continue;
        }
        const auto frameData = seq.IsUsingBoneTag()
            ? RpAnimBlendClumpFindBone(clump, seq.GetBoneTag())
            : RpAnimBlendClumpFindFrameFromHashKey(clump, seq.GetNameHashKey());
        if (!frameData) {
            continue;
        }
        const auto nodeIdx = frameData - clumpAnimData->m_FrameDatas;
        assert(nodeIdx >= 0 && nodeIdx < m_NumBlendNodes);
        m_BlendSeqs[nodeIdx] = &seq;
    }
}

// 0x4CE960
void CAnimBlendStaticAssociation::AllocateSequenceArray(int32 count) {
    m_BlendSeqs = (CAnimBlendSequence**)CMemoryMgr::Malloc(sizeof(CAnimBlendSequence*) * count);
    for (auto& array : std::span{ m_BlendSeqs, (size_t)count }) {
        array = nullptr;
    }
}

// 0x4CE9A0
void CAnimBlendStaticAssociation::FreeSequenceArray() {
    if (m_BlendSeqs) {
        CMemoryMgr::Free(m_BlendSeqs);
    }
}
