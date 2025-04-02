/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"
#include "IplStore.h"
#include "tBinaryIplFile.h"
#include "TheCarGenerators.h"

int32& ms_currentIPLAreaCode = *(int32*)0x8E3EF8;

// Izzotop: tested, but not reviewed

void CIplStore::InjectHooks() {
    RH_ScopedClass(CIplStore);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(AddIplsNeededAtPosn, 0x4045B0);
    RH_ScopedInstall(LoadIpl, 0x406080);
    RH_ScopedInstall(Shutdown, 0x405FA0);
    RH_ScopedInstall(Initialise, 0x405EC0);
    RH_ScopedInstall(LoadIplBoundingBox, 0x405C00);
    RH_ScopedInstall(RemoveIplSlot, 0x405B60);
    RH_ScopedInstall(AddIplSlot, 0x405AC0);
    RH_ScopedInstall(RemoveIplWhenFarAway, 0x4058D0);
    RH_ScopedInstall(RemoveIplAndIgnore, 0x405890);
    RH_ScopedInstall(RequestIplAndIgnore, 0x405850);
    RH_ScopedInstall(LoadAllRemainingIpls, 0x405780);
    RH_ScopedInstall(RemoveAllIpls, 0x405720);
    RH_ScopedInstall(HaveIplsLoaded, 0x405600);
    RH_ScopedInstall(RequestIpls, 0x405520);
    RH_ScopedInstall(Load, 0x5D54A0);
    RH_ScopedInstall(Save, 0x5D5420);
    RH_ScopedInstall(EnsureIplsAreInMemory, 0x4053F0);
    RH_ScopedInstall(RemoveRelatedIpls, 0x405110);
    RH_ScopedInstall(SetupRelatedIpls, 0x404DE0, { .reversed = false });
    RH_ScopedInstall(EnableDynamicStreaming, 0x404D30);
    RH_ScopedInstall(IncludeEntity, 0x404C90);
    RH_ScopedInstall(GetBoundingBox, 0x404C70);
    RH_ScopedInstall(RemoveIpl, 0x404B20);
    RH_ScopedInstall(FindIplSlot, 0x404AC0);
    RH_ScopedInstall(SetIsInterior, 0x404A90);
    RH_ScopedInstall(GetIplName, 0x404A60);
    RH_ScopedInstall(GetIplEntityIndexArray, 0x4047B0);
    RH_ScopedInstall(GetNewIplEntityIndexArray, 0x404780);
    RH_ScopedInstall(SetIplsRequired, 0x404700);
    RH_ScopedInstall(ClearIplsNeededAtPosn, 0x4045E0);
    RH_ScopedInstall(LoadIpls, 0x405170);
    RH_ScopedGlobalInstall(SetIfInteriorIplIsRequired, 0x4045F0);
    RH_ScopedGlobalInstall(SetIfIplIsRequired, 0x404660);
    RH_ScopedGlobalInstall(SetIfIplIsRequiredReducedBB, 0x404690);
}

/*!
 * @addr 0x405EC0
 * @brief Allocate pool if it doesn't exist yet, allocate quad tree, etc..
 */
void CIplStore::Initialise() {
    gbIplsNeededAtPosn = false;

    if (!ms_pPool) {
        ms_pPool = new CPool<IplDef>(TOTAL_IPL_MODEL_IDS, "IPL Files");
    }

    AddIplSlot("generic");

    NumIplEntityIndexArrays = 0;
    ppCurrIplInstance = nullptr;

    ms_pQuadTree = new CQuadTreeNode(WORLD_BOUNDS, 3);
    assert(ms_pQuadTree);
}

/*!
 * @addr 0x405FA0
 */
void CIplStore::Shutdown() {
    RemoveAllIpls();
    for (auto i = 0; i < ms_pPool->GetSize(); i++) {
        if (!ms_pPool->IsFreeSlotAtIndex(i)) {
            RemoveIplSlot(i);
        }
    }
    delete ms_pPool;
    ms_pPool = nullptr;

    for (auto array : IplEntityIndexArrays | rng::views::take(NumIplEntityIndexArrays)) {
        delete array;
    }
    NumIplEntityIndexArrays = 0;

    delete ms_pQuadTree;
    ms_pQuadTree = nullptr;
}

/*!
 * @addr 0x405AC0
 * @returns Slot index
 */
