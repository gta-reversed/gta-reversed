#pragma once

enum eControllerType {
    KEYBOARD,
    OPTIONAL_EXTRA_KEY,
    MOUSE,
    JOY_STICK,


    CONTROLLER_NUM
};

constexpr eControllerType CONTROLLER_TYPES_ALL[CONTROLLER_NUM] = { KEYBOARD, OPTIONAL_EXTRA_KEY, MOUSE, JOY_STICK };
constexpr eControllerType CONTROLLER_TYPES_KEYBOARD[]          = { KEYBOARD, OPTIONAL_EXTRA_KEY };
