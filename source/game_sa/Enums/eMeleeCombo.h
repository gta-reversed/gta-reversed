#pragma once

#include <extensions/WEnum.hpp>

enum class eMeleeCombo : int8 {
    IDLE        = 0,
    MOVE        = 1,
    BLOCK       = 2,
    END         = 3,

    // The ones below are defined in `melee.dat` (Subtract 4 to use it as an index into `CTaskSimpleFight::m_aComboData`
    UNARMED_1   = 4,
    UNARMED_2   = 5,
    UNARMED_3   = 6,
    UNARMED_4   = 7,
    BBALLBAT    = 8,
    KNIFE       = 9,
    GOLFCLUB    = 10,
    SWORD       = 11,
    CHAINSAW    = 12,
    DILDO       = 13,
    FLOWERS     = 14,
    KICK_STD    = 15,
    PISTOL_WHIP = 16,

    //
    // Add above
    //
    NUM,
    NUM_SETS = +PISTOL_WHIP - +UNARMED_1 + 1
};
NOTSA_WENUM_DEFS_FOR(eMeleeCombo);

constexpr auto MeleeCombo2MeleeDataIdx(eMeleeCombo e) {
    return std::max(+e - +eMeleeCombo::UNARMED_1, 0);
}
