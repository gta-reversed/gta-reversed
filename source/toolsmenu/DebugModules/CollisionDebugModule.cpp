// Based on https://github.com/Sergeanur/DrawColsSA
#include "StdInc.h"

#include <imgui.h>

#include "CollisionDebugModule.h"

#include "Lines.h"
#include "TaskSimpleClimb.h"
#include "extensions/utility.hpp"

constexpr auto BB_COLOR       = 0xFF0000FF; // red
constexpr auto BOX_COLOR      = 0xFFFFFFFF; // white
constexpr auto SPHERE_COLOR   = 0xFF00FFFF; // magenta
constexpr auto LINE_COLOR     = 0x00FFFFFF; // light blue
constexpr auto TRI_COLOR      = 0x00FF00FF; // green
constexpr auto SHDW_TRI_COLOR = 0xFFFFFFFF; // white

void CollisionDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Visualization", "Collision" }, [&] {
        ImGui::Checkbox("Enabled", &m_VisuEnabled);

        notsa::ui::ScopedDisable disabledScope{ !m_VisuEnabled };

        ImGui::Checkbox("Boxes",            &m_DrawBoxes);
        ImGui::Checkbox("Lines",            &m_DrawLines);
        ImGui::Checkbox("Spheres",          &m_DrawSpheres);
        ImGui::Checkbox("Triangles",        &m_DrawTris);
        ImGui::Checkbox("Bounding Boxes",   &m_DrawBBs);
        ImGui::Checkbox("Shadow Triangles", &m_DrawShdwTris);
    });

    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Collision", nullptr, &m_IsOpen);
    });
}

void CollisionDebugModule::Render3D() {
    if (m_VisuEnabled) {
        RenderVisibleColModels();
    }
}

void RenderShapeShapeCollisionStuff() {
    auto& states = CCollision::s_DebugSettings.ShapeShapeCollision.States;
    using Shape  = CCollision::DebugSettings::ShapeShapeCollision::Shape;
    using State  = CCollision::DebugSettings::ShapeShapeCollision::State;

    const auto BoolToSSState = [](bool b) {
        return b
            ? State::ENABLED
            : State::DISABLED;
    };

    constexpr const char* s_ShapeNames[]{
        "Box",
        "Sphere",
        "Triangle",
        "Line",
    };

    const auto SetSSState = [&](Shape a, Shape b, bool state) {
        states[a][b] = states[b][a] = BoolToSSState(state);
    };

    for (auto s = 0; s < Shape::SNUM; s++) {
        notsa::ui::ScopedID outerID{ s };

        const auto CountState = [&](State stcnt) {
            return rng::count_if(states[s], [&](State st) { return st == stcnt; });
        };

        // Count number of enabled and unsupported entires
        const auto nenabled = CountState(State::ENABLED);
        const auto nunsupp  = CountState(State::NOT_SUPPORTED);

        // Figure out cb state to use
        const auto cbTriState = nenabled == (Shape::SNUM - nunsupp)
            ? ImGui::ImTristate::ALL
            : nenabled != 0
                ? ImGui::ImTristate::MIXED
                : ImGui::ImTristate::NONE;

        // Draw tree node with checkbox
        const auto [treeNodeOpen, cbStateChanged, cbState] =
            notsa::ui::TreeNodeWithTriStateCheckBox(s_ShapeNames[s], cbTriState, false, ImGuiTreeNodeFlags_DefaultOpen);

        // If checkbox's state changed, reflect it
        if (cbStateChanged) {
            for (auto&& [si, ssState] : rngv::enumerate(states[s])) {
                if (ssState != State::NOT_SUPPORTED) {
                    SetSSState((Shape)s, (Shape)si, cbState);
                }
            }
        }

        if (!treeNodeOpen) {
            continue;
        }

        // Draw shape checkboxes
        for (auto&& [si, ssState] : rngv::enumerate(states[s])) {
            if (si != 0) {
                ImGui::SameLine();
            }

            notsa::ui::ScopedID innerID{ si };
            notsa::ui::ScopedDisable disabler{ ssState == State::NOT_SUPPORTED };

            bool cbState = ssState == State::ENABLED;
            if (ImGui::Checkbox(s_ShapeNames[si], &cbState)) {
                SetSSState((Shape)s, (Shape)si, cbState);
            }
        }

        ImGui::TreePop();
    }
}

void CollisionDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Collision", {600.f, 400.f}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    if (ImGui::TreeNode("Shape-Shape Collision")) {
        RenderShapeShapeCollisionStuff();
        ImGui::TreePop();
    }
}

void CollisionDebugModule::DrawColModel(const CMatrix& transform, const CColModel& cm) {
    const auto cd = cm.GetData();
    if (!cd) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

    if (m_DrawBoxes) {
        for (auto& box : cm.m_pColData->GetBoxes()) {
            box.DrawWireFrame({BB_COLOR}, transform);
        }
    }

    if (m_DrawLines) {
        for (auto& lines : cm.m_pColData->GetLines()) {
            lines.DrawWireFrame({ LINE_COLOR }, transform);
        }
    }

    if (m_DrawSpheres) {
        for (auto& spheres : cm.m_pColData->GetSpheres()) {
            spheres.DrawWireFrame({ SPHERE_COLOR }, transform );
        }
    }

    if (m_DrawTris) {
        for (const auto& triangle : cd->GetTris()) {
            triangle.DrawWireFrame({ TRI_COLOR }, cd->GetTriVerts(), transform);
        }
    }

    if (m_DrawBBs) {
        cm.m_boundBox.DrawWireFrame({ BB_COLOR }, transform);
    }

    if (cm.m_pColData->bHasShadowInfo) {
        if (m_DrawShdwTris) {
            for (const auto& triangle : cd->GetTris()) {
                triangle.DrawWireFrame({ SHDW_TRI_COLOR }, cd->m_pShadowVertices, transform);
            }
        }
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
}

void CollisionDebugModule::RenderVisibleColModels() {
    for (auto& entity : std::span{ CRenderer::ms_aVisibleEntityPtrs, (size_t)CRenderer::ms_nNoOfVisibleEntities }) {
        if (!entity || !entity->m_matrix)
            continue;

        const auto& mi = entity->GetModelInfo();
        if (!mi)
            continue;

        const auto& cm = mi->GetColModel();
        if (!cm)
            continue;

        DrawColModel(entity->GetMatrix(), *cm);
    }
}
