#include "StdInc.h"

#include "ColStore.h"
#include "TheScripts.h"

CVector& CColStore::ms_vecCollisionNeeded = *(CVector*)0x965580;
bool& CColStore::ms_bCollisionNeeded = *(bool*)0x965558;
int32 CColStore::ms_nRequiredCollisionArea = *(int32*)0x965554;

CColPool* &CColStore::ms_pColPool = *(CColPool**)0x965560;
CQuadTreeNode* &CColStore::ms_pQuadTree = *(CQuadTreeNode**)0x96555C;

void CColStore::InjectHooks()
{
    RH_ScopedClass(CColStore);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Initialise, 0x4113F0);
    RH_ScopedInstall(Shutdown, 0x4114D0);
    RH_ScopedInstall(AddCollisionNeededAtPosn, 0x4103A0);
    RH_ScopedInstall(AddColSlot, 0x411140);
    RH_ScopedInstall(AddRef, 0x4107A0);
    RH_ScopedInstall(RemoveRef, 0x4107D0);
    RH_ScopedInstall(EnsureCollisionIsInMemory, 0x410AD0);
    RH_ScopedInstall(SetCollisionRequired, 0x4104E0);
    RH_ScopedInstall(RequestCollision, 0x410C00);
    RH_ScopedInstall(RemoveColSlot, 0x411330);
    RH_ScopedInstall(RemoveCol, 0x410730);
    RH_ScopedOverloadedInstall(LoadCol, "", 0x410690, void(*)(int32, const char*));
    RH_ScopedOverloadedInstall(LoadCol, "2", 0x4106D0, bool(*)(int32, uint8*, int32));
    RH_ScopedInstall(IncludeModelIndex, 0x410820);
    RH_ScopedInstall(RemoveAllCollision, 0x410E00);
    RH_ScopedInstall(LoadAllCollision, 0x410E60);
    RH_ScopedInstall(LoadAllBoundingBoxes, 0x4113D0);
    RH_ScopedInstall(BoundingBoxesPostProcess, 0x410EC0);
    RH_ScopedInstall(GetBoundingBox, 0x410800);
    RH_ScopedInstall(HasCollisionLoaded, 0x410CE0);
    RH_ScopedInstall(LoadCollision, 0x410860);

    RH_ScopedGlobalInstall(SetIfCollisionIsRequired, 0x4103D0);
    RH_ScopedGlobalInstall(SetIfCollisionIsRequiredReducedBB, 0x410470);
}

void* ColDef::operator new(unsigned size)
{
    return CColStore::ms_pColPool->New();
}

void ColDef::operator delete(void* data)
{
    CColStore::ms_pColPool->Delete(static_cast<ColDef*>(data));
}

// 0x4113F0
// Initialise
void CColStore::Initialise()
{
    const auto rect = CRect(-3000.0F, -3000.0F, 3000.0F, 3000.0F);
    ms_bCollisionNeeded = false;
    if (!ms_pColPool)
        ms_pColPool = new CColPool(TOTAL_COL_MODEL_IDS, "CollisionFiles");

    AddColSlot("generic");
    ms_pQuadTree = new CQuadTreeNode(rect, 3);
}

// 0x4114D0
void CColStore::Shutdown()
{
    for (auto i = 0; i < ms_pColPool->GetSize(); i++) {
        if (ms_pColPool->GetAt(i)) {
            RemoveColSlot(i);
        }
    }

    delete ms_pColPool;
    ms_pColPool = nullptr;

    delete ms_pQuadTree;
    ms_pQuadTree = nullptr;
}

