#include "StdInc.h"

#include "TextDebugModule.h"
#include <imgui.h>

TextDebugModule::TextDebugModule() :
    DebugModuleSingleWindow{ "Text Debug", {400.f, 600.f} }
{
}

void TextDebugModule::RenderMainWindow() {
    ImGui::InputText("Key/Hash (Exact)", m_KeyFilter, sizeof(m_KeyFilter));
    if (m_KeyFilter[0]) {
        const auto n = std::strtoul(m_KeyFilter, nullptr, 16);
        m_KeyFilterHash = n != 0
            ? n
            : CKeyGen::GetUppercaseKey(m_KeyFilter);
    } else {
        m_KeyFilterHash = 0;
    }

    if (!ImGui::BeginTable("TextDebugModule_Table", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody)) {
        return;
    }

    ImGui::TableSetupColumn("Table");
    ImGui::TableSetupColumn("Key");
    ImGui::TableSetupColumn("String");
    ImGui::TableHeadersRow();

    auto WriteRow = [&, i = 0](const auto& entry, const auto& table) mutable {
        if (i > 100) {
            return false;
        }
        if (!m_KeyFilterHash || m_KeyFilterHash == entry.hash)  {
            ImGui::TableNextRow();
            ImGui::PushID(i++);

            if (!ImGui::TableNextColumn()) {
                return false;
            }
            ImGui::TextUnformatted(table);

            ImGui::TableNextColumn();
            ImGui::Text("%08X", entry.hash);

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(GxtCharToUTF8(entry.string));

            ImGui::PopID();
        }
        return true;
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
