#include "StdInc.h"
#include "RealTimeShadow.h"
#include "RealTimeShadowManager.h"

void CRealTimeShadow::InjectHooks() {
    RH_ScopedClass(CRealTimeShadow);
    RH_ScopedCategory("Shadows");

    RH_ScopedInstall(Constructor, 0x706410);
    RH_ScopedInstall(Destructor, 0x706440);

    RH_ScopedInstall(SetLightProperties, 0x705900);
    RH_ScopedInstall(GetShadowRwTexture, 0x7059F0);
    RH_ScopedInstall(DrawBorderAroundTexture, 0x705A00);
    RH_ScopedInstall(Create, 0x706460);
    RH_ScopedInstall(Update, 0x706600);
    RH_ScopedInstall(Destroy, 0x705990);
}

CRealTimeShadow::~CRealTimeShadow() {
    Destroy();
}

// 0x705900
RwFrame* CRealTimeShadow::SetLightProperties(float angle, float unused, bool doSetCamLight) {
    const auto frame = RpLightGetFrame(m_pLight);
    if (!frame) {
        return nullptr;
    }

    const RwV3d yaxis{ 0.f, 1.f, 0.f };
    RwFrameRotate(frame, &yaxis, angle, rwCOMBINEREPLACE);

    const RwV3d xaxis{ 1.f, 0.f, 0.f };
    RwFrameRotate(frame, &xaxis, -90.0, rwCOMBINEPOSTCONCAT);

    if (doSetCamLight) {
        m_camera.SetLight(m_pLight);
    }

    return frame;
}

// 0x7059F0
RwTexture* CRealTimeShadow::GetShadowRwTexture() {
    return GetCurrentCamera().GetRwRenderTexture();
}

// 0x705990
void CRealTimeShadow::Destroy() {
    m_pOwner = nullptr;
    m_nRwObjectType = (uint32)-1;
    if (m_pLight) {
        // This is so convoluted... Do we actually have to do it like this? Wouldnt a simple `RwFrameDestroy(RpLightGetFrame(m_pLight))` work?
        const auto frame = RpLightGetFrame(m_pLight);
        rwObjectHasFrameSetFrame(m_pLight, nullptr);
        RwFrameDestroy(frame);

        RpLightDestroy(m_pLight);

        m_pLight = nullptr;
    }
}

// 0x705A00
void CRealTimeShadow::DrawBorderAroundTexture(RwRGBA const& color) {
    GetCurrentCamera().DrawOutlineBorder(color);
}

// 0x706460
bool CRealTimeShadow::Create(bool isBlurred, int32 blurPasses, bool drawMoreBlur) {
    m_pLight = RpLightCreate(rpLIGHTDIRECTIONAL);
    if (!m_pLight) {
        return false;
    }

    const RwRGBAReal lightColor{ 0.8f, 0.8f, 0.8f, 0.f };
    RpLightSetColor(m_pLight, &lightColor);

    rwObjectHasFrameSetFrame(m_pLight, RwFrameCreate());

    if (m_camera.Create(7)) {
        m_nBlurPasses = blurPasses;
        m_bBlurred = isBlurred;
        m_bDrawMoreBlur = drawMoreBlur;
        if (!isBlurred || m_blurCamera.Create(6)) {
            m_camera.SetLight(m_pLight);
            return true;
        }
    }

    Destroy();
    return false;
}

// 0x706600
RwTexture* CRealTimeShadow::Update() {
    // Update BaseSphere's center in the world
    switch (m_nRwObjectType) {
    case rpATOMIC:
    case rpCLUMP: {
        RwV3dTransformPoints(
            &m_baseSphere.m_vecCenter,
            &m_boundingSphere.m_vecCenter,
            1,
            RwFrameGetMatrix(RpClumpGetFrame(m_pOwner->m_pRwClump))
        );
        break;
    }
    }

    // Update camera position
    m_camera.SetCenter(m_baseSphere.m_vecCenter);

    // Render object onto the camera's raster
    switch (m_nRwObjectType) {
    case rpATOMIC:
        m_camera.Update(m_pOwner->m_pRwAtomic);
        break;
    case rpCLUMP:
        m_camera.Update(m_pOwner->m_pRwClump);
        break;
    }

    // Do blur
    auto raster = m_camera.GetRwRenderRaster();
    assert(raster);

    if (m_bBlurred) {
        raster = m_blurCamera.RasterResample(raster);
        assert(raster);
    }

    if (m_nBlurPasses) {
        raster = g_realTimeShadowMan.m_BlurCamera.RasterBlur(raster, m_nBlurPasses);
        assert(raster);
    }

    if (m_bDrawMoreBlur) {
        g_realTimeShadowMan.m_GradientCamera.DrawBlurryRaster2(raster);
    }

    // Finish
    return GetShadowRwTexture();
}
