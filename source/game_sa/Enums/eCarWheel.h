#pragma once

enum eCarWheel {
    CAR_WHEEL_FRONT_LEFT  = 0,
    CAR_WHEEL_REAR_LEFT   = 1,
    CAR_WHEEL_FRONT_RIGHT = 2,
    CAR_WHEEL_REAR_RIGHT  = 3,

    MAX_CARWHEELS,

    // virtual on top of rear wheels
    CAR_WHEEL_VIRTUAL_MIDDLE_LEFT = 4,
    CAR_WHEEL_VIRTUAL_MIDDLE_RIGHT = 5
};

enum eCarPiece CarWheelToCarPiece(eCarWheel i);
