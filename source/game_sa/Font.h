/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "GxtChar.h"
#include "RGBA.h"
#include "eFontAlignment.h"
#include <extensions/WEnum.hpp>

class CSprite2d;

// TODO: put these font "specifiers" into a different header

// NOTSA -- retarded
enum class eFontTextureStyle : uint8 {
    SUBTITLES_AND_GOTHIC = 0,
    PRICEDOWN = 1,
    MENU = 2,
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

// todo: move
struct CFontChar {
    uint8               m_cLetter;
    uint8               m_dLetter;
    CVector2D           m_vPosn;
    float               m_fWidth;
    float               m_fHeight;
    CRGBA               m_color;
    float               m_fWrap;
    float               Slope;
    CVector2D           SlopeRef;
    bool                m_bContainImages;
    eFontTextureStyle   m_nFontStyle;
    bool                m_bPropOn;
    uint16              m_wFontTexture;
    uint8               m_nOutline;

public:
    // NOTSA
    GxtChar* GetText() { return reinterpret_cast<GxtChar*>(this + 1); }

    void Set(const CFontChar* setup) { *this = *setup; } // 0x718E50
};
VALIDATE_SIZE(CFontChar, 0x30);

struct tFontData {
    std::array<uint8, 208> m_propValues;
    uint8 m_spaceValue;
    uint8 m_unpropValue;
};

// initialize values from CFont::Initialise
struct CFontDetails {
    CRGBA Color{255, 255, 255, 0};
    CVector2D Scale{1.0f, 1.0f};
    float Slope{0.0f};
    CVector2D SlopeRef{SCREEN_WIDTH, 0.0f};
    bool Justify{false};
    bool Centre{false};
    bool RightJustify{false};
    bool Background{false};
    bool BackgroundOutline{false};
    bool Proportional{true};
    bool Shadow{/*?*/};
    float AlphaFade{255.0f};
    CRGBA BackgroundColor{128, 128, 128, 128};
    float WrapEnd{SCREEN_WIDTH};
    float CentreX{SCREEN_WIDTH};
    float RightJustifyWrap{0.0f};
    eFontTextureName FontTextureName{eFontTextureName::FONT2}; // The 'style' -- refer to the enum type

    // This looks like it's about the "index" of the font in the texture image. (there are two for each font1/2)
    // Both subtitles and gothic (both are the first font in the images) have 0,
    // Logically pricedown and menu would have 1 but pricedown is 1 and menu is 2.
    // I guess they wanted to differentiate fonts to check in `FindSubFontCharacter`. Not the brightest choice.
    eFontTextureStyle FontTextureStyle{eFontTextureStyle::SUBTITLES_AND_GOTHIC}; // The 'extra font'

    uint8 DropAmount{0};
    CRGBA DropColor{/*uninit*/};
    uint8 EdgeAmount{0};
    uint8 EdgeSpace{/*uninit*/};
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

class CFont {
public:
    static void Initialise();
    static void LoadFontValues();
    static void Shutdown();
    static void PrintChar(float x, float y, char character);
    // Get next ' ' character in a string
    static char* GetNextSpace(char* string);
    static const GxtChar* ParseToken(const GxtChar* text, CRGBA& color, bool isBlip, GxtChar* tag);
    static void InitPerFrame();
    static float GetHeight(bool a1 = false);
    static float GetStringWidth(const GxtChar* string, bool full, bool scriptText);

    // STYLING
    static void SetScaleForCurrentLanguage(float w, float h);
    static void SetColor(CRGBA color);
    static void SetFontStyle(eFontStyle style);
    static void SetDropColor(CRGBA color);

    static void SetScale(float w, float h) { m_Details.Scale.Set(w, h); } // 0x719380
    static void SetSlantRefPoint(float x, float y) { m_Details.SlopeRef.Set(x, y); } // Set text rotation point (0x719400)
    static void SetSlant(float value) { m_Details.Slope = value; } // Set text rotation angle (0x719420)
    static void SetWrapx(float value) { m_Details.WrapEnd = value; } // Set line width at right (0x7194D0)
    static void SetCentreSize(float value) { m_Details.CentreX = value; } // Set line width at center (0x7194E0)
    static void SetRightJustifyWrap(float value) { m_Details.RightJustifyWrap = value; } // 0x7194F0
    static void SetAlphaFade(float alpha) { m_Details.AlphaFade = alpha; } // Like a 'global' font alpha, multiplied with each text alpha (from SetColor) (0x719500)
    static void SetProportional(bool on) { m_Details.Proportional = on; } // Toggles character proportions in text (0x7195B0)
    static void SetBackgroundColor(CRGBA color) { m_Details.BackgroundColor = color; } // Sets background color (0x7195E0)
    static void SetJustify(bool on) { m_Details.Justify = on; } // 0x719600

   // Set shadow size (0x719570)
    static void SetDropShadowPosition(uint8 value) {
        m_Details.EdgeAmount = m_Details.EdgeSpace = 0;
        m_Details.DropAmount = value;
    }

    // Set outline size (0x719590)
    static void SetEdge(int8 value) {
        m_Details.DropAmount = 0;
        m_Details.EdgeAmount = m_Details.EdgeSpace = value;
    }

    // Setups text background (0x7195C0)
    static void SetBackground(bool enable, bool includeWrap) {
        m_Details.Background        = enable;
        m_Details.BackgroundOutline = includeWrap;
    }

    // 0x719610
    static void SetOrientation(eFontAlignment alignment) {
        m_Details.Centre       = alignment == eFontAlignment::ALIGN_CENTER;
        m_Details.RightJustify = alignment == eFontAlignment::ALIGN_RIGHT;
    }

    // RENDERING

    // 0x71A5E0
    static int16 GetNumberLines(float x, float y, const GxtChar* text) {
        return ProcessCurrentString(false, x, y, text);
    }

    // 0x71A600
    static int16 ProcessStringToDisplay(float x, float y, const GxtChar* text) {
        return ProcessCurrentString(true, x, y, text);
    }

    static void RenderFontBuffer();
    static void DrawFonts();
    static int16 ProcessCurrentString(bool print, float x, float y, const GxtChar* text);
    static void GetTextRect(CRect& rect, float x, float y, const GxtChar* text);
    static void PrintString(float x, float y, const GxtChar* start, const GxtChar* end, float wrap);
    static void PrintString(float x, float y, const GxtChar* text);
    static void PrintStringFromBottom(float x, float y, const GxtChar* text);
    static float GetCharacterSize(uint8 ch);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

private:
    friend float GetLetterIdPropValue(uint8);

    static constexpr size_t MAX_FONT_SPRITES = 2;
    static constexpr size_t MAX_FONT_BUTTON_SPRITES = 15;

    static inline auto& RenderState = StaticRef<CFontChar>(0xC71AA0);
    // font textures array
    static inline auto& Sprite = StaticRef<CSprite2d[MAX_FONT_SPRITES]>(0xC71AD0);
    // button textures array
    static inline auto& ButtonSprite = StaticRef<CSprite2d[MAX_FONT_BUTTON_SPRITES]>(0xC71AD8);
    static inline auto& PS2Symbol = StaticRef<eExtraFontSymbol>(0xC71A54);
    static inline auto& m_bNewLine = StaticRef<bool>(0xC71A55);

    static inline auto& m_Details = StaticRef<CFontDetails>(0xC71A60);
};

static void ReadFontsDat();
static float GetScriptLetterSize(uint8 letterId);
static float GetLetterIdPropValue(uint8 letterId);
static uint8 FindSubFontCharacter(uint8 letterId, eFontTextureStyle fontStyle);
