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

    KVRow("Volume Offset",       "%f",   CAEPoliceScannerAudioEntity::s_fVolumeOffset);
    KVRow("Stopping Scanner",    "%d",   CAEPoliceScannerAudioEntity::s_bStoppingScanner);
    KVRow("Play Request",        "0x%p", CAEPoliceScannerAudioEntity::s_pSound);
    KVRow("Abort Playback Time", "%u",   CAEPoliceScannerAudioEntity::s_nAbortPlaybackTime);
    KVRow("Playback Start Time", "%u",   CAEPoliceScannerAudioEntity::s_nPlaybackStartTime);
    KVRow("Section Playing",     "%d",   CAEPoliceScannerAudioEntity::s_nSectionPlaying);
    KVRow("Current Slots",       "0x%p", CAEPoliceScannerAudioEntity::s_pCurrentSlots);

    ImGui::TableNextColumn();
    ImGui::Text("Slot State");
    ImGui::TableNextColumn();
    for (auto& v : CAEPoliceScannerAudioEntity::s_SlotState) {
        ImGui::Text("%d", v);
        ImGui::SameLine();
    }

    const auto PrintSlots = [](auto& slots) {
        for (auto& v : slots) {
            ImGui::Text("%d:%d, ", v.bankId, v.sfxId);
            ImGui::SameLine();
        }
    };

    ImGui::TableNextColumn();
    ImGui::Text("Scanner Slot First");
    ImGui::TableNextColumn();
    PrintSlots(CAEPoliceScannerAudioEntity::s_ScannerSlotFirst);

    ImGui::TableNextColumn();
    ImGui::Text("Scanner Slot Second");
    ImGui::TableNextColumn();
    PrintSlots(CAEPoliceScannerAudioEntity::s_ScannerSlotSecond);


    KVRow("PS Controlling",                "0x%p", CAEPoliceScannerAudioEntity::s_pPSControlling);
    KVRow("Scanner Playback State",        "%d",   CAEPoliceScannerAudioEntity::s_nScannerPlaybackState);
    KVRow("Scanner Disabled",              "%d",   CAEPoliceScannerAudioEntity::s_bScannerDisabled);
    KVRow("Next New Scanner DialogueTime", "%u",   CAEPoliceScannerAudioEntity::s_NextNewScannerDialogueTime);

    ImGui::EndTable();
}

void PoliceScannerAudioEntityDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra", "Audio", "Entities" }, [&] {
        ImGui::MenuItem("Police Scanner", nullptr, &m_IsOpen);
    });
}
