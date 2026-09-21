#include "StdInc.h"
#include "StencilShadowObject.h"

// TODO: Statically allocate after reversing RenderForVehicle&RenderForObject.
static inline auto& s_ShadowTrianglePointsUnk         = StaticRef<RxVertexIndex*>(0xC6A170);
static inline auto& s_ShadowTrianglePoints            = StaticRef<CVector*>(0xC6A174);
static inline auto& s_TransformedShadowTrianglePoints = StaticRef<CVector*>(0xC6A178);
static inline auto& s_SunPosNrm                       = StaticRef<CVector>(0x8D5244); // CVector(1.0, 1.0, -2.0)

// 0x70FA70
// Maintains the silhouette edge list used for shadow volume extrusion.
// Custom calling convention (matches the original binary exactly):
//   ECX   - uint16 edge pairs array (two uint16s per edge)
//   EDX   - uint16* edge count (in/out)
//   EDI   - first vertex index (passed in the register, not saved/restored)
//   stack - second vertex index (caller cleans up)
// Adds the edge, unless it's already present (in either direction),
// in which case it's removed again (shared/interior edges cancel out).
static void __declspec(naked) AddShadowSilhouetteEdge() {
    _asm push ebx
    _asm movzx ebx, word ptr [edx]
    _asm push ebp
    _asm mov bp, word ptr [esp + 0x0C]
    _asm xor eax, eax
    _asm test ebx, ebx
    _asm push esi
    _asm jle EDGE_APPEND
EDGE_LOOP:
    _asm mov si, word ptr [ecx + eax*4]
    _asm cmp si, di
    _asm jnz EDGE_CHECK_SWAPPED
    _asm cmp word ptr [ecx + eax*4 + 2], bp
    _asm jz EDGE_FOUND
EDGE_CHECK_SWAPPED:
    _asm cmp si, bp
    _asm jnz EDGE_NEXT
    _asm cmp word ptr [ecx + eax*4 + 2], di
    _asm jz EDGE_FOUND
EDGE_NEXT:
    _asm movzx esi, word ptr [edx]
    _asm inc eax
    _asm cmp eax, esi
    _asm jl EDGE_LOOP
EDGE_APPEND:
    _asm mov word ptr [ecx + ebx*4], di
    _asm movzx eax, word ptr [edx]
    _asm pop esi
    _asm mov word ptr [ecx + eax*4 + 2], bp
    _asm inc word ptr [edx]
    _asm pop ebp
    _asm pop ebx
    _asm ret
EDGE_FOUND:
    _asm cmp word ptr [edx], 1
    _asm jbe EDGE_DECR
    _asm mov si, word ptr [ecx + ebx*4 - 4]
    _asm mov word ptr [ecx + eax*4], si
    _asm movzx esi, word ptr [edx]
    _asm mov si, word ptr [ecx + esi*4 - 2]
    _asm mov word ptr [ecx + eax*4 + 2], si
EDGE_DECR:
    _asm dec word ptr [edx]
    _asm pop esi
    _asm pop ebp
    _asm pop ebx
    _asm ret
}

// C++-callable wrapper for the custom-convention 0x70FA70 helper above.
// Original callers pass (ECX = edges, EDX = &count, EDI = first index, stack = second index).
static void CallAddShadowSilhouetteEdge(uint16 a, uint16 b, RxVertexIndex* edges, uint16* count) {
    uint16 aa = a, bb = b;
    _asm mov ecx, edges
    _asm mov edx, count
    _asm mov di, aa
    _asm push bb
    _asm call AddShadowSilhouetteEdge
    _asm add esp, 2
}

void CStencilShadows::InjectHooks() {
    RH_ScopedClass(CStencilShadows);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x70F9E0);
    RH_ScopedInstall(Shutdown, 0x711390);
    RH_ScopedInstall(Process, 0x711D90);
    RH_ScopedInstall(GraphicsHighQuality, 0x70F9B0);
    RH_ScopedInstall(UpdateHierarchy, 0x710BC0);
    RH_ScopedInstall(RegisterStencilShadows, 0x711760, {.reversed=false});
    RH_ScopedInstall(RenderStencilShadows, 0x7113B0);
    RH_ScopedInstall(RenderForVehicle, 0x70FAE0, {.reversed=false});
    RH_ScopedInstall(RenderForObject, 0x710310, {.reversed=false});
    RH_ScopedInstall(Render, 0x710D50);
    RH_ScopedInstall(RenderBuffer, 0x710B50);
    RH_ScopedInstall(sub_710CC0, 0x710CC0);
}

