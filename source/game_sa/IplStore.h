/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "IplDef.h"
#include "QuadTreeNode.h"
#include "Pool.h"

class CEntity;

typedef CPool<IplDef> CIplPool;

class CIplStore {
public:
    static inline CQuadTreeNode*& ms_pQuadTree = *(CQuadTreeNode**)0x8E3FAC;
    static inline CIplPool*&      ms_pPool     = *(CIplPool**)0x8E3FB0;

public:
    static void InjectHooks();

    static void Initialise();
    static void Shutdown();

    static int32 AddIplSlot(const char* name);
    static void AddIplsNeededAtPosn(const CVector& posn);
    static void ClearIplsNeededAtPosn();
    static void EnableDynamicStreaming(int32 iplSlotIndex, bool enable);
    static void EnsureIplsAreInMemory(const CVector& posn);
    static int32 FindIplSlot(const char* name);
    static CRect* GetBoundingBox(int32 iplSlotIndex);
    static CEntity** GetIplEntityIndexArray(int32 arrayIndex);
    static const char* GetIplName(int32 iplSlotIndex);
    static int32 GetNewIplEntityIndexArray(int32 entitiesCount);
    static bool HaveIplsLoaded(const CVector& coords, int32 playerNumber = -1);
    static void IncludeEntity(int32 iplSlotIndex, CEntity* entity);
    static void LoadAllRemainingIpls();
    static bool LoadIpl(int32 iplSlotIndex, char* data, int32 dataSize);
    static bool LoadIplBoundingBox(int32 iplSlotIndex, char* data, int32 dataSize);
    static void LoadIpls(CVector posn, bool bAvoidLoadInPlayerVehicleMovingDirection);
    static void RemoveAllIpls();
    static void RemoveIpl(int32 iplSlotIndex);
    static void RemoveIplAndIgnore(int32 iplSlotIndex);
    static void RemoveIplSlot(int32 iplSlotIndex);
    static void RemoveIplWhenFarAway(int32 iplSlotIndex);
    static void RemoveRelatedIpls(int32 entityArraysIndex);
    static void RequestIplAndIgnore(int32 iplSlotIndex);
    static void RequestIpls(const CVector& posn, int32 playerNumber = -1);
    static void SetIplsRequired(const CVector& posn, int32 playerNumber = -1);
    static void SetIsInterior(int32 iplSlotIndex, bool isInterior);
    static int32 SetupRelatedIpls(const char* iplName, int32 entityArraysIndex, CEntity** instances);
    static bool Save();
    static bool Load();

    inline static bool HasDynamicStreamingDisabled(int32 iplSlotIndex) { return GetInSlot(iplSlotIndex)->disableDynamicStreaming; } // 0x59EB20

    // NOTSA
    static IplDef* GetInSlot(int32 slot) { return ms_pPool->GetAt(slot); }
};

static inline auto& ppCurrIplInstance = StaticRef<CEntity**>(0x8E3EFC);
static inline auto& NumIplEntityIndexArrays = StaticRef<int32>(0x8E3F00);
static inline auto& IplEntityIndexArrays = StaticRef<std::array<CEntity**, 40>>(0x8E3F08); // Array of CEntity* array pointers
static inline auto& gbIplsNeededAtPosn = StaticRef<bool>(0x8E3FA8);
static inline auto& gvecIplsNeededAtPosn = StaticRef<CVector>(0x8E3FD0);
static inline auto& gNumLoadedBuildings = StaticRef<uint32>(0xBCC0D8);
static inline auto& gpLoadedBuildings = StaticRef<std::array<CEntity*, 4'096>>(0xBCC0E0);

void SetIfInteriorIplIsRequired(const CVector2D& posn, void* data);
void SetIfIplIsRequired(const CVector2D& posn, void* data);
void SetIfIplIsRequiredReducedBB(const CVector2D& posn, void* data);
inline auto GetLoadedBuildings() { return gpLoadedBuildings | rng::views::take(gNumLoadedBuildings); }
