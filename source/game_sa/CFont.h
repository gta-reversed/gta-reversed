/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CRGBA.h"
#include "CRect.h"
#include "CSprite2d.h"

// todo: move
struct CFontChar {
    uint8_t m_cLetter;
    uint8_t _pad0[3];
    CVector2D m_vPosn;
    float m_fWidth;
    float m_fHeight;
    CRGBA m_color;
    float m_fWrap;
    float m_fSlant;
    CVector2D m_vSlanRefPoint;
    bool m_bContainImages;
    uint8_t m_nFontStyle;
    bool m_bPropOn;
    uint8_t _pad1;
    uint16_t m_dwFontTexture;
    uint8_t m_nOutline;
    uint8_t _pad2;
public:
    CFontChar* Set(CFontChar& setup) {
        m_cLetter = setup.m_cLetter;
        m_vPosn.x = setup.m_vPosn.x;
        m_vPosn.y = setup.m_vPosn.y;
        m_fWidth = setup.m_fWidth;
        m_fHeight = setup.m_fHeight;
        m_color = setup.m_color;
        m_fWrap = setup.m_fWrap;
        m_fSlant = setup.m_fSlant;
        m_vSlanRefPoint.x = setup.m_vSlanRefPoint.x;
        m_vSlanRefPoint.y = setup.m_vSlanRefPoint.y;
        m_bContainImages = setup.m_bContainImages;
        m_nFontStyle = setup.m_nFontStyle;
        m_bPropOn = setup.m_bPropOn;
        m_dwFontTexture = setup.m_dwFontTexture;
        m_nOutline = setup.m_nOutline;
    }
};
VALIDATE_SIZE(CFontChar, 0x30);

struct tFontData {
    char m_propValues[208];
    char m_spaceValue;
    char m_unpropValue;
};

enum class eExtraFontSymbol : uint8_t {
    BUTTON_NONE = 0, // invalid
    BUTTON_DPAD_UP = 1,
    BUTTON_DPAD_DOWN = 2,
    BUTTON_DPAD_LEFT = 3,
    BUTTON_DPAD_RIGHT = 4,
    BUTTON_CROSS = 5,
    BUTTON_CIRCLE = 6,
    BUTTON_SQUARE = 7,
    BUTTON_TRIANGLE = 8,
    BUTTON_KEY = 9, // followed by buttons, L1?
    BUTTON_L2 = 10,
    BUTTON_L3 = 11,
    BUTTON_R1 = 12,
    BUTTON_R2 = 13,
    BUTTON_R3 = 14
};

enum class eFontAlignment : uint8_t {
    ALIGN_CENTER,
    ALIGN_LEFT,
    ALIGN_RIGHT
};

enum class eFontStyle : uint8_t {
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
    static eExtraFontSymbol& m_nExtraFontSymbolId;
    static bool& m_bNewLine;
    static CRGBA* m_Color;
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
    static unsigned int m_dwFontAlpha;
    static CRGBA& m_FontBackgroundColor;
    static float& m_fWrapx;
    static float& m_fFontCentreSize;
    static float& m_fRightJustifyWrap;
    static unsigned char& m_FontTextureId;
    static unsigned char& m_FontStyle;
    static unsigned char& m_nFontShadow;
    static CRGBA* m_FontDropColor;
    static unsigned char& m_nFontOutlineSize;
    static unsigned char& m_nFontOutline;
    static unsigned char& m_nFontOutlineOrShadow;

    // static functions
    static void InjectHooks();

    // CFont initialisation
    static void Initialise();
    // CFont closing
    static void Shutdown();
    // this adds a single character into rendering buffer
    static void PrintChar(float x, float y, char character);
    // get next ' ' character in a string
    static char *GetNextSpace(char *string);
    // tags processing
    static char *ParseToken(char *text, CRGBA & color, bool isBlip, char *tag);
    // text scaling
    static void SetScale(float w, float h);
    // text scaling depends on current language
    static void SetScaleForCurrentlanguage(float w, float h);
    // set text rotation point
    static void SetSlantRefPoint(float x, float y);
    // set text rotation angle
    static void SetSlant(float value);
    // set text color
    static void SetColor(CRGBA color);
    // set text style
    static void SetFontStyle(eFontStyle style);
    // set line width at right
    static void SetWrapx(float value);
    // set line width at center
    static void SetCentreSize(float value);
    static void SetRightJustifyWrap(float value);
    // like a 'global' font alpha, multiplied with each text alpha (from SetColor)
    static void SetAlphaFade(float alpha);
    // drop color is used for text shadow and text outline
    static void SetDropColor(CRGBA color);
    // set shadow size
    static void SetDropShadowPosition(short value);
    // set outline size
    static void SetEdge(short value);
    // toggles character proportions in text
    static void SetProportional(bool on);
    // setups text background
    static void SetBackground(bool enable, bool includeWrap);
    // sets background color
    static void SetBackgroundColor(CRGBA color);
    static void SetJustify(bool on);
    static void SetOrientation(eFontAlignment alignment);
    // need to call this each frame
    static void InitPerFrame();
    // draw text we have in buffer
    static void RenderFontBuffer();
    static float GetStringWidth(char *string, bool unk1, bool unk2);
    static void DrawFonts();
    static short ProcessCurrentString(bool print, float x, float y, const char *text);
    static short GetNumberLines(float x, float y, const char *text);
    static short ProcessStringToDisplay(float x, float y, const char *text);
    static void GetTextRect(CRect *rect, float x, float y, const char *text);
    static void PrintString(float x, float y, const char *text);
    static void PrintStringFromBottom(float x, float y, const char *text);
};
