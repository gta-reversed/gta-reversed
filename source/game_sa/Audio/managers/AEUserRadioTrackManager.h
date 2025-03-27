#pragma once

#include <cstdio>

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// MF
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include "AEStreamingDecoder.h"

#include "eUserTrackPlayMode.h"
#include "eUserTrackScanState.h"
#include "tAudioExtensionType.h"
#include "tUserTracksInfo.h"

constexpr size_t MAX_SUPPORTED_DECODERS = 8;

class CAEUserRadioTrackManager {
public:
    tUserTracksInfo*    m_pUserTracksInfo;
    bool                m_bUserTracksLoaded;
    bool                m_bUserTracksLoadedCopy;
    uint16              m_nUserTracksCount;
    bool                m_baDecodersSupported[MAX_SUPPORTED_DECODERS]; // index with eAudioFileType
    DWORD               m_nUserTracksScanThreadID;
    HANDLE              m_hwndUserTracksScanThreadHandle;
    eUserTrackScanState m_nUserTracksScanState;

public:
    bool Initialise();
    void Shutdown();

    char*                GetTrackPath(int32 trackID) const;
    CAEStreamingDecoder* LoadUserTrack(int32 trackID);
    bool                 ReadUserTracks();
    bool                 ScanUserTracks();
    void                 DeleteUserTracksInfo();
    void                 SetUserTrackIndex(int32 index);
    int32                SelectUserTrackIndex() const;

    eAudioFileType       GetAudioFileType(const char* filename);
    bool                 IsShortcut(const char* path);
    uint8                GetUserTrackPlayMode();

private:
    static DWORD __stdcall WriteUserTracksThread(CAEUserRadioTrackManager* self);
    int32 WriteUserTracksFile(const char* dir, size_t& currentLength, auto file, std::vector<tUserTracksInfo>& offsets, int32 depth);
    char* ResolveShortcut(const char* path);

    // Private functions which aren't part of GTASA itself
private:
    friend void InjectHooksMain();
    static void InjectHooks();

    int32 WriteUserTracksFile(const std::wstring& dir, size_t& currentLength, auto file, std::vector<tUserTracksInfo>& offsets, int32 depth);

    static std::wstring ResolveShortcut(const std::wstring& path);
};

VALIDATE_SIZE(CAEUserRadioTrackManager, 0x1c);

extern CAEUserRadioTrackManager& AEUserRadioTrackManager;
