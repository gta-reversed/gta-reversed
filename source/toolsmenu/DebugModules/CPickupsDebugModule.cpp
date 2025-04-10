#include "StdInc.h"

#include "CPickupsDebugModule.h"
#include "CTeleportDebugModule.h"

#include "imgui.h"
#include "Pickup.h"
#include "Pickups.h"
#include "extensions/enumerate.hpp"

using namespace ImGui;

constexpr auto GetPickupTypeName(ePickupType type) {
    switch (type) {
    case PICKUP_NONE:                     return "NONE";
    case PICKUP_IN_SHOP:                  return "IN_SHOP";
    case PICKUP_ON_STREET:                return "ON_STREET";
    case PICKUP_ONCE:                     return "ONCE";
    case PICKUP_ONCE_TIMEOUT:             return "ONCE_TIMEOUT";
    case PICKUP_ONCE_TIMEOUT_SLOW:        return "ONCE_TIMEOUT_SLOW";
    case PICKUP_COLLECTABLE1:             return "COLLECTABLE1";
    case PICKUP_IN_SHOP_OUT_OF_STOCK:     return "IN_SHOP_OUT_OF_STOCK";
    case PICKUP_MONEY:                    return "MONEY";
    case PICKUP_MINE_INACTIVE:            return "MINE_INACTIVE";
    case PICKUP_MINE_ARMED:               return "MINE_ARMED";
    case PICKUP_NAUTICAL_MINE_INACTIVE:   return "NAUTICAL_MINE_INACTIVE";
    case PICKUP_NAUTICAL_MINE_ARMED:      return "NAUTICAL_MINE_ARMED";
    case PICKUP_FLOATINGPACKAGE:          return "FLOATINGPACKAGE";
    case PICKUP_FLOATINGPACKAGE_FLOATING: return "FLOATINGPACKAGE_FLOATING";
    case PICKUP_ON_STREET_SLOW:           return "ON_STREET_SLOW";
    case PICKUP_ASSET_REVENUE:            return "ASSET_REVENUE";
    case PICKUP_PROPERTY_LOCKED:          return "PROPERTY_LOCKED";
    case PICKUP_PROPERTY_FORSALE:         return "PROPERTY_FORSALE";
    case PICKUP_MONEY_DOESNTDISAPPEAR:    return "MONEY_DOESNTDISAPPEAR";
    case PICKUP_SNAPSHOT:                 return "SNAPSHOT";
    case PICKUP_2P:                       return "2P";
    case PICKUP_ONCE_FOR_MISSION:         return "ONCE_FOR_MISSION";
    default:                              return "UNKNOWN";
    }
}

using namespace ImGui;

void CPickupsDebugModule::DrawTable() {
    if (!BeginTable("Pickups", 6, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY)) {
        return;
    }

    TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed);
    TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
    TableSetupColumn("Model Id", ImGuiTableColumnFlags_WidthFixed);
    TableSetupColumn("Visible", ImGuiTableColumnFlags_WidthFixed);
    TableSetupColumn("Revenue Value", ImGuiTableColumnFlags_WidthFixed);
    TableSetupColumn("Ammo", ImGuiTableColumnFlags_WidthFixed);
    TableHeadersRow();

    for (const auto& [i, pickup] : notsa::enumerate(CPickups::aPickUps)) {
        if (m_FilterInvisible && !pickup.IsVisible() || m_FilterInactive && pickup.m_nPickupType == PICKUP_NONE) {
            continue;
        }

        PushID(i);
        BeginGroup();
        TableNextRow();

        TableNextColumn();
        Text("%d", i);

        if (TableNextColumn(); Selectable(GetPickupTypeName(pickup.m_nPickupType), m_SelectedPickupIdx == i, ImGuiSelectableFlags_SpanAllColumns)) {
            m_SelectedPickupIdx = i;
        }

        // Teleport on double click
        if (IsItemHovered() && IsMouseDoubleClicked(0)) {
            TeleportDebugModule::TeleportTo(pickup.GetPosn(), FindPlayerPed()->m_nAreaCode);
        }

        TableNextColumn();
        Text("%d", pickup.m_nModelIndex);

        TableNextColumn();
        TextUnformatted(pickup.m_nFlags.bVisible ? "T" : "F");

        TableNextColumn();
        if (pickup.m_fRevenueValue != 0.0f) {
            Text("%.2f", pickup.m_fRevenueValue);
        }

        TableNextColumn();
        if (pickup.m_nAmmo != 0u) {
            Text("%u", pickup.m_nAmmo);
        }

        EndGroup();
        PopID();
    }
    EndTable();
}

void CPickupsDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Pickups", {400.f, 600.f}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    BeginGroup();

    if (m_SelectedPickupIdx != -1) {
        static bool markm_SelectedPickupIdx = true;
        Checkbox("Mark selected pickup", &markm_SelectedPickupIdx);

        const auto& pickup = CPickups::aPickUps[m_SelectedPickupIdx];

        Text("ID: %d", m_SelectedPickupIdx);

        const auto posn = pickup.GetPosn();
        Text("Coords: %.2f %.2f %.2f", posn.x, posn.y, posn.z);

        if (Button("Teleport Player")) {
            TeleportDebugModule::TeleportTo(posn);
        }

        if (markm_SelectedPickupIdx) {
            CVector screenCoors{};
            if (CalcScreenCoors(posn, screenCoors)) {
                CSprite2d::DrawRect({screenCoors, 5.0f}, pickup.m_nFlags.bDisabled ? CRGBA{255, 0, 0, 255} : CRGBA{0, 255, 0, 255});
            }
        }
    }

    Separator();
    Checkbox("Hide inactive (i.e. type=NONE)", &m_FilterInactive);
    SameLine();
    Checkbox("Hide invisible", &m_FilterInvisible);

    EndGroup();

    DrawTable();
}

void CPickupsDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Pickups", nullptr, &m_IsOpen);
    });
}