int32 CIplStore::AddIplSlot(const char* name) {
    const auto def = new (ms_pPool->New()) IplDef{ name }; // MOTE: Seemingly they didn't overload `operator new` for it..
    return ms_pPool->GetIndex(def);
}

/*!
 * @addr 0x4045B0
 */
void CIplStore::AddIplsNeededAtPosn(const CVector& posn) {
    gvecIplsNeededAtPosn = posn;
    gbIplsNeededAtPosn = true;
}

/*!
 * @addr 0x4045E0
 */
void CIplStore::ClearIplsNeededAtPosn() {
    gbIplsNeededAtPosn = false;
}

/*!
 * @addr 0x404D30
 */
void CIplStore::EnableDynamicStreaming(int32 iplSlotIndex, bool enable) {
    GetInSlot(iplSlotIndex)->disableDynamicStreaming = !enable;
}

eAreaCodes ResolveAreaCode(int32 ec) {
    if (ec == -1) {
        if (const auto player = FindPlayerPed()) {
            return player->m_nAreaCode;
        }
        return (eAreaCodes)CGame::currArea;
    }
    return (eAreaCodes)ec;
}

/*!
 * @addr 0x4053F0
 * @brief Make sure all IPls that requested around posn (in a 190 unit circle) are loaded.
 */
void CIplStore::EnsureIplsAreInMemory(const CVector& posn) {
    if (CStreaming::ms_disableStreaming) {
        return;
    }

    if (ResolveAreaCode(-1) != CGame::currArea) {
        return;
    }

    SetIplsRequired(posn);

    for (auto slot = 1 /*skip first*/; slot < TOTAL_IPL_MODEL_IDS; slot++) {
        const auto def = GetInSlot(slot);
        if (!def) {
            continue;
        }

        if (def->disableDynamicStreaming || !def->loadRequested) {
            continue;
        }

        if (!def->bb.IsPointInside(posn, -190.f) || CStreaming::IsModelLoaded(IPLToModelId(slot))) {
            continue;
        }

        // Load IPL

        CStreaming::RequestModel(IPLToModelId(slot), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY);

        if (TheCamera.GetScreenFadeStatus() == 0) {
            FrontEndMenuManager.MessageScreen("LOADCOL", false, false);
        }

        CTimer::Suspend();
        CStreaming::LoadAllRequestedModels(true);
        CTimer::Resume();

        def->loadRequested = false;
    }
}

/*!
 * @addr 0x404AC0
 * @returns -1 if slot not found
 */
int32 CIplStore::FindIplSlot(const char* name) {
    for (auto& def : ms_pPool->GetAllValid()) {
        if (!_stricmp(name, def.name)) {
            return ms_pPool->GetIndex(&def);
        }
    }
    return -1;
}

/*!
 * @addr 0x404C70
 */
CRect* CIplStore::GetBoundingBox(int32 iplSlotIndex) { // No xrefs
    return &GetInSlot(iplSlotIndex)->bb;
}

/*!
 * @addr 0x4047B0
 */
CEntity** CIplStore::GetIplEntityIndexArray(int32 arrayIndex) {
    return IplEntityIndexArrays[arrayIndex];
}

/*!
 * @addr 0x404A60
 */
const char* CIplStore::GetIplName(int32 iplSlotIndex) { // No xrefs
    return GetInSlot(iplSlotIndex)->name;
}

/*!
 * @addr 0x404780
 * @brief Make a new `CEntity*[entitiesCount]` array
 * @returns The index of the array in `IplEntityIndexArrays`
 * @param entitiesCount Size of `CEntity*` array
 */
int32 CIplStore::GetNewIplEntityIndexArray(int32 entitiesCount) {
    const auto idx = NumIplEntityIndexArrays++;
    IplEntityIndexArrays[idx] = new CEntity*[entitiesCount];
    return idx;
}

/*!
 * @addr 0x405600
 * @returns If all IPLs around `coords` in a 190 unit radius have loaded
 */
bool CIplStore::HaveIplsLoaded(const CVector& coords, int32 /*playerNumber*/) {
    // Can't use `ms_pPool->GetAllValid()` here, because we must ignore the first slot (whenever it's valid or not).
    bool ret = true;
    for (auto slot = 1; slot < TOTAL_IPL_MODEL_IDS; slot++) {
        auto def = ms_pPool->GetAt(slot);
        if (!def) {
            continue;
        }
        if (!def->loadRequested) {
            continue;
        }
        if (def->bb.IsPointInside(coords, -190.f) && !def->loaded && !def->disableDynamicStreaming) {
            ret = false;
        }
        def->loadRequested = false;
    }
    return ret;
}

