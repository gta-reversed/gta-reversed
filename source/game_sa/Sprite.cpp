#include "StdInc.h"
#include "Sprite.h"

static inline auto& nSpriteBufferIndex = StaticRef<int32>(0xC6A158);
static inline auto& s_XLUSpriteVertices = StaticRef<std::array<RwIm2DVertex, 4>>(0xC4B8E0);

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
    RH_ScopedInstall(RenderOneXLUSprite, 0x70D000, { .reversed = false });
    RH_ScopedInstall(RenderOneXLUSprite_Triangle, 0x70D320, { .reversed = false });
    RH_ScopedInstall(RenderOneXLUSprite_Rotate_Aspect, 0x70D490, { .reversed = false });
    RH_ScopedInstall(RenderOneXLUSprite2D, 0x70F540);
    RH_ScopedInstall(RenderBufferedOneXLUSprite, 0x70E4A0, { .reversed = false });
    RH_ScopedInstall(RenderBufferedOneXLUSprite_Rotate_Aspect, 0x70E780, { .reversed = false });
    RH_ScopedInstall(RenderBufferedOneXLUSprite_Rotate_Dimension, 0x70EAB0, { .reversed = false });
    RH_ScopedInstall(RenderBufferedOneXLUSprite_Rotate_2Colours, 0x70EDE0, { .reversed = false });
    RH_ScopedInstall(RenderBufferedOneXLUSprite2D, 0x70F440, { .reversed = false });
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
    plugin::Call<0x70D000>(pos, halfSize, r, g, b, intensity, rhw, a, udir, vdir);
}

// 0x70D320
void CSprite::RenderOneXLUSprite_Triangle(float, float, float, float, float, float, float, uint8, uint8, uint8, int16, float, uint8) {
    assert(false);
}

// 0x70D490
void CSprite::RenderOneXLUSprite_Rotate_Aspect(CVector pos, CVector2D size, uint8 r, uint8 g, uint8 b, int16 intensity, float rz, float rotation, uint8 alpha) {
    plugin::Call<0x70D490>(pos, size, r, g, b, intensity, rz, rotation, alpha);
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

    Set4Vertices2D(
        s_XLUSpriteVertices.data(),
        { screen.x - size.x, screen.y - size.y, screen.x + size.x, screen.y + size.y },
        vertsColor,
        vertsColor,
        vertsColor,
        vertsColor
    );

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(false));
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, s_XLUSpriteVertices.data(), 4);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(true));
}

// unused
// 0x70F760
void CSprite::RenderOneXLUSprite2D_Rotate_Dimension(float, float, float, float, const RwRGBA&, int16, float, uint8) {
    assert(false);
}

/* --- Buffered XLU Sprite --- */

// 0x70E4A0
void CSprite::RenderBufferedOneXLUSprite(CVector pos, CVector2D size, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, uint8 a11) {
    plugin::Call<0x70E4A0>(pos, size, r, g, b, intensity, recipNearZ, a11);
}

// 0x70E780
void CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect(float x, float y, float z, float w, float h, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, float angle, uint8 a12) {
    plugin::Call<0x70E780, float, float, float, float, float, uint8, uint8, uint8, int16, float, float, uint8>(x, y, z, w, h, r, g, b, intensity, recipNearZ, angle, a12);
}

void CSprite::RenderBufferedOneXLUSprite_Rotate_Dimension(CVector pos, CVector2D size, uint8 r, uint8 g, uint8 b, int16 intensity, float rz, float rotation, uint8 a) {
    plugin::Call<0x70EAB0>(pos, size, r, g, b, intensity, rz, rotation, a);
}

// 0x70EDE0
void CSprite::RenderBufferedOneXLUSprite_Rotate_2Colours(float, float, float, float, float, uint8, uint8, uint8, uint8, uint8, uint8, float, float, float, float, uint8) {
    assert(false);
}

// 0x70F440
void CSprite::RenderBufferedOneXLUSprite2D(CVector2D pos, CVector2D size, const RwRGBA& color, int16 intensity, uint8 alpha) {
    plugin::Call<0x70F440>(pos, size, &color, intensity, alpha);
}

// unused
// 0x70F600
void CSprite::RenderBufferedOneXLUSprite2D_Rotate_Dimension(float, float, float, float, const RwRGBA&, int16, float, uint8) {
    assert(false);
}
