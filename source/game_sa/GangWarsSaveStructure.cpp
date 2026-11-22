#include "StdInc.h"

#include "GangWarsSaveStructure.h"

// 0x5D2620
void CGangWarsSaveStructure::Construct() {
    bGangWarsActive = CGangWars::bGangWarsActive;
    State = CGangWars::State;
    TimeStarted = CGangWars::TimeStarted;
    GangWarZoneInfoIndex = -1;

    if (CGangWars::pZoneInfoToFightOver) {
        for (auto i = 0; i < CTheZones::TotalNumberOfZoneInfos; i++) {
            if (CGangWars::pZoneInfoToFightOver == &CTheZones::ZoneInfoArray[i]) {
                GangWarZoneInfoIndex = i;
                break;
            }
        }

        for (auto i = 0; i < CTheZones::TotalNumberOfNavigationZones; i++) {
            if (CGangWars::pZoneToFightOver == &CTheZones::NavigationZoneArray[i]) {
                GangWarNavigationZoneIndex = i;
                break;
            }
        }
    }

    CoorsOfPlayerAtStartOfWar = CGangWars::CoorsOfPlayerAtStartOfWar;
    Gang1 = CGangWars::Gang1;
    Gang2 = CGangWars::Gang2;
    WarFerocity = CGangWars::WarFerocity;
    LastTimeInArea = CGangWars::LastTimeInArea;
    State2 = CGangWars::State2;
    TimeTillNextAttack = CGangWars::TimeTillNextAttack;
    PointOfAttack = CGangWars::PointOfAttack;
    FightTimer = CGangWars::FightTimer;
    RadarBlip = CGangWars::RadarBlip;
    bPlayerIsCloseby = CGangWars::bPlayerIsCloseby;
    TerritoryUnderControlPercentage = CGangWars::TerritoryUnderControlPercentage;
    Difficulty = CGangWars::Difficulty;
}

// 0x5D2740
void CGangWarsSaveStructure::Extract() const {
    CGangWars::bGangWarsActive = bGangWarsActive;
    CGangWars::State = State;
    CGangWars::TimeStarted = TimeStarted;
    CGangWars::pZoneInfoToFightOver = (GangWarZoneInfoIndex != -1) ? &CTheZones::ZoneInfoArray[GangWarZoneInfoIndex] : nullptr;
    CGangWars::pZoneToFightOver = (GangWarNavigationZoneIndex != -1) ? &CTheZones::NavigationZoneArray[GangWarNavigationZoneIndex] : nullptr;
    CGangWars::CoorsOfPlayerAtStartOfWar = CoorsOfPlayerAtStartOfWar;
    CGangWars::Gang1 = (eGangID)Gang1;
    CGangWars::Gang2 = Gang2;
    CGangWars::WarFerocity = WarFerocity;
    CGangWars::LastTimeInArea = LastTimeInArea;
    CGangWars::State2 = State2;
    CGangWars::TimeTillNextAttack = TimeTillNextAttack;
    CGangWars::PointOfAttack = PointOfAttack;
    CGangWars::FightTimer = FightTimer;
    CGangWars::RadarBlip = RadarBlip;
    CGangWars::bPlayerIsCloseby = bPlayerIsCloseby;
    CGangWars::TerritoryUnderControlPercentage = TerritoryUnderControlPercentage;
    CGangWars::Difficulty = Difficulty;
}
