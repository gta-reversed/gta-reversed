/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Pools/IplDefPool.h"
#include "Pools/PedPool.h"
#include "Pools/VehiclePool.h"
#include "Pools/BuildingPool.h"
#include "Pools/ObjectPool.h"
#include "Pools/DummyPool.h"
#include "Pools/ColModelPool.h"
#include "Pools/TaskPool.h"
#include "Pools/PedIntelligencePool.h"
#include "Pools/PtrNodeSingleLinkPool.h"
#include "Pools/PtrNodeDoubleLinkPool.h"
#include "Pools/EntryInfoNodePool.h"
#include "Pools/PointRoutePool.h"
#include "Pools/PatrolRoutePool.h"
#include "Pools/EventPool.h"
#include "Pools/NodeRoutePool.h"
#include "Pools/TaskAllocatorPool.h"
#include "Pools/PedAttractorPool.h"

class CPools {
public:
    static inline CPedPool*&               ms_pPedPool               = *(CPedPool**)0xB74490;
    static inline CVehiclePool*&           ms_pVehiclePool           = *(CVehiclePool**)0xB74494;
    static inline CBuildingPool*&          ms_pBuildingPool          = *(CBuildingPool**)0xB74498;
    static inline CObjectPool*&            ms_pObjectPool            = *(CObjectPool**)0xB7449C;
    static inline CDummyPool*&             ms_pDummyPool             = *(CDummyPool**)0xB744A0;
    static inline CColModelPool*&          ms_pColModelPool          = *(CColModelPool**)0xB744A4;
    static inline CTaskPool*&              ms_pTaskPool              = *(CTaskPool**)0xB744A8;
    static inline CPedIntelligencePool*&   ms_pPedIntelligencePool   = *(CPedIntelligencePool**)0xB744C0;
    static inline CPtrNodeSingleLinkPool*& ms_pPtrNodeSingleLinkPool = *(CPtrNodeSingleLinkPool**)0xB74484;
    static inline CPtrNodeDoubleLinkPool*& ms_pPtrNodeDoubleLinkPool = *(CPtrNodeDoubleLinkPool**)0xB74488;
    static inline CEntryInfoNodePool*&     ms_pEntryInfoNodePool     = *(CEntryInfoNodePool**)0xB7448C;
    static inline CPointRoutePool*&        ms_pPointRoutePool        = *(CPointRoutePool**)0xB744B0;
    static inline CPatrolRoutePool*&       ms_pPatrolRoutePool       = *(CPatrolRoutePool**)0xB744B4;
    static inline CEventPool*&             ms_pEventPool             = *(CEventPool**)0xB744AC;
    static inline CNodeRoutePool*&         ms_pNodeRoutePool         = *(CNodeRoutePool**)0xB744B8;
    static inline CTaskAllocatorPool*&     ms_pTaskAllocatorPool     = *(CTaskAllocatorPool**)0xB744BC;
    static inline CPedAttractorPool*&      ms_pPedAttractorPool      = *(CPedAttractorPool**)0xB744C4;

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

static auto& GetPedPool()               { return CPools::ms_pPedPool; }               // 0x404550
static auto& GetVehiclePool()           { return CPools::ms_pVehiclePool; }           // 0x404560
static auto& GetBuildingPool()          { return CPools::ms_pBuildingPool; }          // 0x403DF0
static auto& GetObjectPool()            { return CPools::ms_pObjectPool; }            // 0x404570
static auto& GetDummyPool()             { return CPools::ms_pDummyPool; }             // 0x404580
static auto& GetColModelPool()          { return CPools::ms_pColModelPool; }          // 0x40F000
static auto& GetTaskPool()              { return CPools::ms_pTaskPool; }              // 0x61A330
static auto& GetPedIntelligencePool()   { return CPools::ms_pPedIntelligencePool; }   // 0x5FF930
static auto& GetPtrNodeSingleLinkPool() { return CPools::ms_pPtrNodeSingleLinkPool; } // 0x552190
static auto& GetPtrNodeDoubleLinkPool() { return CPools::ms_pPtrNodeDoubleLinkPool; } // 0x5521A0
static auto& GetEntryInfoNodePool()     { return CPools::ms_pEntryInfoNodePool; }     // 0x536C80
static auto& GetPointRoutePool()        { return CPools::ms_pPointRoutePool; }        // 0x41B400
static auto& GetPatrolRoutePool()       { return CPools::ms_pPatrolRoutePool; }       // 0x41B410
static auto& GetEventPool()             { return CPools::ms_pEventPool; }             // 0x4ABF80
static auto& GetNodeRoutePool()         { return CPools::ms_pNodeRoutePool; }         // 0x41B420
static auto& GetTaskAllocatorPool()     { return CPools::ms_pTaskAllocatorPool; }     // 0x69BB70
static auto& GetPedAttractorPool()      { return CPools::ms_pPedAttractorPool; }      // 0x5E95A0

/*
GetPoolObj(int32)
GetPoolPed(int32)
GetPoolVehicle(int32)
GettPoolObjRef(CObject* object)
GettPoolPedRef(CPed* ped)
GettPoolVehicleRef(CVehicle* vehicle)
*/
