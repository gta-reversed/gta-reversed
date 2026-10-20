#pragma once

// NOTSA
enum class eFontTextureStyle : uint8 {
    SUBTITLES_AND_GOTHIC = 0,
    PRICEDOWN            = 1,
    MENU                 = 2,
};

enum eExtraFontSymbol : uint8 {
    EXSYMBOL_NONE       = 0, // invalid
    EXSYMBOL_DPAD_UP    = 1,
    EXSYMBOL_DPAD_DOWN  = 2,
    EXSYMBOL_DPAD_LEFT  = 3,
    EXSYMBOL_DPAD_RIGHT = 4,
    EXSYMBOL_CROSS      = 5,
    EXSYMBOL_CIRCLE     = 6,
    EXSYMBOL_SQUARE     = 7,
    EXSYMBOL_TRIANGLE   = 8,
    EXSYMBOL_KEY        = 9, // followed by buttons, L1?
    EXSYMBOL_L2         = 10,
    EXSYMBOL_L3         = 11,
    EXSYMBOL_R1         = 12,
    EXSYMBOL_R2         = 13,
    EXSYMBOL_R3         = 14
};

enum eFontStyle : uint8 {
    FONT_GOTHIC,
    FONT_SUBTITLES,
    FONT_MENU,
    FONT_PRICEDOWN
};

NOTSA_WENUM_DEFS_FOR(eFontStyle);

// NOTSA -- refer to MODELS/FONTS.TXD
enum class eFontTextureName : uint8 {
    FONT2      = 0, // Gothic and Menu
    FONT1      = 1, // Subtitles and Pricedown
    FONT_UNK_2 = 2,
};
NOTSA_WENUM_DEFS_FOR(eFontTextureName);

struct CFontChar {
    uint8             CLetter;
    uint8             DLetter;
    CVector2D         Pos;
    float             Width;
    float             Height;
    CRGBA             Color;
    float             Wrap;
    float             Slope;
    CVector2D         SlopeRef;
    bool              ContainImages;
    eFontTextureStyle FontStyle;
    bool              PropOn;
    uint16            FontTexture;
    uint8             Outline;

public:
    // NOTSA
    GxtChar* GetText() { return reinterpret_cast<GxtChar*>(this + 1); }

    void Set(const CFontChar* setup) { *this = *setup; } // 0x718E50
};
VALIDATE_SIZE(CFontChar, 0x30);

struct tFontData {
    std::array<uint8, 208> PropValues;
    uint8                  SpaceValue;
    uint8                  UnpropValue;

    // NOTSA/FIX_BUGS
    // The index can be past `m_propValues` (e.g. `CFont::FindSubFontCharacter` returns 208 for '!' with `FONT_PRICEDOWN`),
    // the original then reads whatever follows (`m_spaceValue`, `m_unpropValue`, the next font's data)
    uint8 GetPropValue(uint8 idx) const {
        switch (idx) {
        case 208:
            return SpaceValue;
        case 209:
            return UnpropValue;
        default:
            break;
        }
        return PropValues[idx];
    }
};

// initialize values from CFont::Initialise
struct CFontDetails {
    CRGBA            Color{ 255, 255, 255, 0 };
    CVector2D        Scale{ 1.0f, 1.0f };
    float            Slope{ 0.0f };
    CVector2D        SlopeRef{ SCREEN_WIDTH, 0.0f };
    bool             Justify{ false };
    bool             Centre{ false };
    bool             RightJustify{ false };
    bool             Background{ false };
    bool             BackgroundOutline{ false };
    bool             Proportional{ true };
    bool             Shadow{ /*?*/ };
    float            AlphaFade{ 255.0f };
    CRGBA            BackgroundColor{ 128, 128, 128, 128 };
    float            WrapEnd{ SCREEN_WIDTH };
    float            CentreX{ SCREEN_WIDTH };
    float            RightJustifyWrap{ 0.0f };
    eFontTextureName FontTextureName{ eFontTextureName::FONT2 }; // The 'style' -- refer to the enum type

    // This looks like it's about the "index" of the font in the texture image. (there are two for each font1/2)
    // Both subtitles and gothic (both are the first font in the images) have 0,
    // Logically pricedown and menu would have 1 but pricedown is 1 and menu is 2.
    // I guess they wanted to differentiate fonts to check in `FindSubFontCharacter`. Not the brightest choice.
    eFontTextureStyle FontTextureStyle{ eFontTextureStyle::SUBTITLES_AND_GOTHIC }; // The 'extra font'

    uint8 DropAmount{ 0 };
    CRGBA DropColor{ /*uninit*/ };
    uint8 EdgeAmount{ 0 };
    uint8 EdgeSpace{ /*uninit*/ };
};
VALIDATE_SIZE(CFontDetails, 0x40);
VALIDATE_OFFSET(CFontDetails, Color, 0x0);
VALIDATE_OFFSET(CFontDetails, Scale, 0x4);
VALIDATE_OFFSET(CFontDetails, Slope, 0xC);
VALIDATE_OFFSET(CFontDetails, SlopeRef, 0x10);
VALIDATE_OFFSET(CFontDetails, Justify, 0x18);
VALIDATE_OFFSET(CFontDetails, Centre, 0x19);
VALIDATE_OFFSET(CFontDetails, RightJustify, 0x1A);
VALIDATE_OFFSET(CFontDetails, Background, 0x1B);
VALIDATE_OFFSET(CFontDetails, BackgroundOutline, 0x1C);
VALIDATE_OFFSET(CFontDetails, Proportional, 0x1D);
VALIDATE_OFFSET(CFontDetails, Shadow, 0x1E);
VALIDATE_OFFSET(CFontDetails, AlphaFade, 0x20);
VALIDATE_OFFSET(CFontDetails, BackgroundColor, 0x24);
VALIDATE_OFFSET(CFontDetails, WrapEnd, 0x28);
VALIDATE_OFFSET(CFontDetails, CentreX, 0x2C);
VALIDATE_OFFSET(CFontDetails, RightJustifyWrap, 0x30);
VALIDATE_OFFSET(CFontDetails, FontTextureName, 0x34);
VALIDATE_OFFSET(CFontDetails, FontTextureStyle, 0x35);
VALIDATE_OFFSET(CFontDetails, DropAmount, 0x36);
VALIDATE_OFFSET(CFontDetails, DropColor, 0x37);
VALIDATE_OFFSET(CFontDetails, EdgeAmount, 0x3B);
VALIDATE_OFFSET(CFontDetails, EdgeSpace, 0x3C);