// 0x411140
int32 CColStore::AddColSlot(const char* name)
{
    auto def = new ColDef();
    def->m_bActive = false;
    def->m_bCollisionIsRequired = false;
    def->m_bProcedural = false;
    def->m_bInterior = false;
    def->m_Area = CRect();
    strcpy_s(def->name, name);
    def->m_nModelIdStart = -1;
    def->m_nModelIdEnd = SHRT_MIN;
    def->m_nRefCount = 0;

    if (!strcmp(name, "procobj") || !strcmp(name, "proc_int") || !strcmp(name, "proc_int2"))
        def->m_bProcedural = true;

    if (!strncmp(name, "int_la", 6)
        || !strncmp(name, "int_sf", 6)
        || !strncmp(name, "int_veg", 7)
        || !strncmp(name, "int_cont", 8)
        || !strncmp(name, "gen_int1", 8)
        || !strncmp(name, "gen_int2", 8)
        || !strncmp(name, "gen_int3", 8)
        || !strncmp(name, "gen_int4", 8)
        || !strncmp(name, "gen_int5", 8)
        || !strncmp(name, "gen_intb", 8)
        || !strncmp(name, "savehous", 8)
        || !strcmp(name, "props")
        || !strcmp(name, "props2")
        || !strncmp(name, "levelmap", 8)
        || !strncmp(name, "stadint", 7))
    {
        def->m_bInterior = true;
    }

    return ms_pColPool->GetIndex(def);
}

// 0x4103A0
void CColStore::AddCollisionNeededAtPosn(const CVector& pos)
{
    ms_vecCollisionNeeded = pos;
    ms_bCollisionNeeded = true;
}

// 0x4107A0
void CColStore::AddRef(int32 colNum)
{
    auto *colDef = ms_pColPool->GetAt(colNum);
    ++colDef->m_nRefCount; //BUG: We don't check whether the GetAt returned nullptr, which it can
}

// We can assume that there was a code similar to `CIplStore::FindIplSlot` here, but it was deleted for some reason
int32 CColStore::FindColSlot(const char* name)
{
    for (auto& def : ms_pColPool->GetAllValid()) {
        if (!_stricmp(name, def.name)) {
            return ms_pColPool->GetIndex(&def);
        }
    }
    return -1;
}

// 0x410EC0
void CColStore::BoundingBoxesPostProcess() {
    ZoneScoped;

    for (auto i = 1; i < ms_pColPool->GetSize(); i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def)
            continue;

        if (CColAccel::isCacheLoading())
        {
            CColAccel::getColDef(*def);
        }
        else
        {
            def->m_Area.Resize(120.0F, 120.0F);
            CColAccel::addColDef(*def);
        }

        ms_pQuadTree->AddItem(def, def->m_Area);
    }
}

// 0x410AD0
void CColStore::EnsureCollisionIsInMemory(const CVector& pos)
{
    if (CStreaming::ms_disableStreaming)
        return;

    auto* player = FindPlayerPed();
    const auto area = player ? player->m_nAreaCode : CGame::currArea;
    if (area != CGame::currArea)
        return;

    SetCollisionRequired(pos, area);
    for (auto i = 1; i < ms_pColPool->GetSize(); i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def || !def->m_bCollisionIsRequired)
            continue;

        if (def->m_Area.IsPointInside(pos, -110.0F) && !CStreaming::GetInfo(COLToModelId(i)).IsLoaded()) {
            CStreaming::RequestModel(COLToModelId(i), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY);
            if (!TheCamera.GetScreenFadeStatus())
                FrontEndMenuManager.MessageScreen("LOADCOL", false, false);

            CTimer::Suspend();
            CStreaming::LoadAllRequestedModels(true);
            CTimer::Resume();
        }

        def->m_bCollisionIsRequired = false;
    }
}

// 0x410800
CRect* CColStore::GetBoundingBox(int32 colSlot)
{
    return &ms_pColPool->GetAt(colSlot)->m_Area;
}

// 0x410820
void CColStore::IncludeModelIndex(int32 colSlot, int32 modelId)
{
    auto* def = ms_pColPool->GetAt(colSlot);

    if (modelId < def->m_nModelIdStart)
        def->m_nModelIdStart = modelId;

    if (modelId > def->m_nModelIdEnd)
        def->m_nModelIdEnd = modelId;
}

