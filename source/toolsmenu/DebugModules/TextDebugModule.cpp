#include "StdInc.h"

#include "TextDebugModule.h"
#include <imgui.h>

TextDebugModule::TextDebugModule() :
    DebugModuleSingleWindow{ "Text Debug", {400.f, 600.f} }
{
}

void TextDebugModule::RenderMainWindow() {
    if (m_Filter.Draw()) {
        m_FilterTextHash = CKeyGen::GetUppercaseKey(m_Filter.InputBuf);
    }

    if (!ImGui::BeginTable("TextDebugModule_Table", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody)) {
        return;
    }

    ImGui::TableSetupColumn("Table");
    ImGui::TableSetupColumn("Key");
    ImGui::TableSetupColumn("String");
    ImGui::TableHeadersRow();

    const auto WriteRow = [this](const auto& entry, const auto& tabl) {
        if (!m_Filter.PassFilter(entry.string) && m_FilterTextHash != entry.hash) {
            return;
        }


        ImGui::TableNextRow();
        ImGui::PushID(&entry);

        if (!ImGui::TableNextColumn()) {
            return false;
        }
        ImGui::TextUnformatted(table);

        ImGui::TableNextColumn();
        ImGui::Text("%08X", entry.hash);

        ImGui::TableNextColumn();
        ImGui::TextUnformatted(GxtCharToUTF8(entry.string));

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            ImGui::SetClipboardText(entry.string);
        }

        ImGui::PopID();
    };
    
    for (const auto& entry : TheText.GetMissionKeys()) {
        if (!WriteRow(entry, TheText.GetMissionName())) {
            break;
        }
    }

    for (const auto& entry : TheText.GetKeys()) {
        if (!WriteRow(entry, "MAIN")) {
            break;
        }
    }

    ImGui::EndTable();
}

void TextDebugModule::RenderMenuEntry() {
    if (ImGui::BeginMenu("Extra")) {
        if (ImGui::MenuItem("The Text")) {
            SetMainWindowOpen(true);
        }
        ImGui::EndMenu();
    }
}