// 0x70F9E0
void CStencilShadows::Init() {
    ZoneScoped;

    RwD3D9SetStencilClear(0);
    pFirstAvailableStencilShadowObject = m_StencilShadowObjects.data();
    pFirstActiveStencilShadowObject = nullptr;

    for (auto&& [i, obj] : rngv::enumerate(m_StencilShadowObjects)) {
        obj.m_pOwner                = nullptr;
        obj.m_NumShadowFaces        = 0;
        obj.m_Type                  = eStencilShadowObjType::NONE;
        obj.m_SizeOfShadowFacesData = 0;
        obj.m_FaceID                = 0;
        obj.m_ShadowFacesData       = 0;

        obj.m_pPrev = i ? &m_StencilShadowObjects[i - 1] : nullptr;
        obj.m_pNext = (i != m_StencilShadowObjects.size() - 1) ? &m_StencilShadowObjects[i + 1] : nullptr;
    }
}

// 0x711390
void CStencilShadows::Shutdown() {
    for (auto* obj = pFirstActiveStencilShadowObject; obj;) {
        auto* next = obj->m_pNext;
        obj->Destroy();
        obj = next;
    }
}

// 0x710D50
void CStencilShadows::Render(const CRGBA& color) {
    uiTempBufferIndicesStored  = 0;
    uiTempBufferVerticesStored = 0;

    for (auto* shadow = pFirstActiveStencilShadowObject; shadow; shadow = shadow->m_pNext) {
        const auto  numFaces = shadow->m_SizeOfShadowFacesData / 6; // 6 CVector components per face // 0x2AAAAAAB
        const auto* facePts  = shadow->m_ShadowFacesData;

        // Each shadow face holds 6 extruded verts (2 triangles) stored as 6 consecutive CVectors.
        for (auto face = 0u; face < numFaces; face++, facePts += 6) {
            // CRGBA is RGBA in memory, but the Im3D vertex color is ABGR - swizzle R and B
            // (original does this with shifts/ors rather than calling ToIntABGR).
            const auto intColor = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
            const auto nextVert = std::exchange(uiTempBufferVerticesStored, static_cast<uint16>(uiTempBufferVerticesStored + 6));

            const auto nextIdx  = std::exchange(uiTempBufferIndicesStored, static_cast<uint16>(uiTempBufferIndicesStored + 6));
            aTempBufferIndices[nextIdx + 0] = nextVert + 0;
            aTempBufferIndices[nextIdx + 1] = nextVert + 1;
            aTempBufferIndices[nextIdx + 2] = nextVert + 2;
            aTempBufferIndices[nextIdx + 3] = nextVert + 3;
            aTempBufferIndices[nextIdx + 4] = nextVert + 4;
            aTempBufferIndices[nextIdx + 5] = nextVert + 5;
            for (auto i = 0; i < 6; i++) {
                auto& vert     = TempBufferVertices.m_3d[nextVert + i];
                vert.objVertex = facePts[i];
                vert.color     = intColor;
            }
        }

        // Odd trailing triangle? - if (m_SizeOfShadowFacesData / 3) is odd, append the 3 leftover verts.
        // (original computes `(size / 3) & 1` via `0x55555556ull * size >> 32` magic division)
        if ((shadow->m_SizeOfShadowFacesData / 3) & 1) {
            const auto* triPts = shadow->m_ShadowFacesData + numFaces * 6;

            sub_710CC0(3, 3);

            const auto intColor = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
            const auto nextVert = std::exchange(uiTempBufferVerticesStored, static_cast<uint16>(uiTempBufferVerticesStored + 3));
            const auto nextIdx  = std::exchange(uiTempBufferIndicesStored, static_cast<uint16>(uiTempBufferIndicesStored + 3));
            aTempBufferIndices[nextIdx + 0] = nextVert + 0;
            aTempBufferIndices[nextIdx + 1] = nextVert + 1;
            aTempBufferIndices[nextIdx + 2] = nextVert + 2;

            for (auto i = 0; i < 3; i++) {
                auto& vert     = TempBufferVertices.m_3d[nextVert + i];
                vert.objVertex = triPts[i];
                vert.color     = intColor;
            }
        }
    }

    // Flush whatever's left in the temp buffer (same as sub_710CC0's flush, but unconditional on overflow).
    if (uiTempBufferIndicesStored && uiTempBufferVerticesStored) {
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
        LittleTest();
        if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
            RwIm3DEnd();
        }
        uiTempBufferVerticesStored = uiTempBufferIndicesStored = 0;
    }
}

// unused
// 0x710AF0
void CStencilShadows::SunSetPositionFromEntity(const CEntity* entity) {
    if (!entity) {
        return;
    }
    s_SunPosNrm = entity->GetPosition().Normalized();
}

// 0x710B50
void CStencilShadows::RenderBuffer(const CVector& pos) {
    s_SunPosNrm = pos.Normalized(); 
}

