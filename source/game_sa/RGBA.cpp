/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "RGBA.h"

CRGBA::CRGBA(RwRGBAReal rgba) :
    CRGBA{
        (uint8)(rgba.red * 255.f),
        (uint8)(rgba.green * 255.f),
        (uint8)(rgba.blue * 255.f),
        (uint8)(rgba.alpha * 255.f)
    }
{
}

void CRGBA::Set(uint8 red, uint8 green, uint8 blue) {
    r = red;
    g = green;
    b = blue;
}

void CRGBA::Set(uint8 red, uint8 green, uint8 blue, uint8 alpha) {
    Set(red, green, blue);
    a = alpha;
}

void CRGBA::Set(uint32 intValue) {
    r = (uint8)(intValue >> 24);
    g = (uint8)(intValue >> 16);
    b = (uint8)(intValue >> 8 );
    a = (uint8)(intValue >> 0 );
}

void CRGBA::Set(const CRGBA& rhs) {
    Set(rhs.r, rhs.g, rhs.b, rhs.a);
}

void CRGBA::Set(const CRGBA& rhs, uint8 alpha) {
    Set(rhs.r, rhs.g, rhs.b, alpha);
}

void CRGBA::Set(const RwRGBA& rwcolor) {
    Set(rwcolor.red, rwcolor.green, rwcolor.blue, rwcolor.alpha);
}

uint32 CRGBA::ToIntRGBA() const { // RGBA (msb(r) -> lsb(a))
    return (ToIntRGB() << 8) | ((uint32)a << 0);
}

uint32 CRGBA::ToIntARGB() const {
    return ((uint32)a << 24) | ToIntRGB();
}

RwRGBA CRGBA::ToRwRGBA() const {
    return { r, g, b, a };
}

void CRGBA::FromRwRGBA(const RwRGBA& rwcolor) {
    Set(rwcolor);
}

void CRGBA::FromARGB(uint32 intValue) {
    a = (intValue >> 24) & 0xFF;
    r = (intValue >> 16) & 0xFF;
    g = (intValue >> 8) & 0xFF;
    b = intValue & 0xFF;
}

CRGBA CRGBA::FromInt32(int32 red, int32 green, int32 blue, int32 alpha) {
    return {
        static_cast<uint8>(red),
        static_cast<uint8>(green),
        static_cast<uint8>(blue),
        static_cast<uint8>(alpha),
    };
}

void CRGBA::Invert() {
    Set(255 - r, 255 - g, 255 - b);
}

CRGBA CRGBA::Inverted() const {
    CRGBA invertedColor = *this;
    invertedColor.Invert();
    return invertedColor;
}

bool CRGBA::operator==(const CRGBA& rhs) const {
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
}

CRGBA& CRGBA::operator=(const CRGBA& rhs) {
    Set(rhs);
    return *this;
}

CRGBA CRGBA::ToRGB() const {
    return CRGBA(r, g, b, 255);
}
