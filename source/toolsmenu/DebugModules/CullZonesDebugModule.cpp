#include "StdInc.h"

#include "CullZonesDebugModule.h"
#include "Lines.h"

void CullZonesDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Cull Zones", {}, m_IsOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize };
    if (!m_IsOpen) {
        return;
    }

    ImGui::Checkbox("Draw Mirrors", &m_DrawMirrors);
    ImGui::Checkbox("Draw Tunnels", &m_DrawTunnels);
    ImGui::Checkbox("Draw Culls",   &m_DrawCulls);

    ImGui::SliderFloat("Z Offset", &m_ZOffset, -100.0f, 100.0f);

    ImGui::Text(
        "Current Mirror: %d\n"
        "Current Tunnel: %d\n"
        "Current Cull:   %d\n",
        m_MirrorCurrentIdx,
        m_TunnelCurrentIdx,
        m_CullCurrentIdx
    );
}

void CullZonesDebugModule::Render3D() {
    const auto RenderLine = [](int32 x1, int32 y1, float z1, int32 x2, int32 y2, float z2, bool isActive) {
        const auto color = isActive ? 0x00FF00FF : 0xFF0000FF;
        CLines::RenderLineNoClipping((float)x1, (float)y1, z1, (float)x2, (float)y2, z2, color, color);
    };

    auto& camPos = TheCamera.GetPosition();
    const auto DrawZone = [&](const char* name, const auto& arr, const auto* zone, bool isActive) {
        UNUSED(name);

        const auto& zdef = zone->zoneDef;
        int32 x1 = zdef.cornerX, y1 = zdef.cornerY, ly = zdef.vec1Y, lx = zdef.vec2X;
        float minZ = zdef.minZ + m_ZOffset, maxZ = zdef.maxZ + m_ZOffset;

        RenderLine(x1 + lx, y1,      maxZ, x1,      y1,      maxZ, isActive);
        RenderLine(x1,      y1,      maxZ, x1,      y1 + ly, maxZ, isActive);
        RenderLine(x1 + lx, y1,      maxZ, x1 + lx, y1 + ly, maxZ, isActive);
        RenderLine(x1 + lx, y1 + ly, maxZ, x1,      y1 + ly, maxZ, isActive);
        RenderLine(x1 + lx, y1,      maxZ, x1 + lx, y1,      minZ, isActive);
        RenderLine(x1,      y1,      maxZ, x1,      y1,      minZ, isActive);
        RenderLine(x1,      y1 + ly, maxZ, x1,      y1 + ly, minZ, isActive);
        RenderLine(x1 + lx, y1 + ly, maxZ, x1 + lx, y1 + ly, minZ, isActive);
        RenderLine(x1,      y1,      minZ, x1,      y1 + ly, minZ, isActive);
        RenderLine(x1 + lx, y1,      minZ, x1 + lx, y1 + ly, minZ, isActive);
        RenderLine(x1 + lx, y1,      minZ, x1,      y1,      minZ, isActive);
        RenderLine(x1 + lx, y1 + ly, minZ, x1,      y1 + ly, minZ, isActive);
    };

    m_MirrorCurrentIdx = m_TunnelCurrentIdx = m_CullCurrentIdx = -1;

    const auto ProcessCull = [&](
        bool        render,
        const char* name,
        auto&       culls,
        size_t      cullCount,
        auto&       outCurrCull,
        auto&       outCurrdIx
    ) {
        if (render) {
            for (auto&& [idx, cull] : rngv::enumerate(culls | rng::views::take(cullCount))) {
                if (cull.zoneDef.IsPointWithin(camPos)) {
                    outCurrCull = &cull;
                    outCurrdIx  = idx;
                }
                DrawZone(name, culls, &cull, outCurrdIx == idx);
            }
        }
    };

    ProcessCull(m_DrawMirrors, "Mirror",  CCullZones::aMirrorAttributeZones, (size_t)CCullZones::NumMirrorAttributeZones, m_MirrorCurrent, m_MirrorCurrentIdx);
    ProcessCull(m_DrawTunnels, "Tunnel",  CCullZones::aTunnelAttributeZones, (size_t)CCullZones::NumTunnelAttributeZones, m_TunnelCurrent, m_TunnelCurrentIdx);
    ProcessCull(m_DrawCulls,   "Default", CCullZones::aAttributeZones,       (size_t)CCullZones::NumAttributeZones,       m_CullCurrent,   m_CullCurrentIdx);
}

void CullZonesDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Visualization", "Culls" }, [&] {
        ImGui::Checkbox("Mirrors", &m_DrawMirrors);
        ImGui::Checkbox("Tunnels", &m_DrawTunnels);
        ImGui::Checkbox("Culls", &m_DrawCulls);

        ImGui::SliderFloat("Z Offset", &m_ZOffset, -100.0f, 100.0f);
    });

    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Culls", nullptr, &m_IsOpen);
    });
}