/*!
 * @addr 0x404C90
 */
void CIplStore::IncludeEntity(int32 iplSlotIndex, CEntity* entity) {
    const auto ipldef = ms_pPool->GetAt(iplSlotIndex);

    switch (entity->GetType()) {
    case ENTITY_TYPE_BUILDING: {
        const auto buildingId = GetBuildingPool()->GetIndex(entity->AsBuilding());
        ipldef->firstBuilding = std::min(ipldef->firstBuilding, (int16)buildingId);
        ipldef->lastBuilding = std::max(ipldef->firstBuilding, (int16)buildingId);
        break;
    }
    case ENTITY_TYPE_DUMMY: {
        const auto dummyId = GetDummyPool()->GetIndex(entity->AsDummy());
        ipldef->firstDummy = std::min(ipldef->firstDummy, (int16)dummyId);
        ipldef->lastDummy = std::max(ipldef->lastDummy, (int16)dummyId);
        break;
    }
    default: {
        NOTSA_UNREACHABLE("Incorrect entity type");
    }
    }
}

/*!
 * @addr 0x405780
 * @brief Load remaining IPL defs (using the streamer)
 */
void CIplStore::LoadAllRemainingIpls() {
    ZoneScoped;

    // Can't use `ms_pPool->GetAllValid()` here, because we must ignore the first slot (whenever it's valid or not).
    for (auto slot = 1 /*skip 1st*/; slot < TOTAL_IPL_MODEL_IDS; slot++) {
        auto def = ms_pPool->GetAt(slot);

        if (!def) {
            continue;
        }

        if (!def->bb.IsFlipped()) {
            continue;
        }

        if (CColAccel::isCacheLoading()) {
            *def = CColAccel::getIplDef(slot);
            def->loaded = false;
            ms_pQuadTree->AddItem(&def, def->bb);
        } else {
            CStreaming::RequestModel(IPLToModelId(slot), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY);
            CStreaming::LoadAllRequestedModels(true);
            CStreaming::RemoveModel(IPLToModelId(slot));
        }
    }
}

/*!
 * @addr 0x406080
 * @NOTSA Originally took `uchar*`, but for simplicity's sake we're going to use `char*`. Makes no difference.
 */
bool CIplStore::LoadIpl(int32 iplSlotIndex, char* data, int32 dataSize) {
    auto& def = *GetInSlot(iplSlotIndex);
    def.loaded = true; // We're setting this here, as it's set in all cases.

    // First of all, load bounding box (if not loaded already)
    if (def.bb.IsFlipped()) {
        // def.field_2D = true; // Moved to top

        if (!LoadIplBoundingBox(iplSlotIndex, data, dataSize)) {
            return false;
        }

        if (def.isLarge) {
            def.bb.Resize(350.f, 350.f);
        } else {
            def.bb.Resize(200.f, 200.f);
        }

        CColAccel::setIplDef(iplSlotIndex, def);
        ms_pQuadTree->AddItem(&def, def.bb);

        return true;
    }

    // Load file.
    // It might be either in text or binary.
    // If in binary the data beings with `bnry`.

    const auto pIPLLODEntities = def.staticIdx != -1 ? IplEntityIndexArrays[def.staticIdx] : nullptr;

    // Anti copy-paste code helper.
    // Finish loading an ObjectInstance
    const auto FinishLoadObjInst = [&](CEntity* obj) {
        obj->m_nIplIndex = (uint8)iplSlotIndex;

        // Assign LOD to `obj` (if set)
        if (obj->m_nLodIndex == -1) {
            obj->m_pLod = nullptr;
        } else {
            assert(pIPLLODEntities);
            obj->m_pLod = pIPLLODEntities[obj->m_nLodIndex];
            obj->m_pLod->m_nNumLodChildren++;
        }

        obj->Add(); // Add it to the world

        IncludeEntity(iplSlotIndex, obj);
    };

    if (strncmp((char*)data, "bnry", 4u) != 0) { // IPL in text format
        bool inSection{};
        for (auto line = CFileLoader::LoadLine(data, dataSize); line; line = CFileLoader::LoadLine(data, dataSize)) {
            switch (line[0]) {
            case 0:   // Empty line
            case '#': // Comment
                continue;
            }

            const auto LineBeginsWith = [linesv = std::string_view{ line }](auto with) {
                return linesv.starts_with(with);
            };

            if (!inSection) {
                inSection = LineBeginsWith("inst");
                continue;
            }

            if (LineBeginsWith("end")) {
                inSection = false;
                continue;
            }

            FinishLoadObjInst(CFileLoader::LoadObjectInstance(line));
        }
    } else { // Binary IPL
        auto ipl = reinterpret_cast<tBinaryIplFile*>(data);

        for (auto& inst : ipl->GetObjInstances()) {
            FinishLoadObjInst(CFileLoader::LoadObjectInstance(&inst, nullptr));
        }

        for (auto& cargen : ipl->GetCarGens()) {
            CFileLoader::LoadCarGenerator(&cargen, iplSlotIndex);
        }
    }

    // def.field_2D = true; // Moved to top

    return true;
}

