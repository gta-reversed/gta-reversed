#pragma once

#include <concepts>

//! Fixed point number (With implicit conversion to float)
template<std::integral T, float CompressValue>
class FixedFloat {
public:
    constexpr FixedFloat() = default;
    constexpr FixedFloat(float v) : value(static_cast<T>(v * CompressValue)) {}
    template<std::integral Y>
    constexpr FixedFloat(Y x) : value(x) {} // TODO: Remove this, its very bug-prone

    constexpr operator float() const { return static_cast<float>(value) / CompressValue; }

    void Set(float v, bool round) { value = round ? static_cast<T>(v * CompressValue + 0.5f) : static_cast<T>(v * CompressValue); }
    //void SetRaw(std::integral v) { value = v; }
    //float Get(bool round) const { return round ? (float)v * CompressValue + 0.5f; }

    // I'm not ready for this
    //friend FixedFloat operator+(const FixedFloat& a, const FixedFloat& b) { return a.value + b.value; }
    //friend FixedFloat operator-(const FixedFloat& a, const FixedFloat& b) { return a.value - b.value; }
    //friend FixedFloat operator*(const FixedFloat& a, const FixedFloat& b) { return a.value * b.value; }
    //friend FixedFloat operator/(const FixedFloat& a, const FixedFloat& b) { return a.value / b.value; }
private:
    T value{};
};
