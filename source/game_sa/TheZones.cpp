/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "TheZones.h"

#include <Extensions/ci_string.hpp>

// Variables
eLevelName& CTheZones::m_CurrLevel = *(eLevelName*)0xBA6718;

int32& CTheZones::ZonesRevealed = *(int32*)0xBA372C;
int16& CTheZones::TotalNumberOfMapZones = *(int16*)0xBA1900;
CZone (&CTheZones::NavigationZoneArray)[380] = *(CZone(*)[380])0xBA3798;

int16& CTheZones::TotalNumberOfNavigationZones = *(int16*)0xBA3794;
CZone (&CTheZones::MapZoneArray)[39] = *(CZone(*)[39])0xBA1908;

int16& CTheZones::TotalNumberOfZoneInfos = *(int16*)0xBA1DE8;

void CTheZones::InjectHooks() {
    RH_ScopedClass(CTheZones);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(ResetZonesRevealed, 0x572110);
    RH_ScopedGlobalInstall(PointLiesWithinZone, 0x572270);
    RH_ScopedGlobalInstall(GetNavigationZone, 0x572590);
    RH_ScopedGlobalInstall(GetMapZone, 0x5725A0);
    RH_ScopedGlobalInstall(Save, 0x5D2E60, {.enabled = SAVE_HOOKS_ENABLED });
    RH_ScopedGlobalInstall(Load, 0x5D2F40, {.enabled = LOAD_HOOKS_ENABLED });
    RH_ScopedGlobalInstall(PostZoneCreation, 0x572B70);
    RH_ScopedGlobalInstall(InitZonesPopulationSettings, 0x5720D0);
    RH_ScopedGlobalInstall(Update, 0x572D10);
    RH_ScopedGlobalInstall(SetZoneRadarColours, 0x572CC0);
    RH_ScopedGlobalInstall(FindZoneByLabel, 0x572C40);
    RH_ScopedGlobalInstall(CreateZone, 0x5728A0);
    RH_ScopedGlobalInstall(Init, 0x572670);
    RH_ScopedGlobalInstall(AssignZoneInfoForThisZone, 0x572180);
    RH_ScopedGlobalOverloadedInstall(FindZone, "", 0x572B80, bool(*)(CVector*, uint64_t, eZoneType));
    RH_ScopedGlobalInstall(Calc2DDistanceBetween2Zones, 0x5725B0);
    RH_ScopedGlobalInstall(FillZonesWithGangColours, 0x572440);
    RH_ScopedGlobalOverloadedInstall(GetZoneInfo, "", 0x572400, CZoneInfo*(*)(const CVector& point, CZone**));
    RH_ScopedGlobalInstall(FindSmallestZoneForPosition, 0x572360);
    RH_ScopedGlobalInstall(GetLevelFromPosition, 0x572300);
    RH_ScopedGlobalInstall(ZoneIsEntirelyContainedWithinOtherZone, 0x572220);

}

// 0x5720D0
void CTheZones::InitZonesPopulationSettings() {
    rng::fill(ZoneInfoArray, CZoneInfo{});
}

// 0x572110
void CTheZones::ResetZonesRevealed() {
    std::memset(&ZonesVisited, 0, sizeof(ZonesVisited));
    ZonesRevealed = 0;
}

// NOTSA
bool& CTheZones::GetZoneWasVisited(CVector2D pos) {
#ifdef FIX_BUGS
    return ZonesVisited[(size_t)((std::clamp(pos.x, -2999.f, 2999.f) + 3000.f) / 600.f)][9LL - (size_t)((std::clamp(pos.y, -2999.f, 2999.f) + 3000.f) / 600.f)];
#else
    return ZonesVisited[10 * (size_t)((pos.x + 3000.f) / 600.f) - (size_t)((pos.y + 3000.f) / 600.f) + 9];
#endif
}

// NOTSA
bool CTheZones::SetZoneWasVisited(CVector2D pos, bool visited) {
    bool& wasVisited = GetZoneWasVisited(pos);
    if (wasVisited == visited) {
        return false;
    }
    wasVisited = visited;
    return true;
}

bool CTheZones::GetCurrentZoneLockedOrUnlocked(CVector2D pos) {
    return GetZoneWasVisited(pos);
}

