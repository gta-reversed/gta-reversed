/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "LinkList.h"

class CEntity;
class CClumpModelInfo;
class CAtomicModelInfo;

// VehicleAtomicType
enum eAtomicComponentFlag {
    ATOMIC_NONE = 0x0,
    ATOMIC_OK = 0x1,
    ATOMIC_DAMAGED = 0x2,

    // Mask for checking whether the value is OK or DAMAGED ^^^
    ATOMIC_MASK = 0x3,

    ATOMIC_LEFT = 0x4,
    ATOMIC_RIGHT = 0x8,
    ATOMIC_FRONT = 0x10,
    ATOMIC_REAR = 0x20,
    ATOMIC_ALPHA = 0x40,
    ATOMIC_FLAT = 0x80,
    ATOMIC_REAR_DOOR = 0x100,
    ATOMIC_FRONT_DOOR = 0x200,
    ATOMIC_DONT_CULL = 0x400, // enabled for opened components (doors)
    ATOMIC_UPGRADE = 0x800,
    ATOMIC_DONT_RENDER_ALPHA = 0x1000,
    ATOMIC_UNIQUE_MATERIALS = 0x2000,
    ATOMIC_PIPE_NO_EXTRA_PASSES_LOD = 0x2000,
    ATOMIC_PIPE_NO_EXTRA_PASSES = 0x4000,
    ATOMIC_TOP = 0x8000
};

// NOTSA:
// Helper to setup vehicle atomic variables (Matrix, ID, DotProduct)
struct VehicleAtomicData {
    uint32 Id;
    float  Dot;
    float  SortDist; // Calculated distance for sorting (usually dist +/- offset)
};

typedef RwBool (*ClumpVisibilityCallback)(RpClump* clump); // In source

struct tAtomicVisibilityPlugin {
    int16 m_modelId;
    uint16 m_flags;
};

VALIDATE_SIZE(tAtomicVisibilityPlugin, 0x4);

struct tClumpVisibilityPlugin {
    ClumpVisibilityCallback m_visibilityCallBack;
    int32 m_alpha;
};

VALIDATE_SIZE(tClumpVisibilityPlugin, 0x8);

struct tFrameVisibilityPlugin {
    int32 m_hierarchyId;
};

VALIDATE_SIZE(tFrameVisibilityPlugin, 0x4);

class CVisibilityPlugins {
public:
    static const int32 TOTAL_ALPHA_LISTS = 20;
    static const int32 TOTAL_ALPHA_BOAT_ATOMIC_LISTS = 20;
    static const int32 TOTAL_ALPHA_ENTITY_LISTS = 200;
    static const int32 TOTAL_ALPHA_UNDERWATER_ENTITY_LISTS = 100;
    static const int32 TOTAL_ALPHA_DRAW_LAST_LISTS = 50;
    static const int32 TOTAL_WEAPON_PEDS_FOR_PC = 100;

    using RenderFunction = void (*)(void* entity, float distance);

    struct AlphaObjectInfo {
        void* m_pObj;
        RenderFunction m_pCallback;
        float m_distance; // alpha
    };

private:
    static RwInt32 ms_atomicPluginOffset; // 0x8D608C
    static RwInt32 ms_framePluginOffset;  // 0x8D6090
    static RwInt32 ms_clumpPluginOffset;  // 0x8D6094
    static RpAtomicCallBackRender ms_defaultRenderer; // 0xC88048, unused
    static RwCamera* ms_pCamera; // 0xC8804C
    static RwV3d* ms_pCameraPosn; // 0xC88050

    static float ms_vehicleLod0RenderMultiPassDist; // 0xC88044
    static float ms_vehicleLod0Dist; // 0xC88040
    static float ms_vehicleLod1Dist; // 0xC8803C
    // static float ms_vehicleFadeDist; // unused
    static float ms_bigVehicleLod0Dist; // 0xC88038
    // static float ms_bigVehicleLod1Dist; // unused
    static float ms_pedLodDist; // 0xC88034
    static float ms_pedFadeDist; // 0xC88030
    static float ms_cullCompsDist; // 0xC8802C
    static float ms_cullBigCompsDist; // 0xC88028

    static CLinkList<AlphaObjectInfo> m_alphaList; // 0xC88070
    static CLinkList<AlphaObjectInfo> m_alphaEntityList; // 0xC88120
    static CLinkList<AlphaObjectInfo> m_alphaUnderwaterEntityList; // 0xC88178
    static CLinkList<AlphaObjectInfo> m_alphaBoatAtomicList; // 0xC880C8
    static CLinkList<AlphaObjectInfo> m_alphaReallyDrawLastList; // 0xC881D0