// 0x710CC0
void CStencilShadows::sub_710CC0(int32 indices, int32 vertices) {
    if (uiTempBufferIndicesStored + indices < TOTAL_TEMP_BUFFER_INDICES
        && uiTempBufferVerticesStored + vertices < TOTAL_TEMP_BUFFER_3DVERTICES) {
        return;
    }

    if (!uiTempBufferIndicesStored || !uiTempBufferVerticesStored) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
    LittleTest();
    if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA)) {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
        RwIm3DEnd();
    }
    uiTempBufferVerticesStored = uiTempBufferIndicesStored = 0;
}

// 0x7113B0
void CStencilShadows::RenderStencilShadows() {
    ZoneScoped;

    if (!GraphicsHighQuality()) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(rwSHADEMODEFLAT));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,                RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,            RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,                 RWRSTATE(rwBLENDZERO));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,                RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONMASK,      RWRSTATE(uint32(-1)));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONWRITEMASK, RWRSTATE(uint32(-1)));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTION,          RWRSTATE(rwSTENCILFUNCTIONALWAYS));
    RwRenderStateSet(rwRENDERSTATESTENCILFAIL,              RWRSTATE(rwSTENCILOPERATIONKEEP));
    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(rwSTENCILOPERATIONKEEP));
    RwRenderStateSet(rwRENDERSTATESTENCILPASS,              RWRSTATE(rwSTENCILOPERATIONKEEP));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONREF,       RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(rwSTENCILOPERATIONINCR));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(rwCULLMODECULLFRONT));

    Render(CRGBA{ 0, 0, 0, 255 });

    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(rwSTENCILOPERATIONDECR));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(rwCULLMODECULLBACK));

    Render(CRGBA{ 0, 0, 0, 255 });

    // WTF is up with these states?
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(FALSE)); // same state
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(rwSHADEMODEGOURAUD));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(rwCULLMODECULLBACK));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,                RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(TRUE)); // same state
    RwRenderStateSet(rwRENDERSTATESRCBLEND,                 RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,                RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(rwSHADEMODEFLAT));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,            RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONREF,       RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTION,          RWRSTATE(rwSTENCILFUNCTIONLESSEQUAL));
    RwRenderStateSet(rwRENDERSTATESTENCILFAIL,              RWRSTATE(rwSTENCILOPERATIONKEEP));
    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(rwSTENCILOPERATIONKEEP));
    RwRenderStateSet(rwRENDERSTATESTENCILPASS,              RWRSTATE(rwSTENCILOPERATIONKEEP));

    CSprite2d::InitPerFrame();
    CSprite2d::DrawRect(
        CRect{ 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT },
        CRGBA{ 0, 0, 0, (uint8)(50u * CTimeCycle::m_CurrentColours.m_nShadowStrength / 256) }
    );

    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(rwSHADEMODEGOURAUD));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(rwCULLMODECULLBACK));
}

