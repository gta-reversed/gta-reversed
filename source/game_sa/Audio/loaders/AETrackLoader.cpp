#include "StdInc.h"
#include "AETrackLoader.h"
#include "FileMgr.h"

void CAETrackLoader::InjectHooks() {
    RH_ScopedClass(CAETrackLoader);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(GetDataStream, 0x4E0D20);
}

CAEDataStream* CAETrackLoader::GetDataStream(uint32 trackId) {
    if (trackId >= m_nTrackCount)
        return nullptr;

    const auto packId = m_paTrackLookups[trackId].m_nbPackId;
    if (packId > m_nStreamPackCount)
        return nullptr;

    const auto length = [&] {
        if (m_bStreamingFromDVD) {
            return strlen(m_pszDvdDrivePath) + strlen(m_paStreamPacks[packId].m_szName) + 21;
        }
        return strlen(m_paStreamPacks[packId].m_szName) + 21;
    }();

    char* path = new char[length]; // owned by to be ctor'd CAEDataStream.
    strcpy_s(path, length,
        std::format("{}\\{}",
            !m_bStreamingFromDVD ? "AUDIO\\STREAMS\\" : std::string{m_pszDvdDrivePath} + "AUDIO\\STREAMS\\",
            m_paStreamPacks[packId].m_szName
        ).c_str()
    );

    auto* stream = new CAEDataStream(
        trackId,
        path,
        m_paTrackLookups[trackId].m_nOffset + sizeof(tTrackInfo),
        m_paTrackLookups[trackId].m_nSize,
        true
    );

    CFileMgr::SetDir("");
    if (!stream->Initialise()) {
        delete stream;
        stream = nullptr;
    }

    return stream;
}
