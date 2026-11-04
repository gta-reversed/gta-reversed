#pragma once

#include "Vector.h"
#include <PlantColEntEntry.h>
#include <PlantLocTri.h>

class CEntity;

constexpr auto MAX_NUM_PLANT_TRIANGLES           = 256;
constexpr auto MAX_NUM_PROC_OBJECTS              = 40;
constexpr auto PLANTS_MAX_DISTANCE               = 100.0f;
constexpr auto PLANTS_MAX_DISTANCE_SQUARED       = PLANTS_MAX_DISTANCE * PLANTS_MAX_DISTANCE;
constexpr auto PROC_OBJECTS_MAX_DISTANCE         = 340.0f;
constexpr auto PROC_OBJECTS_MAX_DISTANCE_SQUARED = PROC_OBJECTS_MAX_DISTANCE * PROC_OBJECTS_MAX_DISTANCE;
constexpr auto PLANTS_ALPHA_MIN_DIST             = 3.0f;
constexpr auto PLANTS_ALPHA_MAX_DIST             = 60.0f;

class CPlantMgr {
public:
    static inline auto& PC_PlantTextureTab = StaticRef<RwTexture*[4][4]>(0xC039A0);
    static inline auto& PC_PlantModelsTab = StaticRef<RpAtomic*[4][4]>(0xC039F0);

    static inline auto& m_scanCode = StaticRef<int16>(0xC03980);
    static inline auto& m_UnusedLocTriListHead = StaticRef<CPlantLocTri*>(0xC03984);
    static inline auto& m_CloseLocTriListHead = StaticRef<CPlantLocTri*[4]>(0xC03988);
    static inline auto& m_UnusedColEntListHead = StaticRef<CPlantColEntEntry*>(0xC03998);
    static inline auto& m_CloseColEntListHead = StaticRef<CPlantColEntEntry*>(0xC0399C);
    static inline auto& m_AmbientColor = StaticRef<CRGBA>(0xC03A44);
    static inline auto& m_LocTrisTab = StaticRef<CPlantLocTri[MAX_NUM_PLANT_TRIANGLES]>(0xC03A48);
    static inline auto& m_ColEntCacheTab = StaticRef<CPlantColEntEntry[MAX_NUM_PROC_OBJECTS]>(0xC08E48);

public:
    static void InjectHooks();

    static bool Initialise();
    static void Shutdown();
    static bool ReloadConfig();

    static void MoveLocTriToList(CPlantLocTri*& oldList, CPlantLocTri*& newList, CPlantLocTri* triangle);
    static void MoveColEntToList(CPlantColEntEntry*& oldList, CPlantColEntEntry*& newList, CPlantColEntEntry* entry);
    static void SetPlantFriendlyFlagInAtomicMI(CAtomicModelInfo*);

    static void Update(const CVector& cameraPosition);
    static void UpdateAmbientColor();
    static void PreUpdateOnceForNewCameraPos(const CVector& posn);
    static float CalculateWindBending();

    static void Render();

private:
    static void _ColEntityCache_Add(CEntity* entity, bool checkAlreadyExists);
    static CPlantColEntEntry* _ColEntityCache_FindInCache(CEntity* entity);
    static void _ColEntityCache_Remove(CEntity* entity);
    static void _ColEntityCache_Update(const CVector& cameraPos, bool fast);

    static void _ProcessEntryCollisionDataSections(const CPlantColEntEntry& entry, const CVector& center, int32 a3);
    static void _ProcessEntryCollisionDataSections_AddLocTris(const CPlantColEntEntry& entry, const CVector& center, int32 a3, int32 start, int32 end);
    static void _ProcessEntryCollisionDataSections_RemoveLocTris(const CPlantColEntEntry& entry, const CVector& center, int32 a3, int32 start, int32 end);

    static void _UpdateLocTris(const CVector& center, int32);

    static float _CalcDistanceSqrToEntity(CEntity* entity, const CVector& posn);

    static bool DbgCountCachedEntities(uint32*);
    static bool DbgCountLocTrisAndPlants(uint32, uint32*, uint32*);
    static bool DbgRenderCachedEntities(uint32*);
    static bool DbgRenderLocTris();

    static void IncrementScanCode() { ++m_scanCode; } // 0x5DB2D0
};

static inline auto& grassTexturesPtr = StaticRef<RwTexture**[4]>(0xC039E0);
static inline auto& grassModelsPtr = StaticRef<RpAtomic*[4]>(0xC03A30);
static inline auto& tex_gras07Si = StaticRef<RwTexture*>(0xC09174);
