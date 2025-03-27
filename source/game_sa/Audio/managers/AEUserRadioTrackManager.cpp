#include "StdInc.h" // TODO: Remove

#include <cstdio>

// COM
#include <ShObjIdl.h>

#include "common.h"

#include "AEUserRadioTrackManager.h"

#include "AEAudioUtility.h"
#include "AEDataStream.h"
#include "AEMFDecoder.h"
#include "AEWaveDecoder.h"
#include "AEWMADecoder.h"
#include "AEVorbisDecoder.h"

#include "FileMgr.h"
#include "MenuManager.h"

CAEUserRadioTrackManager& AEUserRadioTrackManager = *(CAEUserRadioTrackManager*)0xB6B970;

static auto audioExtensionTypes = std::to_array<tAudioExtensionType>({
    {".ogg", AUDIO_FILE_TYPE_VORBIS},
    {".mp3", AUDIO_FILE_TYPE_QUICKTIME}, // replaced to AUDIO_FILE_TYPE_WMA if QuickTime is not installed
    {".wav", AUDIO_FILE_TYPE_WAV},
    {".wma", AUDIO_FILE_TYPE_WMA},
    {".wmv", AUDIO_FILE_TYPE_WMA},
    {".aac", AUDIO_FILE_TYPE_QUICKTIME},
    {".m4a", AUDIO_FILE_TYPE_QUICKTIME},
#ifdef USERTRACK_FLAC_SUPPORT
    {".flac", AUDIO_FILE_TYPE_FLAC},
#endif
});

// 0x4f35b0
bool CAEUserRadioTrackManager::Initialise() {
    m_bUserTracksLoaded = ReadUserTracks();
    m_bUserTracksLoadedCopy = m_bUserTracksLoaded;
    rng::fill(m_baDecodersSupported, 0);

    m_baDecodersSupported[AUDIO_FILE_TYPE_VORBIS] = true;
    m_baDecodersSupported[AUDIO_FILE_TYPE_WAV] = true;
    m_baDecodersSupported[AUDIO_FILE_TYPE_WMA] = CAEWMADecoder::InitLibrary();
    m_baDecodersSupported[AUDIO_FILE_TYPE_QUICKTIME] = CAEMFDecoder::InitLibrary();

    if (!m_baDecodersSupported[AUDIO_FILE_TYPE_QUICKTIME]) {
        // change MP3 decoder from QuickTime to WMA
        const auto typeMP3 = rng::find_if(audioExtensionTypes, [](const auto& type) {
            return std::string_view{type.extension} == ".mp3";
        });

        if (typeMP3 != audioExtensionTypes.end()) {
            typeMP3->type = AUDIO_FILE_TYPE_WMA;
            NOTSA_LOG_DEBUG("Assigned MP3 decoder to be WMA because MediaFoundation decoder failed to load.");
        }
    }
    return true;
}

// 0x4f2fb0
void CAEUserRadioTrackManager::Shutdown() {
    if (m_bUserTracksLoaded && m_pUserTracksInfo) {
        CMemoryMgr::Free(m_pUserTracksInfo);
        m_pUserTracksInfo = nullptr;
    }

    CAEWMADecoder::Shutdown();
    CAEMFDecoder::Shutdown();
}

// 0x4f3050
char* CAEUserRadioTrackManager::GetTrackPath(int32 trackID) const {
    if (trackID >= static_cast<int32>(m_nUserTracksCount))
        return nullptr;

    CFileMgr::SetDirMyDocuments();
    FILESTREAM ufiles = CFileMgr::OpenFile("sa-ufiles.dat", "rb");
    CFileMgr::SetDir("");

    if (ufiles == nullptr)
        return nullptr;

    tUserTracksInfo& targetOffset = m_pUserTracksInfo[trackID];
    char*            dest = new char[targetOffset.pathLength + 1];

    CFileMgr::Seek(ufiles, targetOffset.startUfilesPath, SEEK_SET);
    CFileMgr::Read(ufiles, dest, targetOffset.pathLength);
    CFileMgr::CloseFile(ufiles);

    // NULL-terminate the path
    dest[targetOffset.pathLength] = 0;
    return dest;
}

