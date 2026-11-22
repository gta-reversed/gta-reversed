/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Base.h>

class CObject;
class CPed;
class CVehicle;
class CPed;
class CVehicle;
class CBuilding;
class CObject;
class CDummy;
class CColModel;
class CTask;
class CPedIntelligence;
template<typename T>
class CPtrNodeSingleLink;
template<typename T>
class CPtrNodeDoubleLink;
class CEntryInfoNode;
class CPointRoute;
class CPatrolRoute;
class CEvent;
class CNodeRoute;
class CTaskAllocator;
class CPedAttractor;

class CPedPool;
class CVehiclePool;
class CBuildingPool;
class CObjectPool;
class CDummyPool;
class CColModelPool;
class CTaskPool;
class CPedIntelligencePool;
class CPtrNodeSingleLinkPool;
class CPtrNodeDoubleLinkPool;
class CEntryInfoNodePool;
class CPointRoutePool;
class CPatrolRoutePool;
class CEventPool;
class CNodeRoutePool;
class CTaskAllocatorPool;
class CPedAttractorPool;

CPedPool*               GetPedPool();
CVehiclePool*           GetVehiclePool();
CBuildingPool*          GetBuildingPool();
CObjectPool*            GetObjectPool();
CDummyPool*             GetDummyPool();
CColModelPool*          GetColModelPool();
CTaskPool*              GetTaskPool();
CPedIntelligencePool*   GetPedIntelligencePool();
CPtrNodeSingleLinkPool* GetPtrNodeSingleLinkPool();
CPtrNodeDoubleLinkPool* GetPtrNodeDoubleLinkPool();
CEntryInfoNodePool*     GetEntryInfoNodePool();
CPointRoutePool*        GetPointRoutePool();
CPatrolRoutePool*       GetPatrolRoutePool();
CEventPool*             GetEventPool();
CNodeRoutePool*         GetNodeRoutePool();
CTaskAllocatorPool*     GetTaskAllocatorPool();
CPedAttractorPool*      GetPedAttractorPool();

namespace notsa {
template<typename T>
auto& PoolOf() = delete;

template<>
auto& PoolOf<CColModel>() {
    return *GetColModelPool();
}

template<>
auto& PoolOf<CPedIntelligence>() {
    return *GetPedIntelligencePool();
}

//template<>
//auto& PoolOf<CPtrNodeSingleLink>() { return *GetPtrNodeSingleLinkPool(); }
//
//template<>
//auto& PoolOf<CPtrNodeDoubleLink>() { return *GetPtrNodeDoubleLinkPool(); }

template<>
auto& PoolOf<CEntryInfoNode>() {
    return *GetEntryInfoNodePool();
}

template<>
auto& PoolOf<CPointRoute>() {
    return *GetPointRoutePool();
}

//template<>
//auto& PoolOf<CPatrolRoute>() { return *GetPatrolRoutePool(); }

template<>
auto& PoolOf<CNodeRoute>() {
    return *GetNodeRoutePool();
}

template<>
auto& PoolOf<CTaskAllocator>() {
    return *GetTaskAllocatorPool();
}

template<>
auto& PoolOf<CPedAttractor>() {
    return *GetPedAttractorPool();
}

/*
* Pools of derived types
*/
template<typename T>
    requires std::is_base_of_v<CPed, T>
auto& PoolOf() {
    return *GetPedPool();
}

template<typename T>
    requires std::is_base_of_v<CVehicle, T>
auto& PoolOf() {
    return *GetVehiclePool();
}

template<typename T>
    requires std::is_base_of_v<CBuilding, T>
auto& PoolOf() {
    return *GetBuildingPool();
}

template<typename T>
    requires std::is_base_of_v<CObject, T>
auto& PoolOf() {
    return *GetObjectPool();
}

template<typename T>
    requires std::is_base_of_v<CDummy, T>
auto& PoolOf() {
    return *GetDummyPool();
}

template<typename T>
    requires std::is_base_of_v<CTask, T>
auto& PoolOf() {
    return *GetTaskPool();
}

template<typename T>
    requires std::is_base_of_v<CEvent, T>
auto& PoolOf() {
    return *GetEventPool();
}

template<typename T>
concept PooledType =
    requires { notsa::PoolOf<typename std::remove_cvref_t<T>>(); };
};


class CPools {
public:
    static void InjectHooks();

    static void Initialise();
    static void ShutDown();

    static int32 CheckBuildingAtomics();
    static void CheckPoolsEmpty();
    static CObject* GetObject(int32 handle);
    static int32 GetObjectRef(CObject* object);
    static CPed* GetPed(int32 handle);
    static int32 GetPedRef(CPed* ped);
    static CVehicle* GetVehicle(int32 handle);
    static int32 GetVehicleRef(CVehicle* vehicle);

    static bool Load();
    static bool LoadObjectPool();
    static bool LoadPedPool();
    static bool LoadVehiclePool();

    static void MakeSureSlotInObjectPoolIsEmpty(int32 slot);

    static bool Save();
    static bool SaveObjectPool();
    static bool SavePedPool();
    static bool SaveVehiclePool();

    template<notsa::PooledType T>
    static bool IsValidPointer(T* ptr) { return notsa::PoolOf<T>().IsObjectValid(ptr); }
};