// 0x572180
void CTheZones::AssignZoneInfoForThisZone(int16 newZoneIndex) {
    auto& zone = NavigationZoneArray[newZoneIndex];
    zone.m_nZoneExtraIndexInfo = [&]{
        for (const auto& [i, zoneInfo] : notsa::enumerate(GetNavigationZones())) {
            if (i != newZoneIndex && zoneInfo.GetInfoLabel() == zone.GetInfoLabel()) {
                return zoneInfo.m_nZoneExtraIndexInfo;
            }
        }
        return TotalNumberOfZoneInfos++;
    }();
}

// 0x572220
bool CTheZones::ZoneIsEntirelyContainedWithinOtherZone(CZone* a, CZone* b) {
    return a->m_fX1 >= b->m_fX1
        && a->m_fX2 <= b->m_fX2

        && a->m_fY1 >= b->m_fY1
        && a->m_fY2 <= b->m_fY2

        && a->m_fZ1 >= b->m_fZ1
        && a->m_fZ2 <= b->m_fZ2;
}

// Returns true if point lies within zone
// 0x572270
bool CTheZones::PointLiesWithinZone(const CVector* point, CZone* zone) {
    return (
        (float)zone->m_fX1 <= point->x &&
        (float)zone->m_fX2 >= point->x &&
        (float)zone->m_fY1 <= point->y &&
        (float)zone->m_fY2 >= point->y &&
        (float)zone->m_fZ1 <= point->z &&
        (float)zone->m_fZ2 >= point->z
    );
}

// Returns eLevelName from position
eLevelName CTheZones::GetLevelFromPosition(const CVector& point) {
    const auto& mapZones = GetMapZones();
    for (auto& z : mapZones | rng::views::drop(1)) {
        if (z.GetBB().IsPointInside(point)) {
            return z.m_nLevel;
        }
    }
    return mapZones[0].m_nLevel;
}

// Returns pointer to zone by a point
// 0x572360
CZone* CTheZones::FindSmallestZoneForPosition(const CVector& point, bool checkIsNavi) {
    const auto GetZoneSize = [](CZone* z) {
        return z->m_fX2 - z->m_fX1 + z->m_fY2 - z->m_fY1;
    };

    auto smallestZone     = &NavigationZoneArray[0]; // Start with the whole map
    auto smallestZoneSize = GetZoneSize(smallestZone); 
    for (auto& z : GetNavigationZones()) {
        if (checkIsNavi && z.m_nType != ZONE_TYPE_NAVI) {
            continue;
        }
        if (!z.GetBB().IsPointInside(point)) {
            continue;
        }
        const auto zsize = GetZoneSize(&z);
        if (zsize < smallestZoneSize) {
            smallestZone     = &z;
            smallestZoneSize = zsize;
        }
    }
    return smallestZone;
}

// 0x572400
CZoneInfo* CTheZones::GetZoneInfo(const CVector& point, CZone** outZone) {
    const auto z = FindSmallestZoneForPosition(point, false);
    if (outZone) {
        *outZone = z ? z : &NavigationZoneArray[0]; // Zone 0 is always SAN_AND
    }
    return &ZoneInfoArray[z ? z->m_nZoneExtraIndexInfo : 0];
}

// 0x572440
void CTheZones::FillZonesWithGangColours(bool disableRadarGangColors) {
    for (auto& z : GetZoneInfos()) {
        const auto gdSum = z.GangDensity[GANG_BALLAS] + z.GangDensity[GANG_GROVE] + z.GangDensity[GANG_VAGOS];

        uint8 color[3];
        for (auto i = 0; i < 3; i++) {
            const auto GetVW = [&](eGangID g) {
                return WeightedValue<uint32, uint32>{ gaGangColors[g].components[i], z.GangDensity[g] };
            };
            color[i] = (uint8)(multiply_weighted({ GetVW(GANG_BALLAS), GetVW(GANG_GROVE), GetVW(GANG_VAGOS) }) / std::max(1, gdSum));
        }

        z.radarMode = gdSum && !disableRadarGangColors && CGangWars::CanPlayerStartAGangWarHere(&z)
            ? 1
            : 0;

        z.ZoneColor = {
            color[0],
            color[1],
            color[2],
            std::clamp<uint8>(gdSum * 3, 55, 120)
        };
    }
}

// Returns pointer to zone by index
// 0x572590
CZone* CTheZones::GetNavigationZone(uint16 index) {
    return &CTheZones::NavigationZoneArray[index];
}

// Returns pointer to zone by index
// 0x5725A0
CZone* CTheZones::GetMapZone(uint16 index) {
    return &CTheZones::MapZoneArray[index];
}

