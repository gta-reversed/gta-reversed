/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Base.h>
#include <RenderWare.h>
#include <utility.hpp>

class CZoneInfo {
public:
    uint8  GangDensity[10];
    uint8  DrugDealerCounter; /// Counter updated in `UpdateDealerStrengths`. Only used durning gang wars. Max value is the size of the array in the beforementioned function.
    CRGBA  ZoneColor;
    union {
        struct {
            uint8 zonePopulationType : 5;
            uint8 radarMode : 2;
            uint8 noCops : 1;
            uint8 zonePopulationRace : 4; // Bitfield for race allowed in the zone. See `ePedRace`. Default value (RACE_DEFAULT) isn't counted. See `IsPedAppropriateForCurrentZone` for usage example.
        };
        struct {
            uint8 Flags1, Flags2;
        };
    };

    auto GetSumOfGangDensity() const {
        return notsa::accumulate(
            GangDensity,  
            0.f,
            notsa::cast_to<float>{}
        );
    }
};

VALIDATE_SIZE(CZoneInfo, 0x11);
