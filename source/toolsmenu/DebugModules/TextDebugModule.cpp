#include "StdInc.h"

#include "TextDebugModule.h"
#include <imgui.h>

TextDebugModule::TextDebugModule() :
    DebugModuleSingleWindow{ "Text Debug", {400.f, 600.f} }
{
}

void TextDebugModule::RenderMainWindow() {
    if (m_Filter.Draw(m_FilterByKey ? "Key/Hash (Exact)" : "Text content (inc, -exc)")) {
        if (m_FilterByKey) {
            if (*m_Filter.InputBuf) {
                const auto n = std::strtoul(m_Filter.InputBuf, nullptr, 16);
                m_FilterKeyHash = n != 0
                    ? n
                    : CKeyGen::GetUppercaseKey(m_Filter.InputBuf);
            } else {
                m_FilterKeyHash = 0;
            }
        }
    }
    ImGui::SameLine();
    ImGui::Checkbox("Filter by key", &m_FilterByKey);

    if (!ImGui::BeginTable("TextDebugModule_Table", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody)) {
        return;
    }

    ImGui::TableSetupColumn("Table");
    ImGui::TableSetupColumn("Key");
    ImGui::TableSetupColumn("String");
    ImGui::TableHeadersRow();

    // TODO: This shit is slow, use `ImGuiListClipper`
    const auto WriteRow = [this](const auto& entry, const auto& table) {
        char text[1024]{ 0 };
        GxtCharToUTF8(text, entry.string);

        notsa::ui::ScopedID id{ entry.hash };

        if (m_FilterByKey) {
            if (m_FilterKeyHash && m_FilterKeyHash != entry.hash) {
                return true;
            }
        } else {
            if (!m_Filter.PassFilter(text)) {
                return true;
            }
        }

        ImGui::TableNextRow();

        if (!ImGui::TableNextColumn()) {
            return false;
        }
        ImGui::TextUnformatted(table);

        ImGui::TableNextColumn();
        ImGui::Text("%08X", entry.hash);

        ImGui::TableNextColumn();
        ImGui::TextUnformatted(text);

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            ImGui::SetClipboardText(text);
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
