    #pragma once

    #include <optional>
    #include <Base.h>

enum class eBassSetting : int8 {
    NORMAL = 0, // 0x0
    BOOST  = 1, // 0x1
    CUT    = 2, // 0x2

    // Keep at the bottom
    NUM,
};
inline std::optional<const char*> EnumToString(eBassSetting v) {
#define ENUM_CASE(_e) case _e: #_e
    using enum eBassSetting;
    switch (v) {
    ENUM_CASE(NORMAL);
    ENUM_CASE(BOOST);
    ENUM_CASE(CUT);
    }
    return std::nullopt;
#undef ENUM_CASE
}
