#include "StdInc.h"
#include "Sprite.h"

static inline auto& nSpriteBufferIndex = StaticRef<int32>(0xC6A158);

// NOTSA
constexpr int32 TOTAL_BUFFERED_SPRITES = 384; // The game's sprite buffer fits exactly this many quads

// NOTSA
// Coordinates of the 4 sprite corners along one axis
using CornerCoords1D = std::array<float, 4>;

// NOTSA
// Standard UVs of a full-texture sprite quad
static constexpr CornerCoords1D s_SpriteUs{ 0.0f, 0.0f, 1.0f, 1.0f };
static constexpr CornerCoords1D s_SpriteVs{ 0.0f, 1.0f, 1.0f, 0.0f };

// NOTSA
// Perspective-corrected screen Z. FP op order matches the original binary - do not re-associate.
static float CalcSpriteScreenZ(float z, float nearZ, float farZ) {
    return (z - CDraw::ms_fNearClipZ)
        * (farZ - nearZ)
        * CDraw::ms_fFarClipZ
        / ((CDraw::ms_fFarClipZ - CDraw::ms_fNearClipZ) * z)
        + nearZ;
}

// NOTSA
// RGB premultiplied by the fixed-point intensity, packed as ARGB
static uint32 CalcSpriteColor(uint8 r, uint8 g, uint8 b, uint8 a, int16 intensity) {
    //  intensity is (probably) multiplied by 256, otherwise this bitshift wouldn't make sense
    return CRGBA{
        static_cast<uint8>((int32(r) * intensity) >> 8),
        static_cast<uint8>((int32(g) * intensity) >> 8),
        static_cast<uint8>((int32(b) * intensity) >> 8),
        a
    }.ToIntARGB();
}

// NOTSA
// Sprite is off-screen only when all 4 corners lie beyond the same screen edge
static bool IsSpriteOffScreen(const CornerCoords1D& xs, const CornerCoords1D& ys) {
    const auto maxX = SCREEN_WIDTH;
    const auto maxY = SCREEN_HEIGHT;
    return rng::all_of(xs, [](float v) { return v < 0.0f; })
        || rng::all_of(ys, [](float v) { return v < 0.0f; })
        || rng::all_of(xs, [maxX](float v) { return v > maxX; })
        || rng::all_of(ys, [maxY](float v) { return v > maxY; });
}

// NOTSA
// Per-corner coefficients of the 4 rotated corners in the "aspect" rotation style:
// corner.xs[i] = coeffs.xs[i] * w + pos.x; corner.ys[i] = coeffs.ys[i] * h + pos.y
struct SpriteQuadCoeffs {
    CornerCoords1D xs, ys;
};

// NOTSA
static SpriteQuadCoeffs CalcSpriteQuadCoeffs(float rotation) {
    const float fSin = std::sin(rotation);
    const float fCos = std::cos(rotation);
    return {
        .xs = { -fCos - fSin, fSin - fCos, fCos + fSin, fCos - fSin },
        .ys = { fSin - fCos, fCos + fSin, fCos - fSin, -fCos - fSin },
    };
}

// NOTSA
// Clamp the quad's corners to the screen, remapping UVs using the sprite's full size
static void ClampSpriteQuadToScreen(CornerCoords1D& xs, CornerCoords1D& ys, CornerCoords1D& us, CornerCoords1D& vs, CVector2D halfSize) {
    const auto maxX = SCREEN_WIDTH;
    const auto maxY = SCREEN_HEIGHT;
    for (auto&& [i, x] : rngv::enumerate(xs)) {
        if (x < 0.0f) {
            us[i] = x / halfSize.x * -0.5f;
            x = 0.0f;
        }
        if (x > maxX) {
            us[i] = 1.0f - (x - maxX) * 0.5f / halfSize.x;
            x = maxX;
        }
        if (ys[i] < 0.0f) {
            vs[i] = ys[i] / halfSize.y * -0.5f;
            ys[i] = 0.0f;
        }
        if (ys[i] > maxY) {
            vs[i] = 1.0f - (ys[i] - maxY) * 0.5f / halfSize.y;
            ys[i] = maxY;
        }
    }
}

