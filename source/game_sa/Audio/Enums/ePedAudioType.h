#pragma once

#include <extensions/WEnum.hpp>

enum eAudioPedType : int16 {
    PED_TYPE_UNK    = -1, // notsa
    PED_TYPE_GEN    = 0,
    PED_TYPE_EMG    = 1,
    PED_TYPE_PLAYER = 2,
    PED_TYPE_GANG   = 3,
    PED_TYPE_GFD    = 4,
    PED_TYPE_SPC    = 5, // SPC => Special (?)

    //
    // Add above
    //
    PED_TYPE_NUM // = 6
};
NOTSA_WENUM_DEFS_FOR(eAudioPedType);
