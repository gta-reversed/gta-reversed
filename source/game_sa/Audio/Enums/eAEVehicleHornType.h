#pragma once

#include <optional>
#include <Base.h>

enum class eAEVehicleHornType : int8 {
    NONE      = -1,
    BIKE      = SND_GENRL_HORN_BIKEBELL, // 1

    ESCORT    = SND_GENRL_HORN_ESCORTHORN,  // 1
    CHEVY56   = SND_GENRL_HORN_HORN56CHEV,  // 2
    BMW328    = SND_GENRL_HORN_HORNBMW328,  // 3
    BUS_A     = SND_GENRL_HORN_HORNBUS,     // 4
    BUS_B     = SND_GENRL_HORN_HORNBUS2,    // 5
    JEEP      = SND_GENRL_HORN_HORNJEEP,    // 6
    PICKUP    = SND_GENRL_HORN_HORNPICKUP,  // 7
    PORSCHE   = SND_GENRL_HORN_HORNPORSCHE, // 8
    TRUCK     = SND_GENRL_HORN_HORNTRUCK,   // 9
    CAR_FIRST = ESCORT,                     // 1
    CAR_LAST  = TRUCK,                      // 9

    NUM // 10
};

inline std::optional<const char*> EnumToString(eAEVehicleHornType v) {
#define ENUM_CASE(_e) case _e: #_e
    using enum eAEVehicleHornType;
    switch (v) {
    ENUM_CASE(NONE);
    ENUM_CASE(BIKE);
    ENUM_CASE(ESCORT);
    ENUM_CASE(CHEVY56);
    ENUM_CASE(BMW328);
    ENUM_CASE(BUS_A);
    ENUM_CASE(BUS_B);
    ENUM_CASE(JEEP);
    ENUM_CASE(PICKUP);
    ENUM_CASE(PORSCHE);
    ENUM_CASE(TRUCK);
    }
    return std::nullopt;
#undef ENUM_CASE
}
