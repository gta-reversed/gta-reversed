#include <StdInc.h>

#include "WeaponDebugModule.hpp"

namespace notsa { 
namespace debugmodules {
void WeaponDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "WeaponDebugModule", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    auto& ds = CWeapon::s_DebugSettings;

    ImGui::Checkbox("No Shoot Delay", &ds.NoShotDelay);
    ImGui::SetItemTooltip("Changes shoot delay for rocketlauncher, etc");
}

void WeaponDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Weapon", nullptr, &m_IsOpen);
    });
}
}; // namespace debugmodules
}; // namespace notsa
