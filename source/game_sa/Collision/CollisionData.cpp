#include "StdInc.h"

#include "CollisionData.h"
#include "ColHelpers.h"

void CCollisionData::InjectHooks() {
    RH_ScopedClass(CCollisionData);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(RemoveCollisionVolumes, 0x40F070);
    RH_ScopedInstall(RemoveTrianglePlanes, 0x40F6A0);
    RH_ScopedInstall(Copy, 0x40F120);
    RH_ScopedInstall(GetTrianglePoint, 0x40F5E0);
    RH_ScopedInstall(GetShadTrianglePoint, 0x40F640);
    RH_ScopedInstall(CalculateTrianglePlanes, 0x40F590);
    RH_ScopedInstall(SetLinkPtr, 0x40F6C0);
    RH_ScopedInstall(GetLinkPtr, 0x40F6E0);
}

// 0x40F030
CCollisionData::CCollisionData() {
    bUsesDisks = false;
    bHasFaceGroups = false;
    bHasShadowInfo = false;

    m_nNumSpheres = 0;
    m_nNumBoxes = 0;
    m_nNumTriangles = 0;
    m_nNumLines = 0;

    m_pSpheres = nullptr;
    m_pBoxes = nullptr;
    m_pLines = nullptr;
    m_pVertices = nullptr;
    m_pTriangles = nullptr;
    m_pTrianglePlanes = nullptr;

    m_nNumShadowTriangles = 0;
    m_nNumShadowVertices = 0;
    m_pShadowTriangles = nullptr;
    m_pShadowVertices = nullptr;
    /*
     * todo: initialize bHasFaceGroups, bHasShadow, m_pDisks as NOTSA or something
     * */
}

// 0x40F070
void CCollisionData::RemoveCollisionVolumes() {
    CMemoryMgr::Free(m_pSpheres);
    CMemoryMgr::Free(m_pLines);
    CMemoryMgr::Free(m_pBoxes);
    CMemoryMgr::Free(m_pVertices);
    CMemoryMgr::Free(m_pTriangles);
    CMemoryMgr::Free(m_pShadowTriangles);
    CMemoryMgr::Free(m_pShadowVertices);

    CCollision::RemoveTrianglePlanes(this);

    m_nNumSpheres = 0;
    m_nNumLines = 0;
    m_nNumBoxes = 0;
    m_nNumTriangles = 0;
    m_pSpheres = nullptr;
    m_pLines = nullptr;
    m_pBoxes = nullptr;
    m_pVertices = nullptr;
    m_pTriangles = nullptr;
    m_pShadowTriangles = nullptr;
    m_pShadowVertices = nullptr;
}

