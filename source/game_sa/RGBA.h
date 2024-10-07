/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

struct RwRGBA;

class CRGBA {
public:
    uint8 r{}, g{}, b{}, a{};

public:
    CRGBA() = default;
    CRGBA(const CRGBA& rhs) = default;

    CRGBA(uint8 red, uint8 green, uint8 blue) { Set(red, green, blue); }
    CRGBA(uint8 red, uint8 green, uint8 blue, uint8 alpha){ Set(red, green, blue, alpha); }
    CRGBA(uint32 intValue) { Set(intValue); }
    CRGBA(const RwRGBA& rhs) { Set(rhs); }
    CRGBA(const CRGBA& rhs, uint8 alpha) { Set(rhs, alpha); };
    explicit CRGBA(RwRGBAReal rgba);

    void Set(uint8 red, uint8 green, uint8 blue);
    void Set(uint8 red, uint8 green, uint8 blue, uint8 alpha);
    void Set(uint32 intValue);
    void Set(const CRGBA& rhs);
    void Set(const CRGBA& rhs, uint8 alpha);
    void Set(const RwRGBA& rwcolor);

    CRGBA  ToRGB() const;
    uint32 ToInt() const;
    uint32 ToIntARGB() const;
    RwRGBA ToRwRGBA() const;

    void FromRwRGBA(const RwRGBA& rwcolor);
    void FromARGB(uint32 intValue);
    CRGBA FromInt32(int32 red, int32 green, int32 blue, int32 alpha);

    void  Invert();
    CRGBA Inverted() const;

    bool   operator==(const CRGBA& rhs) const;
    CRGBA& operator=(const CRGBA& rhs);

    CRGBA operator*(float mult) const {
        return {
            (uint8)((float)r * mult),
            (uint8)((float)g * mult),
            (uint8)((float)b * mult),
            (uint8)((float)a * mult)
        };
    }

    CRGBA operator/(float divisor) {
        return *this * (1 / divisor);
    }

    CRGBA& operator*=(float mult) {
        *this = *this * mult;
        return *this;
    }

    operator RwRGBAReal() { return { (RwReal)r / 255.f, (RwReal)g / 255.f, (RwReal)b / 255.f, (RwReal)a / 255.f }; }
    operator RwRGBA()     { return { r, g, b, a }; }

    // NOTSA
    uint8 operator[](size_t i) const {
        return (&r)[i];
    }

    uint8& operator[](size_t i) {
        return (&r)[i];
    }
};
