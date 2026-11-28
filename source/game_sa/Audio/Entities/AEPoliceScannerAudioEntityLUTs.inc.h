#pragma once

//
// NOTE: Do not include this header anywhere other than `AEPoliceScannerAudioEntity.cpp`!
//

// 0x8C8160 - SND_BANK_SCRIPT_SCANNER_INSTRUCTIONS
constexpr std::array<eSoundID, 4> INTRO_SOUNDS{ 0, 5, 8, 0 };

// 0x8C8168 - SND_BANK_SCRIPT_SCANNER_NUMBERS, indexed by `eCrimeType`
constexpr std::array<eSoundID, MAX_CRIMES> CRIME_SOUNDS{
    -1, // CRIME_NONE
    -1, // CRIME_FIRE_WEAPON
    0,  // CRIME_DAMAGED_PED
    7,  // CRIME_DAMAGED_COP
    5,  // CRIME_DAMAGE_CAR
    8,  // CRIME_DAMAGE_COP_CAR
    2,  // CRIME_CAR_STEAL
    1,  // CRIME_RUN_REDLIGHT
    1,  // CRIME_VEHICLE_DAMAGE
    1,  // CRIME_SPEEDING
    0,  // CRIME_KILL_PED_WITH_CAR
    8,  // CRIME_KILL_COP_PED_WITH_CAR
    10, // CRIME_DESTROY_HELI
    3,  // CRIME_SET_PED_ON_FIRE
    8,  // CRIME_SET_COP_PED_ON_FIRE
    3,  // CRIME_SET_CAR_ON_FIRE
    10, // CRIME_DESTROY_PLANE
    4,  // CRIME_EXPLOSION
    5,  // CRIME_STAB_PED
    8,  // CRIME_STAB_COP
    9,  // CRIME_DESTROY_VEHICLE
    6,  // CRIME_HIT_CAR
    6,  // CRIME_AIM_GUN
};

struct tScannerZone {
    char     Name[8];      //!< `CZone::m_TextLabel`
    eSoundID SoundID;      //!< SND_BANK_SCRIPT_SCANNER_AREAS, -1 if the zone has none
    bool     HasDirection; //!< Say the direction inside the zone too
};

