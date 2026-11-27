#include <StdInc.h>

#include "PoliceScannerAudioEntityDebugModule.h"
#include "AEPoliceScannerAudioEntity.h"

void PoliceScannerAudioEntityDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Police Scanner Audio Entity", {400.f, 600.f}, m_IsOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize };
    if (!m_IsOpen) {
        return;
    }

    if (!ImGui::BeginTable("PoliceScannerAudioEntityDebugModule", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders)) {
        return;
    }

    const auto KVRow = [](const char* label, const char* vfmt, auto v) {
        ImGui::TableNextColumn();
        ImGui::Text(label);

        ImGui::TableNextColumn();
        ImGui::Text(vfmt, v);
    };

    KVRow("Volume Offset",       "%f",   CAEPoliceScannerAudioEntity::ms_VolumeOffset);
    KVRow("Stopping Scanner",    "%d",   CAEPoliceScannerAudioEntity::ms_IsStoppingScanner);
    KVRow("Play Request",        "0x%p", CAEPoliceScannerAudioEntity::ms_Sound);
    KVRow("Abort Playback Time", "%u",   CAEPoliceScannerAudioEntity::ms_AbortPlaybackTime);
    KVRow("Playback Start Time", "%u",   CAEPoliceScannerAudioEntity::ms_PlaybackStartTime);
    KVRow("Section Playing",     "%d",   CAEPoliceScannerAudioEntity::ms_SectionPlaying);
    KVRow("Current Slots",       "0x%p", CAEPoliceScannerAudioEntity::ms_CurrentSlots);

    ImGui::TableNextColumn();
    ImGui::Text("Slot State");
    ImGui::TableNextColumn();
    for (auto& v : CAEPoliceScannerAudioEntity::ms_SlotState) {
        ImGui::Text("%d", v);
        ImGui::SameLine();
    }

    const auto PrintSlots = [](auto& slots) {
        for (auto& v : slots) {
            ImGui::Text("%d:%d, ", (int32)(v.Bank), (int32)(v.SoundID));
            ImGui::SameLine();
        }
    };

    ImGui::TableNextColumn();
    ImGui::Text("Scanner Slot First");
    ImGui::TableNextColumn();
    PrintSlots(CAEPoliceScannerAudioEntity::ms_ScannerSlotFirst);

    ImGui::TableNextColumn();
    ImGui::Text("Scanner Slot Second");
    ImGui::TableNextColumn();
    PrintSlots(CAEPoliceScannerAudioEntity::ms_ScannerSlotSecond);


    KVRow("PS Controlling",                "0x%p", CAEPoliceScannerAudioEntity::ms_PSControlling);
    KVRow("Scanner Playback State",        "%d",   CAEPoliceScannerAudioEntity::ms_ScannerPlaybackState);
    KVRow("Scanner Disabled",              "%d",   CAEPoliceScannerAudioEntity::ms_IsScannerDisabled);
    KVRow("Next New Scanner DialogueTime", "%u",   CAEPoliceScannerAudioEntity::ms_NextNewScannerDialogueTime);

    ImGui::EndTable();
}

void PoliceScannerAudioEntityDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra", "Audio", "Entities" }, [&] {
        ImGui::MenuItem("Police Scanner", nullptr, &m_IsOpen);
    });
}