    static CLinkList<CPed*> ms_weaponPedsForPC; // 0xC88224

public:
    static bool PluginAttach();
    static void Initialise();
    static void Shutdown();

    static void InitAlphaAtomicList();
    static void InitAlphaEntityList();
    static void RenderEntity(void* obj, float dist);
    static void RenderAtomic(void* obj, float dist); // dist - unused
    // static void RenderAtomicList(CLinkList<CVisibilityPlugins::AlphaObjectInfo>& alphaObjectInfoList); // unused
    static void RenderAlphaAtomics();
    static void RenderBoatAlphaAtomics();
    static void RenderFadingEntities();
    static void RenderFadingUnderwaterEntities();
    static void RenderReallyDrawLastObjects();
    // static void RenderFadingEntities(CLinkList<CVisibilityPlugins::AlphaObjectInfo>& alphaObjectInfoList); // unused
    static void RenderOrderedList(CLinkList<CVisibilityPlugins::AlphaObjectInfo>& alphaObjectInfoList);
    static bool InsertAtomicIntoSortedList(RpAtomic* atomic, float dist);
    static bool InsertAtomicIntoBoatSortedList(RpAtomic* atomic, float dist);
    static bool InsertEntityIntoSortedList(CEntity* entity, float dist);
    static bool InsertEntityIntoUnderwaterList(CEntity* pEntity, float dist);
    static bool InsertAtomicIntoReallyDrawLastList(RpAtomic* atomic, float dist);
    static bool InsertEntityIntoReallyDrawLastList(CEntity* entity, float dist);
    static bool InsertObjectIntoSortedList(void* obj, float dist, RenderFunction fn);
    static CLinkList<CVisibilityPlugins::AlphaObjectInfo>& GetAlphaList() { return m_alphaEntityList; } // unused
    static CLinkList<CVisibilityPlugins::AlphaObjectInfo>& GetAlphaUnderwaterList() { return m_alphaUnderwaterEntityList; } // unused

    static void SetModelInfoIndex(RpAtomic* atomic, int32 index);
    static int32 GetModelInfoIndex(RpAtomic* atomic);
    static CBaseModelInfo* GetModelInfo(RpAtomic* atomic);
    static void SetAtomicId(RpAtomic* atomic, int32 id);
    static void SetAtomicFlag(RpAtomic* atomic, uint16 flag);
    static void SetClumpForAllAtomicsFlag(RpClump* clump, int32 id);
    static void ClearAtomicFlag(RpAtomic* atomic, uint16 flag);
    static void ClearClumpForAllAtomicsFlag(RpClump* clump, int32 id);
    static int32 GetAtomicId(RpAtomic* atomic);
    static void SetUserValue(RpAtomic* atomic, uint16 value);
    static uint16 GetUserValue(RpAtomic* atomic);

    static void  SetFrameHierarchyId(RwFrame* frame, int32 id);
    static int32 GetFrameHierarchyId(RwFrame* frame);

    static void SetClumpModelInfo(RpClump* clump, CClumpModelInfo* clumpModelInfo);
    static CClumpModelInfo* GetClumpModelInfo(RpClump* clump);
    static void SetClumpAlpha(RpClump* clump, int32 alpha);
    static int32 GetClumpAlpha(RpClump* clump);
    static bool IsClumpVisible(RpClump* clump);
    static bool IsAtomicVisible(RpAtomic* atomic);

    static void SetupVehicleVariables(RpClump* clump);

    static RpAtomic* RenderVehicleReallyLowDetailCB(RpAtomic* atomic);
    static RpAtomic* RenderVehicleHiDetailCB(RpAtomic* atomic);
    static RpAtomic* RenderVehicleHiDetailAlphaCB(RpAtomic* atomic);
    static RpAtomic* RenderTrainHiDetailCB(RpAtomic* atomic);
    static RpAtomic* RenderTrainHiDetailAlphaCB(RpAtomic* atomic);
    static RpAtomic* RenderWheelAtomicCB(RpAtomic* atomic);

    static RpAtomic* RenderHeliRotorAlphaCB(RpAtomic* atomic);
    static RpAtomic* RenderHeliTailRotorAlphaCB(RpAtomic* atomic);