// NOTSA
// Fill the 4 vertices of a sprite quad
static void SetSpriteVertices(RwIm2DVertex* vertices, const CornerCoords1D& xs, const CornerCoords1D& ys, const CornerCoords1D& us, const CornerCoords1D& vs, float z, float rhw, const std::array<uint32, 4>& colors) {
    for (auto&& [i, vertex] : rngv::enumerate(std::span{ vertices, 4u })) {
        vertex = {
            .x             = xs[i],
            .y             = ys[i],
            .z             = z,
            .rhw           = rhw,
            .emissiveColor = colors[i],
            .u             = us[i],
            .v             = vs[i],
        };
    }
}

// NOTSA
// Same, with a single color for all 4 vertices
static void SetSpriteVertices(RwIm2DVertex* vertices, const CornerCoords1D& xs, const CornerCoords1D& ys, const CornerCoords1D& us, const CornerCoords1D& vs, float z, float rhw, uint32 color) {
    SetSpriteVertices(vertices, xs, ys, us, vs, z, rhw, { color, color, color, color });
}

// NOTSA
// Same, with the standard full-texture UVs and a single color
static void SetSpriteVertices(RwIm2DVertex* vertices, const CornerCoords1D& xs, const CornerCoords1D& ys, float z, float rhw, uint32 color) {
    SetSpriteVertices(vertices, xs, ys, s_SpriteUs, s_SpriteVs, z, rhw, color);
}

// NOTSA
// Emit the quad's two triangles `{ 0, 1, 2, 3, 0, 2 }`, advance the buffer and flush it when full
static void PushSpriteQuad() {
    const auto base    = 4 * nSpriteBufferIndex;
    auto*      indices = &aTempBufferIndices[6 * nSpriteBufferIndex];
    indices[0] = base;
    indices[1] = base + 1;
    indices[2] = base + 2;
    indices[3] = base + 3;
    indices[4] = base;
    indices[5] = base + 2;
    if (++nSpriteBufferIndex >= TOTAL_BUFFERED_SPRITES) {
        CSprite::FlushSpriteBuffer();
    }
}

void CSprite::InjectHooks() {
    RH_ScopedClass(CSprite);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x70CE10);
    RH_ScopedInstall(InitSpriteBuffer, 0x70CFB0);
    RH_ScopedInstall(FlushSpriteBuffer, 0x70CF20);
    RH_ScopedInstall(CalcScreenCoors, 0x70CE30);
    RH_ScopedInstall(CalcHorizonCoors, 0x70E3E0);
    RH_ScopedOverloadedInstall(Set4Vertices2D, "CRect", 0x70E1C0, void (*)(RwIm2DVertex*, const CRect&, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&));
    // RH_ScopedOverloadedInstall(Set4Vertices2D, "1", 0x70E2D0, void (*)(RwD3D9Vertex*, float, float, float, float, float, float, float, float, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&));
    RH_ScopedInstall(RenderOneXLUSprite, 0x70D000);
    RH_ScopedInstall(RenderOneXLUSprite_Triangle, 0x70D320);
    RH_ScopedInstall(RenderOneXLUSprite_Rotate_Aspect, 0x70D490);
    RH_ScopedInstall(RenderOneXLUSprite2D, 0x70F540);
    RH_ScopedInstall(RenderBufferedOneXLUSprite, 0x70E4A0);
    RH_ScopedInstall(RenderBufferedOneXLUSprite_Rotate_Aspect, 0x70E780);
    RH_ScopedInstall(RenderBufferedOneXLUSprite_Rotate_Dimension, 0x70EAB0);
    RH_ScopedInstall(RenderBufferedOneXLUSprite_Rotate_2Colours, 0x70EDE0);
    RH_ScopedInstall(RenderBufferedOneXLUSprite2D, 0x70F440);
}

// 0x70CE10
void CSprite::Initialise() {
    // NOP
}

