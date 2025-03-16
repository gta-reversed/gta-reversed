/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

// Sadly none of these can be forward declared simply.
// One possible way to do it would be to put each pool type into a different header
// And include only the necessary types there.
// Either way: The Pool's object type has to be included.

#include "Pool.h"
#include "TaskSimpleSlideToCoord.h"

#include "CopPed.h"
#include "Heli.h"
#include "Building.h"
#include "CutsceneObject.h"
#include "Dummy.h"
#include "ColModel.h"
#include "IplDef.h"
#include "Task.h"
#include "PedIntelligence.h"
#include "PtrNodeSingleLink.h"
#include "PtrNodeDoubleLink.h"
#include "EntryInfoNode.h"
#include "PedGeometryAnalyser.h" // PointRoute
//#include "PatrolRoute.h"
#include "Allocators/TaskAllocator.h"
#include "PedAttractor.h"

class CopPed;
class CHeli;
class CPed;
class CVehicle;
class CBuilding;
class CObject;
class CDummy;
class CColModel;
class CTask;
class CPedIntelligence;
class CPtrNodeSingleLink;
class CPtrNodeDoubleLink;
class CEntryInfoNode;
class CPointRoute;
class CPatrolRoute;
class CEvent;
class CNodeRoute;
class CTaskAllocator;
class CPedAttractor;
class CCutsceneObject;

#ifdef GetObject
#undef GetObject
#endif

class CIplDefPool : public CPool<IplDef> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CIplDefPool);
        RH_ScopedCategory("Pools");

        //RH_ScopedInstall(Constructor, 0x405900);
        RH_ScopedInstall(New, 0x004059B0);
    }
};

class CPedPool : public CPool<CPed, CCopPed> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPedPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x005E45E0);
        RH_ScopedInstall(GetAtRef, 0x404910);
    }
};

class CVehiclePool : public CPool<CVehicle, CHeli> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CVehiclePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x006E2A50);
    }
};

class CBuildingPool : public CPool<CBuilding> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CBuildingPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x403FA0);
    }
};

class CObjectPool : public CPool<CObject, CCutsceneObject> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CObjectPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(GetAt, 0x404870);
        RH_ScopedInstall(New, 0x005A1C20);
    }
};

class CDummyPool : public CPool<CDummy> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CDummyPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x00532630);
    }
};

class CColModelPool : public CPool<CColModel> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CColModelPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0040FB80);
    }
};

class CTaskPool : public CPool<CTask, CTaskSimpleSlideToCoord> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CTaskPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x61A500);
    }
};

class CPedIntelligencePool : public CPool<CPedIntelligence> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPedIntelligencePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0605EC0);
    }
};

class CPtrNodeSingleLinkPool : public CPool<CPtrNodeSingleLink> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPtrNodeSingleLinkPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0552240);
    }
};

class CPtrNodeDoubleLinkPool : public CPool<CPtrNodeDoubleLink> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPtrNodeDoubleLinkPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x05522E0);
    }
};

class CEntryInfoNodePool : public CPool<CEntryInfoNode> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CEntryInfoNodePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(GetAt, 0x00536D10);
    }
};

class CPointRoutePool : public CPool<CPointRoute> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPointRoutePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0041B5B0);
    }
};

class CPatrolRoutePool : public CPool<void*> {
//public:
//    static void InjectHooks() {
//        RH_ScopedClass(CPatrolRoutePool);
//        RH_ScopedCategory("Pools");
//
//        RH_ScopedInstall(New, 0x41B660);
//    }
};

class CEventPool : public CPool<CEvent, CEventDamage> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CEventPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x4B5570);
    }
};

class CNodeRoutePool : public CPool<CNodeRoute> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CNodeRoutePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0041B710);
    }
};

class CTaskAllocatorPool : public CPool<CTaskAllocator, CTaskAllocator, true> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CTaskAllocatorPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0069D8E0);
    }
};

class CPedAttractorPool : public CPool<CPedAttractor> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPedAttractorPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x5EA9F0);
    }
};

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