// 0x8C8198 (names), 0x8C87A8 (sounds), 0x8C8930 (directions)
constexpr auto SCANNER_ZONES = std::to_array<tScannerZone>({
    { "ALDEA",   0,   true  }, // 0
    { "ANGPI",   1,   true  }, // 1
    { "ARCO",    2,   true  }, // 2
    { "CUNTC",   3,   false }, // 3
    { "BACKO",   4,   true  }, // 4
    { "BATTP",   5,   true  }, // 5
    { "SUNNN",   6,   true  }, // 6
    { "SUNMA",   7,   false }, // 7
    { "BYTUN",   8,   true  }, // 8
    { "BEACO",   9,   true  }, // 9
    { "BFLD",    10,  true  }, // 10
    { "BFC",     11,  true  }, // 11
    { "BINT",    12,  true  }, // 12
    { "BLUAC",   13,  true  }, // 13
    { "BLUEB",   14,  true  }, // 14
    { "BONE",    15,  true  }, // 15
    { "CALI",    16,  false }, // 16
    { "CALT",    17,  true  }, // 17
    { "CALT",    18,  true  }, // 18
    { "CITYS",   19,  false }, // 19
    { "LOT",     20,  false }, // 20
    { "COM",     21,  true  }, // 21
    { "CONF",    22,  false }, // 22
    { "CRANB",   23,  false }, // 23
    { "DILLI",   24,  true  }, // 24
    { "DOH",     25,  true  }, // 25
    { "SFDWT",   26,  true  }, // 26
    { "LDT",     27,  true  }, // 27
    { "ELS",     28,  false }, // 28
    { "EBE",     29,  false }, // 29
    { "EASB",    30,  true  }, // 30
    { "SFAIR",   31,  false }, // 31
    { "EBAY",    32,  false }, // 32
    { "ETUNN",   33,  false }, // 33
    { "ELCA",    34,  true  }, // 34
    { "ELCO",    35,  true  }, // 35
    { "ELQUE",   36,  true  }, // 36
    { "ESPE",    37,  false }, // 37
    { "ESPN",    38,  false }, // 38
    { "HAUL",    39,  true  }, // 39
    { "FALLO",   40,  false }, // 40
    { "FERN",    41,  true  }, // 41
    { "FINA",    42,  true  }, // 42
    { "FISH",    43,  true  }, // 43
    { "FLINTC",  44,  true  }, // 44
    { "FLINTI",  45,  true  }, // 45
    { "FLINTR",  46,  true  }, // 46
    { "FLINW",   47,  true  }, // 47
    { "CARSO",   48,  false }, // 48
    { "SILLY",   49,  true  }, // 49
    { "FRED",    50,  false }, // 50
    { "GAN",     51,  true  }, // 51
    { "GANTB",   52,  false }, // 52
    { "GARC",    53,  true  }, // 53
    { "GARV",    54,  false }, // 54
    { "GLN",     55,  true  }, // 55
    { "GGC",     56,  false }, // 56
    { "PALMS",   57,  true  }, // 57
    { "HBARNS",  58,  true  }, // 58
    { "HANKY",   59,  true  }, // 59
    { "HGP",     60,  true  }, // 60
    { "HASH",    61,  true  }, // 61
    { "TOPFA",   62,  false }, // 62
    { "QUARY",   63,  false }, // 63
    { "IWD",     64,  true  }, // 64
    { "JTE",     65,  false }, // 65
    { "JTN",     66,  false }, // 66
    { "JTS",     67,  false }, // 67
    { "JTW",     68,  false }, // 68
    { "JUNIHI",  69,  true  }, // 69
    { "JUNIHO",  70,  true  }, // 70
    { "KACC",    71,  false }, // 71
    { "KINC",    72,  false }, // 72
    { "THEA",    73,  true  }, // 73
    { "BARRA",   74,  true  }, // 74
    { "BRUJA",   75,  true  }, // 75
    { "PAYAS",   76,  true  }, // 76
    { "LDM",     77,  false }, // 77
    { "LEAFY",   79,  true  }, // 78
    { "PROBE",   80,  false }, // 79
    { "LDS",     81,  true  }, // 80
    { "LINDEN",  82,  false }, // 81
    { "LMEX",    83,  true  }, // 82
    { "CHC",     84,  true  }, // 83
    { "LFL",     85,  true  }, // 84
    { "LSINL",   87,  false }, // 85
    { "LAIR",    88,  false }, // 86
    { "???",     89,  true  }, // 87
    { "VAIR",    90,  false }, // 88
    { "LVA",     91,  false }, // 89
    { "MAR",     92,  false }, // 90
    { "MKT",     93,  false }, // 91
    { "MARKST",  94,  false }, // 92
    { "MART",    95,  false }, // 93
    { "HILLP",   96,  true  }, // 94
    { "MONT",    97,  true  }, // 95
    { "MONINT",  98,  true  }, // 96
    { "MTCHI",   99,  true  }, // 97
    { "MUL",     100, true  }, // 98
    { "MULINT",  101, true  }, // 99
    { "NROCK",   102, true  }, // 100
    { "LDOC",    103, true  }, // 101
    { "OCEAF",   104, false }, // 102
    { "OCTAN",   105, false }, // 103
    { "OVS",     106, true  }, // 104
    { "OPEN",    107, false }, // 105
    { "BAYV",    108, true  }, // 106
    { "PALO",    109, true  }, // 107
    { "PARA",    110, true  }, // 108
    { "PILG",    111, true  }, // 109
    { "PINT",    112, true  }, // 110
    { "PLS",     113, true  }, // 111
    { "PRP",     114, true  }, // 112
    { "????",    115, true  }, // 113
    { "RIE",     116, false }, // 114
    { "RED",     117, true  }, // 115
    { "REDE",    118, false }, // 116
    { "REDW",    119, false }, // 117
    { "TOM",     120, false }, // 118
    { "RIH",     121, true  }, // 119
    { "ROCE",    122, true  }, // 120
    { "RSE",     123, false }, // 121
    { "RSW",     124, false }, // 122
    { "ROD",     125, true  }, // 123
    { "ROY",     126, false }, // 124
    { "SASO",    127, false }, // 125
    { "SANB",    129, true  }, // 126
    { "CIVI",    130, true  }, // 127
    { "SMB",     131, true  }, // 128
    { "CREEK",   132, true  }, // 129
    { "SHERR",   133, false }, // 130
    { "SRY",     134, false }, // 131
    { "SPIN",    135, true  }, // 132
    { "STAR",    136, false }, // 133
    { "SUN",     137, true  }, // 134
    { "SUN",     138, true  }, // 135
    { "BIGE",    139, false }, // 136
    { "CAM",     140, false }, // 137
    { "RING",    141, false }, // 138
    { "ISLE",    142, false }, // 139
    { "FARM",    143, false }, // 140
    { "DRAG",    144, false }, // 141
    { "HIGH",    145, false }, // 142
    { "MAKO",    146, false }, // 143
    { "PANOP",   147, false }, // 144
    { "PINK",    148, false }, // 145
    { "PIRA",    149, false }, // 146
    { "DAM",     150, false }, // 147
    { "VISA",    151, false }, // 148
    { "ROBAD",   152, true  }, // 149
    { "UNITY",   153, false }, // 150
    { "VALLE",   154, true  }, // 151
    { "BLUF",    155, false }, // 152
    { "MEAD",    156, true  }, // 153
    { "VERO",    157, true  }, // 154
    { "VIN",     158, true  }, // 155
    { "WHET",    159, true  }, // 156
    { "WWE",     160, true  }, // 157
    { "LIND",    161, true  }, // 158
    { "YBELL",   162, false }, // 159
    { "YELLOW",  163, false }, // 160
    { "VE",      78,  true  }, // 161
    { "LA",      86,  true  }, // 162
    { "SF",      128, true  }, // 163
    { "SAN_AND", -1,  true  }, // 164
    { "JEF",     -1,  true  }, // 165
    { "PER1",    86,  false }, // 166
    { "YBELL1",  162, false }, // 167
    { "YBELL2",  162, false }, // 168
    { "STRIP",   78,  false }, // 169
    { "WESTP",   -1,  true  }, // 170
    { "CUNTC1",  3,   false }, // 171
    { "CUNTC2",  3,   false }, // 172
    { "CUNTC3",  3,   false }, // 173
    { "KINC1",   72,  false }, // 174
    { "KINC2",   72,  false }, // 175
    { "GARV1",   54,  false }, // 176
    { "GARV2",   54,  false }, // 177
    { "MONT1",   97,  false }, // 178
    { "MTCHI1",  99,  false }, // 179
    { "MTCHI2",  99,  false }, // 180
    { "MTCHI3",  99,  false }, // 181
    { "MTCHI4",  99,  false }, // 182
    { "SHACA",   -1,  true  }, // 183
    { "REST",    -1,  true  }, // 184
    { "SANB2",   129, false }, // 185
    { "SFBAG1",  31,  false }, // 186
    { "SFBAG2",  31,  false }, // 187
    { "SFBAG3",  31,  false }, // 188
    { "LVBAG",   90,  false }, // 189
    { "LBAG1",   88,  false }, // 190
    { "LBAG2",   88,  false }, // 191
    { "LBAG3",   88,  false }, // 192
    { "CONST1",  78,  false }, // 193
});