// 0x70CFB0
void CSprite::InitSpriteBuffer() {
    m_f2DNearScreenZ = RwIm2DGetNearScreenZ();
    m_f2DFarScreenZ  = RwIm2DGetFarScreenZ();
}

// unused
// 0x70CFD0
void CSprite::InitSpriteBuffer2D() {
    m_fRecipNearClipPlane = 1.0f / RwCameraGetNearClipPlane(Scene.m_pRwCamera);
    InitSpriteBuffer();
}

// 0x70CF20
void CSprite::FlushSpriteBuffer() {
    if (nSpriteBufferIndex <= 0) {
        return;
    }

    if (m_bFlushSpriteBufferSwitchZTest) {
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
    }

    RwIm2DRenderIndexedPrimitive(
        rwPRIMTYPETRILIST,
        TempBufferVertices.m_2d,
        4 * nSpriteBufferIndex,
        aTempBufferIndices,
        6 * nSpriteBufferIndex
    );

    if (m_bFlushSpriteBufferSwitchZTest) {
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
    }

    nSpriteBufferIndex = 0;
}

// unused
// 0x70CE20
void CSprite::Draw3DSprite(float, float, float, float, float, float, float, float, float) {
    // NOP
}

// 0x70CE30
bool CSprite::CalcScreenCoors(const RwV3d& posn, RwV3d* out, float* w, float* h, bool checkMaxVisible, bool checkMinVisible) {
    *out = TheCamera.GetViewMatrix().TransformPoint(posn);

    if (out->z <= CDraw::GetNearClipZ() + 1.0f && checkMinVisible)
        return false;

    if (out->z >= CDraw::GetFarClipZ() && checkMaxVisible)
        return false;

    const float rd = 1.0f / out->z; // reciprocal of depth

    out->x = SCREEN_WIDTH * rd * out->x;
    out->y = SCREEN_HEIGHT * rd * out->y;

    *w = SCREEN_WIDTH  * rd / CDraw::GetFOV() * 70.0f;
    *h = SCREEN_HEIGHT * rd / CDraw::GetFOV() * 70.0f;

    return true;
}

// 0x70E3E0
float CSprite::CalcHorizonCoors() {
    const auto& cameraPosn = TheCamera.GetPosition();
    CVector point{
        cameraPosn.x + TheCamera.m_fCamFrontXNorm * 3000.0f,
        cameraPosn.y + TheCamera.m_fCamFrontYNorm * 3000.0f,
        0.0f,
    };

    const auto viewPoint = TheCamera.GetViewMatrix().TransformPoint(point);
    return 1.0f / viewPoint.z * SCREEN_HEIGHT * viewPoint.y;
}

// 0x70E1C0
void CSprite::Set4Vertices2D(RwIm2DVertex* verts, const CRect& rt, const CRGBA& topLeftColor, const CRGBA& topRightColor, const CRGBA& bottomLeftColor, const CRGBA& bottomRightColor) {
    for (auto i = 0u; i < 4u; i++) {
        auto& vert = verts[i];

        vert.x = (i == 0 || i == 3) ? rt.left : rt.right;
        vert.y = (i == 0 || i == 1) ? rt.bottom : rt.top;
        vert.z = m_f2DNearScreenZ;
        vert.u = (i == 0 || i == 3) ? 0.0f : 1.0f;
        vert.v = (i == 0 || i == 1) ? 0.0f : 1.0f;
        vert.rhw = m_fRecipNearClipPlane;

        vert.emissiveColor = [&] {
        switch (i) {
            case 0: return bottomLeftColor.ToIntARGB();
            case 1: return bottomRightColor.ToIntARGB();
            case 2: return topRightColor.ToIntARGB();
            case 3: return topLeftColor.ToIntARGB();
            default: NOTSA_UNREACHABLE();
        }
        }();
    }
}

// unused
// 0x70E2D0
void CSprite::Set4Vertices2D(RwD3D9Vertex*, float, float, float, float, float, float, float, float, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&) {
    assert(false);
}

/* --- XLU Sprite --- */

