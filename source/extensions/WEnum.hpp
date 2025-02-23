#pragma once

#include <type_traits>
#include <cstdint>

namespace notsa {
/*!
* Store enums (of type `Enum`) in a given integral type.
* Useful in cases where the class originally used a width
* different to the enums underlaying type.
* WEnum just sounded like a cool name... Venom.. WEnum, idk 
*/
template<typename Enum, typename StoreAs>
    requires(std::is_integral_v<StoreAs>)
struct WEnum {
    StoreAs m_Value;

    //! Implicitly convert from the enum value
    constexpr WEnum(Enum e = {}) noexcept : m_Value{static_cast<StoreAs>(e)} { }

    //! Implicitly convert from another WEnum for the same enum
    template<typename Y>
    constexpr WEnum(WEnum<Enum, Y> other) noexcept : WEnum{static_cast<Enum>(other)} { }

    //! Implicitly convert back to the underlaying `Enum` type
    constexpr operator Enum() const noexcept { return static_cast<Enum>(m_Value); }

    //! Implicitly cast to underlaying type ref, pointer
    constexpr explicit operator StoreAs*() noexcept { return &m_Value; }
    constexpr explicit operator StoreAs&() noexcept { return m_Value; }

    //! Use this in cases you want to cast to an int (for cout or something)
    constexpr Enum get() const noexcept { return static_cast<Enum>(m_Value); }

    //! Get the underlaying value
    constexpr StoreAs get_underlying() const noexcept { return m_Value; }
// std::format support for `WEnum`
template<typename Enum, typename StoreAs>
struct std::formatter<WEnum<Enum, StoreAs>> : std::formatter<std::string> {
    auto format(auto e, format_context& ctx) const {
        if constexpr (requires { EnumToString(e.get()); }) {
            if (const auto name = EnumToString(e.get())) {
                return formatter<string>::format(*name, ctx);
            }
        }
        return formatter<string>::format(std::format("{} ({})", typeid(Enum).name(), e.get_underlying()), ctx);
    }
};

template<typename E>
using WEnumU8 = WEnum<E, std::uint8_t>;
template<typename E>
using WEnumS8 = WEnum<E, std::int8_t>;

template<typename E>
using WEnumU16 = WEnum<E, std::uint16_t>;
template<typename E>
using WEnumS16 = WEnum<E, std::int16_t>;

template<typename E>
using WEnumU32 = WEnum<E, std::uint32_t>;
template<typename E>
using WEnumS32 = WEnum<E, std::int32_t>;
};

#define NOTSA_WENUM_DEFS_FOR(_e) \
    using _e##S8 = notsa::WEnumS8<_e>; \
    using _e##S16 = notsa::WEnumS16<_e>; \
    using _e##S32 = notsa::WEnumS32<_e>;

//! Use unary operator + instead of ugly `static_cast` for enum (class) for casting to underlaying type
template <typename T>
    requires std::is_enum_v<T>
inline constexpr auto operator+(T e) noexcept {
    return static_cast<std::underlying_type_t<T>>(e);
}
