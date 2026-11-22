#include "StdInc.h"

#include "CutsceneTrackManagerDebugModule.h"

#include "AECutsceneTrackManager.h"

void CutsceneTrackManagerDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Custscene Track Manager", {400.f, 600.f}, m_IsOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize };
    if (!m_IsOpen) {
        return;
    }

    if (ImGui::BeginTable("CutsceneTrackManagerDebugModule", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders)) {
        const auto KVRow = [](const char* label, const char* vfmt, auto v) {
            ImGui::TableNextColumn();
            ImGui::Text(label);

            ImGui::TableNextColumn();
            ImGui::Text(vfmt, v);
        };

        KVRow("Play Request",      "%s", AECutsceneTrackManager.m_bPlayRequest ? "true" : "false");
        KVRow("Paused",            "%s", AECutsceneTrackManager.m_bPaused ? "true" : "false");
        KVRow("Audio Channel Id",  "%d", AECutsceneTrackManager.m_nAudioChannel);
        KVRow("Status",            "%d", AECutsceneTrackManager.m_nStatus);
        KVRow("Track Id",          "%d", AECutsceneTrackManager.m_nTrackId);
        KVRow("f10",               "%d", AECutsceneTrackManager._f10);
        KVRow("f14",               "%d", AECutsceneTrackManager._f14);
        KVRow("f18",               "%d", AECutsceneTrackManager._f18);
        KVRow("Is User Track",     "%d", AECutsceneTrackManager.m_bIsUserTrack);
        KVRow("pad",               "%d", AECutsceneTrackManager._pad);
        KVRow("Playing Track Id",  "%d", AECutsceneTrackManager.m_nPlayingTrackId);
        KVRow("Track Play Time",   "%d", AECutsceneTrackManager.m_nTrackPlayTime);
        KVRow("Track Length Ms",   "%d", AECutsceneTrackManager.m_nTrackLengthMs);

        ImGui::EndTable();
    }
}

void CutsceneTrackManagerDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra", "Audio" }, [&] {
        ImGui::MenuItem("Cutscene Track Manager", nullptr, &m_IsOpen);
    });
}
