#pragma once

#include <extensions/WEnum.hpp>

// Each area code can have one or more interiors.
// For more info, check https://wiki.mtasa.com/wiki/Interior_IDs
// Usually stored as int8 in the game, but we use int32 for better compatibility with function arguments
enum eAreaCodes : int32 {
    AREA_CODE_NONE = -1,
    AREA_CODE_NORMAL_WORLD, //!< Used when the player is outside
    AREA_CODE_1,            //!< Interiors
    AREA_CODE_2,            //!< Interiors
    AREA_CODE_3,            //!< Interiors
    AREA_CODE_4,            //!< Interiors
    AREA_CODE_5,            //!< Interiors
    AREA_CODE_6,            //!< Interiors
    AREA_CODE_7,            //!< Interiors
    AREA_CODE_8,            //!< Interiors
    AREA_CODE_9,            //!< Interiors
    AREA_CODE_10,           //!< Interiors
    AREA_CODE_11,           //!< Interiors
    AREA_CODE_12,           //!< Interiors
    AREA_CODE_13,           //!< Interiors
    AREA_CODE_14,           //!< Interiors
    AREA_CODE_15,           //!< Interiors
    AREA_CODE_16,           //!< Interiors
    AREA_CODE_17,           //!< Interiors
    AREA_CODE_18,           //!< Interiors
};

NOTSA_WENUM_DEFS_FOR(eAreaCodes);
