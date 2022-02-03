#include "StdInc.h"

#include "AnimBlendAssociation.h"

void CAnimBlendAssociation::InjectHooks() {
    RH_ScopedClass(CAnimBlendAssociation);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Constructor, 0x4CEFC0);
}

void* CAnimBlendAssociation::operator new(uint32 size) {
    return ((CAnimBlendAssociation * (__cdecl*)(uint32))0x82119A)(size);
}

void CAnimBlendAssociation::operator delete(void* object) {
    ((void(__cdecl*)(void*))0x8214BD)(object);
}

// 0x4CEFC0
CAnimBlendAssociation::CAnimBlendAssociation(RpClump* clump, CAnimBlendHierarchy* animHierarchy) {
    m_fBlendAmount = 1.0f;
    m_fSpeed = 1.0f;
    m_pNodeArray = nullptr;
    m_fBlendDelta = 0.0f;
    m_fCurrentTime = 0.0f;
    m_fTimeStep = 0.0f;
    m_nFlags = 0;
    m_nCallbackType = ANIMBLENDCALLBACK_NONE;
    m_nAnimGroup = -1;
    m_nAnimId = -1;
    m_pNext = nullptr;
    m_pPrevious = nullptr;
    Init(clump, animHierarchy);
}

CAnimBlendAssociation::~CAnimBlendAssociation() {
    if (m_pNodeArray)
        CMemoryMgr::FreeAlign(m_pNodeArray);
    if (m_pPrevious)
        m_pPrevious->m_pNext = m_pNext;
    if (m_pNext)
        m_pNext->m_pPrevious = m_pPrevious;
    m_pNext = nullptr;
    m_pPrevious = nullptr;
    if (m_nFlags & ANIM_FLAG_BLOCK_REFERENCED)
        CAnimManager::RemoveAnimBlockRef(m_pHierarchy->m_nAnimBlockId);
}


CAnimBlendAssociation* CAnimBlendAssociation::Constructor(RpClump* clump, CAnimBlendHierarchy* animHierarchy) {
    this->CAnimBlendAssociation::CAnimBlendAssociation(clump, animHierarchy);
    return this;
}

inline CAnimBlendClumpData * GetAnimClumpData(RpClump * clump)
{
    const DWORD clumpOffset = (*(DWORD*)0xB5F878);
    //return reinterpret_cast <CAnimBlendClumpData *> (*(&clump->object.type + clumpOffset));
    return reinterpret_cast <CAnimBlendClumpData *> (*(DWORD *)(clumpOffset + ((int32)clump)  ));
}

// 0x4CED50
void CAnimBlendAssociation::Init(RpClump* clump, CAnimBlendHierarchy* animHierarchy) {
    return plugin::CallMethod<0x4CED50, CAnimBlendAssociation*, RpClump*, CAnimBlendHierarchy*>(this, clump, animHierarchy);

#if 0
    std::printf("\nCAnimBlendAssociation::Init1: called! clump: %p | m_nSeqCount: %d\n\n", clump, animHierarchy->m_nSeqCount);
    CAnimBlendClumpData * pAnimClumpData = GetAnimClumpData(clump);
    m_nNumBlendNodes = pAnimClumpData->m_nNumFrames;
    AllocateAnimBlendNodeArray(m_nNumBlendNodes);
    for (size_t i = 0; i < m_nNumBlendNodes; i++)
    {
        m_pNodeArray[i].m_pAnimBlendAssociation = this; 
    }

    m_pHierarchy = animHierarchy;
    for (size_t i = 0; i < animHierarchy->m_nSeqCount; i++)
    {
        CAnimBlendSequence& sequence = animHierarchy->m_pSequences[i];
        AnimBlendFrameData *pAnimFrameData = nullptr;

        // is bone id set?
        if (sequence.m_nFlags & 0x10)
        {
            pAnimFrameData = RpAnimBlendClumpFindBone(clump, sequence.m_boneId);
        }
        else 
        {
            pAnimFrameData = RpAnimBlendClumpFindFrameFromHashKey(clump, sequence.m_hash);
        }
        if (pAnimFrameData && sequence.m_nFrameCount > 0)
        {
            auto v11 = ((715827883i64 * ((DWORD)pAnimFrameData - (DWORD)pAnimClumpData->m_pFrames)) >> 32) >> 2;
            auto frameIndex = (v11 >> 31) + v11;
            std::cout << "i: " << i  <<" | frameIndex: " << frameIndex << " | boneid: " << sequence.m_boneId << "\n"; 
            m_pNodeArray[frameIndex].m_pAnimSequence = &sequence;
        }
    }
#endif
}

void CAnimBlendAssociation::Start(float currentTime) {
    plugin::CallMethod<0x4CEB70, CAnimBlendAssociation*, float>(this, currentTime);
}

uint32 CAnimBlendAssociation::GetHashKey() const noexcept {
    return m_pHierarchy->m_hashKey;
}

void CAnimBlendAssociation::AllocateAnimBlendNodeArray(int32 count) {
    plugin::CallMethod<0x4CE9F0, CAnimBlendAssociation*, int32>(this, count);
}

void CAnimBlendAssociation::SetBlend(float blendAmount, float blendDelta) {
    plugin::CallMethod<0x4CEBA0, CAnimBlendAssociation*, float, float>(this, blendAmount, blendDelta);
}

void CAnimBlendAssociation::SetCurrentTime(float currentTime) {
    plugin::CallMethod<0x4CEA80, CAnimBlendAssociation*, float>(this, currentTime);
}

void CAnimBlendAssociation::SetDeleteCallback(void (*callback)(CAnimBlendAssociation*, void*), void* data) {
    plugin::CallMethod<0x4CEBC0, CAnimBlendAssociation*, void (*)(CAnimBlendAssociation*, void*), void*>(this, callback, data);
}

void CAnimBlendAssociation::SetFinishCallback(void (*callback)(CAnimBlendAssociation*, void*), void* data) {
    plugin::CallMethod<0x4CEBE0, CAnimBlendAssociation*, void (*)(CAnimBlendAssociation*, void*), void*>(this, callback, data);
}
