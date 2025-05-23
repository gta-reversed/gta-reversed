#pragma once

enum class eControllerType {
    KEYBOARD,
    OPTIONAL_EXTRA_KEY,
    MOUSE,
    JOY_STICK
};

constexpr auto CONTROLLER_TYPES_COUNT = 4;

constexpr eControllerType CONTROLLER_TYPES_ALL[CONTROLLER_TYPES_COUNT] = { eControllerType::KEYBOARD, eControllerType::OPTIONAL_EXTRA_KEY, eControllerType::MOUSE, eControllerType::JOY_STICK };
constexpr eControllerType CONTROLLER_TYPES_KEYBOARD[]                  = { eControllerType::KEYBOARD, eControllerType::OPTIONAL_EXTRA_KEY };
