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
#include "FontSpecifiers.h"
#include <extensions/WEnum.hpp>

class CSprite2d;

class CFont {
public:
    static void Initialise();
    static void Shutdown();
    static void InitPerFrame();
    static void DrawFonts();
    static void RenderFontBuffer();

    static float GetStringWidth(const GxtChar* string, bool full, bool scriptText);
    static void  GetTextRect(CRect& rect, float x, float y, const GxtChar* text);
    static void  PrintString(float x, float y, const GxtChar* start, const GxtChar* end, float wrap);
    static void  PrintString(float x, float y, const GxtChar* text);
    static void  PrintStringFromBottom(float x, float y, const GxtChar* text);

    // STYLING
    static void SetScaleForCurrentLanguage(float w, float h);
    static void SetColor(CRGBA color);
    static void SetFontStyle(eFontStyle style);
    static void SetDropColor(CRGBA color);
    static void SetScale(float w, float h) { m_Details.Scale.Set(w, h); } /** 0x719380 */
    static void SetSlantRefPoint(float x, float y) { m_Details.SlopeRef.Set(x, y); } /** Set text rotation point (0x719400) */
    static void SetSlant(float value) { m_Details.Slope = value; } /** Set text rotation angle (0x719420) */
    static void SetWrapx(float value) { m_Details.WrapEnd = value; } /** Set line width at right (0x7194D0) */
    static void SetCentreSize(float value) { m_Details.CentreX = value; } /** Set line width at center (0x7194E0) */
    static void SetRightJustifyWrap(float value) { m_Details.RightJustifyWrap = value; } /** 0x7194F0 */
    static void SetAlphaFade(float alpha) { m_Details.AlphaFade = alpha; } /** Like a 'global' font alpha, multiplied with each text alpha (from SetColor) (0x719500) */
    static void SetProportional(bool on) { m_Details.Proportional = on; } /** Toggles character proportions in text (0x7195B0) */
    static void SetBackgroundColor(CRGBA color) { m_Details.BackgroundColor = color; } /** Sets background color (0x7195E0) */
    static void SetJustify(bool on) { m_Details.Justify = on; } /** 0x719600 */

    // Set shadow size (0x719570)
    static void SetDropShadowPosition(uint8 value) {
        m_Details.EdgeAmount = m_Details.EdgeSpace = 0;
        m_Details.DropAmount                       = value;
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

    // 0x71A5E0
    static int16 GetNumberLines(float x, float y, const GxtChar* text) {
        return ProcessCurrentString(false, x, y, text);
    }

    // 0x71A600
    static int16 ProcessStringToDisplay(float x, float y, const GxtChar* text) {
        return ProcessCurrentString(true, x, y, text);
    }

private:
    static void           LoadFontValues();
    static void           PrintChar(float x, float y, uint8 character);
    static const GxtChar* GetNextSpace(const GxtChar* string); // Get next ' ' character in a string
    static const GxtChar* ParseToken(const GxtChar* text, CRGBA& color, bool isBlip, GxtChar* tag);
    static float          GetHeight(bool a1 = false);
    static int16          ProcessCurrentString(bool print, float x, float y, const GxtChar* text);
    static float          GetCharacterSize(uint8 ch);
    static uint8          RemapCharacterWithoutGraphic(uint8 letter);
    static void           ReadFontsDat();
    static float          GetScriptLetterSize(uint8 letterId);
    static float          GetLetterIdPropValue(uint8 letterId);
    static uint8          FindSubFontCharacter(uint8 letterId, eFontTextureStyle fontStyle);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    static constexpr size_t MAX_FONT_SPRITES        = 2;
    static constexpr size_t MAX_FONT_BUTTON_SPRITES = 15;

    static inline auto& RenderState                 = StaticRef<CFontChar>(0xC71AA0);
    static inline auto& Sprite                      = StaticRef<std::array<CSprite2d, MAX_FONT_SPRITES>>(0xC71AD0);        // font textures array
    static inline auto& ButtonSprite                = StaticRef<std::array<CSprite2d, MAX_FONT_BUTTON_SPRITES>>(0xC71AD8); // button textures array
    static inline auto& PS2Symbol                   = StaticRef<eExtraFontSymbol>(0xC71A54);
    static inline auto& m_NewLine                   = StaticRef<bool>(0xC71A55);
    static inline auto& m_Details                   = StaticRef<CFontDetails>(0xC71A60);
};
