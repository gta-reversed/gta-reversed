#pragma once

#include <extensions/WEnum.hpp>

enum eSoundBankSlot : int32 {
    SND_BANK_SLOT_UNK                 = -1,
    SND_BANK_SLOT_FRONTEND_GAME       = 0, // 0x0
    SND_BANK_SLOT_FRONTEND_MENU       = 1, // 0x1
    SND_BANK_SLOT_COLLISIONS          = 2, // 0x2
    SND_BANK_SLOT_BULLET_HITS         = 3, // 0x3
    SND_BANK_SLOT_EXPLOSIONS          = 4, // 0x4
    SND_BANK_SLOT_WEAPON_GEN          = 5, // 0x5
    SND_BANK_SLOT_WEATHER             = 6, // 0x6
    SND_BANK_SLOT_DUMMY_ENGINE_0      = 7, // 0x7
    SND_BANK_SLOT_DUMMY_ENGINE_1      = 8, // 0x8
    SND_BANK_SLOT_DUMMY_ENGINE_2      = 9, // 0x9
    SND_BANK_SLOT_DUMMY_ENGINE_3      = 10, // 0xA
    SND_BANK_SLOT_DUMMY_ENGINE_4      = 11, // 0xB
    SND_BANK_SLOT_DUMMY_ENGINE_5      = 12, // 0xC
    SND_BANK_SLOT_DUMMY_ENGINE_6      = 13, // 0xD
    SND_BANK_SLOT_DUMMY_ENGINE_7      = 14, // 0xE
    SND_BANK_SLOT_DUMMY_ENGINE_8      = 15, // 0xF
    SND_BANK_SLOT_DUMMY_ENGINE_9      = 16, // 0x10
    SND_BANK_SLOT_HORN_AND_SIREN      = 17, // 0x11
    SND_BANK_SLOT_COP_HELI            = 18, // 0x12
    SND_BANK_SLOT_VEHICLE_GEN         = 19, // 0x13

    SND_BANK_SLOT_SPEECH1             = 20, // 0x14
    SND_BANK_SLOT_SPEECH2             = 21, // 0x15
    SND_BANK_SLOT_SPEECH3             = 22, // 0x16
    SND_BANK_SLOT_SPEECH4             = 23, // 0x17
    SND_BANK_SLOT_SPEECH5             = 24, // 0x18
    SND_BANK_SLOT_SPEECH6             = 25, // 0x19

    SND_BANK_SLOT_MISSION1            = 26, // 0x1A
    SND_BANK_SLOT_MISSION2            = 27, // 0x1B
    SND_BANK_SLOT_MISSION3            = 28, // 0x1C
    SND_BANK_SLOT_MISSION4            = 29, // 0x1D
    SND_BANK_SLOT_FOOTSTEPS_PLAYER    = 30, // 0x1E
    SND_BANK_SLOT_DOORS               = 31, // 0x1F
    SND_BANK_SLOT_SWIMMING            = 32, // 0x20
    SND_BANK_SLOT_SCANNER_INSTRUCTION = 33, // 0x21
    SND_BANK_SLOT_SCANNER_NUMBER      = 34, // 0x22
    SND_BANK_SLOT_SCANNER_DIRECTION1  = 35, // 0x23
    SND_BANK_SLOT_SCANNER_DIRECTION2  = 36, // 0x24
    SND_BANK_SLOT_SCANNER_AREA        = 37, // 0x25
    SND_BANK_SLOT_TEST1               = 38, // 0x26
    SND_BANK_SLOT_EFFECT1             = 39, // 0x27
    SND_BANK_SLOT_PLAYER_ENGINE_P     = 40, // 0x28
    SND_BANK_SLOT_FOOTSTEPS_GENERIC   = 41, // 0x29
    SND_BANK_SLOT_BULLET_PASS         = 42, // 0x2A
    SND_BANK_SLOT_TEST2               = 43, // 0x2B
    SND_BANK_SLOT_EFFECT2             = 44, // 0x2C
    NUM_SND_BANK_SLOTS                = 45, // 0x2D
};
NOTSA_WENUM_DEFS_FOR(eSoundBankSlot);
