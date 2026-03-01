#include "StdInc.h"

#include "ShinyTexts.h"
#include "Shadows.h"

uint32& CShinyTexts::NumShinyTexts = *(uint32*)0xC7C6F8;
CRegisteredShinyText(&CShinyTexts::aShinyTexts)[32] = *(CRegisteredShinyText(*)[32])0xC7D258;

void CShinyTexts::InjectHooks() {
    RH_ScopedClass(CShinyTexts);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x7221B0);
    RH_ScopedInstall(RenderOutGeometryBuffer, 0x7221C0);
    RH_ScopedInstall(Render, 0x724890);
    RH_ScopedInstall(RegisterOne, 0x724B60);
}

// 0x7221B0
void CShinyTexts::Init() {
    NumShinyTexts = 0;
}

// 0x7221C0
void CShinyTexts::RenderOutGeometryBuffer() {
    if (uiTempBufferIndicesStored) {
        LittleTest();
        if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXUV))
        {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
            RwIm3DEnd();
        }
        uiTempBufferVerticesStored = 0;
        uiTempBufferIndicesStored = 0;
    }
}

// 0x724890
void CShinyTexts::Render() {
    ZoneScoped;

    if (NumShinyTexts == 0)
        return;

    RwRenderStateSet(rwRENDERSTATECULLMODE,     RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,     RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,    RWRSTATE(rwBLENDONE));

    uiTempBufferIndicesStored  = 0;
    uiTempBufferVerticesStored = 0;

    RwTexture* texture{};
    for (CRegisteredShinyText& text : std::span{ aShinyTexts, NumShinyTexts }) {
        if (uiTempBufferIndicesStored > TOTAL_TEMP_BUFFER_INDICES - 64u ||
            uiTempBufferVerticesStored > TOTAL_TEMP_BUFFER_3DVERTICES - 64u
        )
        {
            RenderOutGeometryBuffer();
        }

        if (!text.m_bAlwaysTrue)
            continue;

        if (texture != gpHandManTex) {
            RenderOutGeometryBuffer();
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpHandManTex)));
            texture = gpHandManTex;
        }

        const auto GetRealVertexIndex = [](unsigned i) { return (unsigned)uiTempBufferVerticesStored + i; };

        const RwRGBA       color  = { text.m_colorR, text.m_colorG, text.m_colorB, 12 };
        const CVector*     posn[] = { &text.m_vecCornerAA, &text.m_vecCornerAB, &text.m_vecCornerBA, &text.m_vecCornerBB };
        const RwTexCoords* uvs[]  = { &text.m_texCoorsAA, &text.m_texCoorsAB, &text.m_texCoorsBA, &text.m_texCoorsBB };
        for (unsigned i = 0u; i < 4u; i++) {
            auto* vertex = &TempBufferVertices.m_3d[GetRealVertexIndex(i)];

            RxObjSpace3DVertexSetPreLitColor(vertex, &color);
            RxObjSpace3DVertexSetPos(vertex, posn[i]);
            RxObjSpace3DVertexSetU(vertex, uvs[i]->u);
            RxObjSpace3DVertexSetV(vertex, uvs[i]->v);
        }

        // Relative to the 0th vertex of the current text.
        constexpr RxVertexIndex indices[] = { 0, 1, 2, 2, 1, 3 };
        for (unsigned i = 0u; i < 6u; i++) {
            aTempBufferIndices[(unsigned)uiTempBufferIndicesStored + i] = GetRealVertexIndex(indices[i]);
        }

        uiTempBufferVerticesStored += 4;
        uiTempBufferIndicesStored += 6;
    }

    RenderOutGeometryBuffer();
    NumShinyTexts = 0;

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
}

// Must be called each frame to re-draw
// They are drawn once, then deleted
// 0x724B60
void CShinyTexts::RegisterOne(CVector cornerAA, CVector cornerAB, CVector cornerBA, CVector cornerBB,
                              float u1, float v1,
                              float u2, float v2,
                              float u3, float v3,
                              float u4, float v4,
                              bool alwaysTrue, uint8 red, uint8 green, uint8 blue, float maxDistance
) {
    if (NumShinyTexts >= MAX_SHINYTEXTS)
        return;

    const float distToCam = (TheCamera.GetPosition() - cornerAA).Magnitude();
    if (distToCam > maxDistance)
        return;

    CRegisteredShinyText& text = aShinyTexts[NumShinyTexts];

    text.m_vecCornerAA       = cornerAA;
    text.m_vecCornerAB       = cornerAB;
    text.m_vecCornerBA       = cornerBA;
    text.m_vecCornerBB       = cornerBB;
    text.m_texCoorsAA        = {u1, v1};
    text.m_texCoorsAB        = {u2, v2};
    text.m_texCoorsBA        = {u3, v3};
    text.m_texCoorsBB        = {u4, v4};
    text.m_fDistanceToCamera = distToCam;
    text.m_bAlwaysTrue       = alwaysTrue;
    text.m_colorR            = red;
    text.m_colorG            = green;
    text.m_colorB            = blue;

    const float halfMaxDist = maxDistance / 2.0f;
    if (halfMaxDist < distToCam) { // TODO: Fix this mess (add CRGB type)
        const float mult = 1.0f - (distToCam - halfMaxDist) / halfMaxDist;
        text.m_colorR = (uint8)((float)red * mult);
        text.m_colorG = (uint8)((float)green * mult);
        text.m_colorB = (uint8)((float)blue * mult);
    }

    NumShinyTexts++;
}
