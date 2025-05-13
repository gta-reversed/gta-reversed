#pragma once

#include <concepts>

//! Fixed point number (With implicit conversion to float)
template<std::integral T, float CompressValue, bool UseRoundingWhenConverting = false>
class FixedFloat {
public:
    //! Construct as 0
    constexpr FixedFloat() = default;

    //! Construct from an uncompressed value
    constexpr FixedFloat(float v) { Set(v, UseRoundingWhenConverting); }

    //! Construct from a pre-compressed value
    //! Be (very) careful when using this constructor, as mixing up this and the float version will cause bugs...
    // TODO: Maybe use tag-based constructors, because this is kinda bug-prone....
    template<std::integral Y>
    explicit constexpr FixedFloat(Y x) : value(x) {}

    //! Construct from another `FixedFloat` with the same CompressValue
    template<std::integral Y>
    constexpr FixedFloat(FixedFloat<Y, CompressValue> x) : value(x.value) {}

    //! Construct from another `FixedFloat` with a different CompressValue
    template<std::integral Y, float CV>
        requires (CV != CompressValue)
    constexpr FixedFloat(FixedFloat<Y, CV> x) : FixedFloat{(float)x} {}

    constexpr operator float() const { return static_cast<float>(value) / CompressValue; }

    //! Set the value (Use this if you want to set using rounding)
    constexpr void Set(float v, bool round = UseRoundingWhenConverting) {
        value = round ? static_cast<T>(v * CompressValue + 0.5f) : static_cast<T>(v * CompressValue);
    }

    // Implementations of (basic) arithmetic ops. Necessary to avoid unnecessary int <=> float conversions
    // (That would otherwise occur when doing arithmetic between 2 FixedFloat instances)
#define IMPLEMENT_FIXED_OPERATOR(_op) \
    template<typename T2, typename CommonT = std::common_type_t<T, T2>> \
    constexpr auto operator _op(FixedFloat<T2, CompressValue> o) { \
        return FixedFloat<CommonT, CompressValue>{static_cast<CommonT>(this->value) _op static_cast<CommonT>(o.value)}; \
    }

    IMPLEMENT_FIXED_OPERATOR(+);
    IMPLEMENT_FIXED_OPERATOR(-);
    IMPLEMENT_FIXED_OPERATOR(/);
    IMPLEMENT_FIXED_OPERATOR(*);
    
private:
    T value{};
};
