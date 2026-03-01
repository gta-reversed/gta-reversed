#include "StdInc.h"

#include "ShadowCamera.h"

/*! InjectHooks at the bottom !*/

// 0x705400
void CShadowCamera::Destroy() {
    if (!m_pRwCamera) {
        return;
    }

    if (auto frame = RwCameraGetFrame(m_pRwCamera)) {
        rwObjectHasFrameSetFrame(frame, nullptr);
        RwFrameDestroy(frame);
    }

    if (auto frameBuffer = GetRwRenderRaster()) {
        RwRasterDestroy(frameBuffer);
        RwCameraSetRaster(m_pRwCamera, nullptr);
    }

    if (m_pRwRenderTexture) {
        RwTextureSetRaster(m_pRwRenderTexture, nullptr);
        RwTextureDestroy(m_pRwRenderTexture);
        m_pRwRenderTexture = nullptr;
    }

    RwCameraDestroy(m_pRwCamera);
    m_pRwCamera = nullptr;
}

// 0x7054C0
void CShadowCamera::SetFrustum(float radius) {
    RwCameraSetFarClipPlane(m_pRwCamera, 2 * radius);
    RwCameraSetNearClipPlane(m_pRwCamera, radius / 1000.0f);

    const CVector2D window(radius, radius);
    RwCameraSetViewWindow(m_pRwCamera, &window);
}

// 0x705520
void CShadowCamera::SetLight(RpLight* light) {
    auto* lightFrame  = RpLightGetFrame(light);
    auto* lightMatrix = RwFrameGetMatrix(lightFrame);
    auto* cameraFrame = RwCameraGetFrame(m_pRwCamera);
    cameraFrame->modelling.right = lightMatrix->right;
    cameraFrame->modelling.up = lightMatrix->up;
    cameraFrame->modelling.at = lightMatrix->at;
    RwMatrixUpdate(&cameraFrame->modelling);
    RwFrameUpdateObjects(cameraFrame);
}

// signature changed
// 0x705590
void CShadowCamera::SetCenter(const CVector& center) {
    auto frame = RwCameraGetFrame(m_pRwCamera);
    auto mat = RwFrameGetMatrix(frame);
    *RwMatrixGetPos(mat) = m_pRwCamera->farPlane / -2.0f * mat->at + center;
    RwMatrixUpdate(&frame->modelling);
    RwFrameUpdateObjects(frame);
    RwFrameOrthoNormalize(frame);
}

// 0x705660
void CShadowCamera::InvertRaster() {
    const auto& raster = GetRwRenderRaster();
    const auto  width = (float)RwRasterGetWidth(raster), height = (float)RwRasterGetHeight(raster);

    // Helper to construct vertices used here
    const auto MkVert = [rhw = 1.f / RwCameraGetNearClipPlane(m_pRwCamera)](float x, float y) {
        return RwIm2DVertex{
            .x = x,
            .y = y,
            .z = RwIm2DGetNearScreenZ(),

            .rhw = rhw,

            .emissiveColor = 0xFFFFFFFF
        };
    };

    RwIm2DVertex vertices[]{
        MkVert(0.f,   0.f),    // Top left
        MkVert(0.f,   height), // Bottom left
        MkVert(width, 0.f),    // Top right
        MkVert(width, height), // Bottom right
    };

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDINVDESTCOLOR));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDZERO));

    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vertices, std::size(vertices));

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
}

// 0x705770
RwRaster* CShadowCamera::GetRwRenderRaster() const {
    return RwCameraGetRaster(m_pRwCamera);
}

// 0x705780
RwTexture* CShadowCamera::GetRwRenderTexture() const {
    return m_pRwRenderTexture;
}

// 0x705790
RwRaster* CShadowCamera::DrawOutlineBorder(const CRGBA& color) {
    // Helper to construct vertices used here
    const auto MkVert = [
        &color,
        rhw = 1.f / RwCameraGetNearClipPlane(m_pRwCamera)
    ](float x, float y) {
        RwIm2DVertex vtx;

        RwIm2DVertexSetScreenX(&vtx, x);
        RwIm2DVertexSetScreenY(&vtx, y);
        RwIm2DVertexSetScreenZ(&vtx, RwIm2DGetNearScreenZ());

        RwIm2DVertexSetRecipCameraZ(&vtx, rhw);

        RwIm2DVertexSetIntRGBA(&vtx, color.r, color.g, color.b, color.a);

        return vtx;
    };

    // NOTE: Code seemingly assumes the aspect ratio of the image is 1:1
    const auto w = RwRasterGetWidth(GetRwRenderRaster()) - 1.f;
    RwIm2DVertex vertices[]{
        MkVert(0.f, 0.f),
        MkVert(w,   0.f),
        MkVert(w,   w),
        MkVert(0.f, w),
    };

    if (RwCameraBeginUpdate(m_pRwCamera)) {
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

        RwImVertexIndex indices[]{0, 1, 2, 3, 0};
        VERIFY(RwIm2DRenderIndexedPrimitive(rwPRIMTYPEPOLYLINE, vertices, std::size(vertices), indices, std::size(indices)));

        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));

        RwCameraEndUpdate(m_pRwCamera);
    }

    return GetRwRenderRaster();
}

