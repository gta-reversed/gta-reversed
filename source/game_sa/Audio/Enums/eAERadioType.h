#pragma once

#include <Base.h>

enum eAERadioType : int8 {
    AE_RT_DISABLED  = -1,
    AE_RT_CIVILIAN  = 0,
    AE_RT_SPECIAL   = 1,
    AE_RT_UNKNOWN   = 2,
    AE_RT_EMERGENCY = 3,
};
