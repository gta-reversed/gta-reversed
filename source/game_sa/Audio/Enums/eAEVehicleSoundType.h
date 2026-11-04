#pragma once

#include <Base.h>

enum eAEVehicleSoundType : int8 {
    AE_CAR                 = 0, // 0x0
    AE_BIKE                = 1, // 0x1
    AE_BMX                 = 2, // 0x2
    AE_BOAT                = 3, // 0x3
    AE_AIRCRAFT_HELICOPTER = 4, // 0x4
    AE_AIRCRAFT_PLANE      = 5, // 0x5
    AE_AIRCRAFT_SEAPLANE   = 6, // 0x6
    AE_ONE_GEAR            = 7, // 0x7
    AE_TRAIN               = 8, // 0x8
    AE_SPECIAL             = 9, // 0x9
    AE_NO_VEHICLE          = 10, // 0xA
};
