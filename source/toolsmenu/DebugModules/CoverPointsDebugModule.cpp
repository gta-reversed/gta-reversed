#include <StdInc.h>
#include "CoverPointsDebugModule.hpp"
#include "CTeleportDebugModule.h"
#include "Lines.h"

namespace ig = ImGui;

constexpr static const char* s_CoverPointTypes[]{
    "NONE",
    "OBJECT",
    "VEHICLE",
    "POINTONMAP",
};

namespace notsa { 
namespace debugmodules {
void CoverPointsDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Cover Points", {700.f, 500.f}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    if (ig::BeginChild("Settings", { 0.f, 50.f }, ImGuiChildFlags_Border)) {
        ig::Checkbox("Bounding boxes for all", &m_AllBBsEnabled);
        ig::DragFloat("Range", &m_Range, 1.f, 10.f, 500.f, "%.2f");
    }
    ig::EndChild();

    if (ig::BeginChild("NearbyCoverPointsTable", {300.f, 0.f}, ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX)) {
        RenderCoverPointsTable();
    }
    ig::EndChild();

    ig::SameLine();

    if (ig::BeginChild("EntityDetails", {300.f, 0.f}, ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX)) {
        if (m_SelectedCpt) {
            RenderSelectedCoverPointDetails();
        }
    }
    ig::EndChild();
}

void CoverPointsDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ig::MenuItem("Cover Points", nullptr, &m_IsOpen);
    });
    notsa::ui::DoNestedMenuIL({ "Visualization", "Cover Points"}, [&] {
        ig::Checkbox("Bounding Boxes", &m_AllBBsEnabled);
    });
}

void CoverPointsDebugModule::Render3D() {
    if (m_AllBBsEnabled) {
        for (const auto& v : m_CptsInRange) {
            RenderCoverPointDetails3D(*v.CoverPoint);
        }
    } else if (m_SelectedCpt) {
        RenderCoverPointDetails3D(*m_SelectedCpt);
    }
}

void CoverPointsDebugModule::Update() {
    if (m_AllBBsEnabled || m_IsOpen) {
        UpdateCoverPointsInRange();
    }
}

void CoverPointsDebugModule::UpdateCoverPointsInRange() {
    m_CptsInRange.clear();
    m_CptsInRange.reserve(0x64);
    const auto plyrPos = FindPlayerCoors();
    for (auto& cpt : CCover::GetCoverPoints()) {
        if (!IsCoverPointValid(cpt)) {
            continue;
        }
        const auto dist = CVector::Dist(plyrPos, cpt.GetPos());
        if (dist > m_Range) {
            continue;
        }
        m_CptsInRange.emplace_back(dist, &cpt, m_CptsInRange.size());
    }
    if (m_SelectedCpt && !IsCoverPointValid(*m_SelectedCpt)) {
        m_SelectedCpt = nullptr;
    }
}

void CoverPointsDebugModule::RenderCoverPointsTable() {
    if (!ig::BeginTable(
        "CoverPoints",
        3,
        ImGuiTableFlags_Sortable
        | ImGuiTableFlags_Resizable
        | ImGuiTableFlags_Reorderable
        | ImGuiTableFlags_BordersInnerH
        | ImGuiTableFlags_ScrollY
        | ImGuiTableFlags_SizingFixedFit
        | ImGuiTableFlags_SortMulti
    )) {
        return;
    }

    ig::TableSetupColumn("#", ImGuiTableColumnFlags_NoResize, 20.f);
    ig::TableSetupColumn("Type", ImGuiTableColumnFlags_NoResize, 120.f);
    ig::TableSetupColumn("Distance", ImGuiTableColumnFlags_NoResize, 90.f);
    ig::TableHeadersRow();

    // Sort data for displaying
    const auto specs = ig::TableGetSortSpecs();
    rng::sort(m_CptsInRange, [&](const InRangeCoverPoint& a, const InRangeCoverPoint& b) {
        for (auto i = 0; i < specs->SpecsCount; i++) {
            const auto spec = &specs->Specs[i];
            std::partial_ordering o;
            switch (spec->ColumnIndex) {
            case 0: o = a.TblIdx <=> b.TblIdx;                               break; // #
            case 1: o = a.CoverPoint->GetType() <=> a.CoverPoint->GetType(); break; // Type
            case 2: o = a.DistToPlayer <=> b.DistToPlayer;                   break; // Distance
            }
            if (o != 0) {
                return spec->SortDirection == ImGuiSortDirection_Ascending
                    ? o < 0
                    : o > 0;
            }
        }
        return a.TblIdx > b.TblIdx;
    });

    // Render data now
    for (auto& v : m_CptsInRange) {
        ig::PushID(v.CoverPoint);
        ig::BeginGroup();

        // # + Selector
        if (!ig::TableNextColumn()) {
            break;
        }
        ig::Text("%u", v.TblIdx);
        ig::SameLine();
        if (ig::Selectable("##s", m_SelectedCpt == v.CoverPoint, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick)) {
            m_SelectedCpt = v.CoverPoint;
            if (ig::IsMouseDoubleClicked(0)) {
                TeleportDebugModule::TeleportTo(v.CoverPoint->GetPos(), eAreaCodes::AREA_CODE_NORMAL_WORLD);
            }
        }

        // Type
        ig::TableNextColumn();
        ig::Text("%s", s_CoverPointTypes[+v.CoverPoint->GetType()]);

        // Distance
        ig::TableNextColumn();
        ig::Text("%.2f", v.DistToPlayer);

        ig::EndGroup();
        ig::PopID();
    }

    ig::EndTable();
}

void CoverPointsDebugModule::RenderSelectedCoverPointDetails() {
    if (!m_SelectedCpt) {
        return;
    }
    ig::Text("%-15s %s", "Type:", s_CoverPointTypes[+m_SelectedCpt->GetType()]);
    ig::Text("%-15s %.3f", "Distance:", CVector::Dist(m_SelectedCpt->GetPos(), FindPlayerCoors()));

    const auto& dir = m_SelectedCpt->GetDirVector();
    ig::Text("%-15s {%2.f, %2.f, %2.f} (%.3f deg)", "Dir", dir.x, dir.y, dir.z, RadiansToDegrees((float)(m_SelectedCpt->GetDir())));
}

void CoverPointsDebugModule::RenderCoverPointDetails3D(const CCoverPoint& cpt) {
    constexpr float RADIUS = 0.25f;

    if (!IsCoverPointValid(cpt)) {
        return;
    }
    CVector pos = cpt.GetPos();
    pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y) + RADIUS;

    const auto color = &cpt == m_SelectedCpt
        ? lerp<CRGBA>({ 0xFF, 0x69, 0xB4, 0xFF }, { 0x0, 0xFF, 0x0, 0xFF }, std::abs(std::sin(TWO_PI * (float)(CTimer::GetTimeInMS() % 4096) / 4096.f))) // hot pink
        : CRGBA{ 0x0, 0xFF, 0x0, 0xFF };

    // Position BB
    CBox{
        pos - CVector{RADIUS},
        pos + CVector{RADIUS},
    }.DrawWireFrame(color);

    // Direction vector
    CLines::RenderLineNoClipping(
        pos,
        pos + cpt.GetDirVector() * 2.f,
        color.ToInt(),
        0xFFFFFFFF
    );
}
bool CoverPointsDebugModule::IsCoverPointValid(const CCoverPoint& cpt) {
    if (cpt.GetType() == CCoverPoint::eType::NONE) {
        return false;
    }
    return !cpt.CanBeRemoved();
}
}; // namespace debugmodules
}; // namespace notsa
