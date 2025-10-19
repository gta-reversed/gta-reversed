#pragma once

enum class eControllerType {
    KEYBOARD,
    OPTIONAL_EXTRA_KEY,
    MOUSE,
    JOY_STICK,

    CONTROLLER_TYPES_COUNT
};

constexpr eControllerType CONTROLLER_TYPES_ALL[] = {
    eControllerType::KEYBOARD,
    eControllerType::OPTIONAL_EXTRA_KEY,
    eControllerType::MOUSE,
    eControllerType::JOY_STICK
};

constexpr eControllerType CONTROLLER_TYPES_KEYBOARD[] = {
    eControllerType::KEYBOARD,
    eControllerType::OPTIONAL_EXTRA_KEY
};
