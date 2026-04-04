#pragma once

enum eHeliNodes {
    HELI_NODE_NONE     = 0,
    HELI_CHASSIS       = 1,
    HELI_WHEEL_RF      = 2,
    HELI_WHEEL_RM      = 3,
    HELI_WHEEL_RB      = 4,
    HELI_WHEEL_LF      = 5,
    HELI_WHEEL_LM      = 6,
    HELI_WHEEL_LB      = 7,
    HELI_DOOR_RF       = 8,
    HELI_DOOR_RR       = 9,
    HELI_DOOR_LF       = 10,
    HELI_DOOR_LR       = 11,
    HELI_STATIC_ROTOR  = 12,
    HELI_MOVING_ROTOR  = 13,
    HELI_STATIC_ROTOR2 = 14,
    HELI_MOVING_ROTOR2 = 15,
    HELI_RUDDER        = 16,
    HELI_ELEVATORS     = 17,
    HELI_MISC_A        = 18,
    HELI_MISC_B        = 19,
    HELI_MISC_C        = 20,
    HELI_MISC_D        = 21,

    HELI_NUM_NODES
};
