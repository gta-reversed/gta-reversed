#include "StdInc.h"

#include "AnimBlendHierarchy.h"

void CAnimBlendHierarchy::InjectHooks() {
    RH_ScopedClass(CAnimBlendHierarchy);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Constructor, 0x4CF270);
    RH_ScopedInstall(Destructor, 0x4CF970);
    RH_ScopedInstall(Shutdown, 0x4CF980);
    RH_ScopedInstall(RemoveAnimSequences, 0x4CF8E0);
    RH_ScopedInstall(AllocSequenceBlock, 0x4CF510);
    RH_ScopedInstall(FindSequence, 0x4CF290);
    RH_ScopedInstall(SetName, 0x4CF2D0);
    RH_ScopedInstall(CalcTotalTime, 0x4CF2F0);
    RH_ScopedInstall(CalcTotalTimeCompressed, 0x4CF3E0);
    RH_ScopedInstall(RemoveQuaternionFlips, 0x4CF4E0);
    RH_ScopedInstall(GetSequenceBlock, 0x4CF560);
    RH_ScopedInstall(Uncompress, 0x4CF5F0);
    RH_ScopedInstall(CompressKeyframes, 0x4CF6C0);
    RH_ScopedInstall(RemoveUncompressedData, 0x4CF760);
    RH_ScopedInstall(MoveMemory, 0x4CF800);
    RH_ScopedInstall(Print, 0x4CF8A0);
}

// 0x4CF270
CAnimBlendHierarchy::CAnimBlendHierarchy() {
    m_pSequences = nullptr;
    m_nSeqCount = 0;
    m_bIsCompressed = false;
    m_bKeepCompressed = false;
    m_nAnimBlockId = -1;
    m_fTotalTime = 0.0f;
    m_Link = nullptr;
    m_hashKey = 0; // NOTSA
}

// 0x4CF970
CAnimBlendHierarchy::~CAnimBlendHierarchy() {
    RemoveAnimSequences();
}

// 0x4CF980
void CAnimBlendHierarchy::Shutdown() {
    RemoveAnimSequences();
    m_bIsCompressed = false;
}

// 0x4CF8E0
void CAnimBlendHierarchy::RemoveAnimSequences() {
    void* oldFrameData = nullptr;
    if (m_pSequences && m_pSequences->m_bUsingExternalMemory) {
        oldFrameData = m_pSequences->m_Frames;
    }
    CAnimManager::RemoveFromUncompressedCache(this);
    delete[] m_pSequences;
    m_pSequences = nullptr;
    m_nSeqCount = 0;
    m_fTotalTime = 0.0f;
    if (oldFrameData) {
        CMemoryMgr::Free(oldFrameData);
    }
}

// 0x4CF510
uint8* CAnimBlendHierarchy::AllocSequenceBlock(bool compressed) const {
    size_t size = 0;
    for (auto& sequence : GetSequences()) {
        size += sequence.GetDataSize(compressed);
    }
    return static_cast<uint8*>(CMemoryMgr::Malloc(size));
}

// 0x4CF290
CAnimBlendSequence* CAnimBlendHierarchy::FindSequence(const char* name) const {
    if (!m_nSeqCount) {
        return nullptr;
    }

    const auto hash = CKeyGen::GetUppercaseKey(name);
    for (auto& seq : GetSequences()) {
        if (seq.GetNameHashKey() == hash) {
            return &seq;
        }
    }

    return nullptr;
}

// 0x4CF2D0
void CAnimBlendHierarchy::SetName(const char* name) {
    m_hashKey = CKeyGen::GetUppercaseKey(name);
}

// 0x4CF4E0
void CAnimBlendHierarchy::RemoveQuaternionFlips() const {
    for (auto& sequence : GetSequences()) {
        sequence.RemoveQuaternionFlips();
    }
}

// 0x4CF560
void* CAnimBlendHierarchy::GetSequenceBlock() const {
    return m_pSequences->m_bUsingExternalMemory
        ? m_pSequences->m_Frames
        : nullptr;
}

// 0x4CF5F0
void CAnimBlendHierarchy::Uncompress() {
    assert(m_nSeqCount > 0);
    assert(m_bIsCompressed);

    //NOTSA_LOG_TRACE("Uncompress(this={:#x})", LOG_PTR(this));

    const auto cSeqData = GetSequenceBlock();

    // Now uncompress sequence data
    auto uSeqData = AllocSequenceBlock(false);
    for (auto& s : GetSequences()) {
        s.Uncompress(uSeqData);
        uSeqData += s.GetDataSize(false);
    }

    // Now we no longer need the old compressed data
    if (cSeqData) {
        CMemoryMgr::Free(cSeqData);
    }

    m_bIsCompressed = false;
    if (m_fTotalTime == 0.0f) {
        RemoveQuaternionFlips();
        CalcTotalTime();
    }
}

// 0x4CF6C0
void CAnimBlendHierarchy::CompressKeyframes() const {
    assert(!m_bIsCompressed);

    //NOTSA_LOG_TRACE("CompressKeyframes(this={:#x})", LOG_PTR(this));

    const auto uSeqData = GetSequenceBlock();

    // Compress current data
    auto cSeqData = AllocSequenceBlock(true);
    for (auto& sequence : GetSequences()) {
        sequence.CompressKeyframes(cSeqData);
        cSeqData += sequence.GetDataSize(true);
    }

    // Now we no longer need the old uncompressed data
    if (uSeqData) {
        CMemoryMgr::Free(uSeqData);
    }
}

// 0x4CF760
void CAnimBlendHierarchy::RemoveUncompressedData() {
    assert(m_nSeqCount > 0);
    assert(!m_bIsCompressed);

    //NOTSA_LOG_TRACE("RemoveUncompressedData(this={:#x})", LOG_PTR(this));

    CompressKeyframes();

    m_bIsCompressed = true;
}

// 0x4CF800
void CAnimBlendHierarchy::MoveMemory() {
    NOTSA_UNREACHABLE("Unused Function"); //plugin::CallMethod<0x4CF800, CAnimBlendHierarchy*>(this);
}

// 0x4CF8A0
void CAnimBlendHierarchy::Print() {
    CAnimManager::UncompressAnimation(this);
    for (auto& sequence : GetSequences()) {
        sequence.Print();
    }
}

// notsa
void CAnimBlendHierarchy::SetNumSequences(size_t n) {
    m_nSeqCount  = (uint16)n;
    m_pSequences = new CAnimBlendSequence[n]; // Yes, they used `new`
}

uint32 CAnimBlendHierarchy::GetIndex() const {
    return CAnimManager::GetAnimIndex(this);
}
