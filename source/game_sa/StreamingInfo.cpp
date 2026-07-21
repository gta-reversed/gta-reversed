#include "StdInc.h"

void CStreamingInfo::InjectHooks() {
    RH_ScopedClass(CStreamingInfo);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x407460);
    RH_ScopedInstall(AddToList, 0x407480);
    RH_ScopedInstall(GetCdPosn, 0x407570);
    RH_ScopedInstall(SetCdPosnAndSize, 0x4075E0);
    RH_ScopedInstall(GetCdPosnAndSize, 0x4075A0);
    RH_ScopedInstall(InList, 0x407560);
    RH_ScopedInstall(RemoveFromList, 0x4074E0);
}

// 0x407460
void CStreamingInfo::Init() {
    *this = {};
}

// 0x407570
CdStreamPos CStreamingInfo::GetCdPosn() const {
    return CdStreamPos{
        .Offset = m_CDOffset,
        .FileID = CdStreamHandleToFileID(CStreaming::ms_files[m_ImgID].StreamHandle)
    };
}

// 0x4075E0
void CStreamingInfo::SetCdPosnAndSize(uint32 offset, size_t size) {
    m_CDOffset = offset;
    m_CDSize   = size;
}

// 0x4075A0
bool CStreamingInfo::GetCdPosnAndSize(CdStreamPos& pos, size_t& size) {
    if (HasCdPosnAndSize()) {
        pos  = GetCdPosn();
        size = m_CDSize;
        return true;
    }
    return false;
}

// 0x407560
bool CStreamingInfo::InList() const {
    assert(m_NextIndex == -1 || m_PrevIndex != -1 && "Entries in the info list must have both a valid next and prev index");

    // These lists work by having a pre-defined begin/end sentinels (they're stored in `CStreaming`).
    // Hence, every entry in the list (except these sentinels) must have a valid `prev`
    // and `next` index. They weren't checking for `prev` because it shouldn't be `-1` by definition iff `next` isn't `-1`.
    // I've added the extra check, just in case (Though the above assert will catch it before this in debug mode)
    return m_NextIndex != -1 /*notsa => */ && m_PrevIndex != -1;
}

// 0x407480
void CStreamingInfo::AddToList(CStreamingInfo* after) {
    assert(!InList()); // May not be in a list (As that would corrupt the list)

    m_NextIndex = after->m_NextIndex;
    m_PrevIndex = static_cast<int16>(after - ms_pArrayBase);

    after->m_NextIndex = static_cast<int16>(this - ms_pArrayBase);
    ms_pArrayBase[m_NextIndex].m_PrevIndex = after->m_NextIndex;
}

// 0x4074E0
void CStreamingInfo::RemoveFromList() {
    assert(InList()); // Must be in a list (Otherwise array access is UB)

    ms_pArrayBase[m_NextIndex].m_PrevIndex = m_PrevIndex;
    ms_pArrayBase[m_PrevIndex].m_NextIndex = m_NextIndex;
    m_NextIndex = -1;
    m_PrevIndex = -1;
}
