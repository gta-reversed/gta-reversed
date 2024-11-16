#include "StdInc.h"

#include "PlantColEntEntry.h"
#include "PlantMgr.h"
#include "PlantLocTri.h"

void CPlantColEntEntry::InjectHooks() {
    RH_ScopedClass(CPlantColEntEntry);
    RH_ScopedCategory("Plant");

    RH_ScopedInstall(AddEntry, 0x5DB7D0);
    RH_ScopedInstall(ReleaseEntry, 0x5DB8A0);
}

// 0x5DB7D0
CPlantColEntEntry* CPlantColEntEntry::AddEntry(CEntity* entity) {
    m_Entity = entity;
    entity->RegisterReference(&m_Entity);

    auto cd = entity->GetColData();
    if (!cd || cd->m_nNumTriangles < 1u) {
        return nullptr;
    }
    m_numTriangles = cd->m_nNumTriangles;
    m_Objects = (CPlantLocTri**)CMemoryMgr::Calloc(m_numTriangles, sizeof(CPlantLocTri*)); // originally Malloc zeroed afterwards

    if (auto prev = m_PrevEntry) {
        if (auto next = m_NextEntry) {
            next->m_PrevEntry = prev;
            prev->m_NextEntry = next;
        } else {
            prev->m_NextEntry = nullptr;
        }
    } else {
        if (CPlantMgr::m_UnusedColEntListHead = m_NextEntry) {
            CPlantMgr::m_UnusedColEntListHead->m_PrevEntry = nullptr;
        }
    }
    m_NextEntry = CPlantMgr::m_CloseColEntListHead;
    m_PrevEntry = nullptr;
    CPlantMgr::m_CloseColEntListHead = this;

    if (m_NextEntry)
        m_NextEntry->m_PrevEntry = this;

    return this;
}

// 0x5DB8A0
void CPlantColEntEntry::ReleaseEntry() {
    if (m_Objects) {
        if (m_numTriangles) {
            for (auto& tri : std::span{m_Objects, m_numTriangles}) {
                if (tri) {
                    tri->Release();
                }
            }
        }
        CMemoryMgr::Free(m_Objects);
        m_Objects = nullptr;
        m_numTriangles = 0u;
    }
    CEntity::SafeCleanUpRef(m_Entity);
    m_Entity = nullptr;

    if (auto prev = m_PrevEntry) {
        if (auto next = m_NextEntry) {
            next->m_PrevEntry = prev;
            prev->m_NextEntry = next;
        } else {
            prev->m_NextEntry = nullptr;
        }
    } else {
        if (CPlantMgr::m_CloseColEntListHead = m_NextEntry) {
            CPlantMgr::m_CloseColEntListHead->m_PrevEntry = nullptr;
        }
    }
    m_NextEntry = CPlantMgr::m_UnusedColEntListHead;
    m_PrevEntry = nullptr;
    CPlantMgr::m_UnusedColEntListHead = this;
    if (m_NextEntry) {
        m_NextEntry->m_PrevEntry = this;
    }
}
