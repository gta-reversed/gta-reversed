#include "StdInc.h"

#include "Lines.h"

// 0x6FF460
void CLines::RenderLineNoClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32 startColor, uint32 endColor) {
    RxObjSpace3DVertex vertices[] = {
        { .objVertex = { startX, startY, startZ }, .color = startColor >> 8 | startColor << 24 },
        { .objVertex = { endX,   endY,   endZ   }, .color =   endColor >> 8 | endColor   << 24 }
    };

    LittleTest();
    if (RwIm3DTransform(vertices, 2u, nullptr, 0)) {
        RwIm3DRenderLine(0, 1);
        RwIm3DEnd();
    }
}

// 0x6FF4F0
void CLines::RenderLineWithClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32 startColor, uint32 endColor) {
    return plugin::Call<0x6FF4F0, float, float, float, float, float, float, uint32, uint32>(startX, startY, startZ, endX, endY, endZ, startColor, endColor);

    const CVector start = { startX, startY, startZ };
    const CVector end   = { endX,   endY,   endZ };

    auto iters = DistanceBetweenPoints(start, end) * 0.4f + 1.0f;
    auto numVerts = (uint32)std::min(iters, 7.0f);
    if (numVerts > 0) {
        // todo:
        for (auto i = 0u; i < numVerts; i++) {
            // todo:
        }
    }

    static RwImVertexIndex indices[] = { // 0x8D503C
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 0
    };
    LittleTest();
    if (RwIm3DTransform(TempBufferVertices.m_3d, 2 * numVerts, nullptr, 0)) {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, 2 * numVerts);
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
