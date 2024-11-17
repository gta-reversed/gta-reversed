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
    m_pEntity = entity;
    entity->RegisterReference(&m_pEntity);

    auto cd = entity->GetColData();
    if (!cd || cd->m_nNumTriangles < 1u) {
        return nullptr;
    }
    m_nNumTris = cd->m_nNumTriangles;
    m_LocTriArray = (CPlantLocTri**)CMemoryMgr::Malloc(m_nNumTris * sizeof(CPlantLocTri*));
    for (int i = 0; i < m_nNumTris; i++)
        m_LocTriArray[i] = nullptr;

    if (auto* prev = m_pPrevEntry) {
        if (auto* next = m_pNextEntry) {
            next->m_pPrevEntry = prev;
            prev->m_pNextEntry = next;
        } else {
            prev->m_pNextEntry = nullptr;
        }
    } else {
        CPlantMgr::m_UnusedColEntListHead = m_pNextEntry;
        if (m_pNextEntry) {
            m_pNextEntry->m_pPrevEntry = nullptr;
        }
    }
    m_pNextEntry = CPlantMgr::m_CloseColEntListHead;
    m_pPrevEntry = nullptr;
    CPlantMgr::m_CloseColEntListHead = this;

    if (m_pNextEntry)
        m_pNextEntry->m_pPrevEntry = this;

    return this;
}

// 0x5DB8A0
void CPlantColEntEntry::ReleaseEntry() {
    if (m_LocTriArray) {
        if (m_nNumTris) {
            for (auto& tri : std::span{m_LocTriArray, m_nNumTris}) {
                if (tri) {
                    tri->Release();
                }
            }
        }
        CMemoryMgr::Free(m_LocTriArray);
        m_LocTriArray = nullptr;
        m_nNumTris = 0u;
    }
    CEntity::SafeCleanUpRef(m_pEntity);
    m_pEntity = nullptr;

    if (auto prev = m_pPrevEntry) {
        if (auto next = m_pNextEntry) {
            next->m_pPrevEntry = prev;
            prev->m_pNextEntry = next;
        } else {
            prev->m_pNextEntry = nullptr;
        }
    } else {
        if (CPlantMgr::m_CloseColEntListHead = m_pNextEntry) {
            CPlantMgr::m_CloseColEntListHead->m_pPrevEntry = nullptr;
        }
    }
    m_pNextEntry = CPlantMgr::m_UnusedColEntListHead;
    m_pPrevEntry = nullptr;
    CPlantMgr::m_UnusedColEntListHead = this;
    if (m_pNextEntry) {
        m_pNextEntry->m_pPrevEntry = this;
    }
}
