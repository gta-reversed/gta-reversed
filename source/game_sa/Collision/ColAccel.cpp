#include "StdInc.h"

#include "ColAccel.h"

CColAccelColBound*& CColAccel::m_colBounds = *(CColAccelColBound**)0xBC4090;
IplDef*& CColAccel::m_iplDefs = *(IplDef**)0xBC4094;
int32*& CColAccel::m_iSectionSize = *(int32**)0xBC4098;
int32& CColAccel::m_iCachingColSize = *(int32*)0xBC409C;
eColAccelState& CColAccel::m_iCacheState = *(eColAccelState*)0xBC40A0;
CColAccelColEntry*& CColAccel::mp_caccColItems = *(CColAccelColEntry**)0xBC40A4;
int32& CColAccel::m_iNumColItems = *(int32*)0xBC40A8;
CColAccelIPLEntry*& CColAccel::mp_caccIPLItems = *(CColAccelIPLEntry**)0xBC40AC;
int32& CColAccel::m_iNumIPLItems = *(int32*)0xBC40B0;
int32& CColAccel::m_iNumSections = *(int32*)0xBC40B4;
int32& CColAccel::m_iNumColBounds = *(int32*)0xBC40B8;
const char* CColAccel::mp_cCacheName = "MODELS\\CINFO.BIN"; // 0x8D0F84

void CColAccel::InjectHooks() {
    RH_ScopedClass(CColAccel);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(isCacheLoading, 0x5B2AC0);
    RH_ScopedInstall(endCache, 0x5B2AD0);
    RH_ScopedInstall(addCacheCol, 0x5B2C20);
    RH_ScopedInstall(cacheLoadCol, 0x5B2CC0);
    RH_ScopedInstall(addColDef, 0x5B2DD0);
    RH_ScopedInstall(getColDef, 0x5B2E60);
    RH_ScopedInstall(setIplDef, 0x5B2ED0);
    RH_ScopedInstall(getIplDef, 0x5B2EF0);
    RH_ScopedInstall(cacheIPLSection, 0x5B2F10);
    RH_ScopedInstall(addIPLEntity, 0x5B3040);
    RH_ScopedInstall(startCache, 0x5B31A0);
}

// 0x5B2AC0
bool CColAccel::isCacheLoading() {
    return m_iCacheState == eColAccelState::COLACCEL_LOADING;
}

// 0x5B31A0
void CColAccel::startCache() {
    ZoneScoped;

    m_iCachingColSize = GetColModelPool()->GetSize();
    m_iSectionSize    = new int32[64];
    m_iplDefs         = new IplDef[TOTAL_IPL_MODEL_IDS]();
    m_colBounds       = new CColAccelColBound[TOTAL_IPL_MODEL_IDS]();
}

// 0x5B2AD0
void CColAccel::endCache() {
    ZoneScoped;

    if (m_iCacheState == eColAccelState::COLACCEL_STARTED) {
        auto* file = CFileMgr::OpenFileForWriting(mp_cCacheName);
        CFileMgr::Write(file, &m_iNumColItems,  sizeof(m_iNumColItems));
        CFileMgr::Write(file, mp_caccColItems,  sizeof(CColAccelColEntry) * m_iNumColItems);
        CFileMgr::Write(file, &m_iNumSections,  sizeof(m_iNumSections));
        CFileMgr::Write(file, m_iSectionSize,   sizeof(m_iSectionSize) * m_iNumSections);
        CFileMgr::Write(file, m_iplDefs,        sizeof(IplDef) * TOTAL_IPL_MODEL_IDS);
        CFileMgr::Write(file, &m_iNumColBounds, sizeof(m_iNumColBounds));
        CFileMgr::Write(file, m_colBounds,      sizeof(CColAccelColBound) * m_iNumColBounds);
        CFileMgr::Write(file, &m_iNumIPLItems,  sizeof(m_iNumIPLItems));
        CFileMgr::Write(file, mp_caccIPLItems,  sizeof(CColAccelIPLEntry) * m_iNumIPLItems);
        CFileMgr::CloseFile(file);
    }

    delete[] mp_caccIPLItems;
    mp_caccIPLItems = nullptr;

    delete[] mp_caccColItems;
    mp_caccColItems = nullptr;

    delete[] m_iSectionSize;
    m_iSectionSize = nullptr;

    delete[] m_iplDefs;
    m_iplDefs = nullptr;

    delete[] m_colBounds;
    m_colBounds = nullptr;

    m_iCacheState = eColAccelState::COLACCEL_ENDED;
}

// signature changed
// 0x5B2C20
void CColAccel::addCacheCol(PackedModelStartEnd startEnd, const CColModel& colModel) {
    if (m_iCacheState != eColAccelState::COLACCEL_STARTED)
        return;

    auto& colEntry = mp_caccColItems[m_iNumColItems];
    colEntry.m_wModelStart = startEnd.wModelStart;
    colEntry.m_wModelEnd = startEnd.wModelEnd;
    colEntry.m_boundBox = colModel.m_boundBox;
    colEntry.m_boundSphere = colModel.m_boundSphere;
    colEntry.m_nColSlot = colModel.m_nColSlot;
    colEntry.m_bColModelNotEmpty = colModel.m_bHasCollisionVolumes;

    ++m_iNumColItems;
}