struct tScannerVehicle {
    eSoundID SoundID;   //!< SND_BANK_SCRIPT_SCANNER_VEHICLES
    bool     HasColour; //!< Say the colour of the vehicle too
};

// 0x8C89F8 (sounds), 0x8C8A54 (colours), indexed by `eAEVehicleAudioTypeForName`
constexpr auto SCANNER_VEHICLES = std::to_array<tScannerVehicle>({
    { 34, true  }, // AE_VAT_OFFROAD
    { 0,  true  }, // AE_VAT_TWO_DOOR
    { 47, true  }, // AE_VAT_SPORTS_CAR
    { 44, true  }, // AE_VAT_RIG
    { 49, true  }, // AE_VAT_STATION_WAGON
    { -1, true  }, // AE_VAT_SEDAN
    { 56, true  }, // AE_VAT_TRUCK
    { 17, false }, // AE_VAT_FIRETRUCK
    { 20, false }, // AE_VAT_GARBAGE_TRUCK
    { 30, true  }, // AE_VAT_STRETCH
    { 31, true  }, // AE_VAT_LOWRIDER
    { 57, true  }, // AE_VAT_VAN
    { 2,  true  }, // AE_VAT_AMBULANCE
    { 25, true  }, // AE_VAT_HELICOPTER
    { 52, false }, // AE_VAT_TAXI
    { 36, true  }, // AE_VAT_PICK_UP
    { 27, true  }, // AE_VAT_ICE_CREAM_VAN
    { 7,  true  }, // AE_VAT_BUGGY
    { 39, false }, // AE_VAT_POLICE_VAN
    { 6,  true  }, // AE_VAT_BOAT
    { 11, true  }, // AE_VAT_COACH
    { 51, false }, // AE_VAT_TANK
    { 14, true  }, // AE_VAT_CONVERTIBLE
    { 24, false }, // AE_VAT_HEARSE
    { 56, true  }, // AE_VAT_MONSTER_TRUCK
    { 32, true  }, // AE_VAT_MOPED
    { 55, false }, // AE_VAT_TRAM
    { 22, true  }, // AE_VAT_GOLF_CART
    { 37, true  }, // AE_VAT_PLANE
    { 33, true  }, // AE_VAT_BIKE
    { 40, true  }, // AE_VAT_QUADBIKE
    { 15, true  }, // AE_VAT_COUPE
    { 8,  false }, // AE_VAT_BULLDOZER
    { 18, true  }, // AE_VAT_FORKLIFT
    { 53, true  }, // AE_VAT_TRACTOR
    { 12, true  }, // AE_VAT_COMBINE_HARVESTER
    { 23, true  }, // AE_VAT_KART
    { 29, true  }, // AE_VAT_MOWER
    { 38, false }, // AE_VAT_POLICE_CAR
    { 54, false }, // AE_VAT_TRAIN
    { 26, true  }, // AE_VAT_HOVERCRAFT
    { 5,  true  }, // AE_VAT_BICYCLE
    { 43, true  }, // AE_VAT_SEA_PLANE
    { 41, false }, // AE_VAT_DINGHY
    { 10, true  }, // AE_VAT_CAMPER_VAN
    { 1,  true  }, // AE_VAT_FOUR_DOOR
});

// 0x8C8A88 - SND_BANK_SCRIPT_SCANNER_COLOURS, indexed by the vehicle colour
// clang-format off
constexpr std::array<eSoundID, 127> SCANNER_COLOURS{
    0, 14, 1, 12, 1, 6, -1, 1, 1, 10, 1, 9, 1, 9, 10, 9,
    8, 12, 12, 10, 1, 12, 12, 9, 9, 6, 9, 9, 1, 9, 6, 12,
    1, 9, 10, 9, 6, 8, 10, 1, 6, 6, 12, 12, 12, 12, 10, 8,
    10, 9, 9, 8, 8, 1, 1, 6, 1, 10, 12, 1, 10, 10, 12, 9,
    9, 10, 6, 1, 10, 10, 12, 1, 9, 10, 12, 6, 9, 10, 12, 1,
    12, 10, 12, 8, 6, 12, 10, 1, 12, 10, 13, 1, 9, 10, 1, 1,
    13, 1, 1, 10, 1, 6, 10, 1, 10, 9, 1, 10, 10, 9, 6, 9,
    1, 6, 8, 12, 1, 12, 10, 6, 10, 12, 9, 6, 12, 1, 11,
};
// clang-format on