// 0x4f35f0
CAEStreamingDecoder* CAEUserRadioTrackManager::LoadUserTrack(int32 trackID) {
    if (!m_bUserTracksLoaded)
        return nullptr;

    if (trackID < 0 || trackID >= m_nUserTracksCount)
        return nullptr;

    tUserTracksInfo& targetOffset = m_pUserTracksInfo[trackID];
    if (!m_baDecodersSupported[targetOffset.fileType])
        return nullptr;

    char* filename = GetTrackPath(trackID);
    if (filename == nullptr)
        return nullptr;

    CAEStreamingDecoder* decoder = nullptr;

    // QuickTime decoder doesn't need the data stream
    if (targetOffset.fileType == AUDIO_FILE_TYPE_QUICKTIME)
        // MikuAuahDark: We've agreed to replace QuickTime with MediaFoundation
        decoder = new CAEMFDecoder(filename, trackID);
    else {
        CAEDataStream* dataStream = new CAEDataStream(trackID, filename, 0, 0, false);

        CFileMgr::SetDirMyDocuments();
        bool dataStreamInitialized = dataStream->Initialise();
        CFileMgr::SetDir("");

        if (!dataStreamInitialized) {
            delete dataStream;
            return nullptr;
        }

        switch (targetOffset.fileType) {
        case AUDIO_FILE_TYPE_UNKNOWN:
        case AUDIO_FILE_TYPE_VORBIS:
            decoder = new CAEVorbisDecoder(dataStream, true);
            break;
        case AUDIO_FILE_TYPE_WAV:
            decoder = new CAEWaveDecoder(dataStream);
            break;
        case AUDIO_FILE_TYPE_WMA:
            decoder = new CAEWMADecoder(dataStream);
            break;
        default:
            delete dataStream;
            return nullptr;
        }
    }

    return decoder;
}

// 0x4f2fd0
bool CAEUserRadioTrackManager::ReadUserTracks() {
    CFileMgr::SetDirMyDocuments();
    auto file = CFileMgr::OpenFile("sa-utrax.dat", "rb");
    CFileMgr::SetDir("");

    if (!file)
        return false;

    const auto size = CFileMgr::GetTotalSize(file);
    if (size == 0) {
        CFileMgr::CloseFile(file);
        return false;
    }

    // NOTSA: Don't leak memory in case of multiple calls.
    if (m_pUserTracksInfo) {
        CMemoryMgr::Free(m_pUserTracksInfo);
    }

    m_nUserTracksCount = size / sizeof(tUserTracksInfo);
    m_pUserTracksInfo = (tUserTracksInfo*)CMemoryMgr::Malloc(size);

    CFileMgr::Read(file, m_pUserTracksInfo, size);
    CFileMgr::CloseFile(file);

    return true;
}

// 0x4f4ba0
bool CAEUserRadioTrackManager::ScanUserTracks() {
    switch (m_nUserTracksScanState) {
    case USER_TRACK_SCAN_OFF:
        m_nUserTracksScanState = USER_TRACK_SCAN_IN_PROGRESS;
        m_hwndUserTracksScanThreadHandle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)WriteUserTracksThread, this, 0, &m_nUserTracksScanThreadID);

        [[fallthrough]];
    case USER_TRACK_SCAN_IN_PROGRESS:
        return false;
    case USER_TRACK_SCAN_COMPLETE:
    case USER_TRACK_SCAN_ERROR:
        return true;
    default:
        return false;
    }
}

// 0x4f32f0
void CAEUserRadioTrackManager::DeleteUserTracksInfo() {
    TerminateThread(m_hwndUserTracksScanThreadHandle, 0);
    CFileMgr::SetDirMyDocuments();
    fs::remove("sa-ufiles.dat");
    fs::remove("sa-utrax.dat");
    CFileMgr::SetDir("");

    m_nUserTracksScanState = USER_TRACK_SCAN_OFF;
}

// 0x4f3340
void CAEUserRadioTrackManager::SetUserTrackIndex(int32 index) {
    if (index != -1)
        FrontEndMenuManager.m_nUserTrackIndex = index;
}

