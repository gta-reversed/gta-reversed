#include "StdInc.h"
#include "AEMP3TrackLoader.h"
#include "app.h"
#include "FileMgr.h"
#include "MemoryMgr.h"
#include "extensions/File.hpp"

void CAEMP3TrackLoader::InjectHooks() {
    RH_ScopedClass(CAEMP3TrackLoader);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x4E0930);
    RH_ScopedInstall(Deconstructor, 0x4E0940);
    RH_ScopedInstall(Initialise, 0x4E0C50);
    RH_ScopedInstall(LoadStreamPackTable, 0x4E0970);
    RH_ScopedInstall(LoadTrackLookupTable, 0x4E09F0);
    RH_ScopedInstall(GetTrackInfo, 0x4E0A70);
    RH_ScopedInstall(IsCurrentAudioStreamAvailable, 0x4E0BD0);
}

// 0x4E0930
CAEMP3TrackLoader::CAEMP3TrackLoader() {
    m_bInitialised = false;
    m_pszDvdDrivePath = nullptr;
}

// 0x4E0940
CAEMP3TrackLoader::~CAEMP3TrackLoader() {
    if (m_bInitialised) {
        CMemoryMgr::Free(m_paTrackLookups);
        CMemoryMgr::Free(m_paStreamPacks);
    }

    if (m_pszDvdDrivePath) {
        delete m_pszDvdDrivePath;
    }
}

// 0x4E0C50
bool CAEMP3TrackLoader::Initialise() {
    if (!LoadStreamPackTable() || !LoadTrackLookupTable())
        return false;

    m_bStreamingFromDVD = false;

    // NOTSA: Originally used a heap allocated string and strcpy/strcat to make paths.
    // Firstly try loading locally.
    const auto localPath = std::format("AUDIO\\STREAMS\\{}", m_paStreamPacks[0].m_szName);
    if (notsa::File audiofp(localPath.c_str(), "r"); audiofp) {
        m_bInitialised = true;
        m_bStreamingFromDVD = false;
        return true;
    }

    // Try loading from a DVD.
    NOTSA_LOG_INFO("Audio files couldn't be found in the local installation, will try looking for a GTASA DVD.");
    return IsCurrentAudioStreamAvailable();
}

// 0x4E0970
bool CAEMP3TrackLoader::LoadStreamPackTable(void) {
    notsa::File strmpaks("AUDIO\\CONFIG\\STRMPAKS.DAT", "rb");

    if (!strmpaks) {
        // NOTSA: Originally (StreamPack*)CMemoryMgr::Malloc(0), return value is implementation-dependent.
        NOTSA_LOG_ERR("Couldn't open strmpaks.dat");
        m_paStreamPacks = nullptr;
        m_nStreamPackCount = 0;
        return false;
    }

    const auto size = strmpaks.GetTotalSize();
    m_paStreamPacks = (StreamPack*)CMemoryMgr::Malloc(size);
    m_nStreamPackCount = size / sizeof(StreamPack);

    // NOTE: Win32 API outputs number of bytes read, most likely checking is unnecessary.
    NOTSA_LOG_DEBUG("read: {}", strmpaks.Read(m_paStreamPacks, size));
    return true;
}

// 0x4E09F0
bool CAEMP3TrackLoader::LoadTrackLookupTable(void) {
    // NOTSA: Originally Win32 file API was used.
    notsa::File traklkup("AUDIO\\CONFIG\\TRAKLKUP.DAT", "rb");
    if (!traklkup) {
        NOTSA_LOG_ERR("Couldn't open traklkup.dat");
        return false;
    }

    const auto size = traklkup.GetTotalSize();
    m_paTrackLookups = (tTrackLookup*)CMemoryMgr::Malloc(size);
    m_nTrackCount = size / sizeof(tTrackLookup);
    NOTSA_LOG_DEBUG("file size: {}, numpaks: {}", size, m_nTrackCount);

    // NOTE: Win32 API outputs number of bytes read, most likely checking is unnecessary.
    NOTSA_LOG_DEBUG("read: {}", traklkup.Read(m_paTrackLookups, size));
    return true;
}

// 0x4E0A70
tTrackInfo* CAEMP3TrackLoader::GetTrackInfo(uint32 trackId) {
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
        std::format("{}AUDIO\\STREAMS\\{}",
            !m_bStreamingFromDVD ? "" : std::string{m_pszDvdDrivePath},
            m_paStreamPacks[packId].m_szName
        ).c_str()
    );

    auto* stream = new CAEDataStream(
        trackId,
        path,
        m_paTrackLookups[trackId].m_nOffset,
        m_paTrackLookups[trackId].m_nSize,
        true
    );
    auto* trackInfo = new tTrackInfo;

    stream->Initialise();
    stream->FillBuffer(trackInfo, sizeof(tTrackInfo));
    delete stream;

    return trackInfo;
}

// 0x4E0BD0
bool CAEMP3TrackLoader::IsCurrentAudioStreamAvailable() {
    if (m_pszDvdDrivePath = getDvdGamePath()) {
        const auto dvdPath = std::format("{}\\AUDIO\\STREAMS\\{}", getDvdGamePath(), m_paStreamPacks[0].m_szName);
        if (notsa::File dvdfp(dvdPath.c_str(), "r"); dvdfp) {
            NOTSA_LOG_INFO("Game will use the audio files from the GTASA DVD.");
            m_bInitialised = true;
            m_bStreamingFromDVD = true;
            return true;
        }
    }

    NOTSA_LOG_ERR("Audio files are missing!");
    m_bStreamingFromDVD = false;
    return false;
}
