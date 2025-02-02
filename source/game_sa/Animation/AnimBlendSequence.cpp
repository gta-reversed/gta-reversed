#include "StdInc.h"

#include "AnimBlendSequence.h"

void CAnimBlendSequence::InjectHooks() {
    RH_ScopedClass(CAnimBlendSequence);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Constructor, 0x4D0C10);
    RH_ScopedInstall(Destructor, 0x4D0C30);
    RH_ScopedInstall(SetName, 0x4D0C50);
    RH_ScopedInstall(SetBoneTag, 0x4D0C70);
    RH_ScopedInstall(GetDataSize, 0x4D0C90);
    RH_ScopedInstall(SetNumFrames, 0x4D0CD0);
    RH_ScopedInstall(Uncompress, 0x4D0D40);
    RH_ScopedInstall(CompressKeyframes, 0x4D0F40);
    RH_ScopedInstall(MoveMemory, 0x4D1150);
    RH_ScopedInstall(Print, 0x4D1180);
    RH_ScopedInstall(RemoveQuaternionFlips, 0x4D1190);
    RH_ScopedInstall(RemoveUncompressedData, 0x4D12A0);
    RH_ScopedInstall(GetUKeyFrame, 0x4CF1F0);
    RH_ScopedInstall(GetCKeyFrame, 0x4CF220);
}

// 0x4D0C30
CAnimBlendSequence::~CAnimBlendSequence() {
    // If we allocated memory, clean it
    if (!m_bUsingExternalMemory && m_Frames) {
        CMemoryMgr::Free(m_Frames);
    }
}

template<typename From, typename To>
bool CAnimBlendSequence::ConvertKeyFrames(byte* pDataBlock) {
    constexpr auto HasTranslation = requires{ To::Trans; };

    assert(HasTranslation == m_bHasTranslation);

    if (m_FramesNum == 0) {
        return false;
    }

    void* const outFrames = pDataBlock
        ? pDataBlock
        : CMemoryMgr::Malloc(GetDataSize(true));

    auto* inKF  = static_cast<From*>(m_Frames);
    auto* outKF = static_cast<To*>(outFrames);
    for (auto i = m_FramesNum; i --> 0; inKF++, outKF++) {
        outKF->Rot = inKF->Rot;
        outKF->DeltaTime = inKF->DeltaTime;
        if constexpr (HasTranslation) {
            outKF->Trans = inKF->Trans;
        }
    }

    if (!m_bUsingExternalMemory) {
        CMemoryMgr::Free(m_Frames);
    }

    m_Frames               = outFrames;
    m_bUsingExternalMemory = pDataBlock != nullptr;

    return true;
}

// 0x4D0F40
void CAnimBlendSequence::CompressKeyframes(uint8* pDataBlock) {
    assert(!m_bIsCompressed);

    if (m_bHasTranslation
            ? ConvertKeyFrames<KeyFrameTrans, KeyFrameTransCompressed>(pDataBlock)
            : ConvertKeyFrames<KeyFrame, KeyFrameCompressed>(pDataBlock)
    ) {
        m_bIsCompressed = true; 
    }
}

// 0x4D0D40
void CAnimBlendSequence::Uncompress(uint8* pDataBlock) {
    assert(m_bIsCompressed);

    if (m_bHasTranslation
            ? ConvertKeyFrames<KeyFrameTransCompressed, KeyFrameTrans>(pDataBlock)
            : ConvertKeyFrames<KeyFrameCompressed, KeyFrame>(pDataBlock)
    ) {
        m_bIsCompressed = false; 
    }
}

// 0x4D12A0
void CAnimBlendSequence::RemoveUncompressedData(uint8* frameData) {
    if (m_FramesNum) {
        CompressKeyframes(frameData);
    }
}

// 0x4D0C90
size_t CAnimBlendSequence::GetDataSize(bool compressed) const {
    const auto kfSize = compressed
        ? m_bHasTranslation ? sizeof(KeyFrameTransCompressed) : sizeof(KeyFrameCompressed)
        : m_bHasTranslation ? sizeof(KeyFrameTrans) : sizeof(KeyFrame);
    return kfSize * m_FramesNum;
}

// 0x4D1190
void CAnimBlendSequence::RemoveQuaternionFlips() const {
    if (m_FramesNum < 2) {
        return;
    }

    KeyFrame* f;
    CQuaternion last = GetUKeyFrame(0)->Rot;
    for (auto i = 1u; i < m_FramesNum; i++) {
        f = GetUKeyFrame(i);
        if (DotProduct(last, f->Rot) < 0.0f) {
            f->Rot = -f->Rot;
        }
        last = f->Rot;
    }
}

// 0x4D0C50
void CAnimBlendSequence::SetName(const char* name) {
    if (notsa::IsFixBugs()) {
        m_IsUsingBoneTag = false;
    }
    m_NameHashKey = CKeyGen::GetUppercaseKey(name);
}

// 0x4D0C70
void CAnimBlendSequence::SetBoneTag(eBoneTag32 boneId) {
    if (boneId != BONE_UNKNOWN) {
        m_IsUsingBoneTag = true;
        m_BoneTag         = boneId;
    }
}

// 0x4D0CD0
void CAnimBlendSequence::SetNumFrames(uint32 count, bool bHasTranslation, bool compressed, void* frameData) {
    m_bHasTranslation      = bHasTranslation;
    m_FramesNum            = (uint16)count;
    m_Frames               = frameData ? frameData : CMemoryMgr::Malloc(GetDataSize(compressed));
    m_bUsingExternalMemory = frameData != nullptr; // NOTSA
    m_bHasRotation         = true;
    m_bIsCompressed        = compressed; // condition has been removed
}

// 0x4D1150
bool CAnimBlendSequence::MoveMemory() {
    if (m_bUsingExternalMemory || !m_Frames) {
        return false;
    }

    const auto prevPtr = std::exchange(m_Frames, (CAnimBlendSequence*)CMemoryMgr::MoveMemory(m_Frames));
    return prevPtr != m_Frames; // Return true if the memory was moved
}

// 0x4D1180
void CAnimBlendSequence::Print() {
#ifndef FINAL

#endif
}