// 0x70D000
void CSprite::RenderOneXLUSprite(CVector pos, CVector2D halfSize, uint8 r, uint8 g, uint8 b, int16 intensity, float rhw, uint8 a, uint8 udir, uint8 vdir) {
    CornerCoords1D x{ pos.x - halfSize.x, pos.x - halfSize.x, pos.x + halfSize.x, pos.x + halfSize.x };
    CornerCoords1D y{ pos.y - halfSize.y, pos.y + halfSize.y, pos.y + halfSize.y, pos.y - halfSize.y };

    const float flipU = udir ? 1.0f : 0.0f;
    const float flipV = vdir ? 1.0f : 0.0f;
    CornerCoords1D u{ flipU, flipU, 1.0f - flipU, 1.0f - flipU };
    CornerCoords1D v{ flipV, 1.0f - flipV, 1.0f - flipV, flipV };

    ClampSpriteQuadToScreen(x, y, u, v, halfSize);

    std::array<RwIm2DVertex, 4> vertices;
    SetSpriteVertices(
        vertices.data(),
        x,
        y,
        u,
        v,
        CalcSpriteScreenZ(pos.z, RWSRCGLOBAL(dOpenDevice).zBufferNear, RWSRCGLOBAL(dOpenDevice).zBufferFar),
        rhw,
        CalcSpriteColor(r, g, b, a, intensity)
    );
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, vertices.data(), 4);
}

// 0x70D320
void CSprite::RenderOneXLUSprite_Triangle(CVector2D screen1, CVector2D screen2, CVector2D screen3, float screenZ, uint8 r, uint8 g, uint8 b, int16 intensity, float recipZ, uint8 alpha) {
    if (screenZ < 1.3f) {
        return;
    }
    const uint32 factor = static_cast<uint32>(std::min(255.0f * (screenZ - 1.3f), 255.0f));
    const uint32 R      = (factor * r) >> 8;
    const uint32 G      = (factor * g) >> 8;
    const uint32 B      = (factor * b) >> 8;
    const uint32 depthI = (factor * intensity) >> 8;

    const auto emissiveColor = CRGBA{
        static_cast<uint8>(((R & 0xff) * depthI) >> 8),
        static_cast<uint8>(((G & 0xff) * depthI) >> 8),
        static_cast<uint8>(((B & 0xff) * depthI) >> 8),
        alpha
    }.ToIntARGB();

    const auto z  = (RwIm2DGetFarScreenZ() - RwIm2DGetNearScreenZ())
        * (screenZ - CDraw::ms_fNearClipZ)
        * CDraw::ms_fFarClipZ
        / ((CDraw::ms_fFarClipZ - CDraw::ms_fNearClipZ) * screenZ)
        + RwIm2DGetNearScreenZ();

    std::array<RwIm2DVertex, 3> vertices;
    vertices[0] = {
        .x = screen1.x,
        .y = screen1.y,
        .z = z,
        .rhw = recipZ,
        .emissiveColor = emissiveColor
    };
    vertices[1] = {
        .x = screen2.x,
        .y = screen2.y,
        .z = z,
        .rhw = recipZ,
        .emissiveColor = emissiveColor
    };
    vertices[2] = {
        .x = screen3.x,
        .y = screen3.y,
        .z = z,
        .rhw = recipZ,
        .emissiveColor = emissiveColor
    };
    RwIm2DRenderPrimitive(rwPRIMTYPETRILIST, vertices.data(), 3);
}

