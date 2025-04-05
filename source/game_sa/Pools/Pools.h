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

    static auto& GetPedPool();
    static auto& GetVehiclePool();
    static auto& GetBuildingPool();
    static auto& GetObjectPool();
    static auto& GetDummyPool();
    static auto& GetColModelPool();
    static auto& GetTaskPool();
    static auto& GetPedIntelligencePool();
    static auto& GetPtrNodeSingleLinkPool();
    static auto& GetPtrNodeDoubleLinkPool();
    static auto& GetEntryInfoNodePool();
    static auto& GetPointRoutePool();
    static auto& GetPatrolRoutePool();
    static auto& GetEventPool();
    static auto& GetNodeRoutePool();
    static auto& GetTaskAllocatorPool();
    static auto& GetPedAttractorPool();
};

/*
GetPoolObj(int32)
GetPoolPed(int32)
GetPoolVehicle(int32)
GettPoolObjRef(CObject* object)
GettPoolPedRef(CPed* ped)
GettPoolVehicleRef(CVehicle* vehicle)
*/