// 0x705B60
RwCamera* CShadowCamera::Create(int32 rasterSizePower) {
    const auto rasterSizeInPx = 1 << rasterSizePower; // Size of raster in pixels. Same as `pow(2, rasterSize)`

    m_pRwCamera = RwCameraCreate();
    if (!m_pRwCamera) {
    fail:
        Destroy();
        return nullptr;
    }

    const auto frame = RwFrameCreate();
    if (!frame) {
        goto fail;
    }

    rwObjectHasFrameSetFrame(m_pRwCamera, frame);

    if (!RwCameraGetFrame(m_pRwCamera)) {
        goto fail;
    }

    const auto raster = RwRasterCreate(rasterSizeInPx, rasterSizeInPx, 0, rwRASTERTYPECAMERATEXTURE);
    if (!raster) {
        goto fail;
    }

    RwCameraSetRaster(m_pRwCamera, raster);

    m_pRwRenderTexture = RwTextureCreate(raster);
    if (!m_pRwRenderTexture) {
        goto fail;
    }

    RwTextureSetAddressing(m_pRwRenderTexture, rwFILTERMIPNEAREST | rwFILTERMIPNEAREST);
    RwTextureSetFilterMode(m_pRwRenderTexture, rwFILTERLINEAR);

    RwCameraSetProjection(m_pRwCamera, rwPARALLEL);

    return m_pRwCamera;
}

/*!
* @addr 0x705620
* @brief Render the atomic using the default render callback
*
* @param          atomic The atomic to render
* @param [unused] data
*
* @return The `atomic`
*
* Used in 3-4 places, but (mostly) inlined in the final exe
*/
RpAtomic* atomicQuickRender(RpAtomic* atomic, void* data) {
    UNUSED(data);

    const auto original = RpAtomicGetRenderCallBack(atomic);

    RpAtomicSetRenderCallBack(atomic, NULL);
    RpAtomicRender(atomic);
    RpAtomicSetRenderCallBack(atomic, original);

    return atomic;
}

// notsa
template<typename RenderFnT>
void CShadowCamera::Update_Internal(RpAtomic* atomic, uint32 geoFlagsToClear, RenderFnT&& Render) {
    // Clear camera raster to be transparent
    RwRGBA color(0xFF, 0xFF, 0xFF, 0);
    RwCameraClear(m_pRwCamera, &color, rwCAMERACLEARZ | rwCAMERACLEARIMAGE);

    // Now update camera
    if (RwCameraBeginUpdate(m_pRwCamera)) {
        const auto geo = RpAtomicGetGeometry(atomic);

        // Save original flags
        const auto ogflags = RpGeometryGetFlags(geo);

        // Clear some for this render pass (optimization)
        RpGeometrySetFlags(geo, ogflags & ~geoFlagsToClear);

        // Render
        Render();

        // Restore flags
        RpGeometrySetFlags(geo, ogflags);

        // Render the camera's current raster, but inverted
        // This is basically the shadow (I think)
        InvertRaster();

        // Finish update
        RwCameraEndUpdate(m_pRwCamera);
    }
}

// 0x705BF0
RwCamera* CShadowCamera::Update(RpClump* clump) {
    Update_Internal(
        GetFirstAtomic(clump), // Not quite sure... They render all atomics, but only modify the first atomics geometry's flags? Why not render all atomics separately?
        rpGEOMETRYTEXTURED | rpGEOMETRYPRELIT | rpGEOMETRYLIGHT | rpGEOMETRYMODULATEMATERIALCOLOR | rpGEOMETRYTEXTURED2,
        [&] { RpClumpForAllAtomics(clump, &atomicQuickRender, nullptr); }
    );

    return m_pRwCamera;
}

