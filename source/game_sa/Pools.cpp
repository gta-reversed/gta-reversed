#include "StdInc.h"

#include "Pools.h"
#include "CarCtrl.h"

// #include "PointRoute.h"

void CPools::InjectHooks() {
    RH_ScopedClass(CPools);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x550F10, { .reversed = false });
    RH_ScopedInstall(ShutDown, 0x5519F0, { .reversed = false });
    RH_ScopedInstall(CheckBuildingAtomics, 0x550170);
    RH_ScopedInstall(CheckPoolsEmpty, 0x551950);
    RH_ScopedInstall(GetObject, 0x550050);
    RH_ScopedInstall(GetObjectRef, 0x550020);
    RH_ScopedInstall(GetPed, 0x54FF90);
    RH_ScopedInstall(GetPedRef, 0x54FF60);
    RH_ScopedInstall(GetVehicle, 0x54FFF0);
    RH_ScopedInstall(GetVehicleRef, 0x54FFC0);
    RH_ScopedInstall(Load, 0x5D0890, { .enabled = LOAD_HOOKS_ENABLED });
    RH_ScopedInstall(LoadObjectPool, 0x5D4A40, { .enabled = LOAD_HOOKS_ENABLED });
    RH_ScopedInstall(LoadPedPool, 0x5D2D70, { .enabled = LOAD_HOOKS_ENABLED });
    RH_ScopedInstall(LoadVehiclePool, 0x5D2A20);
    RH_ScopedInstall(MakeSureSlotInObjectPoolIsEmpty, 0x550080);
    RH_ScopedInstall(Save, 0x5D0880, {.enabled = SAVE_HOOKS_ENABLED });
    RH_ScopedInstall(SaveObjectPool, 0x5D4940, { .reversed = false });
    RH_ScopedInstall(SavePedPool, 0x5D4B40, { .reversed = SAVE_HOOKS_ENABLED });
    RH_ScopedInstall(SaveVehiclePool, 0x5D4800, { .reversed = false });
}

// 0x550F10
void CPools::Initialise() {
    ZoneScoped;

    plugin::Call<0x550F10>();
    /*
    CMemoryMgr::PushMemId(MEM_POOLS);
    ms_pPtrNodeSingleLinkPool = new CPtrNodeSingleLinkPool(70000, "PtrNode Single");
    ms_pPtrNodeDoubleLinkPool = new CPtrNodeDoubleLinkPool(3200, "PtrNode Double");
    ms_pEntryInfoNodePool     = new CEntryInfoNodePool(500, "EntryInfoNode");
    ms_pPedPool               = new CPedPool(140, "Peds");
    ms_pVehiclePool           = new CVehiclePool(110, "Vehicles");
    ms_pBuildingPool          = new CBuildingPool(13000, "Buildings");
    ms_pObjectPool            = new CObjectPool(350, "Objects");
    ms_pDummyPool             = new CDummyPool(2500, "Dummys");
    ms_pColModelPool          = new CColModelPool(10150, "ColModel");
    ms_pTaskPool              = new CTaskPool(500, "Task");
    ms_pEventPool             = new CEventPool(200, "Event");
    ms_pPointRoutePool        = new CPointRoutePool(64, "PointRoute");
    ms_pPatrolRoutePool       = new CPatrolRoutePool(32, "PatrolRoute");
    ms_pNodeRoutePool         = new CNodeRoutePool(64, "NodeRoute");;
    ms_pTaskAllocatorPool     = new CTaskAllocatorPool(16, "TaskAllocator");
    ms_pPedIntelligencePool   = new CPedIntelligencePool(140, "PedIntelligence");
    ms_pPedAttractorPool      = new CPedAttractorPool(64, "PedAttractors");
    CMemoryMgr::PopMemId();
    */
}

// 0x5519F0
void CPools::ShutDown() {
    plugin::Call<0x5519F0>();
    /*
    DEV_LOG("Shutdown pool started");
    delete ms_pPtrNodeSingleLinkPool;
    delete ms_pPtrNodeDoubleLinkPool;
    delete ms_pEntryInfoNodePool;
    delete ms_pPedPool;
    delete ms_pVehiclePool;
    delete ms_pBuildingPool;
    delete ms_pObjectPool;
    delete ms_pDummyPool;
    delete ms_pColModelPool;
    delete ms_pTaskPool;
    delete ms_pEventPool;
    delete ms_pPointRoutePool;
    delete ms_pPatrolRoutePool;
    delete ms_pNodeRoutePool;
    delete ms_pTaskAllocatorPool;
    delete ms_pPedIntelligencePool;
    delete ms_pPedAttractorPool;
    DEV_LOG("Shutdown pool done");
    */
}

// 0x550170
int32 CPools::CheckBuildingAtomics() {
    return -1;
}

// 0x551950
void CPools::CheckPoolsEmpty() {
    for (auto i = 0; i < ms_pObjectPool->GetSize(); i++) {
        const auto obj = ms_pObjectPool->GetAt(i);
        if (!obj)
            continue;

        if (obj->m_nObjectType != OBJECT_TYPE_DECORATION)
            continue;

        const auto& objPos = obj->GetPosition();
        DEV_LOG("Offending object: MI: {} Coors:{} {} {}", obj->m_nModelIndex, objPos.x, objPos.y, objPos.z);
    }
    DEV_LOG("Pools have been cleared!");
}

// 0x550050
CObject* CPools::GetObject(int32 handle) {
    return ms_pObjectPool->GetAtRef(handle);
}

// 0x550020
int32 CPools::GetObjectRef(CObject* object) {
    return ms_pObjectPool->GetRef(object);
}

