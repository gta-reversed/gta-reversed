/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CVector.h"
#include "CZone.h"

class CTheZones {
public:
    static eLevelName& m_CurrLevel;
    static char*       ZonesVisited;                 // Explored territories. Count: 100
    static int32&      ZonesRevealed;                // Number of explored territories
    static int16&      TotalNumberOfNavigationZones; // Info zones
    static CZone*      NavigationZoneArray;          // Count: 380
    static int16&      TotalNumberOfMapZones;        // Map zones
    static CZone*      MapZoneArray;                 // Count: 39
    static int16&      TotalNumberOfZoneInfos;
    static CZoneInfo*  ZoneInfoArray;

public:
    static void InjectHooks();

    static void InitZonesPopulationSettings();
    static void ResetZonesRevealed();
    static void AssignZoneInfoForThisZone(int16 index);
    static bool ZoneIsEntirelyContainedWithinOtherZone(CZone* zone1, CZone* zone2);
    static bool GetCurrentZoneLockedOrUnlocked(float posx, float posy);
    // Returns true if point lies within zone
    static bool PointLiesWithinZone(const CVector* point, CZone* zone);
    // Returns eLevelName from position
    static eLevelName GetLevelFromPosition(CVector const& point);
    // Returns pointer to zone by a point
    static CZone* FindSmallestZoneForPosition(const CVector& point, bool FindOnlyZonesType0);
    static CZoneExtraInfo* GetZoneInfo(const CVector& point, CZone** outZone);
    static void FillZonesWithGangColours(bool disableRadarGangColors);
    // Returns pointer to zone by index
    static CZone* GetNavigationZone(uint16 index);
    // Returns pointer to zone by index
    static CZone* GetMapZone(uint16 index);
    static long double Calc2DDistanceBetween2Zones(CZone* zone1, CZone* zone2);

    static void Init();
    static void SetCurrentZoneAsUnlocked();
    static void CreateZone(const char* name, eZoneType type, float posX1, float posY1, float posZ1, float posX2, float posY2, float posZ2, eLevelName island, const char* GXT_key);
    static bool FindZone(CVector* point, int32 zonename_part1, int32 zonename_part2, eZoneType type);
    static int16 FindZoneByLabel(const char* name, eZoneType type);
    static void SetZoneRadarColours(int16 index, char flag, uint8 red, uint8 green, uint8 blue);

    static void Update();
    static void Save();
    static void Load();
    static void PostZoneCreation();
};
