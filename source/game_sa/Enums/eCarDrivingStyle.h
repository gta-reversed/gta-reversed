#pragma once

enum eCarDrivingStyle : int8 {
    DRIVING_STYLE_STOP_FOR_CARS                                = 0,
    DRIVING_STYLE_SLOW_DOWN_FOR_CARS                           = 1,
    DRIVING_STYLE_AVOID_CARS                                   = 2,
    DRIVING_STYLE_PLOUGH_THROUGH                               = 3,
    DRIVING_STYLE_STOP_FOR_CARS_IGNORE_LIGHTS                  = 4,
    DRIVING_STYLE_DRIVINGMODE_AVOIDCARS_OBEYLIGHTS             = 5,
    DRIVING_STYLE_DRIVINGMODE_AVOIDCARS_STOPFORPEDS_OBEYLIGHTS = 6,
};
