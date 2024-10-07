#include "StdInc.h"
#include "StencilShadowObject.h"

// TODO: Statically allocate after reversing RenderForVehicle&RenderForObject.
static inline auto& s_ShadowTrianglePointsUnk         = StaticRef<RxVertexIndex*>(0xC6A170);
static inline auto& s_ShadowTrianglePoints            = StaticRef<CVector*>(0xC6A174);
static inline auto& s_TransformedShadowTrianglePoints = StaticRef<CVector*>(0xC6A178);

void CStencilShadows::InjectHooks() {
    RH_ScopedClass(CStencilShadows);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x70F9E0);
    RH_ScopedInstall(Shutdown, 0x711390);
    RH_ScopedInstall(Process, 0x711D90);
    RH_ScopedInstall(GraphicsHighQuality, 0x70F9B0, {.reversed=false});
    RH_ScopedInstall(UpdateHierarchy, 0x710BC0);
    RH_ScopedInstall(RegisterStencilShadows, 0x711760, {.reversed=false});
    RH_ScopedInstall(RenderStencilShadows, 0x7113B0);
    RH_ScopedInstall(RenderForVehicle, 0x70FAE0, {.reversed=false});
    RH_ScopedInstall(RenderForObject, 0x710310, {.reversed=false});
    RH_ScopedInstall(Render, 0x710D50, {.reversed=false});
    RH_ScopedInstall(sub_710CC0, 0x710CC0);
}

// 0x70F9E0
void CStencilShadows::Init() {
    ZoneScoped;

    RwD3D9SetStencilClear(0);
    pFirstAvailableStencilShadowObject = m_StencilShadowObjects.data();
    pFirstActiveStencilShadowObject = nullptr;

    for (auto&& [i, obj] : notsa::enumerate(m_StencilShadowObjects)) {
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
    plugin::Call<0x710D50>();

    /*
    uiTempBufferIndicesStored = 0;
    uiTempBufferVerticesStored = 0;

    const auto intColor = color.ToInt();
    for (auto* shadow = pFirstActiveStencilShadowObject; shadow; shadow = shadow->m_pNext) {
        auto*      facePts  = shadow->m_ShadowFacesData;
        const auto numFaces = shadow->m_SizeOfShadowFacesData / sizeof(*facePts);
         
        for (auto face = 0u; face < numFaces; face++, facePts += 6) {
            auto points = std::span{ facePts, 6 };

            sub_710CC0(6, 6);

            auto nextVert = std::exchange(uiTempBufferVerticesStored, uiTempBufferVerticesStored + 6);
            auto nextIdx  = std::exchange(uiTempBufferIndicesStored, uiTempBufferIndicesStored + 6);
            for (auto i = 0; i < 6; i++) {
                auto& vert     = TempBufferVertices.m_3d[nextVert + i];
                vert.objVertex = points[0];
                vert.color     = intColor;

                aTempBufferIndices[nextIdx + i] = nextIdx + i;
            }
        }

        // what the fuck?
        // TODO: maybe this just boils down to size > 0 && size % 2 == 0
        const auto v14 = (0x55555556ull * shadow->m_SizeOfShadowFacesData) >> 32;
        if (((v14 & 0x80000000) != 0) + (v14 & 1) != 0) {
        }
    }
    */
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
    plugin::Call<0x70FAE0, CStencilShadowObject*>(object);
}

// 0x710310
void CStencilShadows::RenderForObject(CStencilShadowObject* object) {
    plugin::Call<0x710310, CStencilShadowObject*>(object);
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