// 0x705C80
RwCamera* CShadowCamera::Update(RpAtomic* atomic) {
    Update_Internal(
        atomic,
        rpGEOMETRYTEXTURED | rpGEOMETRYPRELIT | rpGEOMETRYNORMALS | rpGEOMETRYLIGHT | rpGEOMETRYMODULATEMATERIALCOLOR | rpGEOMETRYTEXTURED2,
        [&] { atomicQuickRender(atomic, nullptr); }
    );

    return m_pRwCamera;
}

// 0x705D20
RwCamera* CShadowCamera::MakeGradientRaster() {
    // Monocolor intensity of the gardient
    constexpr float MIN_COLOR_INTENSITY{ 64.f }, MAX_COLOR_INTENSITY{ 128.f };

    if (!m_pRwCamera) {
        return nullptr;
    }

    const auto& raster = GetRwRenderRaster();
    const auto  width  = RwRasterGetWidth(raster), height = RwRasterGetHeight(raster);

    // Raster too small, do nothing (Assumes aspect ratio is 1:1)
    if (height < 1) {
        return nullptr;
    }

    if (!RwCameraBeginUpdate(m_pRwCamera)) {
        return nullptr;
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDZERO));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDINVDESTCOLOR));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,         RWRSTATE(rwSHADEMODEFLAT));

    // Draw lines along the X axis while moving along the Y axis from top to bottom.
    // Each line is less and less intense in color
    // TODO: Maybe just replace this abnomination with 2 triangles with the top vertices being max intensity, and the other (bottom) 2 being min intensity?
    {
        const auto cistep = (MIN_COLOR_INTENSITY - MAX_COLOR_INTENSITY) / (float)height; // Color intensity step each iteration
        float      cicurr = MAX_COLOR_INTENSITY; // Current color intensity

        const auto nscrz  = RwIm2DGetNearScreenZ();

        for (auto y{0}; y < height; y++, cicurr += cistep) {
            const auto MkVert = [
                &,
                rhw = 1.f / RwCameraGetNearClipPlane(m_pRwCamera),
                cc  = (uint8)(cicurr)
            ] (float x) {
                RwIm2DVertex vtx;

                RwIm2DVertexSetScreenX(&vtx, x);
                RwIm2DVertexSetScreenY(&vtx, (float)(y));
                RwIm2DVertexSetScreenZ(&vtx, nscrz);

                RwIm2DVertexSetRecipCameraZ(&vtx, rhw);

                RwIm2DVertexSetIntRGBA(&vtx, cc, cc, cc, cc);

                return vtx;
            };

            RwIm2DVertex vertices[2]{
                MkVert(0.f),
                MkVert((float)(width - 1)),
            };

            // TODO/NOTE: Horrible efficiency (should batch these calls)
            RwIm2DRenderLine(vertices, std::size(vertices), 0, 1);
        }
    }

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,   RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,    RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,   RWRSTATE(rwSHADEMODEGOURAUD));

    RwCameraEndUpdate(m_pRwCamera);

    return m_pRwCamera;
}

// 0x705A20
bool Im2DRenderQuad(RwReal x1, RwReal y1, RwReal x2, RwReal y2, RwReal z, RwReal recipCamZ, RwReal uvOffset) {
    const auto MkVert = [=] (float x, float y, float u, float v) {
        return RwIm2DVertex{
            .x = x,
            .y = y,
            .z = z,

            .rhw = recipCamZ,

            .emissiveColor = 0xFFFFFFFF,

            .u = u,
            .v = v
        };
    };

    RwIm2DVertex vertices[]{
        MkVert(x1, y1, uvOffset,       uvOffset      ),
        MkVert(x1, y2, uvOffset,       uvOffset + 1.f),
        MkVert(x2, y1, uvOffset + 1.f, uvOffset      ),
        MkVert(x2, y2, uvOffset + 1.f, uvOffset + 1.f),
    };

    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vertices, std::size(vertices));

    return true;
}

// 0x706070
RwRaster* CShadowCamera::RasterResample(RwRaster* sourceRaster) {
    if (!m_pRwCamera) {
        return nullptr;
    }

    const auto camRaster = GetRwRenderRaster();

    if (!RwCameraBeginUpdate(m_pRwCamera)) {
        return nullptr;
    }

    RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDZERO));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(sourceRaster));

    const auto size = RwRasterGetWidth(camRaster);
    Im2DRenderQuad(
        0.f,          0.f,
        (RwReal)size, (RwReal)size,
        RwIm2DGetNearScreenZ(),
        1.f / RwCameraGetNearClipPlane(m_pRwCamera),
        0.5f / size
    );

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,    RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,   RWRSTATE(rwBLENDINVSRCALPHA));

    RwCameraEndUpdate(m_pRwCamera);

    return camRaster;
}

