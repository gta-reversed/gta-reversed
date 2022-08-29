#pragma once

enum ePedType : uint32 {
    PED_TYPE_PLAYER1 = 0,
    PED_TYPE_PLAYER2,
    PED_TYPE_PLAYER_NETWORK,
    PED_TYPE_PLAYER_UNUSED,
    PED_TYPE_CIVMALE,
    PED_TYPE_CIVFEMALE,
    PED_TYPE_COP,
    PED_TYPE_GANG1,         // Ballas
    PED_TYPE_GANG2,         // Grove Street Families
    PED_TYPE_GANG3,         // Los Santos Vagos
    PED_TYPE_GANG4,         // San Fierro Rifa
    PED_TYPE_GANG5,         // Da Nang Boys
    PED_TYPE_GANG6,         // Mafia
    PED_TYPE_GANG7,         // Mountain Cloud Triad
    PED_TYPE_GANG8,         // Varrio Los Aztecas
    PED_TYPE_GANG9,         // Russian Mafia
    PED_TYPE_GANG10,        // Bikers
    PED_TYPE_DEALER,
    PED_TYPE_MEDIC,
    PED_TYPE_FIREMAN,
    PED_TYPE_CRIMINAL,
    PED_TYPE_BUM,
    PED_TYPE_PROSTITUTE,
    PED_TYPE_SPECIAL,
    PED_TYPE_MISSION1,
    PED_TYPE_MISSION2,
    PED_TYPE_MISSION3,
    PED_TYPE_MISSION4,
    PED_TYPE_MISSION5,
    PED_TYPE_MISSION6,
    PED_TYPE_MISSION7,
    PED_TYPE_MISSION8,

    PED_TYPE_COUNT // 32
};

// 0x8D23B8
constexpr const char* aPedTypeNames[PED_TYPE_COUNT] = {
    "PLAYER1",
    "PLAYER2",
    "PLAYER_NETWORK",
    "PLAYER_UNUSED",
    "CIVMALE",
    "CIVFEMALE",
    "COP",
    "GANG1",
    "GANG2",
    "GANG3",
    "GANG4",
    "GANG5",
    "GANG6",
    "GANG7",
    "GANG8",
    "GANG9",
    "GANG10",
    "DEALER",
    "MEDIC",
    "FIREMAN",
    "CRIMINAL",
    "BUM",
    "PROSTITUTE",
    "SPECIAL",
    "MISSION1",
    "MISSION2",
    "MISSION3",
    "MISSION4",
    "MISSION5",
    "MISSION6",
    "MISSION7",
    "MISSION8",
};
