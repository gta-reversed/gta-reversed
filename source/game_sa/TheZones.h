/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "eLevelName.h"
#include "Zone.h"
#include "ZoneInfo.h"

class CTheZones {
public:
    static inline auto&       ZonesVisited = StaticRef<notsa::mdarray<bool, 10, 10>, 0xBA3730>(); // Explored territories. Count: 100

    static eLevelName& m_CurrLevel;
    static int32&      ZonesRevealed;                // Number of explored territories
    static int16&      TotalNumberOfNavigationZones; // Info zones
    static CZone       (&NavigationZoneArray)[380];
    static int16&      TotalNumberOfMapZones;        // Map zones
    static CZone       (&MapZoneArray)[39];
    static int16&      TotalNumberOfZoneInfos;
    
    static inline std::array<CZoneInfo, 380>& ZoneInfoArray = *(std::array<CZoneInfo, 380>*)0xBA1DF0;

public:
    static void InjectHooks();

    static void InitZonesPopulationSettings();
    static void ResetZonesRevealed();
    static void AssignZoneInfoForThisZone(int16 index);
    static bool ZoneIsEntirelyContainedWithinOtherZone(CZone* zone1, CZone* zone2);
    static bool& GetZoneWasVisited(CVector2D pos); // NOTSA
    static bool SetZoneWasVisited(CVector2D pos, bool locked); // NOTSA
    static bool GetCurrentZoneLockedOrUnlocked(CVector2D pos);
    // Returns true if point lies within zone
    static bool PointLiesWithinZone(const CVector* point, CZone* zone);
    // Returns eLevelName from position
    static eLevelName GetLevelFromPosition(const CVector& point);
    // Returns pointer to zone by a point
    static CZone* FindSmallestZoneForPosition(const CVector& point, bool FindOnlyZonesType0);
    static CZoneInfo* GetZoneInfo(const CVector& point, CZone** outZone);
    static void FillZonesWithGangColours(bool disableRadarGangColors);
    // Returns pointer to zone by index
    static CZone* GetNavigationZone(uint16 index);
    // Returns pointer to zone by index
    static CZone* GetMapZone(uint16 index);
    static float Calc2DDistanceBetween2Zones(CZone* zone1, CZone* zone2);

    static void Init();
    static void SetCurrentZoneAsUnlocked();
    static void CreateZone(const char* name, eZoneType type, CVector pos1, CVector pos2, eLevelName level, const char* GXT_key);

    static bool FindZone(CVector* point, uint64_t zoneName, eZoneType type);
    static bool FindZone(const CVector& point, std::string_view name, eZoneType type);
    static int16 FindZoneByLabel(const char* name, eZoneType type);
    static void SetZoneRadarColours(int16 index, char flag, uint8 red, uint8 green, uint8 blue);

    static void Update();
    static void Save();
    static void Load();
    static void PostZoneCreation();

    // NOTSA
    static const GxtChar* GetZoneName(const CVector& point);

    static CZoneInfo* GetZoneInfo(const CZone* zone) {
        auto idx = zone->m_nZoneExtraIndexInfo;

        if (!idx)
            return nullptr;

        return &ZoneInfoArray[idx];
    }

    [[deprecated]]
    static auto GetNaviZones() { return std::span{ NavigationZoneArray, (size_t)(TotalNumberOfNavigationZones) }; }
    
    static auto GetNavigationZones() {
        return std::span{NavigationZoneArray, (size_t)TotalNumberOfNavigationZones};
    }

    static auto GetMapZones() {
        return std::span{MapZoneArray, (size_t)TotalNumberOfMapZones};
    }

    static auto GetZoneInfos() {
        return ZoneInfoArray | rng::views::take((size_t)TotalNumberOfZoneInfos);
    }
};