// 0x70D490
void CSprite::RenderOneXLUSprite_Rotate_Aspect(CVector pos, CVector2D size, uint8 r, uint8 g, uint8 b, int16 intensity, float rz, float rotation, uint8 alpha) {
    if (pos.z < 1.3f) {
        return;
    }

    // Fade both the colour and intensity between z = 1.3 and z = 2.3.
    if (pos.z < 2.3f) {
        const auto factor = static_cast<int32>(255.0f * (pos.z - 1.3f));
        r = static_cast<uint8>((r * factor) >> 8);
        g = static_cast<uint8>((g * factor) >> 8);
        b = static_cast<uint8>((b * factor) >> 8);
        intensity = static_cast<int16>((intensity * factor) >> 8);
    }

    auto [xs, ys] = CalcSpriteQuadCoeffs(rotation);
    for (auto&& [cx, cy] : rngv::zip(xs, ys)) {
        cx = cx * size.x + pos.x;
        cy = cy * size.y + pos.y;
    }
    if (IsSpriteOffScreen(xs, ys)) {
        return;
    }

    std::array<RwIm2DVertex, 4> vertices;
    SetSpriteVertices(
        vertices.data(),
        xs,
        ys,
        CalcSpriteScreenZ(pos.z, RWSRCGLOBAL(dOpenDevice).zBufferNear, RWSRCGLOBAL(dOpenDevice).zBufferFar),
        rz,
        CalcSpriteColor(r, g, b, alpha, intensity)
    );
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, vertices.data(), 4);
}

// Android
void CSprite::RenderOneXLUSprite_Rotate_Dimension(float, float, float, float, float, uint8, uint8, uint8, int16, float, float, uint8) {
    assert(false);
}

// Android
void CSprite::RenderOneXLUSprite_Rotate_2Colours(float, float, float, float, float, uint8, uint8, uint8, uint8, uint8, uint8, float, float, float, float, uint8) {
    assert(false);
}

// 0x70F540
void CSprite::RenderOneXLUSprite2D(CVector2D screen, CVector2D size, const CRGBA& color, int16 intensity, uint8 alpha) {
    CRGBA vertsColor{};
    for (auto i = 0; i < 4; i++) {
        vertsColor[i] = static_cast<uint8>((intensity * color[i]) >> 8);
    }

    std::array<RwIm2DVertex, 4> vertices;
    Set4Vertices2D(
        vertices.data(),
        { screen.x - size.x, screen.y - size.y, screen.x + size.x, screen.y + size.y },
        vertsColor,
        vertsColor,
        vertsColor,
        vertsColor
    );

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(false));
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, vertices.data(), 4);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(true));
}

// unused
// 0x70F760
void CSprite::RenderOneXLUSprite2D_Rotate_Dimension(float, float, float, float, const RwRGBA&, int16, float, uint8) {
    assert(false);
}

/* --- Buffered XLU Sprite --- */

// 0x70E4A0
void CSprite::RenderBufferedOneXLUSprite(CVector pos, CVector2D size, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, uint8 alpha) {
    m_bFlushSpriteBufferSwitchZTest = false;

    CornerCoords1D x{ pos.x - size.x, pos.x - size.x, pos.x + size.x, pos.x + size.x };
    CornerCoords1D y{ pos.y - size.y, pos.y + size.y, pos.y + size.y, pos.y - size.y };
    auto u = s_SpriteUs;
    auto v = s_SpriteVs;
    ClampSpriteQuadToScreen(x, y, u, v, size);

    SetSpriteVertices(
        &TempBufferVertices.m_2d[4 * nSpriteBufferIndex],
        x,
        y,
        u,
        v,
        CalcSpriteScreenZ(pos.z, m_f2DNearScreenZ, m_f2DFarScreenZ),
        recipNearZ,
        CalcSpriteColor(r, g, b, alpha, intensity)
    );
    PushSpriteQuad();
}

// 0x70E780
void CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect(float x, float y, float z, float w, float h, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, float angle, uint8 alpha) {
    m_bFlushSpriteBufferSwitchZTest = false;

    auto [xs, ys] = CalcSpriteQuadCoeffs(angle);
    for (auto&& [cx, cy] : rngv::zip(xs, ys)) {
        cx = cx * w + x;
        cy = cy * h + y;
    }
    if (IsSpriteOffScreen(xs, ys)) {
        return;
    }

    SetSpriteVertices(
        &TempBufferVertices.m_2d[4 * nSpriteBufferIndex],
        xs,
        ys,
        CalcSpriteScreenZ(z, m_f2DNearScreenZ, m_f2DFarScreenZ),
        recipNearZ,
        CalcSpriteColor(r, g, b, alpha, intensity)
    );
    PushSpriteQuad();
}