    static RpAtomic* RenderVehicleReallyLowDetailCB_BigVehicle(RpAtomic* atomic);
    // static RpAtomic* RenderVehicleLowDetailCB_BigVehicle(RpAtomic* atomic); // unused
    static RpAtomic* RenderVehicleHiDetailCB_BigVehicle(RpAtomic* atomic);
    // static RpAtomic* RenderVehicleLowDetailAlphaCB_BigVehicle(RpAtomic* atomic); // unused
    static RpAtomic* RenderVehicleHiDetailAlphaCB_BigVehicle(RpAtomic* atomic);

    static RpAtomic* RenderVehicleHiDetailCB_Boat(RpAtomic* atomic);
    static RpAtomic* RenderVehicleHiDetailAlphaCB_Boat(RpAtomic* atomic);
    static RpAtomic* RenderVehicleLoDetailCB_Boat(RpAtomic* atomic);

    static RpAtomic* RenderPlayerCB(RpAtomic* atomic);
    static RpAtomic* RenderPedCB(RpAtomic* atomic);

    static RpAtomic* RenderWeaponCB(RpAtomic* atomic);

    static RpAtomic* RenderFadingClumpCB(RpAtomic* atomic);

    static void SetAtomicRenderCallback(RpAtomic* atomic, RpAtomicCallBackRender renderCB);

    static void SetRenderWareCamera(RwCamera* pRwCamera);
    static float GetDistanceSquaredFromCamera(RwV3d* pos);

    static void RenderWeaponPedsForPC();
    static void ResetWeaponPedsForPC() { ms_weaponPedsForPC.Clear(); } // inline
    static void AddWeaponPedForPC(CPed* ped) { ms_weaponPedsForPC.Insert(ped); } // 0x5E46D0

private:
    static void* AtomicConstructor(void* atomic, RwInt32 offset, RwInt32 size);
    static void* AtomicCopyConstructor(void* destAtomic, const void* srcAtomic, RwInt32 offset, RwInt32 size);
    static void* AtomicDestructor(void* atomic, RwInt32 offset, RwInt32 size);

    static void* ClumpConstructor(void* clump, RwInt32 offset, RwInt32 size);
    static void* ClumpCopyConstructor(void* destClump, const void* srcObject, RwInt32 offset, RwInt32 size);
    static void* ClumpDestructor(void* clump, RwInt32 offset, RwInt32 size);

    static void* FrameConstructor(void* frame, RwInt32 offset, RwInt32 size);
    static void* FrameCopyConstructor(void* destFrame, const void* srcFrame, RwInt32 offset, RwInt32 size);
    static void* FrameDestructor(void* frame, RwInt32 offset, RwInt32 size);

    static RpAtomic* RenderObjNormalAtomic(RpAtomic* atomic);

    static void RenderAtomicWithAlpha(RpAtomic* atomic, int32 alpha);
    static int32 CalculateFadingAtomicAlpha(CBaseModelInfo* modelInfo, CEntity* entity, float distance);
    static void SetupRenderFadingAtomic(CBaseModelInfo* modelInfo, int32 alpha); // alpha - unused
    static void ResetRenderFadingAtomic(CBaseModelInfo* modelInfo);
    static void RenderFadingAtomic(CBaseModelInfo* modelInfo, RpAtomic* atomic, int32 alpha);
    static void RenderFadingClump(CBaseModelInfo* modelInfo, RpClump* clump, int32 alpha);
    static RpAtomic* RenderAtomicWithAlphaCB(RpAtomic* atomic, void* data);

    static RwBool DefaultVisibilityCB(RpClump* clump);
    static RwBool FrustumSphereCB(RpClump* clump);
    static RwBool VehicleVisibilityCB(RpClump* clump);
    static RwBool VehicleVisibilityCB_BigVehicle(RpClump* clump);

    static float GetDistanceSquaredFromCamera(RwFrame* frame);
    static float GetDotProductWithCameraVector(RwMatrix* matrix, RwMatrix* carMatrix, uint32 atomicId);

    static void RenderVehicleCB_ControlRenderMultiPassLOD(RpAtomic* pAtomic, float vehicleDistanceFromCamera);

public:
    // NOTSA:
    static void InjectHooks();

    static float GetVehicleDotProduct(RpAtomic* atomic, uint32& outAtomicId);
    static bool ShouldCullVehicleAtomic(float distFromCam, float cullDist, float dotProduct, uint32 atomicId, bool bUseComplexHeuristic);
};

RpAtomic*   clearClumpForAllAtomicsFlagCB(RpAtomic* atomic, void* data);
RpMaterial* SetAlphaCB(RpMaterial* material, void* data);
RpMaterial* SetTextureCB(RpMaterial* material, void* texture);
RpAtomic*   setClumpForAllAtomicsFlagCB(RpAtomic* atomic, void* data);