// 0x410CE0
bool CColStore::HasCollisionLoaded(const CVector& pos, int32 areaCode)
{
    SetCollisionRequired(pos, areaCode);
    auto foundInd = -1;
    for (auto i = 1; i < ms_pColPool->GetSize(); i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def || !def->m_bCollisionIsRequired)
            continue;

        def->m_bCollisionIsRequired = false;
        if (def->m_Area.IsPointInside(pos, -110.0F) && !def->m_bActive)
        {
            foundInd = i; // Slightly modified the method logic to be more readable, result is the same
            break;
        }
    }

    if (foundInd == -1)
        return true;

    for (auto i = foundInd; i < ms_pColPool->GetSize(); i++) {
        auto* innerDef = ms_pColPool->GetAt(i);
        if (!innerDef)
            continue;

        innerDef->m_bCollisionIsRequired = false;
    }

    return false;
}

// 0x4113D0
void CColStore::LoadAllBoundingBoxes() {
    ZoneScoped;

    if (CColAccel::isCacheLoading())
        CColAccel::cacheLoadCol();
    else
        LoadAllCollision();
}

// 0x410E60
void CColStore::LoadAllCollision() {
    for (auto i = 1; i < ms_pColPool->GetSize(); i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def)
            continue;

        CStreaming::RequestModel(COLToModelId(i), 0);
        CStreaming::LoadAllRequestedModels(false);
        CStreaming::RemoveModel(COLToModelId(i));
    }
}

// 0x410690
void CColStore::LoadCol(int32 colSlot, const char* filename)
{
    auto* def = ms_pColPool->GetAt(colSlot);
    CFileLoader::LoadCollisionFile(filename, colSlot);
    def->m_bActive = true;
}

// 0x4106D0
bool CColStore::LoadCol(int32 colSlot, uint8* data, int32 dataSize)
{
    assert(colSlot <= 255); // Otherwise it would wrap when casted to `uint8` (As that's the type it's stored as in CColModel)

    auto* def = ms_pColPool->GetAt(colSlot);

    bool bLoaded;
    if (def->m_nModelIdStart <= def->m_nModelIdEnd)
        bLoaded = CFileLoader::LoadCollisionFile(data, dataSize, colSlot);
    else
        bLoaded = CFileLoader::LoadCollisionFileFirstTime(data, dataSize, colSlot);

    if (!bLoaded)
        return false;

    def->m_bActive = true;
    return true;
}

// 0x410860
void CColStore::LoadCollision(CVector pos, bool bIgnorePlayerVeh)
{
    if (CStreaming::ms_disableStreaming)
        return;

    auto* player = FindPlayerPed();
    if (player && !bIgnorePlayerVeh)
    {
        if (auto* vehicle = FindPlayerVehicle()) {
            pos += vehicle->m_vecMoveSpeed * CVector(20.0F, 20.0F, 0.0F);
        }
    }

    SetCollisionRequired(pos, -1);
    if (ms_bCollisionNeeded)
        SetCollisionRequired(ms_vecCollisionNeeded, CGame::currArea);

    for (auto& obj : CTheScripts::MissionCleanUp.m_Objects)
    {
        CEntity* entity = nullptr;
        if (obj.type == MissionCleanUpEntityType::MISSION_CLEANUP_ENTITY_TYPE_VEHICLE)
        {
            entity = GetVehiclePool()->GetAtRef(obj.handle);
            if (!entity || entity->m_nStatus == eEntityStatus::STATUS_WRECKED)
                continue;
        }
        else if (obj.type == MissionCleanUpEntityType::MISSION_CLEANUP_ENTITY_TYPE_PED)
        {
            entity = GetPedPool()->GetAtRef(obj.handle);
            if (!entity || entity->AsPed()->m_nPedState == PEDSTATE_DIE || entity->AsPed()->m_nPedState == PEDSTATE_DEAD)
                continue;
        }

        if (!entity || entity->AsPhysical()->physicalFlags.b15 || entity->AsPhysical()->physicalFlags.bDontApplySpeed)
            continue;

        ms_nRequiredCollisionArea = entity->m_nAreaCode;
        ms_pQuadTree->ForAllMatching(entity->GetPosition(), SetIfCollisionIsRequiredReducedBB);
    }

    for (auto i = 1; i < ms_pColPool->GetSize(); i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def)
            continue;

        if (def->m_bCollisionIsRequired || def->m_bProcedural || def->m_nRefCount > 0)
        {
            if (!def->m_bActive)
                CStreaming::RequestModel(COLToModelId(i), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY);

            def->m_bCollisionIsRequired = false;
        }
        else if (def->m_bActive)
            CStreaming::RemoveModel(COLToModelId(i));
    }

    if (!bIgnorePlayerVeh)
        ms_bCollisionNeeded = false;
}