/*!
 * @addr 0x405C00
 * @brief Nearly 100% same as `LoadIpl`, but it modifies `ppCurrIplInstance`, and calculates bounding box of the IPL. Unsure honestly.
 * @returns Always true
 * @NOTSA Originally took `uchar*`, but for simplicity's sake we're going to use `char*`. Makes no difference.
 */
bool CIplStore::LoadIplBoundingBox(int32 iplSlotIndex, char* data, int32 dataSize) {
    auto& def = *GetInSlot(iplSlotIndex);

    const auto pIPLLODEntities = def.staticIdx != -1 ? IplEntityIndexArrays[def.staticIdx] : nullptr;

    // Anti copy-paste code helper.
    // Finish loading an ObjectInstance
    const auto FinishLoadObjInst = [&](CEntity* obj) {
        obj->m_nIplIndex = (uint8)iplSlotIndex;

        if (obj->m_nLodIndex == -1) {
            obj->m_pLod = nullptr;
        } else {
            assert(pIPLLODEntities);
            obj->m_pLod = pIPLLODEntities[obj->m_nLodIndex];
            obj->m_pLod->m_nNumLodChildren++;

            if (ppCurrIplInstance) {
                *ppCurrIplInstance = obj;
                ppCurrIplInstance++;
            }
        }

        if (obj->GetModelInfo()->m_fDrawDistance > 150.f) {
            def.isLarge = true;
        }

        obj->Add(); // Add it to the world

        IncludeEntity(iplSlotIndex, obj);
        def.bb.Restrict(obj->GetBoundRect());
    };

    if (strncmp((char*)data, "bnry", 4u) != 0) { // IPL in text format
        bool inSection{};
        for (auto line = CFileLoader::LoadLine(data, dataSize); line; line = CFileLoader::LoadLine(data, dataSize)) {
            switch (line[0]) {
            case 0:   // Empty line
            case '#': // Comment
                continue;
            }

            const auto LineBeginsWith = [linesv = std::string_view{ line }](auto with) {
                return linesv.starts_with(with);
            };

            if (!inSection) {
                inSection = LineBeginsWith("inst");
                continue;
            }

            if (LineBeginsWith("end")) {
                inSection = false;
                continue;
            }

            FinishLoadObjInst(CFileLoader::LoadObjectInstance(line));
        }
    } else { // Binary IPL
        auto ipl = reinterpret_cast<tBinaryIplFile*>(data);
        for (auto& inst : ipl->GetObjInstances()) {
            FinishLoadObjInst(CFileLoader::LoadObjectInstance(&inst, nullptr));
        }
    }

    return true;
}

/*!
 * @addr 0x405170
 */
