#pragma once

#include <Base.h>
#include <extensions/WEnum.hpp>

enum class eAEVehicleAudioType : int16 {
    CAR     = 0, // 0x0
    BIKE    = 1, // 0x1
    GENERIC = 2, // 0x2
};
NOTSA_WENUM_DEFS_FOR(eAEVehicleAudioType);