// 0x70FAE0
void CStencilShadows::RenderForVehicle(CStencilShadowObject* object) {
    auto* entity = object->m_pOwner;

    // Long shadows for airborne heli/plane, short ones otherwise.
    // OG tests (entityType == VEHICLE && vehicleType == 3|4): vehicleType is CVehicle::m_nVehicleType
    // (VEHICLE_TYPE_HELI/PLANE = 3/4), distinct from the subtype tested for bikes below.
    auto fShadowLength = 5.0f; // 0x40A00000
    if (entity->GetType() == ENTITY_TYPE_VEHICLE) {
        const auto vehicleType = static_cast<const CVehicle*>(entity)->m_nVehicleType;
        if (vehicleType == VEHICLE_TYPE_HELI || vehicleType == VEHICLE_TYPE_PLANE) {
            fShadowLength = 40.0f; // 0x42200000
        }
    }

    const auto* colModel = entity->GetColModel();
    auto* colData  = colModel->m_pColData;
    if (!colData || colData->m_nNumShadowTriangles != object->m_NumShadowFaces) {
        return;
    }

    if (!entity->m_matrix) {
        entity->AllocateMatrix();
        entity->m_placement.UpdateMatrix(entity->m_matrix);
    }
    CMatrix* matrix = entity->m_matrix;

    // Bikes cast shadows from their lean matrix.
    if (entity->GetType() == ENTITY_TYPE_VEHICLE
        && static_cast<const CVehicle*>(entity)->m_nVehicleSubType == VEHICLE_TYPE_BIKE /* 9 */) {
        static_cast<CBike*>(entity)->CalculateLeanMatrix();
        matrix = &static_cast<CBike*>(entity)->m_mLeanMatrix;
    }

    CMatrix invMatrix;
    Invert(*matrix, invMatrix);
    const CVector sunDirLocalHi = invMatrix * s_SunPosNrm; // multiplied pos of sun?
    const CVector sunDirLocalLo = invMatrix * CVector{}; // multiplied pos of origin?

    const CVector sunDir = sunDirLocalHi - sunDirLocalLo;

    object->m_SizeOfShadowFacesData = 0;

    const auto numShadowVerts = colData->m_nNumShadowVertices;
    for (auto i = 0u; i < numShadowVerts; i++) {
        colData->GetShadTrianglePoint(s_ShadowTrianglePoints[i], i);
    }
    TransformPoints(
        reinterpret_cast<RwV3d*>(s_TransformedShadowTrianglePoints),
        numShadowVerts,
        *matrix,
        reinterpret_cast<RwV3d*>(s_ShadowTrianglePoints)
    );

    uint16 numSilhouetteEdges{};
    auto*  silhouetteEdges = s_ShadowTrianglePointsUnk;

    const auto  numShadowTris = object->m_NumShadowFaces;
    const auto* shadowTris    = colData->m_pShadowTriangles;
    for (auto i = 0; i < numShadowTris; i++) {
        const auto& tri = shadowTris[i];

        // Uncompressed verts for the facing test (local/model space).
        const CVector& v0 = s_ShadowTrianglePoints[tri.vA];
        const CVector& v1 = s_ShadowTrianglePoints[tri.vB];
        const CVector& v2 = s_ShadowTrianglePoints[tri.vC];

        // World-space verts for extrusion.
        const CVector& w0 = s_TransformedShadowTrianglePoints[tri.vA];
        const CVector& w1 = s_TransformedShadowTrianglePoints[tri.vB];
        const CVector& w2 = s_TransformedShadowTrianglePoints[tri.vC];

        const CVector edge1 = v2 - v0;
        const CVector edge2 = v1 - v0;
        const CVector edge3 = v2 - v1;

        // Triangle normal (unnormalized).
        const CVector normal = CVector{
            (v2.z - v1.z) * (v1.y - v0.y) - (v1.z - v0.z) * (v2.y - v1.y),
            edge1.x * (v2.y - v1.y) - (v2.z - v1.z) * edge2.x,
            (v1.z - v0.z) * edge2.x - edge1.x * (v1.y - v0.y),
        };
        CVector* out = object->m_ShadowFacesData + object->m_SizeOfShadowFacesData * 3;
        if (normal.Dot(sunDir) < 0.0f /* 0x858B50 */) {
            // Back-facing: record silhouette edges, they cancel out shared ones.
            CallAddShadowSilhouetteEdge(tri.vB, tri.vA, silhouetteEdges, &numSilhouetteEdges);
            CallAddShadowSilhouetteEdge(tri.vA, tri.vC, silhouetteEdges, &numSilhouetteEdges);
            CallAddShadowSilhouetteEdge(tri.vC, tri.vB, silhouetteEdges, &numSilhouetteEdges);

            // Already at capacity? (m_NumShadowFaces is the max triangle count)
            if (static_cast<uint32>(object->m_NumShadowFaces) <= object->m_SizeOfShadowFacesData + 3) {
                break;
            }

            // Extrude w0 along the sun direction.
            out[0] = w0 + s_SunPosNrm * fShadowLength;
            out[1] = w1 + s_SunPosNrm * fShadowLength;
            out[2] = w2 + s_SunPosNrm * fShadowLength;
            out[3] = w1;
            out[4] = w2;
            out[5] = w0 + s_SunPosNrm * fShadowLength;
            out[6] = w0;
        } else {
            // Front-facing: extrude the whole triangle into a 6-vert prism side?
            const CVector extr0 = w0 + s_SunPosNrm * fShadowLength;
            const CVector extr1 = w1 + s_SunPosNrm * fShadowLength;
            const CVector extr2 = w2 + s_SunPosNrm * fShadowLength;

            if (static_cast<uint32>(object->m_NumShadowFaces) <= object->m_SizeOfShadowFacesData + 3) {
                break;
            }

            out[0] = extr0;
            out[1] = extr1;
            out[2] = extr2;
            out[3] = w1;
            out[4] = w2;
            out[5] = w0;
            out[6] = w0;
        }
        out[7] = out[4];
        out[8] = out[5];
        object->m_SizeOfShadowFacesData += 3;
    }

    // Extrude silhouette edges into quads (2 triangles = 6 verts each).
    for (auto i = 0; i < numSilhouetteEdges; i++) {
        const CVector& w0 = s_TransformedShadowTrianglePoints[silhouetteEdges[i * 2 + 0]];
        const CVector& w1 = s_TransformedShadowTrianglePoints[silhouetteEdges[i * 2 + 1]];

        if (static_cast<uint32>(object->m_NumShadowFaces) <= object->m_SizeOfShadowFacesData + 6) {
            break;
        }

        auto* out = object->m_ShadowFacesData + object->m_SizeOfShadowFacesData * 3;
        out[0]  = w0;
        out[1]  = w1;
        out[2]  = w0 + s_SunPosNrm * fShadowLength;
        out[3]  = w1;
        out[4]  = w1 + s_SunPosNrm * fShadowLength;
        out[5]  = w0 + s_SunPosNrm * fShadowLength;
        out[6]  = w0;
        out[7]  = w1 + s_SunPosNrm * fShadowLength;
        out[8]  = w0 + s_SunPosNrm * fShadowLength;
        out[9]  = w0;
        out[10] = w1;
        out[11] = w1 + s_SunPosNrm * fShadowLength;
        out[12] = w0 + s_SunPosNrm * fShadowLength;
        out[13] = w0;
        out[14] = w1;
        out[15] = w1 + s_SunPosNrm * fShadowLength;
        out[16] = w0 + s_SunPosNrm * fShadowLength;
        out[17] = w0 + s_SunPosNrm * fShadowLength;
        object->m_SizeOfShadowFacesData += 6;
    }
}

