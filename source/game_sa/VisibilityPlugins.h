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

enum eAtomicComponentFlag {
    ATOMIC_IS_NOT_PRESENT = 0x0,
    ATOMIC_IS_OK_STATE = 0x1,
    ATOMIC_IS_DAM_STATE = 0x2,
    ATOMIC_IS_LEFT = 0x4,
    ATOMIC_IS_RIGHT = 0x8,
    ATOMIC_IS_FRONT = 0x10,
    ATOMIC_IS_REAR = 0x20,
    ATOMIC_HAS_ALPHA = 0x40,
    ATOMIC_CULL = 0x80,
    ATOMIC_IS_REAR_DOOR = 0x100,
    ATOMIC_IS_FRONT_DOOR = 0x200,
    ATOMIC_RENDER_ALWAYS = 0x400, // enabled for opened components (doors)
    ATOMIC_IS_REPLACEMENT_UPGRADE = 0x800,
    ATOMIC_IS_DOOR_WINDOW_OPENED = 0x1000,
    ATOMIC_DISABLE_REFLECTIONS = 0x2000,
    // ATOMIC_HAS_DYNAMIC_TEXTURES  = 0x2000, // TODO: WTF is this?
    ATOMIC_IS_BLOWN_UP = 0x4000,
    ATOMIC_VEHCOMP_15 = 0x8000
};

struct tAtomicVisibilityPlugin {
    int16 m_modelId;
    union {
        uint16 m_flags; // eAtomicComponentFlag
        int16 m_userValue;
    };
};
VALIDATE_SIZE(tAtomicVisibilityPlugin, 0x4);

struct tClumpVisibilityPlugin {
    void* m_visibilityCallBack;
    int32 m_alpha;
};
VALIDATE_SIZE(tClumpVisibilityPlugin, 0x8);

struct tFrameVisibilityPlugin {
    union {
        int32 m_hierarchyId;
        CClumpModelInfo* m_modelInfo;
    };
};
VALIDATE_SIZE(tFrameVisibilityPlugin, 0x4);

// TODO: Probably belongs inside `CVisibilityPlugins`
void weaponPedsForPc_Insert(CPed* ped);

class CVisibilityPlugins {
public:
    static const int32 TOTAL_ALPHA_LISTS = 20;
    static const int32 TOTAL_ALPHA_BOAT_ATOMIC_LISTS = 20;
    static const int32 TOTAL_ALPHA_ENTITY_LISTS = 200;
    static const int32 TOTAL_ALPHA_UNDERWATER_ENTITY_LISTS = 100;
    static const int32 TOTAL_ALPHA_DRAW_LAST_LISTS = 50;
    static const int32 TOTAL_WEAPON_PEDS_FOR_PC = 100;
    using tAlphaRenderOrderedListCB = void(__cdecl*)(CEntity* entity, float distance);
    struct AlphaObjectInfo {
        union {
            RpAtomic* m_atomic;
            CEntity* m_entity;
        };
        void* m_pCallback;
        float m_distance; // alpha
    };

    static RwInt32& ms_atomicPluginOffset;
    static RwInt32& ms_clumpPluginOffset;
    static RwInt32& ms_framePluginOffset;
    static CLinkList<AlphaObjectInfo>& m_alphaEntityList;
    static CLinkList<AlphaObjectInfo>& m_alphaList;
    static CLinkList<AlphaObjectInfo>& m_alphaBoatAtomicList;
    static CLinkList<AlphaObjectInfo>& m_alphaUnderwaterEntityList;
    static CLinkList<AlphaObjectInfo>& m_alphaReallyDrawLastList;
    static CLinkList<CPed*>& ms_weaponPedsForPC;
    static RwCamera*& ms_pCamera;
    static RwV3d*& ms_pCameraPosn;
    static float& ms_cullCompsDist;
    static float& ms_cullBigCompsDist;
    static float& ms_vehicleLod0RenderMultiPassDist;
    static float& ms_vehicleLod0Dist;
    static float& ms_vehicleLod1Dist;
    static float& ms_bigVehicleLod0Dist;
    static float& ms_pedLodDist;
    static float& ms_pedFadeDist;
    static float& gVehicleDistanceFromCamera;
    static float& gVehicleAngleToCamera;

    static void InjectHooks();

