#pragma once

#include <extensions/WEnum.hpp>

// Each area code can have one or more interiors.
// For more info, check https://wiki.mtasa.com/wiki/Interior_IDs
enum eAreaCodes : uint8 {
    AREA_CODE_NONE = -1,
    AREA_CODE_NORMAL_WORLD,
    AREA_CODE_1,
    AREA_CODE_2,
    AREA_CODE_3,
    AREA_CODE_4,
    AREA_CODE_5,
    AREA_CODE_6,
    AREA_CODE_7,
    AREA_CODE_8,
    AREA_CODE_9,
    AREA_CODE_10,
    AREA_CODE_11,
    AREA_CODE_12,
    AREA_CODE_13,
    AREA_CODE_14,
    AREA_CODE_15,
    AREA_CODE_16,
    AREA_CODE_17,
    AREA_CODE_18,
};
NOTSA_WENUM_DEFS_FOR(eAreaCodes);
