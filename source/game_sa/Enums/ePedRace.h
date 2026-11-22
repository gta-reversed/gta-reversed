#pragma once

#include <Base.h>

enum ePedRace : int32 {
    RACE_DEFAULT = 0, //< iUsed to mark any race is allowed to spawn, look `IsRaceAllowedInCurrentZone`
    RACE_BLACK = 1,    //< 'B'
    RACE_WHITE = 2,    //< 'W'
    RACE_ORIENTAL = 3, //< 'O', 'I'
    RACE_HISPANIC = 4  //< 'H'
};
