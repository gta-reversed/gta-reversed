#include <StdInc.h>
#include <Stats.h>

#include <ZoneInfo.h>
#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>

namespace {
CZone* GetZoneByLabel(const char* label) {
    const auto zoneID = CTheZones::FindZoneByLabel(label, eZoneType::ZONE_TYPE_INFO);
    if (zoneID == -1) {
        return nullptr;
    }
    return &CTheZones::GetNavigationZones()[zoneID];
}

CZoneInfo* GetZoneInfoByLabel(const char* label) {
    if (auto* const zone = GetZoneByLabel(label)) {
        return CTheZones::GetZoneInfo(zone);
    }
    return nullptr;
}

// SET_ZONE_POPULATION_TYPE (1895)
void SetZonePopulationType(const char* label, eZonePopulationType popType) {
    if (auto* const zi = GetZoneInfoByLabel(label)) {
        zi->PopType = +popType;
    }
}

// SET_ZONE_DEALER_STRENGTH (1898)
void SetZoneDealerStrength(const char* label, uint8 strength) {
    if (auto* const zi = GetZoneInfoByLabel(label)) {
        zi->DealerStrength = strength;
    }
}

// GET_ZONE_DEALER_STRENGTH (1899)
auto GetZoneDealerStrength(const char* label) {
    if (auto* const zi = GetZoneInfoByLabel(label)) {
        return zi->DealerStrength;
    }
    NOTSA_UNREACHABLE();
}

// SET_ZONE_GANG_STRENGTH (1900)
void SetZoneGangStrength(const char* label, eGangID gang, uint8 strength) {
    if (auto* const zi = GetZoneInfoByLabel(label)) {
        zi->GangStrength[gang] = strength;
    }
}

// GET_ZONE_GANG_STRENGTH (1901)
auto GetZoneGangStrength(const char* label, eGangID gang) {
    if (auto* const zi = GetZoneInfoByLabel(label)) {
        return zi->GangStrength[gang];
    }
    NOTSA_UNREACHABLE();
}

// todo
// GET_NAME_OF_ZONE (2115)
//void GetNameOfZone() {
//    
//}

// SET_ZONE_POPULATION_RACE (2164)
void SetZonePopulationRace(const char* label, uint8 races) {
    if (auto* const zi = GetZoneInfoByLabel(label)) {
        zi->PopRaces = races;
    }
}

// SET_ZONE_FOR_GANG_WARS_TRAINING (2227)
void SetZoneForGangWarsTraining(const char* label) {
    if (auto* const zone = GetZoneByLabel(label)) {
        CGangWars::ZoneInfoForTraining = zone->m_ZoneInfoIndex;
    }
}

// INIT_ZONE_POPULATION_SETTINGS (2250)
void InitZonePopulationSettings() {
    CTheZones::InitZonesPopulationSettings();
}

// GET_CURRENT_POPULATION_ZONE_TYPE (2259)
auto GetCurrentPopulationZoneType() {
    return CPopCycle::m_nCurrentZoneType;
}

// TODO
// GET_NAME_OF_INFO_ZONE (2289)
//void GetNameOfInfoZone() {
//    NOTSA_UNREACHABLE("COMMAND_GET_NAME_OF_INFO_ZONE is not implemented");
//}

// SET_SPECIFIC_ZONE_TO_TRIGGER_GANG_WAR (2316)
void SetSpecificZoneToTriggerGangWar(const char* label) {
    CGangWars::SetSpecificZoneToTriggerGangWar(CTheZones::FindZoneByLabel(label, eZoneType::ZONE_TYPE_INFO));
}

// CLEAR_SPECIFIC_ZONES_TO_TRIGGER_GANG_WAR (2317)
void ClearSpecificZonesToTriggerGangWar() {
    CGangWars::ClearSpecificZonesToTriggerGangWar();
}

// SET_ZONE_NO_COPS (2487)
void SetZoneNoCops(const char* label, bool isNoCops) {
    if (auto* const zi = GetZoneInfoByLabel(label)) {
        zi->IsNoCops = isNoCops;
    }
}

// DISPLAY_ZONE_NAMES (2490)
void DisplayZoneNames(bool enabled) {
    CHud::bScriptDontDisplayAreaName = !enabled;
}
};

void notsa::script::commands::zone::RegisterHandlers() {
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_RANDOM_COP_IN_ZONE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PICKUP_IN_ZONE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_ZONE_CIVILIAN_CAR_INFO);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_RANDOM_ICE_CREAM_CUSTOMER_IN_ZONE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_ZONE_RADAR_COLOURS);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_ZONE_POPULATION_TYPE, SetZonePopulationType);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_ZONE_GANG_CAP);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_ZONE_GANG_CAP);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_ZONE_DEALER_STRENGTH, SetZoneDealerStrength);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_ZONE_DEALER_STRENGTH, GetZoneDealerStrength);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_ZONE_GANG_STRENGTH, SetZoneGangStrength);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_ZONE_GANG_STRENGTH, GetZoneGangStrength);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_BEAT_ZONE_SIZE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_3D_COORD_IN_ZONE);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_NAME_OF_ZONE, GetNameOfZone);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_ZONE_POPULATION_RACE, SetZonePopulationRace);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_ZONE_FOR_GANG_WARS_TRAINING, SetZoneForGangWarsTraining);
    REGISTER_COMMAND_HANDLER(COMMAND_INIT_ZONE_POPULATION_SETTINGS, InitZonePopulationSettings);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CURRENT_POPULATION_ZONE_TYPE, GetCurrentPopulationZoneType);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_NAME_OF_INFO_ZONE, GetNameOfInfoZone);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_SPECIFIC_ZONE_TO_TRIGGER_GANG_WAR, SetSpecificZoneToTriggerGangWar);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_SPECIFIC_ZONES_TO_TRIGGER_GANG_WAR, ClearSpecificZonesToTriggerGangWar);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_ZONE_NO_COPS, SetZoneNoCops);
    REGISTER_COMMAND_HANDLER(COMMAND_DISPLAY_ZONE_NAMES, DisplayZoneNames);
}