// 0x4f3250
int32 CAEUserRadioTrackManager::SelectUserTrackIndex() const {
    if (m_nUserTracksCount > 0 && FrontEndMenuManager.m_nRadioMode >= 0) {
        switch (FrontEndMenuManager.m_nRadioMode) {
        case USER_TRACK_PLAY_RADIO:
        case USER_TRACK_PLAY_RANDOM: {
            if (m_nUserTracksCount == 1)
                return FrontEndMenuManager.m_nUserTrackIndex = 1;
            else {
                int32 index;

                do
                    index = static_cast<int32>(CAEAudioUtility::GetRandomNumberInRange(0, m_nUserTracksCount - 1));
                while (index == FrontEndMenuManager.m_nUserTrackIndex);

                return FrontEndMenuManager.m_nUserTrackIndex = index;
            }
        }
        case USER_TRACK_PLAY_SEQUENTAL: {
            return FrontEndMenuManager.m_nUserTrackIndex = (FrontEndMenuManager.m_nUserTrackIndex + 1) % m_nUserTracksCount;
        }
        }
    }

    return -1;
}

// 0x4f31f0
eAudioFileType CAEUserRadioTrackManager::GetAudioFileType(const char* filename) {
    const char* dotPosition = strrchr(filename, '.');

    if (dotPosition == nullptr)
        return AUDIO_FILE_TYPE_UNKNOWN;

    for (const tAudioExtensionType& audioExtType : audioExtensionTypes) {
        if (_stricmp(audioExtType.extension, dotPosition) == 0)
            return audioExtType.type;
    }

    return AUDIO_FILE_TYPE_UNKNOWN;
}

// 0x4f32c0
bool CAEUserRadioTrackManager::IsShortcut(const char* path) {
    const char* dot = strrchr(path, '.');
    return dot ? _stricmp(dot, ".lnk") == 0 : false;
}

// 0x4f3330
uint8 CAEUserRadioTrackManager::GetUserTrackPlayMode() {
    return (uint8)FrontEndMenuManager.m_nRadioMode;
}

// 0x4f4a20
DWORD __stdcall CAEUserRadioTrackManager::WriteUserTracksThread(CAEUserRadioTrackManager* self) {
    VERIFY(SUCCEEDED(CoInitialize(nullptr)));

    // Open sa-ufiles.dat
    CFileMgr::SetDirMyDocuments();

    if (const auto ufiles = CFileMgr::OpenFile("sa-ufiles.dat", "wb")) {
        size_t dummy{};
        std::vector<tUserTracksInfo> offsets;

        // Retrieve all user tracks info
        int32 amountOfTracks = self->WriteUserTracksFile(fs::path(CFileMgr::ms_dirName) / "User Tracks", dummy, ufiles, offsets, 0);
        CFileMgr::CloseFile(ufiles);

        // Open sa-utrax.dat containing the offsets
        if (const auto utrax = CFileMgr::OpenFile("sa-utrax.dat", "wb")) {
            if (amountOfTracks > 0)
                CFileMgr::Write(utrax, offsets.data(), amountOfTracks * sizeof(tUserTracksInfo));

            // SA: Game duplicates the entry if there is only one?
            if (amountOfTracks == 1)
                CFileMgr::Write(utrax, offsets.data(), sizeof(tUserTracksInfo));

            CFileMgr::CloseFile(utrax);

            FrontEndMenuManager.m_nUserTrackIndex = 0;
            self->m_nUserTracksScanState = USER_TRACK_SCAN_COMPLETE;

            CoUninitialize();
        } else {
            // NOTSA(MikuAuahDark): GTASA doesn't check if sa-utrax.dat fails to open
            self->m_nUserTracksScanState = USER_TRACK_SCAN_ERROR;
        }
    } else {
        self->m_nUserTracksScanState = USER_TRACK_SCAN_ERROR;
    }

    // Read user tracks back
    self->m_bUserTracksLoaded = self->ReadUserTracks();
    self->m_bUserTracksLoadedCopy = self->m_bUserTracksLoaded;

    CFileMgr::SetDir("");
    CoUninitialize();
    return 0;
}

// 0x4f4690
int32 CAEUserRadioTrackManager::WriteUserTracksFile(const char* dir, size_t& currentLength, auto file, std::vector<tUserTracksInfo>& offsets, int32 depth) {
    // todo: FIX_BUGS
    return WriteUserTracksFile(UTF8ToUnicode(dir), currentLength, file, offsets, depth);
}

