#include "StdInc.h"

#include "FallingGlassPane.h"
#include "Glass.h"

void CFallingGlassPane::InjectHooks() {
    RH_ScopedClass(CFallingGlassPane);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Update, 0x71AA10);
    RH_ScopedInstall(Render, 0x71B100);
}

// 0x71AA10
void CFallingGlassPane::Update() {
    if (CTimer::GetTimeInMS() < m_nCreatedTime)
        return;

    auto& pos = m_Matrix.GetPosition();

    if (m_f6F) {
        pos += CTimer::GetTimeStep() * 0.35f * m_Velocity;
        m_Velocity.z -= CTimer::GetTimeStep() / 100.f;
    } else {
        pos += CTimer::GetTimeStep() * m_Velocity;
        m_Velocity.z -= CTimer::GetTimeStep() / 50.f;
    }

    m_Matrix.GetRight()   += CrossProduct(m_RandomNumbers, m_Matrix.GetRight());
    m_Matrix.GetForward() += CrossProduct(m_RandomNumbers, m_Matrix.GetForward());
    m_Matrix.GetUp()      += CrossProduct(m_RandomNumbers, m_Matrix.GetUp());

    if (pos.z < m_fGroundZ) {
        m_bExist = false;
        AudioEngine.ReportGlassCollisionEvent(AE_GLASS_HIT_GROUND, { pos.x, pos.y, m_fGroundZ });
        if (!m_f6F) {
            RwRGBA color{ 255, 255, 255, 32 };
            for (auto i = 0; i < 4; i++) {
                g_fx.AddGlass(pos, color, CGeneral::GetRandomNumberInRange(0.02f, 0.06f), 1);
            }
        }
    }
}

RwRGBA CFallingGlassPane::CalculateHiLightPolyColor() {
    const auto alpha       = static_cast<float>(CGlass::CalcAlphaWithNormal(Normalized(m_Matrix.GetForward())));
    const auto delta       = static_cast<float>(std::clamp(CTimer::GetTimeInMS() - m_nCreatedTime, 0u, 500u));
    const auto scaledAlpha = static_cast<uint32>(alpha * delta / 500.f);
    const auto color       = static_cast<uint8>(m_f6F ? std::max(64u, scaledAlpha) : scaledAlpha);
    return { color, color, color, color };
}

RwRGBA CFallingGlassPane::CalculateShatterPolyColor() {
    const auto camDist = (m_Matrix.GetPosition() - TheCamera.GetPosition()).Magnitude();
    if (camDist > 30.f) {
        const auto alpha = (uint8)((1.0f - (camDist - 30.f) / 10.f) * 140.f);
        return { alpha, alpha, alpha, alpha };
    }
    return { 140u, 140u, 140u, 140u};
}

// 0x71B100
void CFallingGlassPane::Render() {
    constexpr RwImVertexIndex indices[]{ 0u, 1u, 2u, 0u, 2u, 1u };

    // Render HiLightPoly's
    CVector vertices[3]{};
    {
        const auto color = CalculateHiLightPolyColor();
        constexpr RwTexCoords uv[3] = {
            { 0.5f, 0.5f },
            { 0.5f, 0.6f },
            { 0.6f, 0.6f },
        };
        for (auto i = 0u; i < 3u; i++) {
            auto& vertex = TempBufferVertices.m_3d[CGlass::H1iLightPolyVerticesIdx + i];

            const auto pos2DOS = CGlass::PanePolyPositions[m_nPieceIndex][i] - CGlass::PanePolyCenterPositions[m_nPieceIndex];
            vertices[i] = m_Matrix.TransformPoint({ pos2DOS.x, 0.f, pos2DOS.y });
            RxObjSpace3DVertexSetPos(&vertex, &vertices[i]);

            RxObjSpace3DVertexSetU(&vertex, uv[i].u);
            RxObjSpace3DVertexSetV(&vertex, uv[i].v);

            RxObjSpace3DVertexSetPreLitColor(&vertex, &color);
        }

        // Store indices
        uint32 i = CGlass::HiLightPolyIndicesIdx;
        for (const auto idx : indices) {
            aTempBufferIndices[i++] = (RwImVertexIndex)(CGlass::H1iLightPolyVerticesIdx + idx);
        }

        CGlass::H1iLightPolyVerticesIdx += 3;
        CGlass::HiLightPolyIndicesIdx += 6;
    }

    if (m_bRenderShatter) {
        const auto color = CalculateShatterPolyColor();
        for (auto i = 0u; i < 3u; i++) {
            auto vertex = &TempBufferVertices.m_3d[CGlass::ShatteredVerticesBaseIdx + i];

            RxObjSpace3DVertexSetPos(vertex, &vertices[i]);
            RxObjSpace3DVertexSetPreLitColor(vertex, &color);

            const auto uv = CGlass::PanePolyPositions[m_nPieceIndex][i] * m_fSize * 4.f;
            RxObjSpace3DVertexSetU(vertex, uv.x);
            RxObjSpace3DVertexSetV(vertex, uv.y);
        }

        uint32 i = CGlass::ShatteredVerticesBaseIdx;
        for (const auto idx : indices) {
            aTempBufferIndices[i++] = (RwImVertexIndex)(CGlass::ShatteredVerticesBaseIdx - 1024u - idx);
        }
    }
}