void CIplStore::LoadIpls(CVector posn, bool bAvoidLoadInPlayerVehicleMovingDirection) {
    if (CStreaming::ms_disableStreaming) {
        return;
    }

    if (FindPlayerPed() && !bAvoidLoadInPlayerVehicleMovingDirection) {
        if (const auto veh = FindPlayerVehicle()) {
            const auto& velocity = veh->m_vecMoveSpeed;
            posn.x += std::min(40.f, velocity.x * 80.f);
            posn.y += std::min(40.f, velocity.y * 80.f);
        }
    }

    SetIplsRequired(posn);
    if (gbIplsNeededAtPosn) {
        SetIplsRequired(gvecIplsNeededAtPosn, CGame::currArea);
    }

    const auto ProcessEntity = [](CPhysical* e) {
        if (e->m_pAttachedTo || e->physicalFlags.bDontApplySpeed || e->physicalFlags.b15) {
            return;
        }
        ms_currentIPLAreaCode = e->m_nAreaCode;
        ms_pQuadTree->ForAllMatching(e->GetPosition2D(), SetIfIplIsRequiredReducedBB);
    };

    for (auto& mce : CTheScripts::MissionCleanUp.m_Objects) {
        switch (mce.type) {
        case MISSION_CLEANUP_ENTITY_TYPE_PED: {
            const auto ped = GetPedPool()->GetAtRef(mce.handle);
            assert(ped);
            if (ped->IsStateDying()) {
                continue;
            }
            ProcessEntity(ped);
            break;
        }
        case MISSION_CLEANUP_ENTITY_TYPE_VEHICLE: {
            const auto veh = GetVehiclePool()->GetAtRef(mce.handle);
            assert(veh);
            switch (veh->GetStatus()) {
            case STATUS_PHYSICS:
            case STATUS_SIMPLE:
            case STATUS_ABANDONED:
                continue;
            }
            ProcessEntity(veh);
            break;
        }
        }
    }

    for (auto slot = ms_pPool->GetSize(); slot-- > 1;) { // skips 1st
        const auto def = ms_pPool->GetAt(slot);
        if (!def || def->disableDynamicStreaming) {
            continue;
        }
        if (def->loadRequested) {
            if (!def->loaded) {
                CStreaming::RequestModel(IPLToModelId(slot), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY);
            }
            def->loadRequested = false;
        } else if (def->loaded) {
            CStreaming::RemoveModel(IPLToModelId(slot));
            if (def->ignoreWhenDeleted) {
                def->disableDynamicStreaming = true;
            }
        }
    }

    if (!bAvoidLoadInPlayerVehicleMovingDirection) {
        gbIplsNeededAtPosn = false;
    }
}

/*!
 * @addr 0x405720
 * @brief Unload all loaded IPL's using `CStreaming::RemoveModel`
 */
void CIplStore::RemoveAllIpls() {
    // Can't use `ms_pPool->GetAllValid()` here, because we must ignore the first slot (whenever it's valid or not).
    for (auto slot = 1 /*skip 1st*/; slot < TOTAL_IPL_MODEL_IDS; slot++) {
        auto def = ms_pPool->GetAt(slot);
        if (!def) {
            continue;
        }
        if (!CStreaming::GetInfo(IPLToModelId(slot)).IsMissionOrGameRequired()) {
            CStreaming::RemoveModel(IPLToModelId(slot));
        }
    }
}

/*!
 * @addr 0x404B20
 */
void CIplStore::RemoveIpl(int32 iplSlotIndex) {
    auto def = ms_pPool->GetAt(iplSlotIndex);
    def->loaded = false;

    const auto ProcessPool = [iplSlotIndex]<typename PoolT>(PoolT& pool, int32 minId, int32 maxId) {
        for (auto i = minId; i < maxId; i++) {
            const auto entity = pool.GetAt(i);
            if (!entity)
                continue;

            if (entity->m_nIplIndex == iplSlotIndex) {
                if constexpr (std::is_same_v<PoolT, CObjectPool>) {
                    if (entity->m_pDummyObject) {
                        CWorld::Add(entity->m_pDummyObject);
                    }
                }

                CWorld::Remove(entity);
                delete entity;
            }
        }
    };

    // In same order as originally
    ProcessPool(*GetBuildingPool(), def->firstBuilding, def->lastBuilding);
    ProcessPool(*GetObjectPool(),   0,                     GetObjectPool()->GetSize());
    ProcessPool(*GetDummyPool(),    def->firstDummy,    def->lastDummy);

    CTheCarGenerators::RemoveCarGenerators((uint8)iplSlotIndex);
}

/*!
 * @addr 0x405890
 */
void CIplStore::RemoveIplAndIgnore(int32 iplSlotIndex) {
    auto& def = *ms_pPool->GetAt(iplSlotIndex);

    CStreaming::RemoveModel(IPLToModelId(iplSlotIndex));

    def.disableDynamicStreaming = true;
    def.ignoreWhenDeleted = false;
}