// 0x40F120
void CCollisionData::Copy(const CCollisionData& src) {
    assert(!bHasFaceGroups); // Avoid possible random bugs - See header for more info.

    // ----- SPHERES -----
    if (m_nNumSpheres != src.m_nNumSpheres || !src.m_nNumSpheres) {
        m_nNumSpheres = src.m_nNumSpheres;
        CMemoryMgr::Free(m_pSpheres);
        m_pSpheres = nullptr;

        if (src.m_nNumSpheres)
            m_pSpheres = static_cast<CColSphere*>(CMemoryMgr::Malloc(m_nNumSpheres * sizeof(CColSphere)));
    }
    for (auto i = 0; i < m_nNumSpheres; ++i) {
        m_pSpheres[i] = src.m_pSpheres[i];
    }

    // ----- LINES / DISKS -----
    if (m_nNumLines != src.m_nNumLines || !src.m_nNumLines || bUsesDisks != src.bUsesDisks) {
        bUsesDisks = src.bUsesDisks;
        m_nNumLines = src.m_nNumLines;
        CMemoryMgr::Free(m_pLines);
        m_pLines = nullptr;

        if (src.m_nNumLines) {
            if (src.bUsesDisks)
                m_pDisks = static_cast<CColDisk*>(CMemoryMgr::Malloc(m_nNumLines * sizeof(CColDisk)));
            else
                m_pLines = static_cast<CColLine*>(CMemoryMgr::Malloc(m_nNumLines * sizeof(CColLine)));
        }
    }
    for (auto i = 0; i < m_nNumLines; ++i) {
        if (bUsesDisks)
            m_pDisks[i] = src.m_pDisks[i];
        else
            m_pLines[i] = src.m_pLines[i];
    }

    // ----- BOXES -----
    if (m_nNumBoxes != src.m_nNumBoxes || !src.m_nNumBoxes) {
        m_nNumBoxes = src.m_nNumBoxes;
        CMemoryMgr::Free(m_pBoxes);
        m_pBoxes = nullptr;

        if (src.m_nNumBoxes) {
            m_pBoxes = static_cast<CColBox*>(CMemoryMgr::Malloc(m_nNumBoxes * sizeof(CColBox)));
        }
    }
    for (auto i = 0; i < m_nNumBoxes; ++i) {
        m_pBoxes[i] = src.m_pBoxes[i];
    }

    // ----- TRIANGLES & VERTICES -----
    if (m_nNumTriangles != src.m_nNumTriangles || !src.m_nNumTriangles) {
        m_nNumTriangles = src.m_nNumTriangles;
        CMemoryMgr::Free(m_pTriangles);
        CMemoryMgr::Free(m_pVertices);
        m_pTriangles = nullptr;
        m_pVertices = nullptr;

        if (src.m_nNumTriangles) {
            // Get number of vertices
            uint16 iHighestVertInd = 0;
            for (auto i = 0; i < src.m_nNumTriangles; ++i)
                iHighestVertInd = std::max({iHighestVertInd, src.m_pTriangles[i].vA, src.m_pTriangles[i].vB, src.m_pTriangles[i].vC});

            if (iHighestVertInd) {
                iHighestVertInd++; // allocated space needs to be 1 bigger to compensate for index 0
                m_pVertices = static_cast<CompressedVector*>(CMemoryMgr::Malloc(iHighestVertInd * sizeof(CompressedVector)));
                for (auto i = 0; i < iHighestVertInd; ++i) {
                    m_pVertices[i] = src.m_pVertices[i];
                }
            }

            m_pTriangles = static_cast<CColTriangle*>(CMemoryMgr::Malloc(m_nNumTriangles * sizeof(CColTriangle)));
        }
    }
    for (auto i = 0; i < m_nNumTriangles; ++i)
        m_pTriangles[i] = src.m_pTriangles[i];

    // ----- SHADOW TRIANGLES & SHADOW VERTICES -----
    if (m_nNumShadowTriangles != src.m_nNumShadowTriangles || !src.m_nNumShadowTriangles || bHasShadowInfo != src.bHasShadowInfo) {
        bHasShadowInfo = src.bHasShadowInfo;
        m_nNumShadowTriangles = src.m_nNumShadowTriangles;
        m_nNumShadowVertices = src.m_nNumShadowVertices;
        CMemoryMgr::Free(m_pShadowTriangles);
        CMemoryMgr::Free(m_pShadowVertices);
        m_pShadowTriangles = nullptr;
        m_pShadowVertices = nullptr;

        if (src.m_nNumShadowTriangles) {
            // Get number of vertices
            uint16 iHighestVertInd = 0;
            for (uint32 i = 0; i < src.m_nNumShadowTriangles; ++i)
                iHighestVertInd = std::max({iHighestVertInd, src.m_pShadowTriangles[i].vA, src.m_pShadowTriangles[i].vB, src.m_pShadowTriangles[i].vC});

            if (iHighestVertInd) {
                iHighestVertInd++; // allocated space needs to be 1 bigger to compensate for index 0
                m_pShadowVertices = static_cast<CompressedVector*>(CMemoryMgr::Malloc(iHighestVertInd * sizeof(CompressedVector)));
                for (auto i = 0; i < iHighestVertInd; ++i)
                    m_pShadowVertices[i] = src.m_pShadowVertices[i];
            }

            m_pShadowTriangles = static_cast<CColTriangle*>(CMemoryMgr::Malloc(m_nNumShadowTriangles * sizeof(CColTriangle)));
        }
    }
    for (uint32 i = 0; i < m_nNumShadowTriangles; ++i) {
        m_pShadowTriangles[i] = src.m_pShadowTriangles[i];
    }
}

