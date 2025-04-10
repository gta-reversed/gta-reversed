#pragma once

#include <type_traits>
#include <cstdint>

namespace notsa {
//! Store enums (of type `Enum`) in a given integral type.
//! Useful in cases where the class originally used a width
//! different to the enums underlaying type.
//! WEnum just sounded like a cool name... Venom.. WEnum, idk
template<typename Enum, typename StoreAs>
    requires(std::is_integral_v<StoreAs>)
struct WEnum {
    StoreAs m_Value;

    //! Implicitly convert from the enum value
    WEnum(Enum e = {}) : m_Value{static_cast<StoreAs>(e)} { }

    //! Implicitly convert from another WEnum for the same enum
    template<typename Y>
    WEnum(WEnum<Enum, Y> other) : WEnum{static_cast<Enum>(other)} { }

    //! Implicitly convert back to the underlaying `Enum` type
    operator Enum() const { return static_cast<Enum>(m_Value); }

    //! Implicitly cast to underlaying type ref, pointer
    explicit operator StoreAs*() { return &m_Value; }
    explicit operator StoreAs&() { return m_Value; }

    //! Use this in cases you want to cast to an int (for cout or something)
    Enum get() const { return static_cast<Enum>(m_Value); }

    //! Get the underlaying value
    StoreAs get_underlying() const { return m_Value; }
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

#define NOTSA_WENUM_DEFS_FOR(_e) \
    using _e##S8 = notsa::WEnumS8<_e>; \
    using _e##S16 = notsa::WEnumS16<_e>; \
    using _e##S32 = notsa::WEnumS32<_e>;
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