/*!
 * @addr 0x405B60
 */
void CIplStore::RemoveIplSlot(int32 iplSlotIndex) {
    auto def = ms_pPool->GetAt(iplSlotIndex);
    if (def->loaded) {
        RemoveIpl(iplSlotIndex);
    }
    ms_pQuadTree->DeleteItem(def);
    ms_pPool->Delete(def);
}

/*!
 * @addr 0x4058D0
 */
void CIplStore::RemoveIplWhenFarAway(int32 iplSlotIndex) {
    auto def = ms_pPool->GetAt(iplSlotIndex);
    def->disableDynamicStreaming = false;
    def->ignoreWhenDeleted = true;
}

/*!
 * @addr 0x405110
 */
void CIplStore::RemoveRelatedIpls(int32 iplSlotIndex) {
    for (auto&& [idx, def] : ms_pPool->GetAllValidWithIndex()) {
        if (def.staticIdx == iplSlotIndex) {
            CStreaming::RemoveModel(IPLToModelId(idx));
        }
    }
}

/*!
 * @addr 0x405850
 */
void CIplStore::RequestIplAndIgnore(int32 iplSlotIndex) {
    auto& def = *ms_pPool->GetAt(iplSlotIndex);
    CStreaming::RequestModel(IPLToModelId(iplSlotIndex), STREAMING_KEEP_IN_MEMORY);
    def.disableDynamicStreaming = true;
    def.ignoreWhenDeleted = false;
}

/*!
 * @addr 0x405520
 */
void CIplStore::RequestIpls(const CVector& posn, int32 playerNumber) {
    for (auto slot = 1 /*skip first*/; slot < TOTAL_IPL_MODEL_IDS; slot++) {
        const auto def = GetInSlot(slot);
        if (!def) {
            continue;
        }

        if (def->loadRequested) {
            if (def->bb.IsPointInside(posn, -190.f)) {
                CStreaming::RequestModel(IPLToModelId(slot), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY);
            }
            def->loadRequested = false;
        }
    }
}

/*!
 * @addr 0x404700
 */
void CIplStore::SetIplsRequired(const CVector& posn, int32 gameArea) {
    ms_currentIPLAreaCode = ResolveAreaCode(gameArea);

    const auto GetCallback = [&] {
        if (ms_currentIPLAreaCode != AREA_CODE_NORMAL_WORLD && posn.z >= 900.f) {
            return SetIfIplIsRequired;
        } else if (ms_currentIPLAreaCode == CGame::currArea) {
            return SetIfInteriorIplIsRequired;
        } else {
            return SetIfIplIsRequiredReducedBB;
        }
    };
    ms_pQuadTree->ForAllMatching(posn, GetCallback());
}

/*!
 * @addr 0x404A90
 */
void CIplStore::SetIsInterior(int32 iplSlotIndex, bool isInterior) {
    GetInSlot(iplSlotIndex)->isInterior = isInterior;
}

bool IsIPLAnInterior(const char* iplFileName) {
    return rng::any_of(std::array{
        "gen_int1",
        "gen_int2",
        "gen_int3",
        "gen_int4",
        "gen_int5",
        "gen_intb",
        "savehous",
        "stadint",
        "int_la",
        "int_sf",
        "int_veg",
        "int_cont",
        "levelmap"
    }, [iplFileName](auto name) {
        return _stricmp(iplFileName, name) == 0;
    });
}

template<size_t N>
bool ExtractIPLNameFromPath(const char* iplFilePath, char(&out)[N]) {
    // Extract name of IPL from path
    const auto fileNameWithExt = strrchr(iplFilePath, '\\');
    if (!fileNameWithExt) {
        NOTSA_LOG_DEBUG("Failed to extract ipl name from path ({}) [No path separator]", iplFilePath);
        return false;
    }
    const auto dot = strchr(fileNameWithExt, '.');
    if (!dot) {
        NOTSA_LOG_DEBUG("Failed to extract ipl name from path ({}) [No file ext]", iplFilePath);
        return false;
    }
    memcpy_s(out, rng::size(out), fileNameWithExt + 1, dot - (fileNameWithExt + 1)); // They used a manual loop, but this is better.
    return true;
}

/*!
 * @addr 0x404DE0
 */
