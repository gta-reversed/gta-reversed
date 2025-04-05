#include "StdInc.h"

#include <Pools/Pools.h>
#include "CarCtrl.h"

#include <Pools/IplDefPool.h>
#include <Pools/PedPool.h>
#include <Pools/VehiclePool.h>
#include <Pools/BuildingPool.h>
#include <Pools/ObjectPool.h>
#include <Pools/DummyPool.h>
#include <Pools/ColModelPool.h>
#include <Pools/TaskPool.h>
#include <Pools/PedIntelligencePool.h>
#include <Pools/PtrNodeSingleLinkPool.h>
#include <Pools/PtrNodeDoubleLinkPool.h>
#include <Pools/EntryInfoNodePool.h>
#include <Pools/PointRoutePool.h>
#include <Pools/PatrolRoutePool.h>
#include <Pools/EventPool.h>
#include <Pools/NodeRoutePool.h>
#include <Pools/TaskAllocatorPool.h>
#include <Pools/PedAttractorPool.h>

auto& ms_pPedPool               = StaticRef<CPedPool*>(0xB74490);
auto& ms_pVehiclePool           = StaticRef<CVehiclePool*>(0xB74494);
auto& ms_pBuildingPool          = StaticRef<CBuildingPool*>(0xB74498);
auto& ms_pObjectPool            = StaticRef<CObjectPool*>(0xB7449C);
auto& ms_pDummyPool             = StaticRef<CDummyPool*>(0xB744A0);
auto& ms_pColModelPool          = StaticRef<CColModelPool*>(0xB744A4);
auto& ms_pTaskPool              = StaticRef<CTaskPool*>(0xB744A8);
auto& ms_pPedIntelligencePool   = StaticRef<CPedIntelligencePool*>(0xB744C0);
auto& ms_pPtrNodeSingleLinkPool = StaticRef<CPtrNodeSingleLinkPool*>(0xB74484);
auto& ms_pPtrNodeDoubleLinkPool = StaticRef<CPtrNodeDoubleLinkPool*>(0xB74488);
auto& ms_pEntryInfoNodePool     = StaticRef<CEntryInfoNodePool*>(0xB7448C);
auto& ms_pPointRoutePool        = StaticRef<CPointRoutePool*>(0xB744B0);
auto& ms_pPatrolRoutePool       = StaticRef<CPatrolRoutePool*>(0xB744B4);
auto& ms_pEventPool             = StaticRef<CEventPool*>(0xB744AC);
auto& ms_pNodeRoutePool         = StaticRef<CNodeRoutePool*>(0xB744B8);
auto& ms_pTaskAllocatorPool     = StaticRef<CTaskAllocatorPool*>(0xB744BC);
auto& ms_pPedAttractorPool      = StaticRef<CPedAttractorPool*>(0xB744C4);

void CPools::InjectHooks() {
    RH_ScopedClass(CPools);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x550F10);
    RH_ScopedInstall(ShutDown, 0x5519F0);
    RH_ScopedInstall(CheckBuildingAtomics, 0x550170);
    RH_ScopedInstall(CheckPoolsEmpty, 0x551950);
    RH_ScopedInstall(GetObject, 0x550050);
    RH_ScopedInstall(GetObjectRef, 0x550020);
    RH_ScopedInstall(GetPed, 0x54FF90);
    RH_ScopedInstall(GetPedRef, 0x54FF60);
    RH_ScopedInstall(GetVehicle, 0x54FFF0);
    RH_ScopedInstall(GetVehicleRef, 0x54FFC0);
    RH_ScopedInstall(Load, 0x5D0890);
    RH_ScopedInstall(LoadObjectPool, 0x5D4A40);
    RH_ScopedInstall(LoadPedPool, 0x5D2D70);
    RH_ScopedInstall(LoadVehiclePool, 0x5D2A20);
    RH_ScopedInstall(MakeSureSlotInObjectPoolIsEmpty, 0x550080);
    RH_ScopedInstall(Save, 0x5D0880);
    RH_ScopedInstall(SaveObjectPool, 0x5D4940, { .reversed = false });
    RH_ScopedInstall(SavePedPool, 0x5D4B40, { .reversed = false });
    RH_ScopedInstall(SaveVehiclePool, 0x5D4800, { .reversed = false });
}

// 0x550F10
void CPools::Initialise() {
    ZoneScoped;

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
}

