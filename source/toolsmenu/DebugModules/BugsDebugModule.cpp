#include <StdInc.h>
#include "BugsDebugModule.hpp"
#include <reversiblebugfixes/Bugs.hpp>
#include <libs/imgui/misc/cpp/imgui_stdlib.h>

namespace notsa { 
namespace debugmodules {
void BugsDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "BugsDebugModule", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    NameFilter.Draw();

    if (!ImGui::BeginTable("Saved Positions", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit)) {
        return;
    }

    ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Requires Restart", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);

    ImGui::TableHeadersRow();

    for (auto bug = notsa::bugfixes::ReversibleBugFix::Tail; bug; bug = bug->Prev) {
        if (!NameFilter.PassFilter(bug->Name)) {
            continue;
        }

        ImGui::PushID(bug);
        ImGui::BeginGroup();

        // Enabled?
        if (!ImGui::TableNextColumn()) {
            break; // No more visible rows
        }
        {
            notsa::ui::ScopedDisable disable{bug->Locked};
            ImGui::Checkbox("##Enabled", &bug->Enabled);
        }

        // Requires restart?
        ImGui::TableNextColumn();
        {
            notsa::ui::ScopedDisable disable{true};
            ImGui::Checkbox("##RequiresRestart", &bug->RequiresRestart);
        }

        // Name
        ImGui::TableNextColumn();
        ImGui::Text(bug->Name);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", bug->Name);
        }

        // Description
        ImGui::TableNextColumn();
        ImGui::Text(bug->Description);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", bug->Description);
        }

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
