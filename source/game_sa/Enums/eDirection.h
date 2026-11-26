#pragma once

#include <Base.h>
#include <extensions/WEnum.hpp>

/*!
* Unsure if it's original game class, but these constants are widely used around the code.
*/
enum class eDirection : int32 {
    FORWARD,
    LEFT,
    BACKWARD,
    RIGHT
};
NOTSA_WENUM_DEFS_FOR(eDirection);