// 0x5725b0
float CTheZones::Calc2DDistanceBetween2Zones(CZone* a, CZone* b) {
    int16 dx, dy;

    if (a->m_fX1 <= b->m_fX2) {
        dx = a->m_fX2 >= b->m_fX1
            ? 0
            : b->m_fX1 - a->m_fX2;
    } else {
        dx = a->m_fX1 - b->m_fX2;
    }

    if (a->m_fY1 <= b->m_fY2) { // Copy paste of the above but for the `y` axis
        dy = a->m_fY2 >= b->m_fY1
            ? 0
            : b->m_fY1 - a->m_fY2;
    } else {
        dy = a->m_fY1 - b->m_fY2;
    }

    return (float)std::sqrt(sq(dx) + sq(dy));
}

// Initializes CTheZones
// 0x572670
void CTheZones::Init() {
    ZoneScoped;
    
    rng::fill(NavigationZoneArray, CZone{});
    rng::fill(MapZoneArray, CZone{});

    InitZonesPopulationSettings();
    ResetZonesRevealed();

    TotalNumberOfZoneInfos       = 0;
    TotalNumberOfNavigationZones = 0;
    TotalNumberOfMapZones        = 0;
    m_CurrLevel                  = LEVEL_NAME_COUNTRY_SIDE;

    CreateZone("SAN_AND", ZONE_TYPE_NAVI, { -3000.f, -3000.f, -2000.f }, { 3000.f, 3000.f, 2000.f }, LEVEL_NAME_COUNTRY_SIDE, "SAN_AND");
    CreateZone("THEMAP", ZONE_TYPE_MAP, { -3000.f, -3000.f, -2000.f }, { 3000.f, 3000.f, 2000.f }, LEVEL_NAME_COUNTRY_SIDE, "THEMAP");
}

// Unlock the current zone
// 0x572800
void CTheZones::SetCurrentZoneAsUnlocked() {
    ((void(__cdecl*)())0x572800)();
}

// Creates a zone
// 0x5728A0
void CTheZones::CreateZone(
    const char* infoLabel,
    eZoneType   type,
    CVector     pos1,
    CVector     pos2,
    eLevelName  level,
    const char* textLabel
) {
    const auto z = [type]{
        switch (type) {
        case ZONE_TYPE_LOCAL_NAVI:
        case ZONE_TYPE_NAVI:       return &NavigationZoneArray[TotalNumberOfNavigationZones++];
        case ZONE_TYPE_MAP:        return &MapZoneArray[TotalNumberOfMapZones++];
        }
        NOTSA_UNREACHABLE();
    }();

    const auto StrCpyUpper = [](auto& dst, auto& src) {
        rng::fill(dst, 0);
        strcpy_s(dst, src);
        for (auto& ch : dst) {
            if (ch == 0) {
                break;
            }
            ch = toupper(ch);
        }
    };
    StrCpyUpper(z->m_TextLabel, textLabel);
    StrCpyUpper(z->m_InfoLabel, infoLabel);

    std::tie(z->m_fX1, z->m_fX2) = std::minmax((int16)pos1.x, (int16)pos2.x);
    std::tie(z->m_fY1, z->m_fY2) = std::minmax((int16)pos1.y, (int16)pos2.y);
    std::tie(z->m_fZ1, z->m_fZ2) = std::minmax((int16)pos1.z, (int16)pos2.z);

    z->m_nLevel = level;

    switch (type) {
    case ZONE_TYPE_LOCAL_NAVI:
    case ZONE_TYPE_NAVI:
        AssignZoneInfoForThisZone(TotalNumberOfNavigationZones - 1);
        break;
    }
}

// Returns 1 if point lies within the specified zonename otherwise return 0
// 0x572B80
bool CTheZones::FindZone(CVector* point, uint64_t packedZoneName, eZoneType type) {
    char zoneName[sizeof(packedZoneName)];
    memcpy(zoneName, &packedZoneName, sizeof(packedZoneName));
    assert(zoneName[sizeof(packedZoneName) - 1] == 0);
    return FindZone(*point, { zoneName }, type);
}

//! Find zone by name `name` and of type `type` and check if `point` lies within it
bool CTheZones::FindZone(const CVector& point, std::string_view name, eZoneType type) {
    switch (type) {
    case ZONE_TYPE_INFO:
    case ZONE_TYPE_NAVI:
        break;
    default:
        return false;
    }

    for (auto& zone : GetNavigationZones()) {
        if ((type == ZONE_TYPE_INFO ? zone.GetInfoLabel() : zone.GetNaviLabel()) == name) {
            if (PointLiesWithinZone(&point, &zone)) {
                return true;
            }
        }
    }

    return false;
}

