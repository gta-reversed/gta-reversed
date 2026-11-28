#include "StdInc.h"
#include "StencilShadowObject.h"
#include "RenderBuffer.hpp"

// TODO: Statically allocate after reversing RenderForVehicle&RenderForObject.
static inline auto& s_ShadowTrianglePointsUnk         = StaticRef<RxVertexIndex*>(0xC6A170);
static inline auto& s_ShadowTrianglePoints            = StaticRef<CVector*>(0xC6A174);
static inline auto& s_TransformedShadowTrianglePoints = StaticRef<CVector*>(0xC6A178);
static inline auto& s_SunPosNrm                       = StaticRef<CVector>(0x8D5244); // CVector(1.0, 1.0, -2.0)

namespace {
constexpr auto VEHICLE_SHADOW_LENGTH        = 5.f;
constexpr auto FLYING_VEHICLE_SHADOW_LENGTH = 40.f;
constexpr auto OBJECT_SHADOW_LENGTH         = 60.f;

//! An edge of the silhouette, `s_ShadowTrianglePointsUnk` holds these (as 2 vertex indices each)
struct ShadowEdge {
    RxVertexIndex a, b;
};

// 0x70FA70
void ToggleEdge(ShadowEdge* edges, uint16& numEdges, RxVertexIndex a, RxVertexIndex b) {
    // An edge shared by 2 faces facing the sun isn't on the silhouette, so it's removed when it's seen again
    for (auto i = 0; i < numEdges; i++) {
        auto& e = edges[i];
        if (e.a == a && e.b == b || e.a == b && e.b == a) {
            e = edges[--numEdges];
            return;
        }
    }
    edges[numEdges++] = { a, b };
}

//! Add vertices to the shadow volume, returns false if they don't fit
bool AddShadowVertices(CStencilShadowObject& object, std::initializer_list<CVector> vertices) {
    // NOTE: `m_FaceID` is the size of `m_ShadowFacesData` (15 vertices per shadow face, set in 0x711280)
    if (object.m_SizeOfShadowFacesData + vertices.size() >= object.m_FaceID) {
        return false;
    }
    rng::copy(vertices, object.m_ShadowFacesData + object.m_SizeOfShadowFacesData);
    object.m_SizeOfShadowFacesData += vertices.size();
    return true;
}

//! NOTSA: The common part of `RenderForVehicle` and `RenderForObject`
void BuildShadowVolume(CStencilShadowObject& object, CCollisionData& colData, const CMatrix& mat, float shadowLength) {
    // NOTE: The original transforms the sun as `Invert(mat) * sun - Invert(mat) * CVector{}`, only the sign of the dot product below uses it
    const auto sunDir              = mat.InverseTransformVector(s_SunPosNrm);

    object.m_SizeOfShadowFacesData = 0;

    auto* const points = s_ShadowTrianglePoints;
    auto* const world  = s_TransformedShadowTrianglePoints;
    for (auto i = 0; i < (int32)colData.m_nNumShadowVertices; i++) {
        colData.GetShadTrianglePoint(points[i], i);
    }
    TransformPoints(world, (int32)colData.m_nNumShadowVertices, mat, points);

    auto* const edges    = reinterpret_cast<ShadowEdge*>(s_ShadowTrianglePointsUnk);
    uint16      numEdges = 0;

    const auto offset    = s_SunPosNrm * shadowLength;
    for (auto i = 0; i < object.m_NumShadowFaces; i++) {
        const auto& tri = colData.m_pShadowTriangles[i];
        const auto& a   = points[tri.vA];
        if ((points[tri.vB] - a).Cross(points[tri.vC] - a).Dot(sunDir) >= 0.f) { // 0x70fee9 - Facing the sun: the near cap + its edges
            ToggleEdge(edges, numEdges, tri.vA, tri.vB);
            ToggleEdge(edges, numEdges, tri.vB, tri.vC);
            ToggleEdge(edges, numEdges, tri.vC, tri.vA);
            if (!AddShadowVertices(object, { world[tri.vA], world[tri.vC], world[tri.vB] })) {
                break;
            }
        } else if (!AddShadowVertices(object, { world[tri.vA] + offset, world[tri.vC] + offset, world[tri.vB] + offset })) { // 0x70ffa4 - The far cap
            break;
        }
    }

    // 0x7100ff - The sides of the volume
    for (const auto& e : std::span{ edges, numEdges }) {
        const auto& a = world[e.a];
        const auto& b = world[e.b];
        if (!AddShadowVertices(object, { a, b, a + offset, b, b + offset, a + offset })) {
            break;
        }
    }
}
}; // namespace

