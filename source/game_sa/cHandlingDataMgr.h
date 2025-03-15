/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "tHandlingData.h"
#include "tBikeHandlingData.h"
#include "tFlyingHandlingData.h"
#include "tBoatHandlingData.h"

enum tVehicleType : int32 {
    VT_LANDSTAL    = 0, // 0x0
    VT_BRAVURA     = 1, // 0x1
    VT_BUFFALO     = 2, // 0x2
    VT_LINERUN     = 3, // 0x3
    VT_PEREN       = 4, // 0x4
    VT_SENTINEL    = 5, // 0x5
    VT_DUMPER      = 6, // 0x6
    VT_FIRETRUK    = 7, // 0x7
    VT_TRASH       = 8, // 0x8
    VT_STRETCH     = 9, // 0x9
    VT_MANANA      = 10, // 0xA
    VT_INFERNUS    = 11, // 0xB
    VT_VOODOO      = 12, // 0xC
    VT_PONY        = 13, // 0xD
    VT_MULE        = 14, // 0xE
    VT_CHEETAH     = 15, // 0xF
    VT_AMBULAN     = 16, // 0x10
    VT_MOONBEAM    = 17, // 0x11
    VT_ESPERANT    = 18, // 0x12
    VT_TAXI        = 19, // 0x13
    VT_WASHINGTON  = 20, // 0x14
    VT_BOBCAT      = 21, // 0x15
    VT_MRWHOOP     = 22, // 0x16
    VT_BFINJECT    = 23, // 0x17
    VT_PREMIER     = 24, // 0x18
    VT_ENFORCER    = 25, // 0x19
    VT_SECURICA    = 26, // 0x1A
    VT_BANSHEE     = 27, // 0x1B
    VT_BUS         = 28, // 0x1C
    VT_RHINO       = 29, // 0x1D
    VT_BARRACKS    = 30, // 0x1E
    VT_HOTKNIFE    = 31, // 0x1F
    VT_ARTICT1     = 32, // 0x20
    VT_PREVION     = 33, // 0x21
    VT_COACH       = 34, // 0x22
    VT_CABBIE      = 35, // 0x23
    VT_STALLION    = 36, // 0x24
    VT_RUMPO       = 37, // 0x25
    VT_RCBANDIT    = 38, // 0x26
    VT_ROMERO      = 39, // 0x27
    VT_PACKER      = 40, // 0x28
    VT_MONSTER     = 41, // 0x29
    VT_ADMIRAL     = 42, // 0x2A
    VT_TRAM        = 43, // 0x2B
    VT_AIRTRAIN    = 44, // 0x2C
    VT_ARTICT2     = 45, // 0x2D
    VT_TURISMO     = 46, // 0x2E
    VT_FLATBED     = 47, // 0x2F
    VT_YANKEE      = 48, // 0x30
    VT_GOLFCART    = 49, // 0x31
    VT_SOLAIR      = 50, // 0x32
    VT_TOPFUN      = 51, // 0x33
    VT_GLENDALE    = 52, // 0x34
    VT_OCEANIC     = 53, // 0x35
    VT_PATRIOT     = 54, // 0x36
    VT_HERMES      = 55, // 0x37
    VT_SABRE       = 56, // 0x38
    VT_ZR350       = 57, // 0x39
    VT_WALTON      = 58, // 0x3A
    VT_REGINA      = 59, // 0x3B
    VT_COMET       = 60, // 0x3C
    VT_BURRITO     = 61, // 0x3D
    VT_CAMPER      = 62, // 0x3E
    VT_BAGGAGE     = 63, // 0x3F
    VT_DOZER       = 64, // 0x40
    VT_RANCHER     = 65, // 0x41
    VT_FBIRANCHER  = 66, // 0x42
    VT_VIRGO       = 67, // 0x43
    VT_GREENWOOD   = 68, // 0x44
    VT_HOTRING     = 69, // 0x45
    VT_SANDKING    = 70, // 0x46
    VT_BLISTAC     = 71, // 0x47
    VT_BOXVILLE    = 72, // 0x48
    VT_BENSON      = 73, // 0x49
    VT_MESA        = 74, // 0x4A
    VT_BLOODRA     = 75, // 0x4B
    VT_BLOODRB     = 76, // 0x4C
    VT_SUPERGT     = 77, // 0x4D
    VT_ELEGANT     = 78, // 0x4E
    VT_JOURNEY     = 79, // 0x4F
    VT_PETROL      = 80, // 0x50
    VT_RDTRAIN     = 81, // 0x51
    VT_NEBULA      = 82, // 0x52
    VT_MAJESTIC    = 83, // 0x53
    VT_BUCCANEE    = 84, // 0x54
    VT_CEMENT      = 85, // 0x55
    VT_TOWTRUCK    = 86, // 0x56
    VT_FORTUNE     = 87, // 0x57
    VT_CADRONA     = 88, // 0x58
    VT_FBITRUCK    = 89, // 0x59
    VT_WILLARD     = 90, // 0x5A
    VT_FORKLIFT    = 91, // 0x5B
    VT_TRACTOR     = 92, // 0x5C
    VT_COMBINE     = 93, // 0x5D
    VT_FELTZER     = 94, // 0x5E
    VT_REMINGTON   = 95, // 0x5F
    VT_SLAMVAN     = 96, // 0x60
    VT_BLADE       = 97, // 0x61
    VT_FREIGHT     = 98, // 0x62
    VT_STREAK      = 99, // 0x63
    VT_VINCENT     = 100, // 0x64
    VT_BULLET      = 101, // 0x65
    VT_CLOVER      = 102, // 0x66
    VT_SADLER      = 103, // 0x67
    VT_RANGER      = 104, // 0x68
    VT_HUSTLER     = 105, // 0x69
    VT_INTRUDER    = 106, // 0x6A
    VT_PRIMO       = 107, // 0x6B
    VT_TAMPA       = 108, // 0x6C
    VT_SUNRISE     = 109, // 0x6D
    VT_MERIT       = 110, // 0x6E
    VT_UTILITY     = 111, // 0x6F
    VT_YOSEMITE    = 112, // 0x70
    VT_WINDSOR     = 113, // 0x71
    VT_MONSTER_A   = 114, // 0x72
    VT_MONSTER_B   = 115, // 0x73
    VT_URANUS      = 116, // 0x74
    VT_JESTER      = 117, // 0x75
    VT_SULTAN      = 118, // 0x76
    VT_STRATUM     = 119, // 0x77
    VT_ELEGY       = 120, // 0x78
    VT_TIGER       = 121, // 0x79
    VT_FLASH       = 122, // 0x7A
    VT_TAHOMA      = 123, // 0x7B
    VT_SAVANNA     = 124, // 0x7C
    VT_BANDITO     = 125, // 0x7D
    VT_FREIFLAT    = 126, // 0x7E
    VT_STREAKC     = 127, // 0x7F
    VT_KART        = 128, // 0x80
    VT_MOWER       = 129, // 0x81
    VT_DUNE        = 130, // 0x82
    VT_SWEEPER     = 131, // 0x83
    VT_BROADWAY    = 132, // 0x84
    VT_TORNADO     = 133, // 0x85
    VT_DFT30       = 134, // 0x86
    VT_HUNTLEY     = 135, // 0x87
    VT_STAFFORD    = 136, // 0x88
    VT_NEWSVAN     = 137, // 0x89
    VT_TUG         = 138, // 0x8A
    VT_PETROTR     = 139, // 0x8B
    VT_EMPEROR     = 140, // 0x8C
    VT_FLOAT       = 141, // 0x8D
    VT_EUROS       = 142, // 0x8E
    VT_HOTDOG      = 143, // 0x8F
    VT_CLUB        = 144, // 0x90
    VT_ARTICT3     = 145, // 0x91
    VT_RCCAM       = 146, // 0x92
    VT_POLICE_SF   = 147, // 0x93
    VT_POLICE_LS   = 148, // 0x94
    VT_POLICE_VG   = 149, // 0x95
    VT_POLRANGER   = 150, // 0x96
    VT_PICADOR     = 151, // 0x97
    VT_SWATVAN     = 152, // 0x98
    VT_ALPHA       = 153, // 0x99
    VT_PHEONIX     = 154, // 0x9A
    VT_BAGBOXA     = 155, // 0x9B
    VT_BAGBOXB     = 156, // 0x9C
    VT_TUGSTAIR    = 157, // 0x9D
    VT_BOXBURG     = 158, // 0x9E
    VT_FARMTR1     = 159, // 0x9F
    VT_UTILTR1     = 160, // 0xA0
    VT_ROLLER      = 161, // 0xA1
    VT_BIKE        = 162, // 0xA2
    VT_MOPED       = 163, // 0xA3
    VT_DIRTBIKE    = 164, // 0xA4
    VT_FCR900      = 165, // 0xA5
    VT_NRG500      = 166, // 0xA6
    VT_HPV1000     = 167, // 0xA7
    VT_BF400       = 168, // 0xA8
    VT_WAYFARER    = 169, // 0xA9
    VT_QUADBIKE    = 170, // 0xAA
    VT_BMX         = 171, // 0xAB
    VT_CHOPPERBIKE = 172, // 0xAC
    VT_MTB         = 173, // 0xAD
    VT_FREEWAY     = 174, // 0xAE
    VT_PREDATOR    = 175, // 0xAF
    VT_SPEEDER     = 176, // 0xB0
    VT_REEFER      = 177, // 0xB1
    VT_RIO         = 178, // 0xB2
    VT_SQUALO      = 179, // 0xB3
    VT_TROPIC      = 180, // 0xB4
    VT_COASTGUARD  = 181, // 0xB5
    VT_DINGHY      = 182, // 0xB6
    VT_MARQUIS     = 183, // 0xB7
    VT_CUPBOAT     = 184, // 0xB8
    VT_LAUNCH      = 185, // 0xB9
    VT_SEAPLANE    = 186, // 0xBA
    VT_VORTEX      = 187, // 0xBB
    VT_RUSTLER     = 188, // 0xBC
    VT_BEAGLE      = 189, // 0xBD
    VT_CROPDUST    = 190, // 0xBE
    VT_STUNT       = 191, // 0xBF
    VT_SHAMAL      = 192, // 0xC0
    VT_HYDRA       = 193, // 0xC1
    VT_NEVADA      = 194, // 0xC2
    VT_AT400       = 195, // 0xC3
    VT_ANDROM      = 196, // 0xC4
    VT_DODO        = 197, // 0xC5
    VT_SPARROW     = 198, // 0xC6
    VT_SEASPARROW  = 199, // 0xC7
    VT_MAVERICK    = 200, // 0xC8
    VT_COASTMAV    = 201, // 0xC9
    VT_POLICEMAV   = 202, // 0xCA
    VT_HUNTER      = 203, // 0xCB
    VT_LEVIATHN    = 204, // 0xCC
    VT_CARGOBOB    = 205, // 0xCD
    VT_RAINDANC    = 206, // 0xCE
    VT_RCBARON     = 207, // 0xCF
    VT_RCGOBLIN    = 208, // 0xD0
    VT_RCRAIDER    = 209, // 0xD1
    VT_MAX         = 210, // 0xD2
};