// 0x70EAB0
void CSprite::RenderBufferedOneXLUSprite_Rotate_Dimension(CVector pos, CVector2D size, uint8 r, uint8 g, uint8 b, int16 intensity, float rz, float rotation, uint8 a) {
    m_bFlushSpriteBufferSwitchZTest = false;

    const float fSin = std::sin(rotation);
    const float fCos = std::cos(rotation);
    const float wCos = size.x * fCos;
    const float hSin = fSin * size.y;
    const float hCos = size.y * fCos;
    const float wSin = fSin * size.x;

    const CornerCoords1D xs{
        (pos.x - wCos) - hSin,
        (pos.x - wCos) + hSin,
        hSin + wCos + pos.x,
        (pos.x + wCos) - hSin,
    };
    const CornerCoords1D ys{
        (pos.y - hCos) + wSin,
        hCos + wSin + pos.y,
        (hCos + pos.y) - wSin,
        (pos.y - hCos) - wSin,
    };
    if (IsSpriteOffScreen(xs, ys)) {
        return;
    }

    SetSpriteVertices(
        &TempBufferVertices.m_2d[4 * nSpriteBufferIndex],
        xs,
        ys,
        CalcSpriteScreenZ(pos.z, m_f2DNearScreenZ, m_f2DFarScreenZ),
        rz,
        CalcSpriteColor(r, g, b, a, intensity)
    );
    PushSpriteQuad();
}

// 0x70EDE0
void CSprite::RenderBufferedOneXLUSprite_Rotate_2Colours(float x, float y, float z, float w, float h, uint8 r1, uint8 g1, uint8 b1, uint8 r2, uint8 g2, uint8 b2, float rotFactorX, float rotFactorY, float rz, float rotation, uint8 alpha) {
    m_bFlushSpriteBufferSwitchZTest = false;

    const auto coeffs = CalcSpriteQuadCoeffs(rotation);
    CornerCoords1D        xs, ys;
    std::array<uint32, 4> colors;
    for (auto&& [i, cx] : rngv::enumerate(coeffs.xs)) {
        xs[i] = cx * w + x;
        ys[i] = coeffs.ys[i] * h + y;

        const auto t = std::clamp((coeffs.ys[i] * rotFactorY + cx * rotFactorX + 1.0f) * 0.5f, 0.0f, 1.0f);
        colors[i] = CRGBA{
            static_cast<uint8>(r2 * (1.0f - t) + r1 * t),
            static_cast<uint8>(g2 * (1.0f - t) + g1 * t),
            static_cast<uint8>(b2 * (1.0f - t) + b1 * t),
            alpha
        }.ToIntARGB();
    }
    if (IsSpriteOffScreen(xs, ys)) {
        return;
    }

    SetSpriteVertices(
        &TempBufferVertices.m_2d[4 * nSpriteBufferIndex],
        xs,
        ys,
        s_SpriteUs,
        s_SpriteVs,
        CalcSpriteScreenZ(z, m_f2DNearScreenZ, m_f2DFarScreenZ),
        rz,
        colors
    );
    PushSpriteQuad();
}

// 0x70F440
void CSprite::RenderBufferedOneXLUSprite2D(CVector2D pos, CVector2D size, const RwRGBA& color, int16 intensity, uint8 alpha) {
    m_bFlushSpriteBufferSwitchZTest = true;
    const CRect rect(pos, size.x);
    const CRGBA scaledColor(
        (color.red * intensity) >> 8,
        (color.green * intensity) >> 8,
        (color.blue * intensity) >> 8,
        alpha
    );
    RwD3D9Vertex* vertices = &TempBufferVertices.m_2d[4 * nSpriteBufferIndex];
    Set4Vertices2D(
        vertices, rect, scaledColor, scaledColor, scaledColor, scaledColor
    );
    PushSpriteQuad();
}

// unused
// 0x70F600
void CSprite::RenderBufferedOneXLUSprite2D_Rotate_Dimension(float, float, float, float, const RwRGBA&, int16, float, uint8) {
    assert(false);
}
