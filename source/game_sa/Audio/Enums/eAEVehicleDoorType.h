#pragma once

#include <optional>
#include <Base.h>

enum class eAEVehicleDoorType : int8 {
    UNSET = -1,
    LIGHT = 0,
    OLD   = 1,
    NEW   = 2,
    TRUCK = 3,
    VAN   = 4,
};
inline std::optional<const char*> EnumToString(eAEVehicleDoorType v) {
#define ENUM_CASE(_e) case _e: #_e
    using enum eAEVehicleDoorType;
    switch (v) {
    ENUM_CASE(UNSET);
    ENUM_CASE(LIGHT);
    ENUM_CASE(OLD);
    ENUM_CASE(NEW);
    ENUM_CASE(TRUCK);
    ENUM_CASE(VAN);
    }
    return std::nullopt;
#undef ENUM_CASE
}