class cHandlingDataMgr {
public:
    float               field_0;
    float               fWheelFriction;
    float               field_8;
    float               field_C;
    float               field_10;

    std::array<tHandlingData, 210>      m_aVehicleHandling;
    std::array<tBikeHandlingData, 13>   m_aBikeHandling;
    std::array<tFlyingHandlingData, 24> m_aFlyingHandling;
    std::array<tBoatHandlingData, 12>   m_aBoatHandling;

public:
    static void InjectHooks();

    cHandlingDataMgr();

    void Initialise();

    /// Process handling.cfg
    void LoadHandlingData();

    /// Get vehicle number id in names table
    int32 FindExactWord(const char* line, const char* nameTable, uint32 entrySize, uint32 entryCount);

    bool HasFrontWheelDrive(uint8 handlingId);
    bool HasRearWheelDrive(uint8 handlingId);
    int32 GetHandlingId(const char* name);

    void ConvertDataToWorldUnits(tHandlingData* handling);
    void ConvertDataToGameUnits(tHandlingData* handling);

    void ConvertBikeDataToWorldUnits(tBikeHandlingData* bikeHandling);
    void ConvertBikeDataToGameUnits(tBikeHandlingData* bikeHandling);

    tFlyingHandlingData* GetFlyingPointer(uint8 handlingId);
    tBoatHandlingData*   GetBoatPointer(uint8 handlingId);
    tHandlingData*       GetVehiclePointer(uint32 handlingId);
    tBikeHandlingData*   GetBikeHandlingPointer(uint32 handlingId);
};

VALIDATE_SIZE(cHandlingDataMgr, 0xC624);

// If you're searching for `mod_HandlingManager`, you can use this:
static inline cHandlingDataMgr& gHandlingDataMgr = *(cHandlingDataMgr*)0xC2B9C8;
