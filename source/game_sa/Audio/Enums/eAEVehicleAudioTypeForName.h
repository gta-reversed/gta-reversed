#pragma once

#include <Base.h>

enum eAEVehicleAudioTypeForName : int8 {
    AE_VAT_NONE              = -1,  // 0xFF
    AE_VAT_OFFROAD           = 0,   // 0x0
    AE_VAT_TWO_DOOR          = 1,   // 0x1
    AE_VAT_SPORTS_CAR        = 2,   // 0x2
    AE_VAT_RIG               = 3,   // 0x3
    AE_VAT_STATION_WAGON     = 4,   // 0x4
    AE_VAT_SEDAN             = 5,   // 0x5
    AE_VAT_TRUCK             = 6,   // 0x6
    AE_VAT_FIRETRUCK         = 7,   // 0x7
    AE_VAT_GARBAGE_TRUCK     = 8,   // 0x8
    AE_VAT_STRETCH           = 9,   // 0x9
    AE_VAT_LOWRIDER          = 10,  // 0xA
    AE_VAT_VAN               = 11,  // 0xB
    AE_VAT_AMBULANCE         = 12,  // 0xC
    AE_VAT_HELICOPTER        = 13,  // 0xD
    AE_VAT_TAXI              = 14,  // 0xE
    AE_VAT_PICK_UP           = 15,  // 0xF
    AE_VAT_ICE_CREAM_VAN     = 16,  // 0x10
    AE_VAT_BUGGY             = 17,  // 0x11
    AE_VAT_POLICE_VAN        = 18,  // 0x12
    AE_VAT_BOAT              = 19,  // 0x13
    AE_VAT_COACH             = 20,  // 0x14
    AE_VAT_TANK              = 21,  // 0x15
    AE_VAT_CONVERTIBLE       = 22,  // 0x16
    AE_VAT_HEARSE            = 23,  // 0x17
    AE_VAT_MONSTER_TRUCK     = 24,  // 0x18
    AE_VAT_MOPED             = 25,  // 0x19
    AE_VAT_TRAM              = 26,  // 0x1A
    AE_VAT_GOLF_CART         = 27,  // 0x1B
    AE_VAT_PLANE             = 28,  // 0x1C
    AE_VAT_BIKE              = 29,  // 0x1D
    AE_VAT_QUADBIKE          = 30,  // 0x1E
    AE_VAT_COUPE             = 31,  // 0x1F
    AE_VAT_BULLDOZER         = 32,  // 0x20
    AE_VAT_FORKLIFT          = 33,  // 0x21
    AE_VAT_TRACTOR           = 34,  // 0x22
    AE_VAT_COMBINE_HARVESTER = 35,  // 0x23
    AE_VAT_KART              = 36,  // 0x24
    AE_VAT_MOWER             = 37,  // 0x25
    AE_VAT_POLICE_CAR        = 38,  // 0x26
    AE_VAT_TRAIN             = 39,  // 0x27
    AE_VAT_HOVERCRAFT        = 40,  // 0x28
    AE_VAT_BICYCLE           = 41,  // 0x29
    AE_VAT_SEA_PLANE         = 42,  // 0x2A
    AE_VAT_DINGHY            = 43,  // 0x2B
    AE_VAT_CAMPER_VAN        = 44,  // 0x2C
    AE_VAT_FOUR_DOOR         = 45,  // 0x2D
    AE_VAT_END               = 46,  // 0x2E
};