// Returns pointer to zone by index
// 0x572C40
int16 CTheZones::FindZoneByLabel(const char* name, eZoneType type) {
    assert(type == eZoneType::ZONE_TYPE_INFO); // Originally an `if` returning `-1`, but let's be safe

    for (auto&& [i, v] : notsa::enumerate(GetNavigationZones())) {
        if (name == v.GetInfoLabel()) {
            return (int16)i;
        }
    }
    return -1;
}

// 0x572cc0
void CTheZones::SetZoneRadarColours(int16 index, char radarMode, uint8 red, uint8 green, uint8 blue) {
    const auto zone = &ZoneInfoArray[NavigationZoneArray[index].m_nZoneExtraIndexInfo];
    zone->radarMode = radarMode;
    zone->ZoneColor = { red, green, blue, 255 };
}

// Updates CTheZones info
// 0x572D10
void CTheZones::Update() {
    ZoneScoped;

    const auto DELAY = 5000;

    static auto CountSeconds = CTimer::GetTimeInMS() - DELAY; // NOTE/BUG: This can underflow in the special case that `CTimer::GetTimeMS() < DELAY`
    
    if (CGame::CanSeeOutSideFromCurrArea()) { // Inverted
        if (CTimer::GetTimeInMS() - CountSeconds > DELAY) {
            // Code from 0x572800
            const auto pos = FindPlayerCoors();
            m_CurrLevel = CTheZones::GetLevelFromPosition(pos);
            if (!CTheScripts::bPlayerIsOffTheMap && CGame::CanSeeOutSideFromCurrArea() && SetZoneWasVisited(pos, true)) {
                CTheZones::ZonesRevealed++;
            }
        }
    } else {
        CountSeconds = CTimer::GetTimeInMS();
    }
}

// Save CTheZones info
// 0x5D2E60
void CTheZones::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer((uint32)m_CurrLevel);
    CGenericGameStorage::SaveDataToWorkBuffer(TotalNumberOfNavigationZones);
    CGenericGameStorage::SaveDataToWorkBuffer(TotalNumberOfZoneInfos);
    CGenericGameStorage::SaveDataToWorkBuffer(TotalNumberOfMapZones);

    for (int32 i = 0; i < TotalNumberOfNavigationZones; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(NavigationZoneArray[i]);
    }

    for (int32 i = 0; i < TotalNumberOfZoneInfos; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(ZoneInfoArray[i]);
    }

    for (int32 i = 0; i < TotalNumberOfMapZones; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(MapZoneArray[i]);
    }

    CGenericGameStorage::SaveDataToWorkBuffer(ZonesVisited);
    CGenericGameStorage::SaveDataToWorkBuffer(ZonesRevealed);
}

// Load CTheZones info
// 0x5D2F40
void CTheZones::Load() {
    Init();

    m_CurrLevel = static_cast<eLevelName>(CGenericGameStorage::LoadDataFromWorkBuffer<uint32>());
    CGenericGameStorage::LoadDataFromWorkBuffer(TotalNumberOfNavigationZones);
    CGenericGameStorage::LoadDataFromWorkBuffer(TotalNumberOfZoneInfos);
    CGenericGameStorage::LoadDataFromWorkBuffer(TotalNumberOfMapZones);

    for (int32 i = 0; i < TotalNumberOfNavigationZones; i++) {
        CGenericGameStorage::LoadDataFromWorkBuffer(NavigationZoneArray[i]);
    }

    for (int32 i = 0; i < TotalNumberOfZoneInfos; i++) {
        CGenericGameStorage::LoadDataFromWorkBuffer(ZoneInfoArray[i]);
    }

    for (int32 i = 0; i < TotalNumberOfMapZones; i++) {
        CGenericGameStorage::LoadDataFromWorkBuffer(MapZoneArray[i]);
    }
    CGenericGameStorage::LoadDataFromWorkBuffer(ZonesVisited);
    CGenericGameStorage::LoadDataFromWorkBuffer(ZonesRevealed);
}

// dummy function
// 0x572B70
void CTheZones::PostZoneCreation() {
    // NOP
}

const GxtChar* CTheZones::GetZoneName(const CVector& point) {
    CZone* zone{};
    auto extraInfo = GetZoneInfo(point, &zone);
    if (zone)
        return zone->GetTranslatedName();

    return "Unknown zone"_gxt;
}