// 0x410E00
void CColStore::RemoveAllCollision() {
    ZoneScoped;

    for (auto i = 1; i < ms_pColPool->GetSize(); i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def)
            continue;

        if (!CStreaming::GetInfo(COLToModelId(i)).IsMissionOrGameRequired())
            CStreaming::RemoveModel(COLToModelId(i));
    }
}

// 0x410730
void CColStore::RemoveCol(int32 colSlot)
{
    auto* def = ms_pColPool->GetAt(colSlot);
    def->m_bActive = false;
    for (auto i = def->m_nModelIdStart; i <= def->m_nModelIdEnd; ++i) {
        auto* mi = CModelInfo::GetModelInfo(i);
        if (!mi)
            continue;

        auto* cm = mi->GetColModel();
        if (!cm)
            continue;

        if (mi->IsLod() && cm->m_nColSlot == colSlot)
            cm->RemoveCollisionVolumes();
    }
}

// 0x411330
void CColStore::RemoveColSlot(int32 colSlot)
{
    auto* def = ms_pColPool->GetAt(colSlot);
    if (def->m_bActive)
        RemoveCol(colSlot);

    ms_pQuadTree->DeleteItem(def);
    delete def;
}

// 0x4107D0
void CColStore::RemoveRef(int32 colNum)
{
    auto* def = ms_pColPool->GetAt(colNum);
    --def->m_nRefCount; // BUG: We don't check whether the GetAt returned nullptr, which it can
}

// 0x410C00
void CColStore::RequestCollision(const CVector& pos, int32 areaCode)
{
    SetCollisionRequired(pos, areaCode);
    for (auto i = 1; i < ms_pColPool->GetSize(); i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def || !def->m_bCollisionIsRequired)
            continue;

        if (def->m_Area.IsPointInside(pos, -110.0F))
            CStreaming::RequestModel(COLToModelId(i), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY);

        def->m_bCollisionIsRequired = false;
    }
}

// 0x4104E0
void CColStore::SetCollisionRequired(const CVector& pos, int32 areaCode)
{
    auto usedArea = areaCode;
    if (areaCode == -1) {
        auto* player = FindPlayerPed();
        usedArea = player ? player->m_nAreaCode : CGame::currArea;
    }

    ms_nRequiredCollisionArea = usedArea;
    if (usedArea == CGame::currArea)
        ms_pQuadTree->ForAllMatching(pos, SetIfCollisionIsRequired);
    else
        ms_pQuadTree->ForAllMatching(pos, SetIfCollisionIsRequiredReducedBB);
}

// 0x4103D0
void SetIfCollisionIsRequired(const CVector2D& vecPos, void* data)
{
    auto* def = static_cast<ColDef*>(data);
    if (!CColStore::ms_nRequiredCollisionArea && def->m_bInterior)
        return;

    if (!def->m_Area.IsPointInside(vecPos))
        return;

    def->m_bCollisionIsRequired = true;
}

// 0x410470
void SetIfCollisionIsRequiredReducedBB(const CVector2D& vecPos, void* data)
{
    auto* def = static_cast<ColDef*>(data);
    if (!def->m_Area.IsPointInside(vecPos, -80.0F))
        return;

    if (!CColStore::ms_nRequiredCollisionArea && def->m_bInterior)
        return;

    def->m_bCollisionIsRequired = true;
}