// Memory layout: | CColTrianglePlane[] | (4 Byte aligned)CLink<CCollisionData*>* |
// 0x40F590
void CCollisionData::CalculateTrianglePlanes() {
    m_pTrianglePlanes = static_cast<CColTrianglePlane*>(CMemoryMgr::Malloc((m_nNumTriangles + 1) * sizeof(CColTrianglePlane)));
    for (auto i = 0; i < m_nNumTriangles; ++i) {
        m_pTrianglePlanes[i].Set(m_pVertices, m_pTriangles[i]);
    }
}

// 0x40F6A0
void CCollisionData::RemoveTrianglePlanes() {
    CMemoryMgr::Free(m_pTrianglePlanes);
    m_pTrianglePlanes = nullptr;
}

// 0x40F5E0
void CCollisionData::GetTrianglePoint(CVector& outVec, int32 vertId) {
    outVec = UncompressVector(m_pVertices[vertId]);
}

// 0x40F640
void CCollisionData::GetShadTrianglePoint(CVector& outVec, int32 vertId) {
    outVec = UncompressVector(m_pShadowVertices[vertId]);
}

// 0x40F6C0
void CCollisionData::SetLinkPtr(CLink<CCollisionData*>* link) {
    // Original calculation method:
    // const auto dwLinkAddress = (reinterpret_cast<uint32>(&m_pTrianglePlanes[m_nNumTriangles]) + 3) & 0xFFFFFFFC; // 4 bytes aligned address

    auto* linkPtr = static_cast<void*>(&m_pTrianglePlanes[m_nNumTriangles]);
    auto space = sizeof(CColTrianglePlane);
    auto* alignedAddress = std::align(4, sizeof(CLink<CCollisionData*>*), linkPtr, space); // 4 bytes aligned address
    *static_cast<CLink<CCollisionData*>**>(alignedAddress) = link;
}

// 0x40F6E0
CLink<CCollisionData*>* CCollisionData::GetLinkPtr() {
    // Original calculation method:
    // const auto dwLinkAddress = (reinterpret_cast<uint32>(&m_pTrianglePlanes[m_nNumTriangles]) + 3) & 0xFFFFFFFC; // 4 bytes aligned address

    auto* linkPtr = static_cast<void*>(&m_pTrianglePlanes[m_nNumTriangles]);
    auto space = sizeof(CColTrianglePlane);
    auto* alignedAddress = std::align(4, sizeof(CLink<CCollisionData*>*), linkPtr, space); // 4 bytes aligned address
    const auto l = *static_cast<CLink<CCollisionData*>**>(alignedAddress);
    assert(l->data == this); // Sanity check
    return l;
}

auto CCollisionData::GetNumFaceGroups() const -> uint32 {
    if (bHasFaceGroups) {
        assert(m_pTriangles);
        return *reinterpret_cast<uint32*>(reinterpret_cast<byte*>(m_pTriangles) - sizeof(uint32)); // See `CCollisionData` header for explanation :)
    }
    return 0;
}

auto CCollisionData::GetFaceGroups() const -> std::span<ColHelpers::TFaceGroup> {
    using namespace ColHelpers;
    if (const auto numfg = GetNumFaceGroups()) {
        assert(numfg);
        return std::span{ // See `CCollisionData` header for explanation
            reinterpret_cast<TFaceGroup*>(reinterpret_cast<uint8*>(m_pTriangles) - sizeof(uint32) - sizeof(TFaceGroup) * numfg),
            numfg
        };
    }
    return {};
}

auto CCollisionData::GetTriVertices(const CColTriangle& tri) const->std::array<CVector, 3> {
    std::array<CVector, 3> verts;
    for (auto&& [i, j] : notsa::enumerate(tri.m_vertIndices)) {
        verts[i] = UncompressVector(m_pVertices[j]);
    }
    return verts;
}

// NOTSA
void CCollisionData::AllocateLines(uint32 num) {
    m_nNumLines = num;
    m_pLines = static_cast<CColLine*>(CMemoryMgr::Malloc(sizeof(CColLine) * num));
}

// NOTSA
void CCollisionData::SetSpheres(const CColSphere* spheres) {
    for (auto i = 0; i < m_nNumSpheres; ++i) {
        m_pSpheres[i] = spheres[i];
    }
}
