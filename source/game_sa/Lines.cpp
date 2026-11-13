#include "StdInc.h"

#include "Lines.h"

void CLines::InjectHooks() {
    RH_ScopedClass(CLines);
    RH_ScopedCategoryGlobal();

    RH_ScopedOverloadedInstall(RenderLineNoClipping, "", 0x6FF460, void(*)(float, float, float, float, float, float, uint32, uint32));
    RH_ScopedOverloadedInstall(RenderLineWithClipping, "", 0x6FF4F0, void(*)(float, float, float, float, float, float, uint32, uint32));
    RH_ScopedOverloadedInstall(ImmediateLine2D, "", 0x6FF790, void(*)(int32, int32, int32, int32, uint8, uint8, uint8, uint8, uint8, uint8, uint8, uint8));
}

// 0x6FF460
void CLines::RenderLineNoClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32 startColor, uint32 endColor) {
    RxObjSpace3DVertex vertices[] = {
        { .objVertex = { startX, startY, startZ }, .color = startColor >> 8 | startColor << 24 }, // Convert color to ARGB
        { .objVertex = { endX,   endY,   endZ   }, .color =   endColor >> 8 | endColor   << 24 }, // Convert color to ARGB
    };

    LittleTest();
    if (RwIm3DTransform(vertices, 2u, nullptr, 0)) {
        RwIm3DRenderLine(0, 1);
        RwIm3DEnd();
    }
}

// 0x6FF4F0
void CLines::RenderLineWithClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32 startColor, uint32 endColor) {
    const CVector start = { startX, startY, startZ };
    const CVector end   = { endX,   endY,   endZ };
    const CRGBA   startRGBA{ startColor }, endRGBA{ endColor };

    // The line is split into 1 to 7 segments, depending on its length
    const auto numSegments = (int16)std::min(DistanceBetweenPoints(start, end) / 2.5f + 1.0f, 7.0f);

    const auto SetVertex = [&](RwIm3DVertex* vertex, float t) {
        const auto pos   = start + (end - start) * t;
        const auto color = lerp(startRGBA, endRGBA, t); // NOTE: Original computes `from + (to - from) * t`, so the components may be off by 1
        RwIm3DVertexSetPos(vertex, pos.x, pos.y, pos.z);
        RwIm3DVertexSetRGBA(vertex, color.r, color.g, color.b, color.a);
    };

    auto* vertex = TempBufferVertices.m_3d;
    for (auto i = 0; i < numSegments; i++) {
        SetVertex(vertex++, (float)(i) / (float)numSegments);
        SetVertex(vertex++, (float)(i + 1) / (float)numSegments);
    }

    static RwImVertexIndex indices[] = { // 0x8D503C
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 0
    };
    LittleTest();
    if (RwIm3DTransform(TempBufferVertices.m_3d, 2 * numSegments, nullptr, 0)) {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, 2 * numSegments);
        RwIm3DEnd();
    }
}

// 0x6FF790
void CLines::ImmediateLine2D(int32 startX, int32 startY, int32 endX, int32 endY, uint8 startR, uint8 startG, uint8 startB, uint8 startA, uint8 endR, uint8 endG, uint8 endB, uint8 endA) {
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

    RwIm2DVertex vertices[] = {
        { .x = float(startX), .y = float(startY), .emissiveColor = CRGBA(startR, startG, startB, startA).ToIntARGB() },
        { .x = float(endX),   .y = float(endY),   .emissiveColor = CRGBA(endR, endG, endB, endA).ToIntARGB() }
    };
    RwIm2DRenderLine(vertices, 2, 0, 1);

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
}
