#include "StdInc.h"

#include "Sprite.h"

float& CSprite::m_f2DNearScreenZ = *(float*)0xC4B8D8;
float& CSprite::m_f2DFarScreenZ = *(float*)0xC4B8D4;
float& CSprite::m_fRecipNearClipPlane = *(float*)0xC4B8D0;

void CSprite::InjectHooks() {
    RH_ScopedClass(CSprite);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x70CE10);
    RH_ScopedInstall(InitSpriteBuffer, 0x70CFB0);
    RH_ScopedInstall(FlushSpriteBuffer, 0x70CF20, { .reversed = false });
    RH_ScopedInstall(CalcScreenCoors, 0x70CE30);
    RH_ScopedInstall(CalcHorizonCoors, 0x70E3E0, { .reversed = false });
    // RH_ScopedOverloadedInstall(Set4Vertices2D, "0", 0x70E1C0, void (*)(RwD3D9Vertex*, const CRect&, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&));
    // RH_ScopedOverloadedInstall(Set4Vertices2D, "1", 0x70E2D0, void (*)(RwD3D9Vertex*, float, float, float, float, float, float, float, float, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&));
    RH_ScopedInstall(RenderOneXLUSprite, 0x70D000, { .reversed = false });
    RH_ScopedInstall(RenderOneXLUSprite_Triangle, 0x70D320, { .reversed = false });
    RH_ScopedInstall(RenderOneXLUSprite_Rotate_Aspect, 0x70D490, { .reversed = false });
    RH_ScopedInstall(RenderOneXLUSprite2D, 0x70F540, { .reversed = false });
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
    plugin::Call<0x70CF20>();
}

// unused
// 0x70CE20
void CSprite::Draw3DSprite(float, float, float, float, float, float, float, float, float) {
    // NOP
}

// 0x70CE30
bool CSprite::CalcScreenCoors(const RwV3d& posn, RwV3d* out, float* w, float* h, bool checkMaxVisible, bool checkMinVisible) {
    *out = MultiplyMatrixWithVector(TheCamera.GetViewMatrix(), posn);

    if (out->z <= CDraw::GetNearClipZ() + 1.0f && checkMinVisible)
        return false;

    if (out->z >= CDraw::GetFarClipZ() && checkMaxVisible)
        return false;

    const float recip = 1.0f / out->z;

    out->x = SCREEN_WIDTH * recip * out->x;
    out->y = SCREEN_HEIGHT * recip * out->y;

    *w = SCREEN_WIDTH  * recip / CDraw::GetFOV() * 70.0f;
    *h = SCREEN_HEIGHT * recip / CDraw::GetFOV() * 70.0f;

    return true;
}

// 0x70E3E0
float CSprite::CalcHorizonCoors() {
    return plugin::CallAndReturn<float, 0x70E3E0>();

    /*
    auto cameraPosn = TheCamera.GetPosition();
    CVector point{
        cameraPosn.x + TheCamera.m_fCamFrontXNorm * 3000.0f,
        cameraPosn.y + TheCamera.m_fCamFrontYNorm * 3000.0f,
        0.0f,
    };

    CVector outPoint;
    auto vecPos = MultiplyMatrixWithVector(&outPoint, &TheCamera.m_mViewMatrix, &point);
    return 1.0f / vecPos->z * SCREEN_HEIGHT * vecPos->y;
    */
}

// 0x70E1C0
void CSprite::Set4Vertices2D(RwD3D9Vertex*, const CRect&, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&) {
    assert(false);
}

// unused
// 0x70E2D0
void CSprite::Set4Vertices2D(RwD3D9Vertex*, float, float, float, float, float, float, float, float, const CRGBA&, const CRGBA&, const CRGBA&, const CRGBA&) {
    assert(false);
}

/* --- XLU Sprite --- */

// 0x70D000
void CSprite::RenderOneXLUSprite(float x, float y, float z, float halfWidth, float halfHeight, uint8 r, uint8 g, uint8 b, int16 a, float rhw, uint8 intensity, uint8 udir, uint8 vdir) {
    plugin::Call<0x70D000, float, float, float, float, float, uint8, uint8, uint8, int16, float, uint8, uint8, uint8>(x, y, z, halfWidth, halfHeight, r, g, b, a, rhw, intensity, udir, vdir);
}

// 0x70D320
void CSprite::RenderOneXLUSprite_Triangle(float, float, float, float, float, float, float, uint8, uint8, uint8, int16, float, uint8) {
    assert(false);
}

// 0x70D490
void CSprite::RenderOneXLUSprite_Rotate_Aspect(float, float, float, float, float, uint8, uint8, uint8, int16, float, float, uint8) {
    assert(false);
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
void CSprite::RenderOneXLUSprite2D(float, float, float, float, const RwRGBA&, int16, uint8) {
    assert(false);
}

// unused
// 0x70F760
void CSprite::RenderOneXLUSprite2D_Rotate_Dimension(float, float, float, float, const RwRGBA&, int16, float, uint8) {
    assert(false);
}

/* --- Buffered XLU Sprite --- */

// 0x70E4A0
void CSprite::RenderBufferedOneXLUSprite(float x, float y, float z, float w, float h, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, uint8 a11) {
    plugin::Call<0x70E4A0, float, float, float, float, float, uint8, uint8, uint8, int16, float, uint8>(x, y, z, w, h, r, g, b, intensity, recipNearZ, a11);
}

// 0x70E780
void CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect(float x, float y, float z, float w, float h, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, float angle, uint8 a12) {
    plugin::Call<0x70E780, float, float, float, float, float, uint8, uint8, uint8, int16, float, float, uint8>(x, y, z, w, h, r, g, b, intensity, recipNearZ, angle, a12);
}

// 0x70EAB0
void CSprite::RenderBufferedOneXLUSprite_Rotate_Dimension(float, float, float, float, float, uint8, uint8, uint8, int16, float, float, uint8) {
    assert(false);
}

// 0x70EDE0
void CSprite::RenderBufferedOneXLUSprite_Rotate_2Colours(float, float, float, float, float, uint8, uint8, uint8, uint8, uint8, uint8, float, float, float, float, uint8) {
    assert(false);
}

// 0x70F440
void CSprite::RenderBufferedOneXLUSprite2D(float, float, float, float, const RwRGBA&, int16, uint8) {
    assert(false);
}

// unused
// 0x70F600
void CSprite::RenderBufferedOneXLUSprite2D_Rotate_Dimension(float, float, float, float, const RwRGBA&, int16, float, uint8) {
    assert(false);
}
