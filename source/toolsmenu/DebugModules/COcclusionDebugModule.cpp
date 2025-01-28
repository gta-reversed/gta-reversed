#include "StdInc.h"

#include "Occlusion.h"
#include "Lines.h"

void COcclusionDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Occlusions", {}, m_IsOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize };
    if (!m_IsOpen) {
        return;
    }
    {
        const notsa::ui::ScopedDisable disabledScope{ true };

        ImGui::InputScalar("NumInteriorOcculdersOnMap", ImGuiDataType_S32, &COcclusion::NumInteriorOcculdersOnMap);
        ImGui::InputScalar("NumOccludersOnMap", ImGuiDataType_S32, &COcclusion::NumOccludersOnMap);
        ImGui::InputScalar("NumActiveOccluders", ImGuiDataType_S32, &COcclusion::NumActiveOccluders);

        ImGui::NewLine();

        ImGui::InputScalar("FarAwayList", ImGuiDataType_S16, &COcclusion::FarAwayList);
        ImGui::InputScalar("NearbyList", ImGuiDataType_S16, &COcclusion::NearbyList);
        ImGui::InputScalar("ListWalkThroughFA", ImGuiDataType_S16, &COcclusion::ListWalkThroughFA);
        ImGui::InputScalar("PreviousListWalkThroughFA", ImGuiDataType_S16, &COcclusion::PreviousListWalkThroughFA);
    }
    
    ImGui::NewLine();
    ImGui::Checkbox("Draw Active Occlusions", &m_DrawActiveOcclusions);
}

void COcclusionDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Visualization", "Occlusions" }, [&] {
        ImGui::Checkbox("Draw Active", &m_DrawActiveOcclusions);
    });

    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Occls", nullptr, &m_IsOpen);
    });
}

void COcclusionDebugModule::Render3D() {
    if (!m_DrawActiveOcclusions || !COcclusion::NumActiveOccluders) {
        return;
    }
    CSprite::FlushSpriteBuffer();
    const CRGBA white = { 255, 255, 255, 255 };
    for (auto& occl : COcclusion::GetActiveOccluders()) {
        for (auto& line : occl.GetLines()) {
            CLines::ImmediateLine2D(line.Origin, line.Origin + line.Delta * line.Length, white, white);
        }
    }
}
