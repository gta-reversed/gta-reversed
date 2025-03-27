#include "StdInc.h"
#include "imgui.h"

#include "AEUserRadioTrackManager.h"
#include "UserRadioTrackDebugModule.h"

#ifdef USERTRACK_FLAC_SUPPORT
#include "FLAC/all.h"
#endif

using namespace ImGui;

constexpr const char* AudioTypeFileNames[] = {"Unknown", "Vorbis", "WAV", "WMA", "QuickTime (MediaFoundation)", "FLAC"};

void UserRadioTrackDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{"User Track Audio Manager", {860.f, 290.f}, m_IsOpen};
    if (!m_IsOpen) {
        return;
    }

    BeginGroup();
    Text("Supported decoders:\n");
    for (auto i = 1u; i < TOTAL_AUDIO_FILE_TYPE; i++) {
        Text("\t%s: %s\n", AudioTypeFileNames[i], AEUserRadioTrackManager.m_baDecodersSupported[i] ? "Supported" : "Unsupported");
    }

    Text("\nNum tracks loaded: %u\n", AEUserRadioTrackManager.m_nUserTracksCount);
    Text("User tracks:\n");
    const auto& trackSpan = std::span{AEUserRadioTrackManager.m_pUserTracksInfo, (size_t)AEUserRadioTrackManager.m_nUserTracksCount};
    for (auto&& [i, track] : notsa::enumerate(trackSpan)) {
        Text(
            "Track %u\n\tFormat: %s\n\tPath: '%s'",
            i,
            AudioTypeFileNames[track.fileType],
            fs::path{AEUserRadioTrackManager.GetTrackPath(i)}.filename().string().c_str()
        );
    }

#ifdef USERTRACK_FLAC_SUPPORT
    static bool flacDecoderInit = false;
    static bool runOnce = true;
    if (std::exchange(runOnce, false)) {
        auto* decoder = FLAC__stream_decoder_new();
        flacDecoderInit = decoder != nullptr;
    }
    Text("libFLAC initialize status: %s\n", flacDecoderInit ? "Successful" : "Failed");
#else
    Text("libFLAC is not supported.\n");
#endif

    EndGroup();
}

void UserRadioTrackDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({"Extra", "Audio"}, [&] { ImGui::MenuItem("User Track", nullptr, &m_IsOpen); });
}
