#include <StdInc.h>

#include "./GarageDebugModule.hpp"
#include "./CTeleportDebugModule.h"

#include "game_sa/Garage.h"
#include "game_sa/Garages.h"

namespace notsa::debugmodules {
void Garage::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Garage", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    const auto plyrCoors = FindPlayerCoors();
    const auto plyrVeh   = FindPlayerVehicle();
    const auto plyrPed   = FindPlayerPed();

    // Update distance to player
    for (auto& gi : m_GarageInfos) {
        gi.distSq = (CVector2D{ plyrCoors } - gi.g->GetCenter2D()).SquaredMagnitude();
        gi.dist   = std::sqrt(gi.distSq);
    }

    // Draw a table of all the garages now
    if (ImGui::BeginTable("Garages", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SortTristate | ImGuiTableFlags_SortMulti | ImGuiTableFlags_Sortable)) {
        ImGui::TableSetupColumn("Name",     ImGuiTableColumnFlags_DefaultSort, 0.f, GarageInfo::CNAME);
        ImGui::TableSetupColumn("Type",     0,                                 0.f, GarageInfo::CTYPE);
        ImGui::TableSetupColumn("Distance", 0,                                 0.f, GarageInfo::CDISTANCE);
        ImGui::TableHeadersRow();

        if (const auto tss = ImGui::TableGetSortSpecs(); tss && tss->SpecsDirty) {
            ::notsa::ui::RangeSortImGui(*tss, m_GarageInfos);
        }

        for (const GarageInfo& gi : m_GarageInfos) {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (ImGui::Selectable(gi.g->m_Name, m_Selected == gi.g, ImGuiSelectableFlags_SpanAllColumns)) {
                m_Selected = gi.g;
            }
            
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(EnumToString(gi.g->m_Type));

            ImGui::TableNextColumn();
            ImGui::Text("%.1f", gi.dist);

            if (m_Selected == gi.g && ImGui::IsMouseDoubleClicked(0)) {
                TeleportDebugModule::TeleportToGround(gi.g->GetCenter2D());
            }
        }

        ImGui::EndTable();
    }
}

void Garage::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Garage", nullptr, &m_IsOpen);
    });
}

void Garage::HandleEvent(const notsa::events::OnGameInit& e) {
    if (e.stage == notsa::events::OnGameInit::Stage::POST) {
        m_GarageInfos.clear();
        m_GarageInfos.reserve(CGarages::NumGarages);
        rng::copy(CGarages::GetAll(), std::back_inserter(m_GarageInfos));
    }
}

std::partial_ordering Garage::GarageInfo::ImGuiTableOrdering(const ImGuiTableColumnSortSpecs& css, const GarageInfo& a, const GarageInfo& b) {
    switch (css.ColumnUserID) {
    case CNAME:     return a.name <=> b.name;
    case CTYPE:     return a.g->m_Type <=> b.g->m_Type;
    case CDISTANCE: return a.distSq <=> b.distSq;
    default:        NOTSA_UNREACHABLE();
    }
}
}; // namespace notsa::debugmodules