int32 CAEUserRadioTrackManager::WriteUserTracksFile(const std::wstring& dir, size_t& currentLength, auto file, std::vector<tUserTracksInfo>& offsets, int32 depth) {
    // Limit folder scan to 16 folders deep
    if (depth >= 15 || !fs::exists(dir))
        return 0;

    int32 numTracks{};
    for (auto& entry : fs::directory_iterator(dir)) {
        if (fs::is_directory(entry)) {
            // NOTE: STD can also do recursive search, maybe use that?
            numTracks += WriteUserTracksFile(entry.path(), currentLength, file, offsets, depth + 1);
            continue;
        }

        auto path = entry.path();
#ifdef WIN32
        if (path.extension() == ".lnk")
            path = ResolveShortcut(path.wstring());
#endif

        // TODO: symlink
        if (!fs::is_regular_file(path))
            continue;

        // if shortcut was a directory.
        if (fs::is_directory(path)) {
            numTracks += WriteUserTracksFile(path, currentLength, file, offsets, depth + 1);
            continue;
        }

        // NOTSA: Use UTF-8 generic path (POSIX like) instead of Windows paths.
        const auto pathStr = UnicodeToUTF8(path.generic_wstring());
        const auto fileType = GetAudioFileType(pathStr.c_str());
        if (fileType != AUDIO_FILE_TYPE_UNKNOWN) {
            size_t pathLen = pathStr.length();
            CFileMgr::Write(file, pathStr.c_str(), pathLen);
            offsets.push_back({currentLength, pathLen, fileType});
            currentLength += pathLen;
            numTracks++;
        }
    }

    return numTracks;
}

// todo: FIX_BUGS
// 0x4f30f0
char* CAEUserRadioTrackManager::ResolveShortcut(const char* path) {
    std::string str = UnicodeToUTF8(ResolveShortcut(UTF8ToUnicode(path)));
    char*       result = new char[str.length() + 1];

    memcpy(result, str.c_str(), str.length() + 1);
    return result;
}

std::wstring CAEUserRadioTrackManager::ResolveShortcut(const std::wstring& path) {
    IShellLinkW* shellLink{};
    IPersistFile* persistFile{};

    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&shellLink))) {
        NOTSA_UNREACHABLE("CoCreateInstance(CLSID_ShellLink) failed");
    }

    if (FAILED(shellLink->QueryInterface(IID_IPersistFile, (void**)&persistFile))) {
        NOTSA_LOG_ERR("QueryInterface(IID_IPersistFile) failed.");
        shellLink->Release();
        return {};
    }

    std::wstring target(MAX_PATH, L'\0');
    WIN32_FIND_DATAW findData{};
    if (FAILED(persistFile->Load(path.c_str(), STGM_READ)) || FAILED(shellLink->GetPath(target.data(), MAX_PATH, &findData, 0))) {
        persistFile->Release();
        shellLink->Release();
        NOTSA_UNREACHABLE("Load or GetPath failed");
    }

    persistFile->Release();
    shellLink->Release();

    return target;
}

void CAEUserRadioTrackManager::InjectHooks() {
    RH_ScopedClass(CAEUserRadioTrackManager);
    RH_ScopedCategory("Audio/Managers");

    RH_ScopedInstall(Initialise, 0x4f35b0);
    RH_ScopedInstall(Shutdown, 0x4f2fb0);
    RH_ScopedInstall(ReadUserTracks, 0x4f2fd0);
    RH_ScopedInstall(GetTrackPath, 0x4f3050);
    RH_ScopedOverloadedInstall(ResolveShortcut, "", 0x4f30f0, char* (CAEUserRadioTrackManager::*)(const char*));
    RH_ScopedInstall(GetAudioFileType, 0x4f31f0);
    RH_ScopedInstall(SelectUserTrackIndex, 0x4f3250);
    RH_ScopedInstall(DeleteUserTracksInfo, 0x4f32f0);
    RH_ScopedInstall(IsShortcut, 0x4f32c0);
    RH_ScopedInstall(LoadUserTrack, 0x4f35f0);
    RH_ScopedInstall(GetUserTrackPlayMode, 0x4f3330);
    RH_ScopedInstall(SetUserTrackIndex, 0x4f3340);
    RH_ScopedOverloadedInstall(WriteUserTracksFile, "", 0x4f4690, int32(CAEUserRadioTrackManager::*)(const char*, size_t&, FILE*, std::vector<tUserTracksInfo>&, int32), {.locked=true});
    RH_ScopedInstall(WriteUserTracksThread, 0x4f4a20, {.locked=true});
    RH_ScopedInstall(ScanUserTracks, 0x4f4ba0);
}
