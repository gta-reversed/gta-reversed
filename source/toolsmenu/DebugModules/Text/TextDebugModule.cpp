#include "StdInc.h"

#include "TextDebugModule.h"

#include <imgui.h>

namespace TextDebugModule {

void ProcessImgui() {
    const auto flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody;
    if (ImGui::BeginTable("PoolsDebugModule", 2, flags)) {
        ImGui::TableSetupColumn("Key");
        ImGui::TableSetupColumn("Str");
        ImGui::TableHeadersRow();


        for (const auto& entry : TheText.GetKeys()) {
            ImGui::TableNextRow();
            ImGui::PushID(&entry);

            ImGui::TableNextColumn();
            ImGui::Text("%u", entry.hash);

            ImGui::TableNextColumn();
            ImGui::Text("%s", entry.string);

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

} // namespace TextDebugModule
