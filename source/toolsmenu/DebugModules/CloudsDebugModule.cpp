#include <StdInc.h>
#include "CloudsDebugModule.hpp"
#include <Clouds.h>

using namespace ImGui;

namespace notsa { 
namespace debugmodules {
void RenderDebugSettingPairs() {
    if (!BeginTable("Settings", 6, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY)) {
        return;
    }

    TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
    TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthFixed);
    TableSetupColumn("Force Render", ImGuiTableColumnFlags_WidthFixed);

    TableHeadersRow();

    const auto DoRenderSettingPair = [](const char* name, auto& pair) {
        const ui::ScopedID scopeID{ name };

        TableNextRow();
        BeginGroup();

        TableNextColumn();
        TextUnformatted(name);

        TableNextColumn();
        Checkbox("###Enabled", &pair.Enabled);

        TableNextColumn();
        Checkbox("###Force", &pair.Force);

        EndGroup();
    };

#define RenderSettingPair(_name) DoRenderSettingPair(#_name, CClouds::s_DebugSettings._name)
    RenderSettingPair(Moon);
    RenderSettingPair(Rockstar);
    RenderSettingPair(LowClouds);
    RenderSettingPair(Rainbow);
    RenderSettingPair(Streaks);
    RenderSettingPair(VolumetricClouds);
#undef RenderSettingPair

    EndTable();
}

void CloudsDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "CloudsDebugModule", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    if (TreeNodeEx("Render Settings")) {
        RenderDebugSettingPairs();
        TreePop();
    }
}

void CloudsDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Clouds", nullptr, &m_IsOpen);
    });
}
}; // namespace debugmodules
}; // namespace notsa