// 0x710310
void CStencilShadows::RenderForObject(CStencilShadowObject* object) {
    auto* entity = object->m_pOwner;

    const auto* colModel = entity->GetColModel();
    auto* colData  = colModel->m_pColData;
    if (!colData || colData->m_nNumShadowTriangles != object->m_NumShadowFaces) {
        return;
    }

    if (!entity->m_matrix) {
        entity->AllocateMatrix();
        entity->m_placement.UpdateMatrix(entity->m_matrix);
    }
    CMatrix* matrix = entity->m_matrix;

    constexpr auto fShadowLength = 60.0f; // 0x858B34

    CMatrix invMatrix;
    Invert(*matrix, invMatrix);
    const CVector sunDirLocalHi = invMatrix * s_SunPosNrm; // multiplied pos of sun?
    const CVector sunDirLocalLo = invMatrix * CVector{}; // multiplied pos of origin?

    const CVector sunDir = sunDirLocalHi - sunDirLocalLo;

    object->m_SizeOfShadowFacesData = 0;

    const auto numShadowVerts = colData->m_nNumShadowVertices;
    for (auto i = 0u; i < numShadowVerts; i++) {
        colData->GetShadTrianglePoint(s_ShadowTrianglePoints[i], i);
    }
    TransformPoints(
        reinterpret_cast<RwV3d*>(s_TransformedShadowTrianglePoints),
        numShadowVerts,
        *matrix,
        reinterpret_cast<RwV3d*>(s_ShadowTrianglePoints)
    );

    uint16 numSilhouetteEdges{};
    auto*  silhouetteEdges = s_ShadowTrianglePointsUnk;

    const auto  numShadowTris = object->m_NumShadowFaces;
    const auto* shadowTris    = colData->m_pShadowTriangles;
    for (auto i = 0; i < numShadowTris; i++) {
        const auto& tri = shadowTris[i];

        const CVector& v0 = s_ShadowTrianglePoints[tri.vA];
        const CVector& v1 = s_ShadowTrianglePoints[tri.vB];
        const CVector& v2 = s_ShadowTrianglePoints[tri.vC];

        const CVector& w0 = s_TransformedShadowTrianglePoints[tri.vA];
        const CVector& w1 = s_TransformedShadowTrianglePoints[tri.vB];
        const CVector& w2 = s_TransformedShadowTrianglePoints[tri.vC];

        const CVector edge1 = v2 - v0;
        const CVector edge2 = v1 - v0;

        const CVector normal = CVector{
            (v2.z - v1.z) * (v1.y - v0.y) - (v1.z - v0.z) * (v2.y - v1.y),
            edge1.x * (v2.y - v1.y) - (v2.z - v1.z) * edge2.x,
            (v1.z - v0.z) * edge2.x - edge1.x * (v1.y - v0.y),
        };
        CVector* out = object->m_ShadowFacesData + object->m_SizeOfShadowFacesData * 3;
        if (normal.Dot(sunDir) < 0.0f /* 0x858B50 */) {
            CallAddShadowSilhouetteEdge(tri.vB, tri.vA, silhouetteEdges, &numSilhouetteEdges);
            CallAddShadowSilhouetteEdge(tri.vA, tri.vC, silhouetteEdges, &numSilhouetteEdges);
            CallAddShadowSilhouetteEdge(tri.vC, tri.vB, silhouetteEdges, &numSilhouetteEdges);

            if (static_cast<uint32>(object->m_NumShadowFaces) <= object->m_SizeOfShadowFacesData + 3) {
                break;
            }

            out[0] = w0 + s_SunPosNrm * fShadowLength;
            out[1] = w1 + s_SunPosNrm * fShadowLength;
            out[2] = w2 + s_SunPosNrm * fShadowLength;
            out[3] = w1;
            out[4] = w2;
            out[5] = w0 + s_SunPosNrm * fShadowLength;
            out[6] = w0;
        } else {
            const CVector extr0 = w0 + s_SunPosNrm * fShadowLength;
            const CVector extr1 = w1 + s_SunPosNrm * fShadowLength;
            const CVector extr2 = w2 + s_SunPosNrm * fShadowLength;

            if (static_cast<uint32>(object->m_NumShadowFaces) <= object->m_SizeOfShadowFacesData + 3) {
                break;
            }

            out[0] = extr0;
            out[1] = extr1;
            out[2] = extr2;
            out[3] = w1;
            out[4] = w2;
            out[5] = w0;
            out[6] = w0;
        }
        out[7] = out[4];
        out[8] = out[5];
        object->m_SizeOfShadowFacesData += 3;
    }

    for (auto i = 0; i < numSilhouetteEdges; i++) {
        const CVector& w0 = s_TransformedShadowTrianglePoints[silhouetteEdges[i * 2 + 0]];
        const CVector& w1 = s_TransformedShadowTrianglePoints[silhouetteEdges[i * 2 + 1]];

        if (static_cast<uint32>(object->m_NumShadowFaces) <= object->m_SizeOfShadowFacesData + 6) {
            break;
        }

        auto* out = object->m_ShadowFacesData + object->m_SizeOfShadowFacesData * 3;
        out[0]  = w0;
        out[1]  = w1;
        out[2]  = w0 + s_SunPosNrm * fShadowLength;
        out[3]  = w1;
        out[4]  = w1 + s_SunPosNrm * fShadowLength;
        out[5]  = w0 + s_SunPosNrm * fShadowLength;
        out[6]  = w0;
        out[7]  = w1 + s_SunPosNrm * fShadowLength;
        out[8]  = w0 + s_SunPosNrm * fShadowLength;
        out[9]  = w0;
        out[10] = w1;
        out[11] = w1 + s_SunPosNrm * fShadowLength;
        out[12] = w0 + s_SunPosNrm * fShadowLength;
        out[13] = w0;
        out[14] = w1;
        out[15] = w1 + s_SunPosNrm * fShadowLength;
        out[16] = w0 + s_SunPosNrm * fShadowLength;
        out[17] = w0 + s_SunPosNrm * fShadowLength;
        object->m_SizeOfShadowFacesData += 6;
    }
}

