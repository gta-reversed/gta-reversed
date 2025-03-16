/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RGBA.h"
#include "eFontAlignment.h"

class CSprite2d;


// todo: move
struct CFontChar {
    uint8     m_cLetter;
    uint8     m_dLetter;
    CVector2D m_vPosn;
    float     m_fWidth;
    float     m_fHeight;
    CRGBA     m_color;
    float     m_fWrap;
    float     m_fSlant;
    CVector2D m_vSlanRefPoint;
    bool      m_bContainImages;
    uint8     m_nFontStyle;
    bool      m_bPropOn;
    uint16    m_wFontTexture;
    uint8     m_nOutline;

public:
    // 0x718E50
    void Set(const CFontChar& setup) {
        m_cLetter        = setup.m_cLetter;
        m_dLetter        = setup.m_dLetter;
        m_vPosn          = setup.m_vPosn;
        m_fWidth         = setup.m_fWidth;
        m_fHeight        = setup.m_fHeight;
        m_color          = setup.m_color;
        m_fWrap          = setup.m_fWrap;
        m_fSlant         = setup.m_fSlant;
        m_vSlanRefPoint  = setup.m_vSlanRefPoint;
        m_bContainImages = setup.m_bContainImages;
        m_nFontStyle     = setup.m_nFontStyle;
        m_bPropOn        = setup.m_bPropOn;
        m_wFontTexture   = setup.m_wFontTexture;
        m_nOutline       = setup.m_nOutline;
    }
};
VALIDATE_SIZE(CFontChar, 0x30);

struct tFontData {
    uint8 m_propValues[208];
    uint8 m_spaceValue;
    uint8 m_unpropValue;
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

class CFont {
public:
    // static variables
    static constexpr size_t MAX_FONT_SPRITES = 2;
    static constexpr size_t MAX_FONT_BUTTON_SPRITES = 15;

    static CFontChar& RenderState;
    // font textures array
    static CSprite2d (&Sprite)[MAX_FONT_SPRITES];
    // button textures array
    static CSprite2d (&ButtonSprite)[MAX_FONT_BUTTON_SPRITES];
    static eExtraFontSymbol& PS2Symbol;
    static bool& m_bNewLine;
    static CRGBA& m_Color;
    static CVector2D& m_Scale;
    static float& m_fSlant;
    static CVector2D& m_fSlantRefPoint;
    static bool& m_bFontJustify;
    static bool& m_bFontCentreAlign;
    static bool& m_bFontRightAlign;
    static bool& m_bFontBackground;
    static bool& m_bEnlargeBackgroundBox;
    static bool& m_bFontPropOn;
    static bool& m_bFontIsBlip;
    static float& m_fFontAlpha;
    static CRGBA& m_FontBackgroundColor;
    static float& m_fWrapx;
    static float& m_fFontCentreSize;
    static float& m_fRightJustifyWrap;
    static uint8& m_FontTextureId;
    static uint8& m_FontStyle;
    static uint8& m_nFontShadow;
    static CRGBA& m_FontDropColor;
    static uint8& m_nFontOutlineSize;
    static uint8& m_nFontOutline;
    static uint8& m_nFontOutlineOrShadow;

public:
    static void InjectHooks();

    static void Initialise();
    static void LoadFontValues();
    static void Shutdown();
    static void PrintChar(float x, float y, char character);
    // Get next ' ' character in a string
    static char* GetNextSpace(char* string);
    static char* ParseToken(char* text, CRGBA& color, bool isBlip, char* tag);
    static void SetScale(float w, float h);
    static void SetScaleForCurrentLanguage(float w, float h);
    static void SetSlantRefPoint(float x, float y);
    static void SetSlant(float value);
    static void SetColor(CRGBA color);
    static void SetFontStyle(eFontStyle style);
    static void SetWrapx(float value);
    static void SetCentreSize(float value);
    static void SetRightJustifyWrap(float value);
    static void SetAlphaFade(float alpha);
    static void SetDropColor(CRGBA color);
    static void SetDropShadowPosition(int16 value);
    static void SetEdge(int8 value);
    static void SetProportional(bool on);
    static void SetBackground(bool enable, bool includeWrap);
    static void SetBackgroundColor(CRGBA color);
    static void SetJustify(bool on);
    static void SetOrientation(eFontAlignment alignment);
    static void InitPerFrame();
    static void RenderFontBuffer();
    static float GetHeight(bool a1 = false);
    static float GetStringWidth(const GxtChar* string, bool full, bool scriptText);
    static void DrawFonts();
    static int16 ProcessCurrentString(bool print, float x, float y, const GxtChar* text);
    static int16 GetNumberLines(float x, float y, const GxtChar* text);
    static int16 ProcessStringToDisplay(float x, float y, const GxtChar* text);
    static void GetTextRect(CRect* rect, float x, float y, const GxtChar* text);
    static void PrintString(float x, float y, const GxtChar* text);
    static void PrintStringFromBottom(float x, float y, const GxtChar* text);
    static float GetCharacterSize(uint8 ch);
    static uint8 FindSubFontCharacter(uint8 letterId, uint8 fontStyle);
};

static void ReadFontsDat();
static float GetScriptLetterSize(uint8 letterId);
static float GetLetterIdPropValue(uint8 letterId);
