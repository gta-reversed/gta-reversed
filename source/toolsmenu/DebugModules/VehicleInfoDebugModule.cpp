#include "StdInc.h"
#include "VehicleInfoDebugModule.h"


void VehicleInfoDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{
        "Vehicle Info", {446.f, 512.f},
         m_IsOpen
    };
    if (!m_IsOpen) {
        return;
    }

    auto* veh = FindPlayerVehicle(0, true);

    if (!veh) {
        ImGui::Text("Player not in a vehicle");
        return;
    }

    if (veh->IsAutomobile()) {
        auto* automobile = veh->AsAutomobile();
        if (ImGui::BeginTable("Automobile Suspension", 5, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody)) {
            ImGui::TableSetupColumn("Wheel");
            ImGui::TableSetupColumn("Compression");
            ImGui::TableSetupColumn("SpringLength");
            ImGui::TableSetupColumn("LineLength");
            ImGui::TableHeadersRow();

            for (auto i = 0; i < 4; i++) {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text("%d", i);

                ImGui::TableNextColumn();
                ImGui::Text("%.2f", automobile->m_fWheelsSuspensionCompression[i]);

                ImGui::TableNextColumn();
                ImGui::Text("%.2f", automobile->m_aSuspensionSpringLength[i]);

                ImGui::TableNextColumn();
                ImGui::Text("%.2f", automobile->m_aSuspensionLineLength[i]);
            }

            ImGui::EndTable();
        }

        if (ImGui::BeginTable("Wheels", 5, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody)) {
            ImGui::TableSetupColumn("Wheel");
            ImGui::TableSetupColumn("Col X");
            ImGui::TableSetupColumn("Col Y");
            ImGui::TableSetupColumn("Col Z");
            ImGui::TableHeadersRow();

            for (auto i = 0; i < 4; i++) {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text("%d", i);

                auto& colPoint = automobile->m_wheelColPoint[i];
                ImGui::TableNextColumn();
                ImGui::Text("%.2f", colPoint.m_vecPoint.x);

                ImGui::TableNextColumn();
                ImGui::Text("%.2f", colPoint.m_vecPoint.y);

                ImGui::TableNextColumn();
                ImGui::Text("%.2f", colPoint.m_vecPoint.z);
            }

            ImGui::EndTable();
        }

        if (ImGui::BeginTable("Doors", 5, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody)) {
            ImGui::TableSetupColumn("Door");
            ImGui::TableSetupColumn("Angle");
            ImGui::TableSetupColumn("Open");
            ImGui::TableSetupColumn("Closed");
            ImGui::TableSetupColumn("Prev");
            ImGui::TableHeadersRow();

            for (auto i = 0; i < 6; i++) {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text("%d", i);

                auto& door = automobile->m_doors[i];
                ImGui::TableNextColumn();
                ImGui::Text("%.2f", door.m_fAngle);

                ImGui::TableNextColumn();
                ImGui::Text("%.2f", door.m_fOpenAngle);

                ImGui::TableNextColumn();
                ImGui::Text("%.2f", door.m_fClosedAngle);

                ImGui::TableNextColumn();
                ImGui::Text("%.2f", door.m_fPrevAngle);
            }

            ImGui::EndTable();
        }
    }
    {
        {
            const notsa::ui::ScopedDisable disabledScope{ true };
            //ImGui::NewLine();
        }
    }
}

void VehicleInfoDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Vehicle Info", nullptr, &m_IsOpen);
    });
}