// 0x54FF90
CPed* CPools::GetPed(int32 handle) {
    return ms_pPedPool->GetAtRef(handle);
}

// 0x54FF60
int32 CPools::GetPedRef(CPed* ped) {
    return ms_pPedPool->GetRef(ped);
}

// 0x54FFF0
CVehicle* CPools::GetVehicle(int32 handle) {
    return ms_pVehiclePool->GetAtRef(handle);
}

// 0x54FFC0
int32 CPools::GetVehicleRef(CVehicle* vehicle) {
    return ms_pVehiclePool->GetRef(vehicle);
}

// 0x5D0890
bool CPools::Load() {
    bool loaded = LoadPedPool();
    if (loaded)
        loaded = LoadObjectPool();

    return loaded;
}

// 0x5D4A40
bool CPools::LoadObjectPool() {
    int32 iNumObjects = 0;
    LoadDataFromWorkBuffer(iNumObjects);
    for (int32 i = 0; i < iNumObjects; ++i)
    {
        int32 iPoolRef = 0, iModelId = 0;
        LoadDataFromWorkBuffer(iPoolRef);
        LoadDataFromWorkBuffer(iModelId);

        auto* objInPool = GetObjectPool()->GetAtRefNoChecks(iPoolRef);
        if (objInPool)
            CPopulation::ConvertToDummyObject(objInPool);

        auto* newObj = new(iPoolRef) CObject(iModelId, false);
        newObj->Load();
        CWorld::Add(newObj);
    }

    return true;
}

// 0x5D2D70
bool CPools::LoadPedPool() {
    int32 pedCount;
    LoadDataFromWorkBuffer(pedCount);

    for (auto i = 0; i < pedCount; i++) {
        int32 poolRef;
        int32 model;
        int32 pedType;

        LoadDataFromWorkBuffer(poolRef);
        LoadDataFromWorkBuffer(model);
        LoadDataFromWorkBuffer(pedType);

        CPlayerPed* playerPed = nullptr;
        if (pedType == PED_TYPE_PLAYER1) {
            playerPed = new(poolRef) CPlayerPed(0, false);
            playerPed->SetWeaponAccuracy(100);
            CWorld::Players[0].m_pPed = playerPed;
        }
        assert(playerPed != nullptr);

        playerPed->Load();
        CWorld::Add(playerPed);
    }
    return true;
}

// 0x5D2A20
// Used in CPools::Load (Android 1.0)
bool CPools::LoadVehiclePool() {
    int32 count;
    CGenericGameStorage::LoadDataFromWorkBuffer(&count, 4);
    for (auto i = 0; i < count; i++) {
        int32 modelId, createdBy;

        CGenericGameStorage::LoadDataFromWorkBuffer(&createdBy, 4);
        CGenericGameStorage::LoadDataFromWorkBuffer(&modelId, 4);

        CStreaming::RequestModel(modelId, STREAMING_KEEP_IN_MEMORY);
        CStreaming::LoadAllRequestedModels(false);

        CVehicle* vehicle = nullptr;
        switch (CModelInfo::GetModelInfo(modelId)->AsVehicleModelInfoPtr()->m_nVehicleType) {
        case VEHICLE_TYPE_MTRUCK:
            vehicle = new CMonsterTruck(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_QUAD:
            vehicle = new CQuadBike(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_HELI:
            vehicle = new CHeli(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_PLANE:
            vehicle = new CPlane(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_BOAT:
            vehicle = new CBoat(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_TRAIN:
            vehicle = new CTrain(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_BIKE:
            vehicle = new CBike(modelId, RANDOM_VEHICLE);
            vehicle->AsBike()->bikeFlags.bOnSideStand = true;
            break;
        case VEHICLE_TYPE_BMX:
            vehicle = new CBmx(modelId, RANDOM_VEHICLE);
            vehicle->AsBike()->bikeFlags.bOnSideStand = true;
            break;
        case VEHICLE_TYPE_TRAILER:
            vehicle = new CTrailer(modelId, RANDOM_VEHICLE);
            break;
        default:
            vehicle = new CAutomobile(modelId, RANDOM_VEHICLE, true);
            break;
        }
        assert(vehicle);
        vehicle->Load();
        --CCarCtrl::NumRandomCars;
        CCarCtrl::UpdateCarCount(vehicle, false);
        CWorld::Add(vehicle);
    }
    return true;
}

// 0x550080
void CPools::MakeSureSlotInObjectPoolIsEmpty(int32 slot) {
    if (GetObjectPool()->IsFreeSlotAtIndex(slot))
        return;

    auto* obj = GetObjectPool()->GetAt(slot);
    if (obj->IsTemporary())
    {
        CWorld::Remove(obj);
        delete obj;
    }
    else if (CProjectileInfo::RemoveIfThisIsAProjectile(obj))
    {
        auto newObj = new CObject(obj->m_nModelIndex, false);
        CWorld::Remove(obj);
        GetObjectPool()->CopyItem(newObj, obj);
        CWorld::Add(newObj);

        obj->m_pRwObject = nullptr;
        delete obj;

        newObj->m_pReferences = nullptr;
    }
}

// 0x5D0880
bool CPools::Save() {
    bool saved = SavePedPool();
    if (saved)
        saved = SaveObjectPool();

    return saved;
}

// 0x5D4940
bool CPools::SaveObjectPool() {
    return plugin::CallAndReturn<bool, 0x5D4940>();
}

// 0x5D4B40
bool CPools::SavePedPool() {
    return plugin::CallAndReturn<bool, 0x5D4B40>();
}

// 0x5D4800
// Used in CPools::Save (Android 1.0)
bool CPools::SaveVehiclePool() {
    return plugin::CallAndReturn<bool, 0x5D4800>();
}
