#pragma once

enum eCarNodes {
    CAR_NODE_NONE  = 0,
    CAR_CHASSIS    = 1,
    CAR_WHEEL_RF   = 2,
    CAR_WHEEL_RM   = 3,
    CAR_WHEEL_RB   = 4,
    CAR_WHEEL_LF   = 5,
    CAR_WHEEL_LM   = 6,
    CAR_WHEEL_LB   = 7,
    CAR_DOOR_RF    = 8,  // Right Front
    CAR_DOOR_RR    = 9,  // Right Rear
    CAR_DOOR_LF    = 10, // Left Front
    CAR_DOOR_LR    = 11, // Left Rear
    CAR_BUMP_FRONT = 12,
    CAR_BUMP_REAR  = 13,
    CAR_WING_RF    = 14,
    CAR_WING_LF    = 15,
    CAR_BONNET     = 16,
    CAR_BOOT       = 17,
    CAR_WINDSCREEN = 18,
    CAR_EXHAUST    = 19,
    CAR_MISC_A     = 20,
    CAR_MISC_B     = 21,
    CAR_MISC_C     = 22,
    CAR_MISC_D     = 23,
    CAR_MISC_E     = 24,

    CAR_NUM_NODES
};
