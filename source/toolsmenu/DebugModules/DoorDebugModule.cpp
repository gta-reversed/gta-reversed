#include <StdInc.h>
#include "DoorDebugModule.hpp"
#include "Door.h"

namespace notsa {
namespace debugmodules {

namespace {
inline std::optional<const char*> EnumToString(eDoorAxis v) {
    using namespace std::string_view_literals;
    switch (v) {
    case DOOR_AXIS_X:     return "AXIS_X";
    case DOOR_AXIS_Y:     return "AXIS_Y";
    case DOOR_AXIS_Z:     return "AXIS_Z";
    case DOOR_AXIS_NEG_X: return "AXIS_NEG_X";
    case DOOR_AXIS_NEG_Y: return "AXIS_NEG_Y";
    case DOOR_AXIS_NEG_Z: return "AXIS_NEG_Z";
    default:              return std::nullopt;
    }
}

inline std::optional<const char*> EnumToString(eDoorState v) {
    using namespace std::string_view_literals;
    switch (v) {
    case DOOR_NOTHING:     return "NOTHING";
    case DOOR_HIT_MAX_END: return "HIT_MAX_END";
    case DOOR_HIT_MIN_END: return "HIT_MIN_END";
    case DOOR_POP_OPEN:    return "POP_OPEN";
    case DOOR_SLAM_SHUT:   return "SLAM_SHUT";
    default:               return std::nullopt;
    }
}

void RenderDoorInfo(CDoor& door) {
    if (ImGui::CollapsingHeader("Angles", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Open Angle:    %.4f rad", door.m_openAngle);
        ImGui::Text("Closed Angle:  %.4f rad", door.m_closedAngle);
        ImGui::Text("Current Angle: %.4f rad", door.m_angle);
        ImGui::Text("Prev Angle:    %.4f rad", door.m_prevAngle);
        ImGui::Text("Open Ratio:    %.4f", door.GetAngleOpenRatio());

        ImGui::Spacing();

        float openAngle = door.m_openAngle;
        if (ImGui::SliderFloat("Set Open Angle", &openAngle, -3.14f, 3.14f)) {
            door.m_openAngle = openAngle;
        }

        float closedAngle = door.m_closedAngle;
        if (ImGui::SliderFloat("Set Closed Angle", &closedAngle, -3.14f, 3.14f)) {
            door.m_closedAngle = closedAngle;
        }
    }

    if (ImGui::CollapsingHeader("State", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Door State: %s (%d)", EnumToString(door.m_doorState), static_cast<int>(door.m_doorState));
        ImGui::Text("Axis:       %s (%d)", EnumToString(door.m_axis), static_cast<int>(door.m_axis));
        ImGui::Text("Is Closed:  %s", door.IsClosed() ? "Yes" : "No");
        ImGui::Text("Is Fully Open: %s", door.IsFullyOpen() ? "Yes" : "No");
    }

    if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Angular Velocity: %.4f", door.m_angVel);

        float angVel = door.m_angVel;
        if (ImGui::SliderFloat("Set Ang Velocity", &angVel, -CDoor::DOOR_SPEED_MAX_CAPPED, CDoor::DOOR_SPEED_MAX_CAPPED)) {
            door.m_angVel = angVel;
        }

        ImGui::Text("Max Capped Speed: %.4f", CDoor::DOOR_SPEED_MAX_CAPPED);
    }

    if (ImGui::CollapsingHeader("Flags")) {
        const uint16 dirn = door.m_dirn;
        ImGui::Text("m_dirn raw: 0x%04X", dirn);
        ImGui::Text("Direction (axis):  %s", EnumToString(static_cast<eDoorAxis>(dirn & DOOR_EXTRA_DIRN_MASK)));
        ImGui::Text("BASED:      %s", (dirn & DOOR_EXTRA_BASED) ? "Yes" : "No");
        ImGui::Text("LOW_GRAVITY:%s", (dirn & DOOR_EXTRA_LOW_GRAVITY) ? "Yes" : "No");
        ImGui::Text("CHASSIS:    %s", (dirn & DOOR_EXTRA_CHASSIS) ? "Yes" : "No");
        ImGui::Text("FIXEDSTATE: %s", (dirn & DOOR_EXTRA_FIXEDSTATE) ? "Yes" : "No");
        ImGui::Text("FIRETRUCK:  %s", (dirn & DOOR_EXTRA_FIRETRUCK) ? "Yes" : "No");
    }

    if (ImGui::CollapsingHeader("Actions")) {
        if (ImGui::Button("Open Fully")) {
            door.OpenFully();
        }
        ImGui::SameLine();
        if (ImGui::Button("Close Fully")) {
            door.CloseFully();
        }
        ImGui::SameLine();
        if (ImGui::Button("Open 50%%")) {
            door.Open(0.5f);
        }
    }
}
} // anonymous namespace

void DoorDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Door", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    auto vehicle = FindPlayerVehicle(-1, false);
    if (!vehicle) {
        ImGui::TextDisabled("Player is not in a vehicle.");
        return;
    }

    static const char* s_DoorNames[] = {
        "Bonnet (Hood)", "Boot (Trunk)", "Left Front", "Right Front", "Left Rear", "Right Rear"
    };

    static int32 s_SelectedDoor = 0;
    ImGui::SetNextItemWidth(180.f);
    ImGui::Combo("Door", &s_SelectedDoor, s_DoorNames, std::size(s_DoorNames));

    ImGui::Separator();

    if (vehicle->IsAutomobile()) {
        auto& door = vehicle->AsAutomobile()->m_doors[s_SelectedDoor];
        RenderDoorInfo(door);
    } else {
        ImGui::TextDisabled("Selected vehicle has no automobile doors.");
    }
}

void DoorDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Door", nullptr, &m_IsOpen);
    });
}

}; // namespace debugmodules
}; // namespace notsa
