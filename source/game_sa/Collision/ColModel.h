/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "BoundingBox.h"
#include "ColSphere.h"
#include "CollisionData.h"

class CColModel {
public:
    CBoundingBox m_boundBox;
    CSphere      m_boundSphere;
    uint8        m_nColSlot;
    union {
        struct {
            uint8 m_bHasCollisionVolumes : 1; // AKA `m_bNotEmpty`
            uint8 m_bIsSingleColDataAlloc : 1;
            uint8 m_bIsActive : 1;
        };
        uint8 m_nFlags;
    };
    CCollisionData* m_pColData;

public:
    static void InjectHooks();

    CColModel();
    ~CColModel();

    static void* operator new(unsigned size);
    static void operator delete(void* data);
    CColModel& operator=(const CColModel& colModel);

public:
    void AllocateData();
    void AllocateData(int32 numSpheres, int32 numBoxes, int32 numLines, int32 numVertices, int32 numTriangles, bool bUsesDisks);
    void MakeMultipleAlloc();
    void RemoveCollisionVolumes();
    void CalculateTrianglePlanes();
    void RemoveTrianglePlanes();

private:
    void AllocateData(int32 size);

public:
    // HELPERS
    [[nodiscard]] auto GetTriCount() const noexcept     { return m_pColData ? m_pColData->m_nNumTriangles : 0u; }
    [[nodiscard]] float GetBoundRadius() const noexcept { return m_boundSphere.m_fRadius; }
    auto& GetBoundCenter() { return m_boundSphere.m_vecCenter; }
    auto& GetBoundingBox() { return m_boundBox; }
    auto& GetBoundingSphere() { return m_boundSphere; }
    CCollisionData* GetData() const { return m_pColData; }
};

VALIDATE_SIZE(CColModel, 0x30);
