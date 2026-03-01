#pragma once

#include <Base.h>
#include <rwcore.h>

namespace RenderBuffer {
void InjectHooks();

/*!
* @addr 0x708290
* @brief Start storing vertices. Must be followed by `StopStoring()`
* If there isn't enough space in the buffer for the
* specified amount of vertices/indices the buffer is
* rendered out first.
*
* @param     nIndicesNeeded  No. of indices that will be stored
* @param     nVerticesNeeded No. of vertices that will be stored
* @param out pFirstIndex     Pointer to the first unused index in the buffer
* @param out pFirstVertex    Pointer to the first unused vertex in the buffer
*/
void StartStoring(
    int32 nIndicesNeeded,
    int32 nVerticesNeeded,
    RwImVertexIndex*& pFirstIndex,
    RwIm3DVertex*& pFirstVertex
);

/*!
* @addr 0x7077A0
* @brief Must be enclosed by a `StartStoring()` operation
*/
void StopStoring();

/*!
* @addr 0x707800
* @brief Render out the contents of the temporary buffers as a trilist.
* Frequently inlined!
*/
void RenderStuffInBuffer();

/*!
* @notsa
* @brief Render out the contents of the temporary buffer as specified by the arguments. Frequently inlined!
*/
void Render(RwPrimitiveType primType, RwMatrix* ltm = nullptr, RwUInt32 /*RwIm3DTransformFlags*/ flags = 0, bool isIndexed = true);

/*!
* @addr 0x707790
* @brief Reset the index/vertex buffer stored counters.
* Frequently inlined!
*/
void ClearRenderBuffer();

/*!
* @addr 0x6E7680
* @brief Render out buffer if the specified number of vertices/indices doesn't fit
*/
void RenderIfDoesntFit(int32 nIdx, int32 nVtx);

/*
* @addr notsa
* @brief Push a vertex to the buffer. Not to be used with `StartStoring`! Use if the VERTEXUV flag **IS NOT** used when calling `Render`
*/
RwIm3DVertex* PushVertex(CVector pos, CRGBA color);

/*
* @addr notsa
* @brief Push a vertex to the buffer. Not to be used with `StartStoring`! Use if the VERTEXUV flag **IS** used when calling `Render`
*/
RwIm3DVertex* PushVertex(CVector pos, CVector2D uv, CRGBA color);

/*!
* @addr notsa
* @brief Push an index into the buffer. Not to be used with `StartStoring`!
*/
void PushIndex(int32 idx, bool useCurrentVtxAsBase);


/*!
* @addr notsa
* @brief Push multiple indices into the buffer. Not to be used with `StartStoring`!
*/
void PushIndices(std::initializer_list<int32> idxs, bool useCurrentVtxAsBase);

/*!
* @addr notsa
* @brief Check if the buffer can fit `nVtxNeeded` vertices
*/
bool CanFitVertices(int32 nVtxNeeded);
}; // namespace RenderBuffer
