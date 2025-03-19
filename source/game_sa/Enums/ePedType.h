#pragma once

#include <ranges>
#include <algorithm>

enum ePedType : uint32 {
    PEDTYPE_NONE = (uint32)(-1),
    PEDTYPE_PLAYER1 = 0,
    PEDTYPE_PLAYER2,
    PEDTYPE_PLAYER_NETWORK,
    PEDTYPE_PLAYER_UNUSED,
    PEDTYPE_CIVMALE,
    PEDTYPE_CIVFEMALE,

    PEDTYPE_COP,

    PEDTYPE_GANG1,         // Ballas
    PEDTYPE_GANG2,         // Grove Street Families
    PEDTYPE_GANG3,         // Los Santos Vagos
    PEDTYPE_GANG4,         // San Fierro Rifa
    PEDTYPE_GANG5,         // Da Nang Boys
    PEDTYPE_GANG6,         // Mafia
    PEDTYPE_GANG7,         // Mountain Cloud Triad
    PEDTYPE_GANG8,         // Varrio Los Aztecas
    PEDTYPE_GANG9,         // Russian Mafia
    PEDTYPE_GANG10,        // Bikers

    PEDTYPE_DEALER,
    PEDTYPE_MEDIC,
    PEDTYPE_FIREMAN,
    PEDTYPE_CRIMINAL,
    PEDTYPE_BUM,
    PEDTYPE_PROSTITUTE,
    PEDTYPE_SPECIAL,
    PEDTYPE_MISSION1,
    PEDTYPE_MISSION2,
    PEDTYPE_MISSION3,
    PEDTYPE_MISSION4,
    PEDTYPE_MISSION5,
    PEDTYPE_MISSION6,
    PEDTYPE_MISSION7,
    PEDTYPE_MISSION8,

    PEDTYPE_COUNT // 32
};

static constexpr auto s_GangPedTypes = std::to_array({ // TODO: Get rid of this (Use `GetAllGangPedTypes()`)
    PEDTYPE_GANG1,
    PEDTYPE_GANG2,
    PEDTYPE_GANG3,
    PEDTYPE_GANG4,
    PEDTYPE_GANG5,
    PEDTYPE_GANG6,
    PEDTYPE_GANG7,
    PEDTYPE_GANG8,
    PEDTYPE_GANG9,
    PEDTYPE_GANG10,
});

static constexpr bool IsPedTypeGang(ePedType ptype) {
    return std::ranges::find(s_GangPedTypes, ptype) != s_GangPedTypes.end();
}

inline bool IsPedTypeFemale(ePedType type) {
    switch (type) {
    case PEDTYPE_PROSTITUTE:
    case PEDTYPE_CIVFEMALE:
        return true;
    }
    return false;
}

static constexpr auto GetAllGangPedTypes() {
    return std::array{
        PEDTYPE_GANG1,
        PEDTYPE_GANG2,
        PEDTYPE_GANG3,
        PEDTYPE_GANG4,
        PEDTYPE_GANG5,
        PEDTYPE_GANG6,
        PEDTYPE_GANG7,
        PEDTYPE_GANG8,
        PEDTYPE_GANG9,
        PEDTYPE_GANG10
    };
}

enum eGangID {
    GANG_BALLAS = 0,
    GANG_GROVE = 1,
    GANG_VAGOS = 2,
    GANG_RIFA = 3,
    GANG_DANANGBOYS = 4,
    GANG_MAFIA = 5,
    GANG_TRIAD = 6,
    GANG_AZTECAS = 7,
    GANG_UNUSED1 = 8, // RUSSIAN_MAFIA
    GANG_UNUSED2 = 9, // BIKERS

    TOTAL_GANGS
};

//! Notsa
inline eGangID GetGangOfPedType(ePedType ptype) {
    switch (ptype) {
    case PEDTYPE_GANG1:  return GANG_BALLAS;
    case PEDTYPE_GANG2:  return GANG_GROVE;
    case PEDTYPE_GANG3:  return GANG_VAGOS;
    case PEDTYPE_GANG4:  return GANG_RIFA;
    case PEDTYPE_GANG5:  return GANG_DANANGBOYS;
    case PEDTYPE_GANG6:  return GANG_MAFIA;
    case PEDTYPE_GANG7:  return GANG_TRIAD;
    case PEDTYPE_GANG8:  return GANG_AZTECAS;
    case PEDTYPE_GANG9:  return GANG_UNUSED1;
    case PEDTYPE_GANG10: return GANG_UNUSED2;
    default:              NOTSA_UNREACHABLE();
    }
}

// 0x8D23B8
constexpr const char* aPedTypeNames[PEDTYPE_COUNT] = {
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
