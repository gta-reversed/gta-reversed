#pragma once

#include <extensions/WEnum.hpp>

enum eAudioPedType : int16 {
    PEDTYPE_UNK    = -1, // notsa
    PEDTYPE_GEN    = 0,
    PEDTYPE_EMG    = 1,
    PEDTYPE_PLAYER = 2,
    PEDTYPE_GANG   = 3,
    PEDTYPE_GFD    = 4,
    PEDTYPE_SPC    = 5, // SPC => Special (?)

    //
    // Add above
    //
    PEDTYPE_NUM // = 6
};
NOTSA_WENUM_DEFS_FOR(eAudioPedType);
