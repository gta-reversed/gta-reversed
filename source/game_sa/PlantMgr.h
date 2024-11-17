#pragma once

#include "Vector.h"

class CEntity;
class CPlantColEntEntry;
class CPlantLocTri;

class CPlantMgr {
public:
    static inline RwTexture* (&PC_PlantTextureTab)[4][4] = *(RwTexture * (*)[4][4])0xC039A0;
    static inline RpAtomic*  (&PC_PlantModelsTab)[4][4]  = *(RpAtomic * (*)[4][4])0xC039F0;

    static inline int16&              m_scanCode                  = *(int16*)0xC03980;
    static inline CPlantLocTri*&      m_UnusedLocTriListHead      = *(CPlantLocTri**)0xC03984;
    static inline CPlantLocTri*       (&m_CloseLocTriListHead)[4] = *(CPlantLocTri * (*)[4])0xC03988;
    static inline CPlantColEntEntry*& m_UnusedColEntListHead      = *(CPlantColEntEntry**)0xC03998;
    static inline CPlantColEntEntry*& m_CloseColEntListHead       = *(CPlantColEntEntry**)0xC0399C;
    static inline CRGBA&              m_AmbientColor              = *(CRGBA*)0xC03A44;
    static inline CPlantLocTri        (&m_LocTrisTab)[256]        = *(CPlantLocTri(*)[256])0xC03A48;
    static inline CPlantColEntEntry   (&m_ColEntCacheTab)[40]     = *(CPlantColEntEntry(*)[40])0xC08E48;

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

static inline RwTexture** (&grassTexturesPtr)[4] = *(RwTexture** (*)[4])0xC039E0;
static inline RpAtomic* (&grassModelsPtr)[4] = *(RpAtomic * (*)[4])0xC03A30;
static inline RwTexture*& tex_gras07Si = *(RwTexture**)0xC09174;
