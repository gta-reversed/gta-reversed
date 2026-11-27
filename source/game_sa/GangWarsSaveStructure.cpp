#include "StdInc.h"

#include "GangWarsSaveStructure.h"

// 0x5D2620
void CGangWarsSaveStructure::Construct() {
    bGangWarsActive      = CGangWars::ms_GangWarsActive;
    State                = CGangWars::ms_State;
    TimeStarted          = CGangWars::ms_TimeStarted;
    GangWarZoneInfoIndex = -1;

    if (CGangWars::ms_ZoneInfoToFightOver) {
        for (auto i = 0; i < CTheZones::TotalNumberOfZoneInfos; i++) {
            if (CGangWars::ms_ZoneInfoToFightOver == &CTheZones::ZoneInfoArray[i]) {
                GangWarZoneInfoIndex = i;
                break;
            }
        }

        for (auto i = 0; i < CTheZones::TotalNumberOfNavigationZones; i++) {
            if (CGangWars::ms_ZoneToFightOver == &CTheZones::NavigationZoneArray[i]) {
                GangWarNavigationZoneIndex = i;
                break;
            }
        }
    }

    CoorsOfPlayerAtStartOfWar       = CGangWars::ms_CoorsOfPlayerAtStartOfWar;
    Gang1                           = CGangWars::ms_Gang1;
    Gang2                           = CGangWars::ms_Gang2;
    WarFerocity                     = CGangWars::ms_WarFerocity;
    LastTimeInArea                  = CGangWars::ms_LastTimeInArea;
    State2                          = CGangWars::ms_State2;
    TimeTillNextAttack              = CGangWars::ms_TimeTillNextAttack;
    PointOfAttack                   = CGangWars::ms_PointOfAttack;
    FightTimer                      = CGangWars::ms_FightTimer;
    RadarBlip                       = CGangWars::ms_RadarBlip;
    bPlayerIsCloseby                = CGangWars::ms_IsPlayerCloseby;
    TerritoryUnderControlPercentage = CGangWars::ms_TerritoryUnderControlPercentage;
    Difficulty                      = CGangWars::ms_Difficulty;
}

// 0x5D2740
void CGangWarsSaveStructure::Extract() const {
    CGangWars::ms_GangWarsActive                  = bGangWarsActive;
    CGangWars::ms_State                           = State;
    CGangWars::ms_TimeStarted                     = TimeStarted;
    CGangWars::ms_ZoneInfoToFightOver             = (GangWarZoneInfoIndex != -1) ? &CTheZones::ZoneInfoArray[GangWarZoneInfoIndex] : nullptr;
    CGangWars::ms_ZoneToFightOver                 = (GangWarNavigationZoneIndex != -1) ? &CTheZones::NavigationZoneArray[GangWarNavigationZoneIndex] : nullptr;
    CGangWars::ms_CoorsOfPlayerAtStartOfWar       = CoorsOfPlayerAtStartOfWar;
    CGangWars::ms_Gang1                           = (eGangID)Gang1;
    CGangWars::ms_Gang2                           = Gang2;
    CGangWars::ms_WarFerocity                     = WarFerocity;
    CGangWars::ms_LastTimeInArea                  = LastTimeInArea;
    CGangWars::ms_State2                          = State2;
    CGangWars::ms_TimeTillNextAttack              = TimeTillNextAttack;
    CGangWars::ms_PointOfAttack                   = PointOfAttack;
    CGangWars::ms_FightTimer                      = FightTimer;
    CGangWars::ms_RadarBlip                       = RadarBlip;
    CGangWars::ms_IsPlayerCloseby                 = bPlayerIsCloseby;
    CGangWars::ms_TerritoryUnderControlPercentage = TerritoryUnderControlPercentage;
    CGangWars::ms_Difficulty                      = Difficulty;
}
