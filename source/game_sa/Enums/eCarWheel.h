#pragma once

enum eCarWheel : uint32 {
    CAR_WHEEL_FRONT_LEFT  = 0,
    CAR_WHEEL_REAR_LEFT   = 1,
    CAR_WHEEL_FRONT_RIGHT = 2,
    CAR_WHEEL_REAR_RIGHT  = 3,

    MAX_CARWHEELS
};

enum eCarPiece CarWheelToCarPiece(eCarWheel i);
