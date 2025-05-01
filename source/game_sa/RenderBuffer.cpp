#include "StdInc.h"
#include "RenderBuffer.hpp"

namespace RenderBuffer {
auto& s_IndicesToBeStored{ StaticRef<int32, 0xC40410>() };
auto& s_VerticesToBeStored{ StaticRef<int32, 0xC40414>() };

const auto IdxBufferSize = (int32)std::size(aTempBufferIndices);
const auto VtxBufferSize = (int32)std::size(TempBufferVertices.m_3d);

void InjectHooks() {
    RH_ScopedNamespace(RenderBuffer);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(StartStoring, 0x708290);
    RH_ScopedGlobalInstall(StopStoring, 0x7077A0);
    RH_ScopedGlobalInstall(RenderStuffInBuffer, 0x707800);
    RH_ScopedGlobalInstall(ClearRenderBuffer, 0x707790);
}

// 0x708290
void StartStoring(
    int32 nIndicesNeeded,
    int32 nVerticesNeeded,
    RwImVertexIndex*& outPtrFirstIndex,
    RwIm3DVertex*& outPtrFirstVertex
) {
    assert(nIndicesNeeded > 0 && nIndicesNeeded <= IdxBufferSize);
    assert(nVerticesNeeded > 0 && nVerticesNeeded <= VtxBufferSize);

    if (uiTempBufferIndicesStored + nIndicesNeeded >= IdxBufferSize) {
        RenderBuffer::RenderStuffInBuffer();
    }

    if (uiTempBufferVerticesStored + nVerticesNeeded >= VtxBufferSize) {
        RenderBuffer::RenderStuffInBuffer();
    }

    outPtrFirstIndex = &aTempBufferIndices[uiTempBufferIndicesStored];
    outPtrFirstVertex = &TempBufferVertices.m_3d[uiTempBufferVerticesStored];

    s_IndicesToBeStored = nIndicesNeeded;
    s_VerticesToBeStored = nVerticesNeeded;
}

// 0x7077A0
void StopStoring() {
    const auto totalNumIdx = uiTempBufferIndicesStored + s_IndicesToBeStored;
    if (uiTempBufferIndicesStored >= totalNumIdx) { // Doesn't make a lot of sense, as it will only be true if `s_IndicesToBeStored` is negative, which isn't allowed.
        uiTempBufferVerticesStored += s_VerticesToBeStored;
        uiTempBufferIndicesStored  += s_IndicesToBeStored;
    } else {
        for (auto n = uiTempBufferIndicesStored; n <= totalNumIdx; n++) {
            aTempBufferIndices[n] += uiTempBufferVerticesStored;
        }
        uiTempBufferVerticesStored += s_VerticesToBeStored;
        uiTempBufferIndicesStored   = totalNumIdx;
    }
}

// NOTSA
void Render(RwPrimitiveType primType, RwMatrix* ltm, RwUInt32 /*RwIm3DTransformFlags*/ flags, bool isIndexed) {
    if (uiTempBufferVerticesStored) {
        if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, ltm, flags)) {
            if (isIndexed) {
                assert(aTempBufferIndices);
                RwIm3DRenderIndexedPrimitive(primType, aTempBufferIndices, uiTempBufferIndicesStored);
            } else {
                RwIm3DRenderPrimitive(primType);
            }
            RwIm3DEnd();
        }
    }
    ClearRenderBuffer();
}

// 0x707800
void RenderStuffInBuffer() {
    Render(rwPRIMTYPETRILIST, nullptr, rwIM3D_VERTEXUV);
}

// 0x707790
void ClearRenderBuffer() {
    uiTempBufferIndicesStored  = 0;
    uiTempBufferVerticesStored = 0;
}

// 0x6E7680 - I know, `LittlTest()` is missing, but nobody cares.
void RenderIfDoesntFit(int32 nIdxNeeded, int32 nVtxNeeded) {
    if (uiTempBufferIndicesStored + nIdxNeeded > IdxBufferSize || uiTempBufferVerticesStored + nVtxNeeded > VtxBufferSize) {
        RenderStuffInBuffer();
    }
}

// notsa
RwIm3DVertex* PushVertex(CVector pos, CRGBA color) {
    const auto vtx = &TempBufferVertices.m_3d[uiTempBufferVerticesStored++];

    RwIm3DVertexSetPos(vtx, pos.x, pos.y, pos.z);
    RwIm3DVertexSetRGBA(vtx, color.r, color.g, color.b, color.a);

    return vtx;
}

// notsa
RwIm3DVertex* PushVertex(CVector pos, CVector2D uv, CRGBA color) {
    const auto vtx = PushVertex(pos, color);

    RwIm3DVertexSetU(vtx, uv.x);
    RwIm3DVertexSetV(vtx, uv.y);

    return vtx;
}

// notsa
void PushIndex(int32 idx, bool useCurrentVtxAsBase) {
    idx = useCurrentVtxAsBase
        ? (int32)uiTempBufferVerticesStored + idx
        : idx;
    assert(idx >= 0);
    aTempBufferIndices[uiTempBufferIndicesStored++] = idx;
}

// notsa
void PushIndices(std::initializer_list<int32> idxs, bool useCurrentVtxAsBase) {
    for (auto idx : idxs) {
        PushIndex(idx, useCurrentVtxAsBase);
    }
}

bool CanFitVertices(int32 nVtxNeeded) {
    return uiTempBufferVerticesStored + nVtxNeeded <= VtxBufferSize;
}

}; // namespace RenderBuffer 