int32 CIplStore::SetupRelatedIpls(const char* filename, int32 index, CEntity** ppLoadedBuildingsArray) {
    char iplName[1024]{}; // OG Size: 32
    if (!ExtractIPLNameFromPath(filename, iplName)) {
        return 0;
    }
    const auto isIPLAnInterior = IsIPLAnInterior(iplName);

    strcat_s(iplName, "_stream");
    const auto iplNameLen = strlen(iplName);

    ppCurrIplInstance = ppLoadedBuildingsArray;

    if (CColAccel::isCacheLoading()) { // NOTSA: Inverted conditional
        for (auto&& [slot, def] : ms_pPool->GetAllValidWithIndex()) {
            if (_strnicmp(iplName, def.name, iplNameLen)) {
                continue;
            }
            def = CColAccel::getIplDef(slot);
            def.staticIdx = index;
            def.isInterior = isIPLAnInterior;
            def.loaded = false;
            ms_pQuadTree->AddItem(&def, def.bb);
        }
    } else {
        for (auto&& [slot, def] : ms_pPool->GetAllValidWithIndex()) {
            if (_strnicmp(iplName, def.name, iplNameLen)) {
                continue;
            }
            def.staticIdx = index;
            def.isInterior = isIPLAnInterior;
            def.disableDynamicStreaming = false; // NOTE: Inlined function was used to set this.
            CStreaming::RequestModel(IPLToModelId(slot), STREAMING_KEEP_IN_MEMORY);
        }
        CStreaming::LoadAllRequestedModels(false);
    }

    const auto numRelatedIPLs = ppCurrIplInstance - ppLoadedBuildingsArray;
    ppCurrIplInstance = nullptr;
    return numRelatedIPLs;
}

/*!
 * @addr 0x5D5420
 */
bool CIplStore::Save() {
    const auto num = ms_pPool->GetSize();
    CGenericGameStorage::SaveDataToWorkBuffer(num);
    for (auto i = 1; i < num; i++) { // skips 1st
        const auto ipl = ms_pPool->GetAt(i);
        CGenericGameStorage::SaveDataToWorkBuffer(ipl && ipl->loaded && ipl->disableDynamicStreaming);
    }
    return true;
}

/*!
 * @addr 0x5D54A0
 */
bool CIplStore::Load() {
    const auto num = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
    assert(num == ms_pPool->GetSize());
    for (auto i = 1; i < num; i++) { // skips 1st
        const auto ipl = ms_pPool->GetAt(i);

        auto requestIpl = CGenericGameStorage::LoadDataFromWorkBuffer<bool>();
        if (!ipl) {
            continue;
        }
        if (requestIpl) {
            RequestIplAndIgnore(i);
        } else if (ipl->loaded && ipl->disableDynamicStreaming) {
            RemoveIplAndIgnore(i);
        }
    }
    return true;
}

/*!
 * @addr 0x4045F0
 * @brief Callback used in `SetIplsRequired`
 */
void SetIfInteriorIplIsRequired(const CVector2D& posn, void* data) {
    auto& def = *static_cast<IplDef*>(data);

    if (def.isInterior) {
        if (!ms_currentIPLAreaCode) {
            return;
        }
        if (def.bb.IsPointInside(posn, -140.f)) {
            def.loadRequested = true;
        }
    } else {
        if (def.bb.IsPointInside(posn)) {
            def.loadRequested = true;
        }
    }
}

/*!
 * @addr 0x404660
 * @brief Callback used in `SetIplsRequired`
 */
void SetIfIplIsRequired(const CVector2D& posn, void* data) {
    auto& def = *static_cast<IplDef*>(data);

    if (def.isInterior && def.bb.IsPointInside(posn, -140.f)) {
        def.loadRequested = true;
    }
}

/*!
 * @addr 0x404690
 * @brief Callback used in `SetIplsRequired`
 */
void SetIfIplIsRequiredReducedBB(const CVector2D& posn, void* data) {
    auto& def = *static_cast<IplDef*>(data);

    if (def.bb.IsPointInside(posn, -160.f)) {
        if (ms_currentIPLAreaCode != AREA_CODE_NORMAL_WORLD) {
            if (!def.isInterior) {
                return;
            }
        } else if (def.isInterior) {
            return;
        }
        def.loadRequested = true;
    }
}