    static void Initialise();
    static void Shutdown();
    static void InitAlphaAtomicList();
    static void InitAlphaEntityList();
    static bool InsertEntityIntoEntityList(CEntity* entity, float distance, void* callback);
    static bool InsertEntityIntoUnderwaterEntities(CEntity* entity, float distance);
    static bool InsertAtomicIntoReallyDrawLastList(RpAtomic* atomic, float distance);
    static bool InsertEntityIntoReallyDrawLastList(CEntity* entity, float distance);
    static bool InsertEntityIntoSortedList(CEntity* entity, float distance);
    static bool PluginAttach();
    static RpAtomic* DefaultAtomicRenderCallback(RpAtomic* atomic);
    static void* AtomicConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
    static void* AtomicCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);
    static void* AtomicDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
    static int32 CalculateFadingAtomicAlpha(CBaseModelInfo* modelInfo, CEntity* entity, float distance);
    static void ClearAtomicFlag(RpAtomic* atomic, uint16 flag);
    static void ClearAtomicFlag(RpAtomic* atomic, int32 flag);
    static RpAtomic* ClearAtomicFlagCB(RpAtomic* atomic, void* data);
    static void ClearClumpForAllAtomicsFlag(RpClump* clump, uint16 flag);
    static void* ClumpConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
    static void* ClumpCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);
    static void* ClumpDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
    static int32 DefaultVisibilityCB(); // return 1;
    static void* FrameConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
    static void* FrameCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);
    static void* FrameDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
    static bool FrustumSphereCB(RpClump* clump);
    static uint16 GetAtomicId(RpAtomic* atomic);
    static CAtomicModelInfo* GetAtomicModelInfo(RpAtomic* atomic);
    static int32 GetClumpAlpha(RpClump* clump);
    static CClumpModelInfo* GetClumpModelInfo(RpClump* clump);
    static float GetDistanceSquaredFromCamera(RwFrame* frame);
    static float GetDistanceSquaredFromCamera(const CVector* pos);
    static float GetDotProductWithCameraVector(RwMatrix* atomicMatrix, RwMatrix* clumpMatrix, uint16 flags);
    static int32 GetFrameHierarchyId(RwFrame* frame);
    static int16 GetModelInfoIndex(RpAtomic* atomic);
    static int16 GetUserValue(RpAtomic* atomic);
    static bool IsAtomicVisible(RpAtomic* atomic);
    static bool IsClumpVisible(RpClump* clump);
    static void RenderAlphaAtomic(RpAtomic* atomic, int32 alpha);
    static void RenderAlphaAtomics();
    static RpAtomic* RenderAtomicWithAlphaCB(RpAtomic* atomic, void* data);
    static void RenderBoatAlphaAtomics();
    static void RenderEntity(CEntity* entity, float distance);
    static void RenderFadingAtomic(CBaseModelInfo* modelInfo, RpAtomic* atomic, int32 alpha);
    static void RenderFadingClump(CBaseModelInfo* modelInfo, RpClump* clump, int32 alpha);
    static RpAtomic* RenderFadingClumpCB(RpAtomic* atomic);
    static void RenderFadingEntities();
    static void RenderFadingUnderwaterEntities();
    static RpAtomic* RenderHeliRotorAlphaCB(RpAtomic* atomic);
    static RpAtomic* RenderHeliTailRotorAlphaCB(RpAtomic* atomic);
    static RpAtomic* RenderObjNormalAtomic(RpAtomic* atomic);
    static void RenderOrderedList(CLinkList<CVisibilityPlugins::AlphaObjectInfo>& alphaObjectInfoList);
    static RpAtomic* RenderPedCB(RpAtomic* atomic);
    static RpMaterial* SetTextureCB(RpMaterial* material, void* texture);
    static RpAtomic* RenderPlayerCB(RpAtomic* atomic);
    static void RenderReallyDrawLastObjects();
    static RpAtomic* RenderTrainHiDetailAlphaCB(RpAtomic* atomic);
    static RpAtomic* RenderTrainHiDetailCB(RpAtomic* atomic);
    static RpAtomic* RenderVehicleHiDetailAlphaCB(RpAtomic* atomic);
    static RpAtomic* RenderVehicleHiDetailAlphaCB_BigVehicle(RpAtomic* atomic);
    static RpAtomic* RenderVehicleHiDetailAlphaCB_Boat(RpAtomic* atomic);
    static RpAtomic* RenderVehicleHiDetailCB(RpAtomic* atomic);
    static RpAtomic* RenderVehicleHiDetailCB_BigVehicle(RpAtomic* atomic);
    static RpAtomic* RenderVehicleHiDetailCB_Boat(RpAtomic* atomic);
    static RpAtomic* RenderVehicleLoDetailCB_Boat(RpAtomic* atomic);
    static RpAtomic* RenderVehicleReallyLowDetailCB(RpAtomic* atomic);
    static RpAtomic* RenderVehicleReallyLowDetailCB_BigVehicle(RpAtomic* atomic);
    static RpAtomic* RenderWeaponCB(RpAtomic* atomic);
    static void RenderWeaponPedsForPC();
    static RpAtomic* SetAtomicFlagCB(RpAtomic* atomic, void* data);
    static void SetAtomicFlag(RpAtomic* atomic, uint16 flag);
    static void SetAtomicFlag(RpAtomic* atomic, int32 flag);
    static void SetClumpForAllAtomicsFlag(RpClump* clump, uint16 flag);
    static void SetAtomicId(void* atomic, int16 id);
    static void SetAtomicRenderCallback(RpAtomic* atomic, RpAtomicCallBackRender renderCB);
    static void SetClumpAlpha(RpClump* clump, int32 alpha);
    static void SetClumpModelInfo(RpClump* clump, CClumpModelInfo* pClumpModelInfo);
    static void SetFrameHierarchyId(RwFrame* frame, int32 id);
    static void SetRenderWareCamera(RwCamera* pRwCamera);
    static void SetUserValue(RpAtomic* atomic, uint16 value);
    static void SetupVehicleVariables(RpClump* clump);
    static bool VehicleVisibilityCB(RpClump* clump);
    static bool VehicleVisibilityCB_BigVehicle(RpClump* clump);
};

#define RpAtomicGetVisibilityPlugin(atomic) RWPLUGINOFFSET(tAtomicVisibilityPlugin, &((RwObject*)(atomic))->flags, CVisibilityPlugins::ms_atomicPluginOffset)
