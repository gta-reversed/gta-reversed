#include <StdInc.h>

#include "Entity/Vehicle/Vehicle.h" // eCarPiece
#include "./eCarWheel.h"

eCarPiece CarWheelToCarPiece(eCarWheel i) {
    switch ((eCarWheel)i) {
    case CAR_WHEEL_FRONT_LEFT:
        return CAR_PIECE_WHEEL_LF;
    case CAR_WHEEL_REAR_LEFT:
        return CAR_PIECE_WHEEL_RL;
    case CAR_WHEEL_FRONT_RIGHT:
        return CAR_PIECE_WHEEL_RF;
    case CAR_WHEEL_REAR_RIGHT:
        return CAR_PIECE_WHEEL_RR;
    default:
        NOTSA_UNREACHABLE();
    }
}