// 0x711D90
void CStencilShadows::Process(CVector& cameraPos) {
    ZoneScoped;

    if (!GraphicsHighQuality()) {
        return;
    }

    static uint8 s_RegisterShadowCounter{}, s_RenderForObjCounter{};

    RegisterStencilShadows(cameraPos, ++s_RegisterShadowCounter % 8);

    // why do we even do this?
    s_ShadowTrianglePointsUnk         = (RxVertexIndex*)CMemoryMgr::Malloc(12'288 * sizeof(RxVertexIndex));
    s_ShadowTrianglePoints            = (CVector*)CMemoryMgr::Malloc(2'048 * sizeof(CVector));
    s_TransformedShadowTrianglePoints = (CVector*)CMemoryMgr::Malloc(2'048 * sizeof(CVector));

    auto i{ 0 };
    for (auto* obj = pFirstActiveStencilShadowObject; obj; obj = obj->m_pNext) {
        switch (obj->m_Type) {
        case eStencilShadowObjType::OBJECT:
            if ((i++ % 4) == s_RenderForObjCounter) {
                RenderForObject(obj);
            }
            break;
        case eStencilShadowObjType::VEHICLE:
            RenderForVehicle(obj);
            break;
        default:
            break;
        }
    }
    s_RenderForObjCounter = (s_RenderForObjCounter + 1) % 4;

    CMemoryMgr::Free(std::exchange(s_ShadowTrianglePointsUnk, nullptr));
    CMemoryMgr::Free(std::exchange(s_ShadowTrianglePoints, nullptr));
    CMemoryMgr::Free(std::exchange(s_TransformedShadowTrianglePoints, nullptr));
}

// 0x70F9B0
bool CStencilShadows::GraphicsHighQuality() {
    return ::GraphicsHighQuality();
}

// 0x710BC0
void CStencilShadows::UpdateHierarchy(CStencilShadowObject*& firstAvailable, CStencilShadowObject*& firstActive, CStencilShadowObject* newOne) {
    if (auto* prev = newOne->m_pPrev) {
        auto* next = newOne->m_pNext;
        if (next) {
            next->m_pPrev = prev;
            newOne->m_pPrev->m_pNext = newOne->m_pNext;
        } else {
            prev->m_pNext = nullptr;
        }
    } else {
        auto* next     = newOne->m_pNext;
        firstAvailable = next;
        if (next) {
            next->m_pPrev = nullptr;
        }
    }
    newOne->m_pNext = firstActive;
    newOne->m_pPrev = nullptr;
    firstActive     = newOne;

    if (newOne->m_pNext) {
        newOne->m_pNext->m_pPrev = newOne;
    }
}

// 0x70FA70-related helpers for RegisterStencilShadows below (originals at 0x710BA0/0x711160/0x7111F0/0x711280).
// Kept file-local: the originals are unreversed free functions with custom conventions/globals we can't name yet.
namespace {
// 0x710BA0 - find active shadow by owner.
CStencilShadowObject* FindActiveStencilShadow(const CEntity* owner) {
    for (auto* obj = CStencilShadows::pFirstActiveStencilShadowObject; obj; obj = obj->m_pNext) {
        if (obj->m_pOwner == owner) {
            return obj;
        }
    }
    return nullptr;
}

// 0x7111F0 / 0x71160 - (dist(camera, entity bound sphere center) - radius)^2 * k, k = +1 outside / -1 inside.
// The two originals differ only in an implicit register arg (vehicle vs entity path); logic is identical.
float CalcStencilShadowDistSq(const CEntity* entity, const CVector& cameraPos) {
    const auto* colModel = entity->GetColModel();
    const auto center = entity->TransformFromObjectSpace(colModel->m_boundSphere.m_vecCenter);
    const auto dist = (cameraPos - center).Magnitude();
    const auto k = dist < colModel->m_boundSphere.m_fRadius ? -1.0f /* 0x858C1C */ : 1.0f; /* 0x858624 */
    const auto d = dist - colModel->m_boundSphere.m_fRadius;
    return k * d * d;
}
// 0x711280 - allocate a shadow slot for entity (type: 1 = OBJECT, 2 = VEHICLE). Returns false when full/invalid.
bool CreateStencilShadowSlot(CEntity* entity, eStencilShadowObjType type) {
    if (type != eStencilShadowObjType::OBJECT && type != eStencilShadowObjType::VEHICLE) {
        return false;
    }
    const auto* colData = entity->GetColModel()->m_pColData;
    if (!colData || !colData->bHasShadowInfo || colData->m_nNumShadowTriangles <= 0) {
        return false;
    }
    auto* obj = CStencilShadows::pFirstAvailableStencilShadowObject;
    obj->m_pOwner = entity;
    obj->m_NumShadowFaces = static_cast<int16>(colData->m_nNumShadowTriangles);
    obj->m_SizeOfShadowFacesData = 0;
    obj->m_Type = type;
    // m_FaceID doubles as the shadow volume buffer capacity (num CVector slots, 12 bytes each).
    obj->m_FaceID = colData->m_nNumShadowTriangles * 0xF;
    obj->m_ShadowFacesData = static_cast<CVector*>(CMemoryMgr::Malloc(obj->m_FaceID * sizeof(CVector)));
    CStencilShadows::UpdateHierarchy(
        CStencilShadows::pFirstAvailableStencilShadowObject,
        CStencilShadows::pFirstActiveStencilShadowObject,
        obj
    );
    return true;
}

// Sector-list scan shared by GetSector/GetRepeatSector loops: register OBJECT shadows for in-range, visible models.
void RegisterShadowsFromList(CEntity* entity, const CVector& cameraPos) {
    const auto* colModel = entity->GetColModel();
    if (!colModel || !colModel->m_pColData || !colModel->m_pColData->bHasShadowInfo) {
        return;
    }
    if (entity->IsScanCodeCurrent()) {
        return;
    }
    entity->SetCurrentScanCode();
    if (entity->GetAreaCode() != CGame::currArea && entity->GetAreaCode() != AREA_CODE_13) {
        return;
    }
    const auto* mi = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    if (mi->GetRwModelType() == 0) { // vtable +0x10: model visibility/type check; exact semantic TBD
        return;
    }
    if (FindActiveStencilShadow(entity)) {
        return;
    }
    if (CalcStencilShadowDistSq(entity, cameraPos) >= 2500.0f /* 0x8598B0 */) {
        return;
    }
    if (!CStencilShadows::pFirstAvailableStencilShadowObject) {
        return;
    }
    CreateStencilShadowSlot(entity, eStencilShadowObjType::OBJECT);
}
} // namespace

// 0x711760
void CStencilShadows::RegisterStencilShadows(CVector& cameraPos, bool doNotCreateNew) {
    if (doNotCreateNew) {
        // Prune dead/invisible shadows.
        for (auto* obj = pFirstActiveStencilShadowObject; obj;) {
            auto* next = obj->m_pNext;
            const auto* owner = obj->m_pOwner;
            if (!owner
                || (owner->GetType() == ENTITY_TYPE_VEHICLE
                    && !owner->m_bUsesCollision
                    && !owner->m_bIsStatic
                    && !owner->m_bIsStaticWaitingForCollision)) {
                obj->Destroy();
            }
            obj = next;
        }
        return;
    }

    // Validate existing shadows, drop dead ones.
    // OG checks each active slot: dead owner, or (VEHICLE owner with !m_bUsesCollision && !static-bits)
    // is destroyed outright; then OBJECT slots are range-checked (>) while VEHICLE slots are
    // range-checked (<=: skip) and area-checked.
    for (auto* obj = pFirstActiveStencilShadowObject; obj;) {
        auto* next = obj->m_pNext;
        const auto* owner = obj->m_pOwner;
        bool drop = false;
        if (!owner) {
            drop = true;
        } else if (owner->GetType() == ENTITY_TYPE_VEHICLE
            && !owner->m_bUsesCollision
            && !owner->m_bIsStatic
            && !owner->m_bIsStaticWaitingForCollision) {
            drop = true;
        } else if (obj->m_Type == eStencilShadowObjType::OBJECT) {
            if (CalcStencilShadowDistSq(owner, cameraPos) > 2500.0f /* 0x8598B0 */) {
                drop = true;
            }
        } else if (obj->m_Type == eStencilShadowObjType::VEHICLE) {
            // OG: JZ-destroy past the area check when in range; area check uses `!= curr && != 13` drop.
            if (CalcStencilShadowDistSq(owner, cameraPos) > 2500.0f /* 0x8598B0 */) {
                drop = true;
            } else if (owner->GetAreaCode() != CGame::currArea && owner->GetAreaCode() != AREA_CODE_13) {
                drop = true;
            }
        }
        if (drop) {
            obj->Destroy();
        }
        obj = next;
    }

    if (!pFirstAvailableStencilShadowObject) {
        return;
    }

    // Register vehicle shadows.
    for (auto& vehicle : GetVehiclePool()->GetAllValid()) {
        const auto* colModel = vehicle.GetColModel();
        if (!colModel || !colModel->m_pColData
            || !colModel->m_pColData->bHasShadowInfo
            || !vehicle.m_pHandlingData /* +0xB0-ish: original tests dword at +0xB0, likely handling data */) {
            continue;
        }
        if (FindActiveStencilShadow(&vehicle)) {
            continue;
        }
        if (CalcStencilShadowDistSq(&vehicle, cameraPos) >= 2500.0f /* 0x8598B0 */) {
            continue;
        }
        if (!pFirstAvailableStencilShadowObject) {
            return;
        }
        CreateStencilShadowSlot(&vehicle, eStencilShadowObjType::VEHICLE);
        if (!pFirstAvailableStencilShadowObject) {
            return;
        }
    }

    if (!pFirstAvailableStencilShadowObject) {
        return;
    }

    // World-space to sector-range mapping (50.0 = 0x858B40, 60.0 = 0x858B34, 0.02 = 0x858B38).
    const auto minX = (cameraPos.x - 50.0f /* 0x858B40 */) * 0.02f /* 0x858B38 */ + 60.0f; /* 0x858B34 */
    const auto minY = (cameraPos.y - 50.0f /* 0x858B40 */) * 0.02f /* 0x858B38 */ + 60.0f; /* 0x858B34 */
    const auto maxX = (cameraPos.x + 50.0f /* 0x858B40 */) * 0.02f /* 0x858B38 */ + 60.0f; /* 0x858B34 */
    const auto maxY = (cameraPos.y + 50.0f /* 0x858B40 */) * 0.02f /* 0x858B38 */ + 60.0f; /* 0x858B34 */

    const auto x0 = std::clamp<int32>(static_cast<int32>(floor(minX)), 0, 0x77);
    const auto y0 = std::clamp<int32>(static_cast<int32>(floor(minY)), 0, 0x77);
    const auto x1 = std::clamp<int32>(static_cast<int32>(floor(maxX)), 0, 0x77);
    const auto y1 = std::clamp<int32>(static_cast<int32>(floor(maxY)), 0, 0x77);

    CWorld::AdvanceCurrentScanCode();

    // Scan static sectors + repeat sectors for shadow-casting buildings/objects.
    for (auto x = x0; x <= x1; x++) {
        for (auto y = y0; y <= y1; y++) {
            for (auto* entity : CWorld::GetSector(x, y).Buildings) {
                RegisterShadowsFromList(entity, cameraPos);
                if (!pFirstAvailableStencilShadowObject) {
                    return;
                }
            }
            for (auto* const entity : CWorld::GetRepeatSector(x, y).Objects) {
                RegisterShadowsFromList(entity, cameraPos);
                if (!pFirstAvailableStencilShadowObject) {
                    return;
                }
            }
        }
    }
}
