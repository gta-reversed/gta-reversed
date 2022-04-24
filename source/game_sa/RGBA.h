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
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 a;

    CRGBA(uint8 red, uint8 green, uint8 blue);
    CRGBA(uint8 red, uint8 green, uint8 blue, uint8 alpha);
    CRGBA(const CRGBA& rhs);
    CRGBA(uint32 intValue);
    CRGBA(const RwRGBA& rhs);
    CRGBA();

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
    }
};