// 0x5B2CC0
void CColAccel::cacheLoadCol() {
    if (!isCacheLoading())
        return;

    for (auto& colEntry : std::span{ mp_caccColItems, (size_t)m_iNumColItems }) {
        auto* mi = CModelInfo::GetModelInfo(colEntry.m_wModelStart);
        auto* cm = new CColModel();

        cm->m_boundBox = colEntry.m_boundBox;
        cm->m_boundSphere = colEntry.m_boundSphere;
        cm->m_nColSlot = colEntry.m_nColSlot;
        cm->m_bHasCollisionVolumes = colEntry.m_bColModelNotEmpty;

        mi->SetColModel(cm, true);
        CColStore::IncludeModelIndex(colEntry.m_nColSlot, colEntry.m_wModelStart);
    }
}

// 0x5B2DD0
void CColAccel::addColDef(ColDef colDef) {
    auto& colBound = m_colBounds[m_iNumColBounds];
    colBound.m_Area = colDef.m_Area;
    colBound.m_wModelStart = colDef.m_nModelIdStart;
    colBound.m_wModelEnd = colDef.m_nModelIdEnd;
    colBound.m_bInterior = colDef.m_bInterior;
    colBound.m_bProcedural = colDef.m_bProcedural;

    ++m_iNumColBounds;
}

// 0x5B2E60
void CColAccel::getColDef(ColDef& colDef) {
    auto& colBound = m_colBounds[m_iNumColBounds];
    colDef.m_Area = colBound.m_Area;
    colDef.m_nModelIdStart = colBound.m_wModelStart;
    colDef.m_nModelIdEnd = colBound.m_wModelEnd;
    colDef.m_bInterior = colBound.m_bInterior;
    colDef.m_bProcedural = colBound.m_bProcedural;

    ++m_iNumColBounds;
}

// 0x5B2ED0
void CColAccel::setIplDef(int32 iplIndex, IplDef iplDef) {
    m_iplDefs[iplIndex] = iplDef;
}

// 0x5B2EF0
IplDef CColAccel::getIplDef(int32 iplIndex) {
    return m_iplDefs[iplIndex];
}

// 0x5B2F10
void CColAccel::cacheIPLSection(CEntity** ppEntities, int32 entitiesCount) {
    if (isCacheLoading()) {
        for (auto i = m_iNumIPLItems; i < m_iSectionSize[m_iNumSections]; ++i) {
            auto& iplEntry = mp_caccIPLItems[i];
            if (iplEntry.m_bIsUnderwater)
                ppEntities[iplEntry.m_nEntityIndex]->m_bUnderwater = true;

            if (iplEntry.m_bOwnsColModel)
                CModelInfo::GetModelInfo(iplEntry.m_nModelId)->SetOwnsColModel(true);

            if (iplEntry.m_bIsFarDrawDist)
                CModelInfo::GetModelInfo(iplEntry.m_nModelId)->m_fDrawDistance = 400.0F;

            if (iplEntry.m_bHasColModel) {
                auto* lodModelInfo = CModelInfo::GetModelInfo(iplEntry.m_nLodModelId);
                auto* entModelInfo = CModelInfo::GetModelInfo(iplEntry.m_nModelId);
                lodModelInfo->DeleteCollisionModel();
                lodModelInfo->SetColModel(entModelInfo->GetColModel(), false);
            }

            ++m_iNumIPLItems;
        }

        ++m_iNumSections;
    } else if (m_iCacheState == eColAccelState::COLACCEL_STARTED) {
        m_iSectionSize[m_iNumSections] = m_iNumIPLItems;
        ++m_iNumSections;
    }
}

// 0x5B3040
void CColAccel::addIPLEntity(CEntity** ppEntities, int32 entitiesCount, int32 entityIndex) {
    if (m_iCacheState != eColAccelState::COLACCEL_STARTED)
        return;

    auto& entity = ppEntities[entityIndex];
    auto* entModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);

    CColAccelIPLEntry iplEntry;
    iplEntry.m_nFlags = 0;
    iplEntry.m_nEntityIndex = entityIndex;
    iplEntry.m_nModelId = entity->m_nModelIndex;
    iplEntry.m_nLodModelId = entity->m_pLod->m_nModelIndex;
    iplEntry.m_nLodIndex = -1;

    if (entity->m_nNumLodChildren || TheCamera.m_fLODDistMultiplier * entModelInfo->m_fDrawDistance > 300.0F)
        iplEntry.m_bIsFarDrawDist = true;

    for (auto i = 0; i < entitiesCount; ++i) {
        if (ppEntities[i] != entity->m_pLod)
            continue;

        iplEntry.m_nLodIndex = i;
        break;
    }

    auto* lodModelInfo = CModelInfo::GetModelInfo(iplEntry.m_nLodModelId);
    if (entity->m_pLod->m_nNumLodChildren == 1) {
        if (entity->m_bUnderwater)
            iplEntry.m_bIsUnderwater = true;

        if (lodModelInfo->GetColModel() != entModelInfo->GetColModel() || !entModelInfo->GetColModel())
            iplEntry.m_bHasColModel = true;
    } else {
        if (entModelInfo->bDoWeOwnTheColModel)
            iplEntry.m_bOwnsColModel = true;
    }

    if (iplEntry.m_nFlags) {
        mp_caccIPLItems[m_iNumIPLItems] = iplEntry;
        ++m_iNumIPLItems;
    }
}