void CStencilShadows::InjectHooks() {
    RH_ScopedClass(CStencilShadows);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x70F9E0);
    RH_ScopedInstall(Shutdown, 0x711390);
    RH_ScopedInstall(Process, 0x711D90);
    RH_ScopedInstall(GraphicsHighQuality, 0x70F9B0);
    RH_ScopedInstall(UpdateHierarchy, 0x710BC0);
    RH_ScopedInstall(RegisterStencilShadows, 0x711760, {.Reversed = false});
    RH_ScopedInstall(RenderStencilShadows, 0x7113B0);
    RH_ScopedInstall(RenderForVehicle, 0x70FAE0);
    RH_ScopedInstall(RenderForObject, 0x710310);
    RH_ScopedInstall(Render, 0x710D50);
    RH_ScopedInstall(RenderBuffer, 0x710B50);
    RH_ScopedInstall(SunSetPositionFromEntity, 0x710AF0);
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
    ::RenderBuffer::ClearRenderBuffer();

    const auto PushTriangles = [&](std::span<const CVector> vertices) {
        sub_710CC0((int32)vertices.size(), (int32)vertices.size());

        // NOTE: The original uses the number of stored indices as the base, it's always the same as the number of stored vertices here
        for (auto i = 0u; i < vertices.size(); i++) {
            ::RenderBuffer::PushIndex((int32)i, true);
        }
        for (const auto& vertex : vertices) {
            ::RenderBuffer::PushVertex(vertex, color);
        }
    };
    for (auto* obj = pFirstActiveStencilShadowObject; obj; obj = obj->m_pNext) {
        const auto vertices = std::span<const CVector>{ obj->m_ShadowFacesData, obj->m_SizeOfShadowFacesData };

        // 2 triangles at a time, then the last one
        auto i = 0u;
        for (; i + 6 <= vertices.size(); i += 6) {
            PushTriangles(vertices.subspan(i, 6));
        }
        if (vertices.size() - i >= 3) { // 0x710fbd
            PushTriangles(vertices.subspan(i, 3));
        }
    }

    // 0x7110fa
    if (uiTempBufferIndicesStored && uiTempBufferVerticesStored) {
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(nullptr));
        LittleTest();
        if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
            RwIm3DEnd();
        }
        ::RenderBuffer::ClearRenderBuffer();
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
bool CStencilShadows::RenderForVehicle(CStencilShadowObject* object) {
    auto* const owner        = object->m_pOwner;
    const auto  shadowLength = owner->GetIsTypeVehicle() && (owner->AsVehicle()->IsSubPlane() || owner->AsVehicle()->IsSubHeli())
        ? FLYING_VEHICLE_SHADOW_LENGTH
        : VEHICLE_SHADOW_LENGTH;

    auto* const colData      = owner->GetColData();
    if (!colData || (int32)colData->m_nNumShadowTriangles != object->m_NumShadowFaces) {
        return false;
    }

    const CMatrix* mat = &owner->GetMatrix();
    if (owner->GetIsTypeVehicle() && owner->AsVehicle()->IsBike()) { // 0x70fb80
        owner->AsBike()->CalculateLeanMatrix();
        mat = &owner->AsBike()->m_mLeanMatrix;
    }
    BuildShadowVolume(*object, *colData, *mat, shadowLength);
    return true;
}

// 0x710310
bool CStencilShadows::RenderForObject(CStencilShadowObject* object) {
    auto* const owner   = object->m_pOwner;
    auto* const colData = owner->GetColData();
    if (!colData || (int32)colData->m_nNumShadowTriangles != object->m_NumShadowFaces) {
        return false;
    }
    BuildShadowVolume(*object, *colData, owner->GetMatrix(), OBJECT_SHADOW_LENGTH);
    return true;
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

// 0x711760
void CStencilShadows::RegisterStencilShadows(CVector& cameraPos, bool doNotCreateNew) {
}
