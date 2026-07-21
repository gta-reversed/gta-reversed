/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrListDoubleLink.h"
#include "PtrListSingleLink.h"

class CVehicle;
class CBaseModelInfo;

enum eRendererVisibility {
    RENDERER_INVISIBLE = 0,
    RENDERER_VISIBLE,
    RENDERER_CULLED,
    RENDERER_STREAMME
};

struct tScanLists {
    CPtrListSingleLink<CBuilding*>* buildingsList;
    CPtrListDoubleLink<CObject*>*   objectsList;
    CPtrListDoubleLink<CVehicle*>*  vehiclesList;
    CPtrListDoubleLink<CPed*>*      pedsList;
    CPtrListDoubleLink<CDummy*>*    dummiesList;

    template<typename Visitor>
    void VisitLists(Visitor&& visitor) {
        if (buildingsList) {
            std::invoke(visitor, *buildingsList);
        }
        if (objectsList) {
            std::invoke(visitor, *objectsList);
        }
        if (vehiclesList) {
            std::invoke(visitor, *vehiclesList);
        }
        if (pedsList) {
            std::invoke(visitor, *pedsList);
        }
        if (dummiesList) {
            std::invoke(visitor, *dummiesList);
        }
    }
};
VALIDATE_SIZE(tScanLists, 0x14);

struct tRenderListEntry {
    CEntity* entity;
    float distance;
};
VALIDATE_SIZE(tRenderListEntry, 8);

constexpr auto MAX_INVISIBLE_ENTITY_PTRS = 150u;
constexpr auto MAX_VISIBLE_ENTITY_PTRS   = 1000u;
constexpr auto MAX_VISIBLE_LOD_PTRS      = 1000u;
constexpr auto MAX_VISIBLE_SUPERLOD_PTRS = 50u;

class CWorldScan {
public:
    using tScanFunction = void(__cdecl*)(int32, int32);
    static void ScanWorld(CVector2D* points, int32 pointsCount, tScanFunction scanFunction);
    static void SetExtraRectangleToScan(float minX, float maxX, float minY, float maxY);
};

class CRenderer {
public:
    static inline auto& ms_bRenderTunnels = StaticRef<bool>(0xB745C0);
    static inline auto& ms_bRenderOutsideTunnels = StaticRef<bool>(0xB745C1);
    static inline auto& ms_pLodDontRenderList = StaticRef<tRenderListEntry*>(0xB745CC);
    static inline auto& ms_pLodRenderList = StaticRef<tRenderListEntry*>(0xB745D0);
    static inline auto& m_pFirstPersonVehicle = StaticRef<CVehicle*>(0xB745D4);
    static inline auto& ms_aInVisibleEntityPtrs = StaticRef<CEntity*[MAX_INVISIBLE_ENTITY_PTRS]>(0xB745D8);
    static inline auto& ms_aVisibleSuperLodPtrs = StaticRef<CEntity*[MAX_VISIBLE_SUPERLOD_PTRS]>(0xB74830);
    static inline auto& ms_aVisibleLodPtrs = StaticRef<CEntity*[MAX_VISIBLE_LOD_PTRS]>(0xB748F8);
    static inline auto& ms_aVisibleEntityPtrs = StaticRef<CEntity*[MAX_VISIBLE_ENTITY_PTRS]>(0xB75898);
    static inline auto& ms_nNoOfVisibleSuperLods = StaticRef<int32>(0xB76838);
    static inline auto& ms_nNoOfInVisibleEntities = StaticRef<int32>(0xB7683C);
    static inline auto& ms_nNoOfVisibleLods = StaticRef<int32>(0xB76840);
    static inline auto& ms_nNoOfVisibleEntities = StaticRef<int32>(0xB76844);
    static inline auto& ms_fFarClipPlane = StaticRef<float>(0xB76848);
    static inline auto& ms_fCameraHeading = StaticRef<float>(0xB7684C);
    static inline auto& m_loadingPriority = StaticRef<bool>(0xB76850);
    static inline auto& ms_bInTheSky = StaticRef<bool>(0xB76851);
    static inline auto& ms_vecCameraPosition = StaticRef<CVector>(0xB76870);
    static inline auto& ms_lodDistScale = StaticRef<float>(0x8CD800); // default 1.2
    static inline auto& ms_lowLodDistScale = StaticRef<float>(0x8CD804); // default 1.0

public:
    static void InjectHooks();

    static void Init();
    static void Shutdown();
    static void RenderFadingInEntities();
    static void RenderFadingInUnderwaterEntities();
    static void RenderOneRoad(CEntity* entity);
    static void RenderOneNonRoad(CEntity* entity);
    static void RemoveVehiclePedLights(CPhysical* entity);
    static void AddEntityToRenderList(CEntity* entity, float distance);
    static tRenderListEntry* GetLodRenderListBase();
    static tRenderListEntry* GetLodDontRenderListBase();
    static void ResetLodRenderLists();
    static void AddToLodRenderList(CEntity* entity, float distance);
    static void AddToLodDontRenderList(CEntity* entity, float distance);
    static void ProcessLodRenderLists();
    static void PreRender();
    static void RenderRoads();
    static void RenderEverythingBarRoads();
    static void RenderFirstPersonVehicle();
    static bool SetupLightingForEntity(CPhysical* entity);
    static int32 SetupMapEntityVisibility(CEntity* entity, CBaseModelInfo* modelInfo, float distance, bool bIsTimeInRange);
    static int32 SetupEntityVisibility(CEntity* entity, float& outDistance);
    static int32 SetupBigBuildingVisibility(CEntity* entity, float& outDistance);
    static void ScanSectorList_ListModels(int32 sectorX, int32 sectorY);
    static void ScanSectorList_ListModelsVisible(int32 sectorX, int32 sectorY);
    static void ScanSectorList(int32 sectorX, int32 sectorY);
    static void ScanBigBuildingList(int32 sectorX, int32 sectorY);
    static bool ShouldModelBeStreamed(CEntity* entity, const CVector& origin, float farClip);
    template<typename PtrListType>
    static void ScanPtrList_RequestModels(PtrListType& list);
    static void ConstructRenderList();
    static void ScanSectorList_RequestModels(int32 sectorX, int32 sectorY);
    static void ScanWorld();
    static int32 GetObjectsInFrustum(CEntity** outEntities, float farPlane, RwMatrix* transformMatrix);
    static void RequestObjectsInFrustum(RwMatrix* transformMatrix, int32 modelRequestFlags);
    static void RequestObjectsInDirection(const CVector& posn, float angle, int32 modelRequestFlags);
    static void SetupScanLists(int32 sectorX, int32 sectorY);

    static void SetLoadingPriority(int8 priority) noexcept { m_loadingPriority = priority; } // 0x407370

    static auto GetVisibleLodPtrs()      { return std::span{ ms_aVisibleLodPtrs,      (size_t)ms_nNoOfVisibleLods }; }
    static auto GetVisibleEntityPtrs()   { return std::span{ ms_aVisibleEntityPtrs,   (size_t)ms_nNoOfVisibleEntities }; }
    static auto GetVisibleSuperLodPtrs() { return std::span{ ms_aVisibleSuperLodPtrs, (size_t)ms_nNoOfVisibleSuperLods }; }
    static auto GetInVisibleEntityPtrs() { return std::span{ ms_aInVisibleEntityPtrs, (size_t)ms_nNoOfInVisibleEntities }; }
};

extern uint32& gnRendererModelRequestFlags;
extern CEntity**& gpOutEntitiesForGetObjectsInFrustum;
