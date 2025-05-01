#include "StdInc.h"

#include "AmbienceTrackManagerDebugModule.h"

#include <imgui.h>
#include "AEAmbienceTrackManager.h"

void AmbienceTrackManagerDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Ambience Track Manager", {400.f, 600.f}, m_IsOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize };
    if (!m_IsOpen) {
        return;
    }

    if (ImGui::BeginTable("AmbienceTrackManagerDebugModule", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders)) {
        const auto KVRow = [](const char* label, const char* vfmt, auto v) {
            ImGui::TableNextColumn();
            ImGui::Text(label);

            ImGui::TableNextColumn();
            ImGui::Text(vfmt, v);
        };

        KVRow("Stop",                             "%s", AEAmbienceTrackManager.m_OverrideRadio ? "true" : "false");
        KVRow("Stop Prev",                        "%s", AEAmbienceTrackManager.m_LastAmbienceOverrodeRadio ? "true" : "false");
        KVRow("StartAmbienceAtBeginning",         "%s", AEAmbienceTrackManager.m_StartAmbienceAtBeginning ? "true" : "false");
        KVRow("Is Ambience Radio Active",         "%d", AEAmbienceTrackManager.m_AmbienceRadioStation);
        KVRow("HwClientHandle",                   "%d", AEAmbienceTrackManager.m_HwClientHandle);
        KVRow("Special Mission Ambience Track",   "%d", AEAmbienceTrackManager.m_SpecialMissionAmbienceTrackID);
        KVRow("PrevAmbiencePlayTimeMs",           "%d", AEAmbienceTrackManager.m_PrevAmbiencePlayTimeMs);
        KVRow("PrevAmbienceStopTimeMs",           "%d", AEAmbienceTrackManager.m_PrevAmbienceStopTimeMs);
        KVRow("RequestedSettings.PlayingTrackID", "%d", AEAmbienceTrackManager.m_RequestedSettings.PlayingTrackID);
        KVRow("RequestedSettings.TrackFlags",     "%d", AEAmbienceTrackManager.m_RequestedSettings.TrackFlags);

        ImGui::EndTable();
    }

    ImGui::SliderFloat("Volume %f",    &AEAmbienceTrackManager.m_Volume, -100.f, 100.f, NULL, ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("Frequency %f", &AEAmbienceTrackManager.m_FreqFactor, 0.0f, 100.0f, NULL, ImGuiSliderFlags_Logarithmic);
    ImGui::InputInt("Track Id %d",     &AEAmbienceTrackManager.m_RequestedSettings.PlayingTrackID);
}

void AmbienceTrackManagerDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra", "Audio" }, [&] {
        ImGui::MenuItem("Ambience Track Manager", nullptr, &m_IsOpen);
    });
}
