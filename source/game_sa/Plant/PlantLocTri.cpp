#include "StdInc.h"

#include "PlantLocTri.h"
#include "PlantMgr.h"
#include "PlantSurfPropMgr.h"
#include "ProcObjectMan.h"

// 0x5DC210
float GetPlantDensity(CPlantLocTri* plant) {
    // The magnitude of the cross product of 2 vectors give the area of a paralellogram they span.
    return (plant->m_V2 - plant->m_V1).Cross(plant->m_V3 - plant->m_V1).Magnitude() / 2.f;
}

void CPlantLocTri::InjectHooks() {
    RH_ScopedClass(CPlantLocTri);
    RH_ScopedCategory("Plant");

    RH_ScopedInstall(Add, 0x5DC290);
    RH_ScopedInstall(Release, 0x5DB6D0);

    RH_ScopedGlobalInstall(GetPlantDensity, 0x5DC210);
}

// 0x5DC290
CPlantLocTri* CPlantLocTri::Add(const CVector& p1, const CVector& p2, const CVector& p3, uint8 surface, tColLighting lightning, bool createsPlants, bool createsObjects) {
    m_V1 = p1;
    m_V2 = p2;
    m_V3 = p3;

    m_nLighting = lightning;
    m_SurfaceId = surface;
    m_createsObjects = createsObjects;
    m_createsPlants = createsPlants;
    m_createdObjects = false;

    m_Center = (p1 + p2 + p3) / 3.0f;

    m_SphereRadius = DistanceBetweenPoints(m_Center, m_V1) * 1.75f;
    if (m_createsObjects && !m_createsPlants) {
        CPlantMgr::MoveLocTriToList(CPlantMgr::m_UnusedLocTriListHead, CPlantMgr::m_CloseLocTriListHead[3], this);
        return this;
    }

    auto properties = CPlantSurfPropMgr::GetSurfProperties(surface);
    auto densityMult = GetPlantDensity(this);
    auto density1 = (uint16)(densityMult * properties->m_PlantData[0].m_fDensity);
    auto density2 = (uint16)(densityMult * properties->m_PlantData[1].m_fDensity);
    auto density3 = (uint16)(densityMult * properties->m_PlantData[2].m_fDensity);
    if (density1 + density2 + density3 > 0) {
        rng::generate(m_Seed, [] { return CGeneral::GetRandomNumberInRange(0.0f, 1.0f); });
        m_nMaxNumPlants[0] = density1;
        m_nMaxNumPlants[1] = density2;
        m_nMaxNumPlants[2] = density3;
        CPlantMgr::MoveLocTriToList(CPlantMgr::m_UnusedLocTriListHead, CPlantMgr::m_CloseLocTriListHead[properties->m_nPlantSlotID], this);
        return this;
    }

    if (m_createsObjects) {
        // m_nFlags = m_nFlags & 0xFE;
        m_createsPlants = false;

        CPlantMgr::MoveLocTriToList(CPlantMgr::m_UnusedLocTriListHead, CPlantMgr::m_CloseLocTriListHead[3], this);
        return this;
    }

    return nullptr;
}

// 0x5DB6D0
void CPlantLocTri::Release() {
    rng::fill(m_nMaxNumPlants, 0u);
    if (m_createdObjects)
        g_procObjMan.ProcessTriangleRemoved(this);

    if (!m_createsObjects || m_createsPlants) {
        auto head = &CPlantMgr::m_CloseLocTriListHead[CPlantSurfPropMgr::GetSurfProperties(m_SurfaceId)->m_nPlantSlotID];
        if (auto prev = m_PrevTri) {
            if (auto next = m_NextTri) {
                next->m_PrevTri = prev;
                prev->m_NextTri = next;
            } else {
                prev->m_NextTri = nullptr;
            }
        } else {
            auto next = m_NextTri;
            *head = next;
            if (next)
                next->m_PrevTri = nullptr;
        }
        m_NextTri = CPlantMgr::m_UnusedLocTriListHead;
        m_PrevTri = nullptr;
        CPlantMgr::m_UnusedLocTriListHead = this;
        if (m_NextTri) {
            m_NextTri->m_PrevTri = this;
        }

        m_SurfaceId = 0xFF;
    } else {
        if (auto prev = m_PrevTri) {
            if (auto next = m_NextTri) {
                next->m_PrevTri = prev;
                prev->m_NextTri = next;
            } else {
                prev->m_NextTri = nullptr;
            }
        } else {
            CPlantMgr::m_CloseLocTriListHead[3] = m_NextTri;
            if (CPlantMgr::m_CloseLocTriListHead[3])
                CPlantMgr::m_CloseLocTriListHead[3]->m_PrevTri = nullptr;
        }
        m_NextTri = CPlantMgr::m_UnusedLocTriListHead;
        m_PrevTri = nullptr;
        CPlantMgr::m_UnusedLocTriListHead = this;
        if (m_NextTri) {
            m_NextTri->m_PrevTri = this;
        }

        m_SurfaceId = 0xFE;
    }

    // m_nFlags = m_nFlags & 0xF8;
    m_createsPlants = false;
    m_createsObjects = false;
    m_createdObjects = false;
}
