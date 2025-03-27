/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Base.h>
#include <RenderWare.h>
#include <extensions/utility.hpp>

// Based on `popcycle.dat`
enum class eZonePopulationType : uint8 {
    BUSINESS,
    DESERT,
    ENTERTAINMENT,
    COUNTRYSIDE,
    RESIDENTIAL_RICH,
    RESIDENTIAL_AVERAGE,
    RESIDENTIAL_POOR,
    GANGLAND,
    BEACH,
    SHOPPING,
    PARK,
    INDUSTRY,
    ENTERTAINMENT_BUSY,
    SHOPPING_BUSY,
    SHOPPING_POSH,
    RESIDENTIAL_RICH_SECLUDED,
    AIRPORT,
    GOLF_CLUB,
    OUT_OF_TOWN_FACTORY,
    AIRPORT_RUNWAY,

    // Add above this
    COUNT,
};
static_assert(+eZonePopulationType::COUNT <= 0b0001'1111, "eZonePopulationType may at most occupy at most 5 bits");

class CZoneInfo {
public:
    uint8  GangStrength[10]{};
    uint8  DealerStrength{}; /// Counter updated in `UpdateDealerStrengths`. Only used durning gang wars. Max value is the size of the array in the beforementioned function (15 currently)
    CRGBA  ZoneColor{};
    struct {
        uint8 PopType : 5{ +eZonePopulationType::RESIDENTIAL_AVERAGE };
        uint8 RadarMode : 2{};
        uint8 IsNoCops : 1{};
        uint8 PopRaces : 4{0b1111}; // Bitfield for race allowed in the zone. See `ePedRace`. Default value (RACE_DEFAULT) isn't counted. See `IsPedAppropriateForCurrentZone` for usage example.
    };

    auto GetSumOfGangDensity() const {
        return notsa::accumulate(
            GangStrength,  
            (size_t)(0u) // Making sure no overflow occurs (because uint8 is used instead of casting to a wider type)
        );
    }
};
VALIDATE_SIZE(CZoneInfo, 0x11);
