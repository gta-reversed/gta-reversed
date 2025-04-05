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

    static CPedPool*               GetPedPool();
    static CVehiclePool*           GetVehiclePool();
    static CBuildingPool*          GetBuildingPool();
    static CObjectPool*            GetObjectPool();
    static CDummyPool*             GetDummyPool();
    static CColModelPool*          GetColModelPool();
    static CTaskPool*              GetTaskPool();
    static CPedIntelligencePool*   GetPedIntelligencePool();
    static CPtrNodeSingleLinkPool* GetPtrNodeSingleLinkPool();
    static CPtrNodeDoubleLinkPool* GetPtrNodeDoubleLinkPool();
    static CEntryInfoNodePool*     GetEntryInfoNodePool();
    static CPointRoutePool*        GetPointRoutePool();
    static CPatrolRoutePool*       GetPatrolRoutePool();
    static CEventPool*             GetEventPool();
    static CNodeRoutePool*         GetNodeRoutePool();
    static CTaskAllocatorPool*     GetTaskAllocatorPool();
    static CPedAttractorPool*      GetPedAttractorPool();
};