// 0x706170
RwRaster* CShadowCamera::RasterBlur(RwRaster* blurRaster, int32 numPasses) {
    if (!m_pRwCamera) {
        return nullptr;
    }

    const auto camRaster = RwCameraGetRaster(m_pRwCamera);

    if (!numPasses) { // NOTSA
        return camRaster;
    }

    const auto DoRenderQuad2D = [
        brwidth = RwRasterGetWidth(blurRaster),
        rhw = 1.f / RwCameraGetNearClipPlane(m_pRwCamera)
    ] (float uv) {
        Im2DRenderQuad(
            0.f, 0.f,
            (RwReal)brwidth, (RwReal)brwidth,
            RwIm2DGetNearScreenZ(),
            rhw,
            uv / brwidth
        );
    };

    for (auto i{ 0 }; i < numPasses; i++) {
        // Set original raster
        RwCameraSetRaster(m_pRwCamera, camRaster);

        if (RwCameraBeginUpdate(m_pRwCamera)) {
            if (i == 0) { // First pass
                RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDONE));
                RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDZERO));
                RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(FALSE));
                RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
            }
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(blurRaster));

            DoRenderQuad2D(0.5f + 0.5f);

            RwCameraEndUpdate(m_pRwCamera);
        }

        // Swap raster to other one
        RwCameraSetRaster(m_pRwCamera, blurRaster);

        if (RwCameraBeginUpdate(m_pRwCamera)) {
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(camRaster));

            DoRenderQuad2D(0.5f - 0.5f);

            if (i == numPasses - 1) { // LastPass (pun intended)
                RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
                RwRenderStateSet(rwRENDERSTATESRCBLEND,    RWRSTATE(rwBLENDSRCALPHA));
                RwRenderStateSet(rwRENDERSTATEDESTBLEND,   RWRSTATE(rwBLENDINVSRCALPHA));
            }

            RwCameraEndUpdate(m_pRwCamera);
        }
    }

    // Use original raster
    RwCameraSetRaster(m_pRwCamera, camRaster);

    return blurRaster;
}

// 0x706330
RwRaster* CShadowCamera::DrawBlurryRaster2(RwRaster* srcRaster) {
    const auto camRaster = GetRwRenderRaster(); // Save original

    RwCameraSetRaster(m_pRwCamera, srcRaster);

    if (RwCameraBeginUpdate(m_pRwCamera)) {
        RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDZERO));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDSRCCOLOR));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(camRaster));

        const auto size = RwRasterGetWidth(srcRaster); // Assumes 1:1 aspect ratio
        Im2DRenderQuad(
            0.f, 0.f,
            (RwReal)size, (RwReal)size,
            RwIm2DGetNearScreenZ(),
            1.f / RwCameraGetNearClipPlane(m_pRwCamera),
            0.f
        );

        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));

        RwCameraEndUpdate(m_pRwCamera);
    }

    RwCameraSetRaster(m_pRwCamera, camRaster); // Restore to original

    return srcRaster;
}

void CShadowCamera::InjectHooks() {
    RH_ScopedClass(CShadowCamera);
    RH_ScopedCategory("Shadows");

    RH_ScopedInstall(Constructor, 0x7053F0);
    RH_ScopedInstall(Destructor, 0x705B50);

    RH_ScopedInstall(Destroy, 0x705400);
    RH_ScopedInstall(SetFrustum, 0x7054C0);
    RH_ScopedInstall(SetLight, 0x705520);
    RH_ScopedInstall(SetCenter, 0x705590);
    RH_ScopedInstall(InvertRaster, 0x705660);
    RH_ScopedInstall(GetRwRenderRaster, 0x705770);
    RH_ScopedInstall(GetRwRenderTexture, 0x705780);
    RH_ScopedInstall(DrawOutlineBorder, 0x705790);
    RH_ScopedInstall(Create, 0x705B60);
    RH_ScopedOverloadedInstall(Update, "Clump", 0x705BF0, RwCamera * (CShadowCamera::*)(RpClump*));
    RH_ScopedOverloadedInstall(Update, "Atomic", 0x705C80, RwCamera * (CShadowCamera::*)(RpAtomic*));
    RH_ScopedInstall(MakeGradientRaster, 0x705D20);
    RH_ScopedInstall(RasterResample, 0x706070);
    RH_ScopedInstall(RasterBlur, 0x706170);
    RH_ScopedInstall(DrawBlurryRaster2, 0x706330);

    RH_ScopedGlobalInstall(atomicQuickRender, 0x705620);
    RH_ScopedGlobalInstall(Im2DRenderQuad, 0x705A20);
}
