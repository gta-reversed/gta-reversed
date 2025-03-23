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
    uint8 r{}, g{}, b{}, a{}; // msb(a) -> lsb(r)

public:
    constexpr CRGBA() = default;
    constexpr CRGBA(const CRGBA& rhs) = default;

    //! From, r, g, b, a components
    constexpr CRGBA(uint8 r, uint8 g, uint8 b, uint8 a = 0x0) :
        r{r},
        g{g},
        b{b},
        a{a}
    {
    }

    //! From a packed RGBA value (msb(r) => lsb(a))
    constexpr CRGBA(uint32 rgba) :
        CRGBA{
            (uint8)((rgba >> 24) & 0xFF),
            (uint8)((rgba >> 16) & 0xFF),
            (uint8)((rgba >> 8 ) & 0xFF),
            (uint8)((rgba >> 0 ) & 0xFF),
        }
    {
    }

    //! From an RW color
    constexpr CRGBA(const RwRGBA& rgba) :
        CRGBA{
            rgba.red,
            rgba.green,
            rgba.blue,
            rgba.alpha
        }
    {
    }

    //! From float RW color
    constexpr CRGBA(RwRGBAReal rgba) :
        CRGBA{
            (uint8)(rgba.red * 255.f),
            (uint8)(rgba.green * 255.f),
            (uint8)(rgba.blue * 255.f),
            (uint8)(rgba.alpha * 255.f)
        }
    {
    }

    // TODO: Replace this with a member function returning a new instance, like `WithAlpha()` (`fs::path` has for ex. `with_stem` too)
    //! Only copy rgb, replace alpha
    constexpr CRGBA(const CRGBA& rgb, uint8 alpha) :
        CRGBA{
            rgb.r,
            rgb.g,
            rgb.b,
            alpha
        }
    {
    }

    //! From an RW color
    constexpr void FromRwRGBA(const RwRGBA& rgba) {
        *this = CRGBA{rgba};
    }

    //! From a packed ARGB value (msb(a) => lsb(b))
    constexpr void FromARGB(uint32 argb) {
        *this = CRGBA{
            (uint8)((argb >> 16) & 0xFF),
            (uint8)((argb >> 8 ) & 0xFF),
            (uint8)((argb >> 0 ) & 0xFF),
            (uint8)((argb >> 24) & 0xFF)
        };
    }

    // TODO: Remove this...
    constexpr CRGBA FromInt32(int32 red, int32 green, int32 blue, int32 alpha) {
        return {
            static_cast<uint8>(red),
            static_cast<uint8>(green),
            static_cast<uint8>(blue),
            static_cast<uint8>(alpha),
        };
    }

    // TODO: Get rid of these (Replace with assignment instead)
    constexpr void Set(uint8 red, uint8 green, uint8 blue)              { *this = CRGBA{ red, green, blue, a }; }
    constexpr void Set(uint8 red, uint8 green, uint8 blue, uint8 alpha) { *this = CRGBA{ red, green, blue, alpha }; }
    constexpr void Set(uint32 rgba)                                     { *this = CRGBA{rgba}; }
    constexpr void Set(const CRGBA& rhs)                                { *this = rhs; }
    constexpr void Set(const CRGBA& rgb, uint8 alpha)                   { *this = CRGBA{rgb, alpha}; }
    constexpr void Set(const RwRGBA& rgba)                              { *this = CRGBA{rgba}; }

    constexpr uint32 ToIntRGB() const {
        return (r << 16)
             | (g << 8)
             | (b << 0);
    }

    // TODO: Rename to `ToIntRGBA`
    //! To U32 in format RGBA (msb(r) -> lsb(a))
    constexpr uint32 ToInt() const {
        return (ToIntRGB() << 8) | (a << 0);
    }

    //! To U32 in format ARGB (msb(a) -> lsb(b))
    constexpr uint32 ToIntARGB() const {
        return (a << 24) | ToIntRGB();
    }

    //! To U32 in format ABGR (msb(a) -> lsb(r)) [For ImGui]
    constexpr uint32 ToIntABGR() const {
        return (a << 24)
             | (b << 16)
             | (g << 8)
             | (r << 0);
    }

    //! To RW color
    constexpr RwRGBA ToRwRGBA() const {
        return { r, g, b, a };
    }

    // TODO: Replace this with `WithAlpha()` (or similar)
    //! To CRGBA with alpha set to 255
    constexpr CRGBA ToRGB() const {
        return CRGBA(r, g, b, 255);
    }

    //! This RGB value with different alpha
    CRGBA WithAlpha(uint8 newAlpha) const {
        return { r, g, b, newAlpha };
    }

    constexpr CRGBA Inverted() const {
        return {
            (uint8)(255u - r),
            (uint8)(255u - g),
            (uint8)(255u - b),
            (uint8)(a)
        };
    }

    constexpr void Invert() { // TODO: Get rid of this
        *this = Inverted();
    }

    constexpr friend bool operator==(const CRGBA&, const CRGBA&) = default;
    constexpr friend bool operator!=(const CRGBA&, const CRGBA&) = default;

    constexpr CRGBA& operator=(const CRGBA&) = default;

    constexpr CRGBA operator*(float m) const {
        return {
            (uint8)((float)r * m),
            (uint8)((float)g * m),
            (uint8)((float)b * m),
            (uint8)((float)a * m)
        };
    }

    constexpr CRGBA operator/(float divisor) {
        return *this * (1 / divisor);
    }

    constexpr CRGBA& operator*=(float m) {
        *this = *this * m;
        return *this;
    }

    constexpr operator RwRGBAReal() { return { (RwReal)r / 255.f, (RwReal)g / 255.f, (RwReal)b / 255.f, (RwReal)a / 255.f }; }
    constexpr operator RwRGBA()     { return { r, g, b, a }; }

    constexpr uint8 operator[](size_t i) const {
        return (&r)[i];
    }

    constexpr uint8& operator[](size_t i) {
        return (&r)[i];
    }

    // Same as operator* but doesn't touch the alpha.
    CRGBA& ScaleRGB(float mult) {
        r = (uint8)((float)r * mult);
        g = (uint8)((float)g * mult);
        b = (uint8)((float)b * mult);

        return *this;
    }
};