// 0x5519F0
void CPools::ShutDown() {
    NOTSA_LOG_DEBUG("Shutdown pool started");
    delete std::exchange(ms_pPtrNodeSingleLinkPool, nullptr);
    delete std::exchange(ms_pPtrNodeDoubleLinkPool, nullptr);
    delete std::exchange(ms_pEntryInfoNodePool, nullptr);
    delete std::exchange(ms_pPedPool, nullptr);
    delete std::exchange(ms_pVehiclePool, nullptr);
    delete std::exchange(ms_pBuildingPool, nullptr);
    delete std::exchange(ms_pObjectPool, nullptr);
    delete std::exchange(ms_pDummyPool, nullptr);
    delete std::exchange(ms_pColModelPool, nullptr);
    delete std::exchange(ms_pTaskPool, nullptr);
    delete std::exchange(ms_pEventPool, nullptr);
    delete std::exchange(ms_pPointRoutePool, nullptr);
    delete std::exchange(ms_pPatrolRoutePool, nullptr);
    delete std::exchange(ms_pNodeRoutePool, nullptr);
    delete std::exchange(ms_pTaskAllocatorPool, nullptr);
    delete std::exchange(ms_pPedIntelligencePool, nullptr);
    delete std::exchange(ms_pPedAttractorPool, nullptr);
    NOTSA_LOG_DEBUG("Shutdown pool done");
}

// 0x550170
int32 CPools::CheckBuildingAtomics() {
    return -1;
}

// 0x551950
void CPools::CheckPoolsEmpty() {
    for (auto& obj : ms_pObjectPool->GetAllValid()) {
        if (obj.m_nObjectType != OBJECT_TYPE_DECORATION)
            continue;

        const auto& objPos = obj.GetPosition();
        NOTSA_LOG_DEBUG("Offending object: MI: {} Coors:{} {} {}", obj.m_nModelIndex, objPos.x, objPos.y, objPos.z);
    }
    NOTSA_LOG_DEBUG("Pools have been cleared!");
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
    auto iNumObjects = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
    for (int32 i = 0; i < iNumObjects; ++i)
    {
        auto iPoolRef = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
        auto iModelId = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();

        auto* objInPool = CPools::GetObjectPool()->GetAtRefNoChecks(iPoolRef);
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
    const auto pedCount = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
    for (auto i = 0; i < pedCount; i++) {
        const auto poolRef = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
        const auto model   = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
        const auto pedType = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();

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
    if (CPools::GetObjectPool()->IsFreeSlotAtIndex(slot))
        return;

    auto* obj = CPools::GetObjectPool()->GetAt(slot);
    if (obj->IsTemporary())
    {
        CWorld::Remove(obj);
        delete obj;
    }
    else if (CProjectileInfo::RemoveIfThisIsAProjectile(obj))
    {
        auto newObj = new CObject(obj->m_nModelIndex, false);
        CWorld::Remove(obj);
        CPools::GetObjectPool()->CopyItem(newObj, obj);
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

// 0x404550
CPedPool* CPools::GetPedPool() {
    return ms_pPedPool;
}

// 0x404560
CVehiclePool* CPools::GetVehiclePool() {
    return ms_pVehiclePool;
}

// 0x403DF0
CBuildingPool* CPools::GetBuildingPool() {
    return ms_pBuildingPool;
}

// 0x404570
CObjectPool* CPools::GetObjectPool() {
    return ms_pObjectPool;
}

// 0x404580
CDummyPool* CPools::GetDummyPool() {
    return ms_pDummyPool;
}

// 0x40F000
CColModelPool* CPools::GetColModelPool() {
    return ms_pColModelPool;
}

// 0x61A330
CTaskPool* CPools::GetTaskPool() {
    return ms_pTaskPool;
}

// 0x5FF930
CPedIntelligencePool* CPools::GetPedIntelligencePool() {
    return ms_pPedIntelligencePool;
}

// 0x552190
CPtrNodeSingleLinkPool* CPools::GetPtrNodeSingleLinkPool() {
    return ms_pPtrNodeSingleLinkPool;
}

// 0x5521A0
CPtrNodeDoubleLinkPool* CPools::GetPtrNodeDoubleLinkPool() {
    return ms_pPtrNodeDoubleLinkPool;
}

// 0x536C80
CEntryInfoNodePool* CPools::GetEntryInfoNodePool() {
    return ms_pEntryInfoNodePool;
}

// 0x41B400
CPointRoutePool* CPools::GetPointRoutePool() {
    return ms_pPointRoutePool;
}

// 0x41B410
CPatrolRoutePool* CPools::GetPatrolRoutePool() {
    return ms_pPatrolRoutePool;
}

// 0x4ABF80
CEventPool* CPools::GetEventPool() {
    return ms_pEventPool;
}

// 0x41B420
CNodeRoutePool* CPools::GetNodeRoutePool() {
    return ms_pNodeRoutePool;
}

// 0x69BB70
CTaskAllocatorPool* CPools::GetTaskAllocatorPool() {
    return ms_pTaskAllocatorPool;
}

// 0x5E95A0
CPedAttractorPool* CPools::GetPedAttractorPool() {
    return ms_pPedAttractorPool;
}
