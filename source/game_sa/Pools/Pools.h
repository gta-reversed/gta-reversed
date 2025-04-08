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

};

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
