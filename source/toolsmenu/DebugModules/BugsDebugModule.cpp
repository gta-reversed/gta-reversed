#include <StdInc.h>
#include "BugsDebugModule.hpp"
#include <reversiblebugfixes/Bugs.hpp>
#include <imgui_stdlib.h>

namespace notsa { 
namespace debugmodules {
void BugsDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "BugsDebugModule", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    NameFilter.Draw();

    if (!ImGui::BeginTable("Saved Positions", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY)) {
        return;
    }

    ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);

    ImGui::TableHeadersRow();

    for (auto bug = notsa::bugfixes::ReversibleBugFix::Tail; bug; bug = bug->Prev) {
        if (!NameFilter.PassFilter(bug->Name.c_str())) {
            continue;
        }

        ImGui::PushID(bug);
        ImGui::BeginGroup();

        ImGui::TableNextRow();

        // Enabled
        ImGui::TableNextColumn();
        {
            notsa::ui::ScopedDisable disable{bug->Locked};
            ImGui::Checkbox("", &bug->Enabled);
        }

        // Name
        ImGui::TableNextColumn();
        ImGui::Text(bug->Name.c_str());

        // Description
        ImGui::TableNextColumn();
        ImGui::Text(bug->Description.c_str());

        ImGui::EndGroup();
        ImGui::PopID();
    }

    ImGui::EndTable();
}

void BugsDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Settings" }, [&] {
        ImGui::MenuItem("Bugs", nullptr, &m_IsOpen);
    });
}
}; // namespace debugmodules
}; // namespace notsa
