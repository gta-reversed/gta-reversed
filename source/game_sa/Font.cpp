/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Font.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <string_view>

#include "eLanguage.h"

auto& FontRenderStateBuf = StaticRef<std::array<CFontChar, 9>>(0xC716B0);
auto& pEmptyChar = StaticRef<CFontChar*>(0xC716A8);

auto& gFontData = StaticRef<std::array<tFontData, 2>>(0xC718B0);

// Registers ReversedHooks for every CFont function. Functions marked { .reversed = false }
// still delegate to the original game code via plugin::Call (see PrintChar, RenderFontBuffer).
void CFont::InjectHooks() {
    RH_ScopedClass(CFont);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BA690);
    RH_ScopedInstall(Shutdown, 0x7189B0);
    RH_ScopedInstall(PrintChar, 0x718A10, { .reversed = false });
    RH_ScopedInstall(ParseToken, 0x718F00);

    // styling functions
    RH_ScopedInstall(SetScale, 0x719380);
    RH_ScopedInstall(SetScaleForCurrentLanguage, 0x7193A0);
    RH_ScopedInstall(SetSlantRefPoint, 0x719400);
    RH_ScopedInstall(SetSlant, 0x719420);
    RH_ScopedInstall(SetColor, 0x719430);
    RH_ScopedInstall(SetFontStyle, 0x719490);
    RH_ScopedInstall(SetWrapx, 0x7194D0);
    RH_ScopedInstall(SetCentreSize, 0x7194E0);
    RH_ScopedInstall(SetRightJustifyWrap, 0x7194F0);
    RH_ScopedInstall(SetAlphaFade, 0x719500);
    RH_ScopedInstall(SetDropColor, 0x719510);
    RH_ScopedInstall(SetDropShadowPosition, 0x719570);
    RH_ScopedInstall(SetEdge, 0x719590);
    RH_ScopedInstall(SetProportional, 0x7195B0);
    RH_ScopedInstall(SetBackground, 0x7195C0);
    RH_ScopedInstall(SetBackgroundColor, 0x7195E0);
    RH_ScopedInstall(SetJustify, 0x719600);
    RH_ScopedInstall(SetOrientation, 0x719610);

    RH_ScopedInstall(InitPerFrame, 0x719800);
    RH_ScopedInstall(RenderFontBuffer, 0x719840, { .reversed = false });
    RH_ScopedInstall(GetStringWidth, 0x71A0E0);
    RH_ScopedInstall(DrawFonts, 0x71A210);
    RH_ScopedInstall(ProcessCurrentString, 0x71A220, { .reversed = false });
    RH_ScopedInstall(GetNumberLines, 0x71A5E0);
    RH_ScopedInstall(ProcessStringToDisplay, 0x71A600);
    RH_ScopedInstall(GetTextRect, 0x71A620);
    RH_ScopedInstall(PrintString, 0x71A700);
    RH_ScopedInstall(PrintStringFromBottom, 0x71A820);
    RH_ScopedInstall(GetCharacterSize, 0x719750);
    RH_ScopedInstall(LoadFontValues, 0x7187C0);
    // Install("", "GetScriptLetterSize", 0x719670, &GetScriptLetterSize);
    RH_ScopedInstall(FindSubFontCharacter, 0x7192C0, { .reversed = false });
    RH_ScopedGlobalInstall(GetLetterIdPropValue, 0x718770);
}

// 0x7187C0
// Loads font proportional / unproportional widths from DATA\FONTS.DAT.
// Rewritten with modern C++17: std::string_view, std::from_chars, and std::array.
// FIX_BUGS: returns early if FONTS.DAT is missing instead of crashing.
void CFont::LoadFontValues() {
    CFileMgr::SetDir("");
    auto* const file = CFileMgr::OpenFile("DATA\\FONTS.DAT", "rb");
    if (!file) return; // FIX_BUGS: handle missing file gracefully

    int totalFonts = 0;
    int fontId = 0;

    auto loadLine = [file]() -> const char* {
        return CFileLoader::LoadLine(file);
    };

    for (const char* line = loadLine(); line; line = loadLine()) {
        if (*line == '\0' || *line == '#')
            continue;

        std::string_view sv{line};
        // Parse first word (attribute name)
        auto firstSpace = sv.find_first_of(" \t");
        std::string_view attrib = (firstSpace == std::string_view::npos) ? sv : sv.substr(0, firstSpace);

        if (attrib == "[TOTAL_FONTS]") {
            auto nextLine = loadLine();
            std::from_chars(nextLine, nextLine + strlen(nextLine), totalFonts);
        }
        else if (attrib == "[FONT_ID]") {
            auto nextLine = loadLine();
            std::from_chars(nextLine, nextLine + strlen(nextLine), fontId);
        }
        else if (attrib == "[REPLACEMENT_SPACE_CHAR]") {
            auto nextLine = loadLine();
            uint8_t spaceValue{};
            std::from_chars(nextLine, nextLine + strlen(nextLine), spaceValue);
            gFontData[fontId].m_spaceValue = spaceValue;
        }
        else if (attrib == "[PROP]") {
            for (int i = 0; i < 26; ++i) {
                auto nextLine = loadLine();
                std::array<int32_t, 8> propValues{};
                // Parse 8 values from a single line
                auto* ptr = nextLine;
                for (auto& val : propValues) {
                    auto [end, ec] = std::from_chars(ptr, ptr + strlen(ptr), val);
                    ptr = end;
                    while (*ptr == ' ' || *ptr == '\t') ++ptr; // Skip whitespace
                }
                const size_t base = i * 8;
                for (size_t j = 0; j < propValues.size(); ++j) {
                    gFontData[fontId].m_propValues[base + j] = propValues[j];
                }
            }
        }
        else if (attrib == "[UNPROP]") {
            auto nextLine = loadLine();
            uint32_t unpropValue{};
            std::from_chars(nextLine, nextLine + strlen(nextLine), unpropValue);
            gFontData[fontId].m_unpropValue = unpropValue;
        }
        // Silently skip unknown attributes
    }

    CFileMgr::CloseFile(file);
}

// 0x5BA690
// Initialises font textures (fonts.TXD) and button sprites (PCBTNS.TXD),
// then loads proportional metrics via LoadFontValues() and resets all state.
void CFont::Initialise() {
    int32 fontsTxd = CTxdStore::AddTxdSlot("fonts");
    CTxdStore::LoadTxd(fontsTxd, "MODELS\\FONTS.TXD");
    CTxdStore::AddRef(fontsTxd);
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(fontsTxd);
    Sprite[0].SetTexture("font2", "font2m");
    Sprite[1].SetTexture("font1", "font1m");

    LoadFontValues();

    SetScale(1.0f, 1.0f);
    SetSlantRefPoint(SCREEN_WIDTH, 0.0f);
    SetSlant(0.0f);

    SetColor(CRGBA(255, 255, 255, 0));
    SetOrientation(eFontAlignment::ALIGN_LEFT);
    SetJustify(false);

    SetWrapx(SCREEN_WIDTH);
    SetCentreSize(SCREEN_WIDTH);
    SetBackground(false, false);

    SetBackgroundColor(CRGBA(128, 128, 128, 128));
    SetProportional(true);
    SetFontStyle(eFontStyle::FONT_GOTHIC);
    SetRightJustifyWrap(0.0f);
    SetAlphaFade(255.0f);
    SetDropShadowPosition(0);
    CTxdStore::PopCurrentTxd();

    int32 ps2btnsTxd = CTxdStore::AddTxdSlot("ps2btns");
    CTxdStore::LoadTxd(ps2btnsTxd, "MODELS\\PCBTNS.TXD");
    CTxdStore::AddRef(ps2btnsTxd);
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(ps2btnsTxd);

    ButtonSprite[1].SetTexture("up", "upA");
    ButtonSprite[2].SetTexture("down", "downA");
    ButtonSprite[3].SetTexture("left", "leftA");
    ButtonSprite[4].SetTexture("right", "rightA");

    CTxdStore::PopCurrentTxd();
}

// 0x7189B0
// Releases font / button sprite textures and unloads their TXD slots.
void CFont::Shutdown() {
    std::ranges::for_each(Sprite, [](CSprite2d& sprite) { sprite.Delete(); });
    CTxdStore::SafeRemoveTxdSlot("fonts");
    std::ranges::for_each(ButtonSprite, [](CSprite2d& sprite) { sprite.Delete(); });
    CTxdStore::SafeRemoveTxdSlot("ps2btns");
}

// 0x718A10
// Renders a single character or PS2 button sprite to the screen.
// NOTE: Not fully reversed — still calls the original game function.
// The dead code below documents the intended UV / sprite-buffer logic.
void CFont::PrintChar(float x, float y, char character) {
    return plugin::Call<0x718A10, float, float, char>(x, y, character);
    if (x < 0.0f || x > SCREEN_WIDTH || y < 0.0f || y > SCREEN_HEIGHT)
        return;

    if (PS2Symbol) {
        const CRect rt = {
            x,
            2.0f * RenderState.m_fHeight + y,
            17.0f * RenderState.m_fHeight + x,
            19.0f * RenderState.m_fHeight + y
        };
        ButtonSprite[PS2Symbol].Draw(rt, { 255, 255, 255, RenderState.m_color.a });
        return;
    }

    if (character == '\0' || character == '?') {
        float propValue = GetLetterIdPropValue(character) / 32.0f;
        bool zeroed = false;

        if (RenderState.m_nFontStyle == 1 && character == 208) {
            character = 0;
            zeroed = true;
        }

        constexpr auto addToBuffer = CSprite2d::AddToBuffer;

        if (RenderState.m_wFontTexture && RenderState.m_wFontTexture != 1) {
            if (!zeroed) {
                const CRect rt = {
                    y,
                    x,
                    32.0f * propValue * RenderState.m_fWidth + x,
                    16.0f * RenderState.m_fHeight + y,
                };
                const float u1 = (character & 0xF) / 16.0f;
                const float v1 = (character >> 4) / 16.0f;
                const float u2 = propValue / 32.0f / 16.0f + u1;
                const float v2 = v1;
                const float u3 = u1;
                const float v3 = v1 + 0.0625f;
                const float u4 = u2 - 0.0001f;
                const float v4 = v3 - 0.0001f;
                addToBuffer(rt, RenderState.m_color, u1, v1, u2, v2, u3, v3, u4, v4);
            }
            return;
        }

        if (!zeroed) {
            CRect rt;
            rt.left = x;
            if (RenderState.m_fSlant == 0.0f) {
                rt.bottom = y;
                rt.right = 32.0f * RenderState.m_fWidth + x;
                if (character < 0xC0) {
                    rt.top = 20.0f * RenderState.m_fHeight + y;
                    const float u1 = (character & 0xF) / 16.0f;
                    const float v1 = (character >> 4) / 12.8f + 0.0021f;
                    const float u2 = u1 + 0.0615f;
                    const float v2 = v1;
                    const float u3 = u1;
                    const float v3 = v2 - 0.0021f;
                    const float u4 = u2;
                    const float v4 = v2 - 0.0021f;
                    addToBuffer(rt, RenderState.m_color, u1, v1, u2, v2, u3, v3, u4, v4);
                } else {
                    rt.top = 16.0f * RenderState.m_fHeight + y;
                    const float u1 = (character & 0xF) / 16.0f;
                    const float v1 = (character >> 4) / 12.8f + 0.0021f;
                    const float u2 = u1 + 0.0615f;
                    const float v2 = v1;
                    const float u3 = u1;
                    const float v3 = v2 - 0.016f;
                    const float u4 = u2;
                    const float v4 = v2 - 0.015f;
                    addToBuffer(rt, RenderState.m_color, u1, v1, u2, v2, u3, v3, u4, v4);
                }
            } else {
                rt.bottom = y + 0.015f;
                rt.right = 32.0f * RenderState.m_fWidth + x;
                rt.top = 20.0f * RenderState.m_fHeight + y + 0.015f;
                const float u1 = (character & 0xF) / 16.0f;
                const float v1 = (character >> 4) / 12.8f + 0.00055f;
                const float u2 = u1 + 0.0615f;
                const float v2 = (character >> 4) / 12.8f + 0.078125f;
                const float u3 = u1;
                const float v3 = v2 - 0.016f;
                const float u4 = u2;
                const float v4 = v2 - 0.015f;
                addToBuffer(rt, RenderState.m_color, u1, v1, u2, v2, u3, v3, u4, v4);
            }
        }
    }
}

// 0x718F00
// Parses a GXT format token (e.g. ~r~ red, ~1~ triangle, ~n~ newline).
// See https://gtamods.com/wiki/GXT#Tokens for the full token specification.
char* CFont::ParseToken(char* text, CRGBA& color, bool isBlip, char* tag) {

    char* next = ++text;

    const auto applyStyle = [&](eHudColours hudColor) {
        if (!isBlip)
            color = HudColour.GetRGBA(hudColor, color.a);
        if (tag)
            *tag = *next;
    };

    const auto brighten = [](uint8 component) -> uint8 {
        return static_cast<uint8>(std::min(static_cast<float>(component) * 1.5f, 255.0f));
    };

    switch (*next) {
    case '<': PS2Symbol = EXSYMBOL_DPAD_LEFT;   break;
    case '>': PS2Symbol = EXSYMBOL_DPAD_RIGHT;  break;
    case 'A':
    case 'a': PS2Symbol = EXSYMBOL_L3;          break;
    case 'B':
    case 'b': applyStyle(HUD_COLOUR_DARK_BLUE); break;
    case 'C':
    case 'c': PS2Symbol = EXSYMBOL_R3;          break;
    case 'D':
    case 'd': PS2Symbol = EXSYMBOL_DPAD_DOWN;   break;
    case 'G':
    case 'g': applyStyle(HUD_COLOUR_GREEN);     break;
    case 'H':
    case 'h':
        if (!isBlip) {
            color = {
                brighten(color.r),
                brighten(color.g),
                brighten(color.b),
                color.a
            };
        }
        if (tag) *tag = *next;
        break;
    case 'J':
    case 'j': PS2Symbol = EXSYMBOL_R1;          break;
    case 'K':
    case 'k': PS2Symbol = EXSYMBOL_KEY;         break;
    case 'M':
    case 'm': PS2Symbol = EXSYMBOL_L2;          break;
    case 'N':
    case 'n': m_bNewLine = true;                break;
    case 'O':
    case 'o': PS2Symbol = EXSYMBOL_CIRCLE;      break;
    case 'P':
    case 'p': applyStyle(HUD_COLOUR_PURPLE);    break;
    case 'Q':
    case 'q': PS2Symbol = EXSYMBOL_SQUARE;      break;
    case 'R':
    case 'r': applyStyle(HUD_COLOUR_RED);       break;
    case 'S':
    case 's': applyStyle(HUD_COLOUR_LIGHT_GRAY); break;
    case 'T':
    case 't': PS2Symbol = EXSYMBOL_TRIANGLE;    break;
    case 'U':
    case 'u': PS2Symbol = EXSYMBOL_DPAD_UP;     break;
    case 'V':
    case 'v': PS2Symbol = EXSYMBOL_R2;          break;
    case 'W':
    case 'w': applyStyle(HUD_COLOUR_LIGHT_GRAY); break;
    case 'X':
    case 'x': PS2Symbol = EXSYMBOL_CROSS;       break;
    case 'Y':
    case 'y': applyStyle(HUD_COLOUR_CREAM);     break;
    case 'l': applyStyle(HUD_COLOUR_BLACK);     break;
    default: break;
    }

    // Advance until next '~' character
    if (*next != '~') {
        while (*next && *next != '~') ++next;
    }

    return (*next) ? next + 1 : next + 2;
}

// --- Styling setters (0x719380 — 0x719610) ---
// Simple wrappers that update CFont static render-state fields.

// 0x719380
void CFont::SetScale(float w, float h) {
    m_Scale.Set(w, h);
}

// 0x7193A0
void CFont::SetScaleForCurrentLanguage(float w, float h) {
    switch (FrontEndMenuManager.m_nPrefsLanguage) {
    case eLanguage::FRENCH:
    case eLanguage::GERMAN:
    case eLanguage::ITALIAN:
    case eLanguage::SPANISH:
        m_Scale.Set(w * 0.8f, h);
        break;
    default:
        m_Scale.Set(w, h);
        break;
    }
}

// 0x719400
void CFont::SetSlantRefPoint(float x, float y) {
    m_fSlantRefPoint.Set(x, y);
}

// 0x719420
void CFont::SetSlant(float value) {
    m_fSlant = value;
}

// 0x719430
void CFont::SetColor(CRGBA color) {
    m_Color = color;
    if (m_fFontAlpha < 255.0f)
        m_Color.a = static_cast<uint8_t>(static_cast<float>(color.a) * m_fFontAlpha / 255.0f);
}

// 0x719490
void CFont::SetFontStyle(eFontStyle style) {
    switch (style) {
    case eFontStyle::FONT_PRICEDOWN:
        m_FontTextureId = 1;
        m_FontStyle = 1;
        break;
    case eFontStyle::FONT_MENU:
        m_FontTextureId = 0;
        m_FontStyle = 2;
        break;
    default:
        m_FontTextureId = static_cast<uint8_t>(style);
        m_FontStyle = 0;
        break;
    }
}

// 0x7194D0
void CFont::SetWrapx(float value) { m_fWrapx = value; }

// 0x7194E0
void CFont::SetCentreSize(float value) { m_fFontCentreSize = value; }

// 0x7194F0
void CFont::SetRightJustifyWrap(float value) { m_fRightJustifyWrap = value; }

// 0x719500
void CFont::SetAlphaFade(float alpha) { m_fFontAlpha = alpha; }

// 0x719510
void CFont::SetDropColor(CRGBA color) {
    m_FontDropColor = color;
    if (m_fFontAlpha < 255.0f)
        m_FontDropColor.a = static_cast<uint8_t>(static_cast<float>(m_Color.a) * m_fFontAlpha);
}

// 0x719570
void CFont::SetDropShadowPosition(int16_t value) {
    m_nFontOutlineSize = 0;
    m_nFontOutlineOrShadow = 0;
    m_nFontShadow = static_cast<uint8_t>(value);
}

// 0x719590
void CFont::SetEdge(int8_t value) {
    m_nFontShadow = 0;
    m_nFontOutlineSize = value;
    m_nFontOutlineOrShadow = value;
}

// 0x7195B0
void CFont::SetProportional(bool on) { m_bFontPropOn = on; }

// 0x7195C0
void CFont::SetBackground(bool enable, bool includeWrap) {
    m_bFontBackground = enable;
    m_bEnlargeBackgroundBox = includeWrap;
}

// 0x7195E0
void CFont::SetBackgroundColor(CRGBA color) { m_FontBackgroundColor = color; }

// 0x719600
void CFont::SetJustify(bool on) { m_bFontJustify = on; }

// 0x719610
void CFont::SetOrientation(eFontAlignment alignment) {
    m_bFontCentreAlign = (alignment == eFontAlignment::ALIGN_CENTER);
    m_bFontRightAlign = (alignment == eFontAlignment::ALIGN_RIGHT);
}

// 0x719800
// Resets transient per-frame state (outline, shadow, PS2 symbol, newline).
// Must be called once each frame before any text is printed.
void CFont::InitPerFrame() {
    ZoneScoped;

    m_nFontOutline = 0;
    m_nFontOutlineOrShadow = 0;
    m_nFontShadow = 0;
    m_bNewLine = false;
    PS2Symbol = EXSYMBOL_NONE;
    RenderState.m_wFontTexture = 0;
    pEmptyChar = &FontRenderStateBuf[0];

    CSprite::InitSpriteBuffer();
}

// 0x719840
// Flushes the internal font sprite buffer to the screen.
// NOTE: Not fully reversed — delegates to the original game code.
void CFont::RenderFontBuffer() {
    plugin::Call<0x719840>();
}

// 0x71A0E0
// Returns the pixel width of a string respecting proportional font metrics.
// FIX_BUGS: buffer is now a fixed-size char array so InsertPlayerControlKeysInString
// receives the correct single-parameter (GxtChar*) signature.
float CFont::GetStringWidth(const GxtChar* string, bool full, bool scriptText) {
    size_t len = CMessages::GetStringLength(string);
    char buffer[400] = { 0 };
    strncpy_s(buffer, sizeof(buffer), AsciiFromGxtChar(string), len);
    CMessages::InsertPlayerControlKeysInString((GxtChar*)buffer);

    float width = 0.0f;
    bool lastWasTag = false, lastWasLetter = false;
    std::string_view text{buffer};

    while (!text.empty()) {
        const char ch = text[0];
        if (ch == ' ' && !full) break;
        if (ch == '\0') break;

        if (ch == '~') {
            if (!full && (lastWasTag || lastWasLetter)) return width;

            text.remove_prefix(1);
            if (!text.empty() && text[0] != '~') {
                size_t nextTilde = text.find('~');
                if (nextTilde != std::string_view::npos)
                    text.remove_prefix(nextTilde + 1);
                else
                    text.remove_prefix(text.size());
            } else if (!text.empty()) {
                text.remove_prefix(1);
            }

            lastWasTag = lastWasLetter || (!text.empty() && text[0] == '~');
        } else {
            if (!full && ch == ' ' && lastWasTag) return width;

            const uint8_t upper = static_cast<uint8_t>(ch) - 0x20; 
            text.remove_prefix(1);

            if (scriptText)
                width += GetScriptLetterSize(upper);
            else
                width += GetCharacterSize(upper);

            lastWasLetter = true;
        }
    }

    return width;
}

// 0x71A210
void CFont::DrawFonts() {
    ZoneScoped;
    RenderFontBuffer();
}

// 0x71A220
int16 CFont::ProcessCurrentString(bool print, float x, float y, const GxtChar* text) {
    return plugin::CallAndReturn<int16, 0x71A220, bool, float, float, const GxtChar*>(print, x, y, text);
}

// 0x71A5E0
int16 CFont::GetNumberLines(float x, float y, const GxtChar* text) {
    return ProcessCurrentString(false, x, y, text);
}

// 0x71A600
int16 CFont::ProcessStringToDisplay(float x, float y, const GxtChar* text) {
    return ProcessCurrentString(true, x, y, text);
}

// 0x71A620
// Computes the bounding rectangle for a text block based on alignment and line count.
void CFont::GetTextRect(CRect* rect, float x, float y, const GxtChar* text) {
    if (m_bFontCentreAlign) {
        rect->left = x - (m_fFontCentreSize / 2.0f + 4.0f);
        rect->right = m_fFontCentreSize / 2.0f + x + 4.0f;
    } else if (m_bFontRightAlign) {
        rect->left = m_fRightJustifyWrap - 4.0f;
        rect->right = x;
    } else {
        rect->left = x - 4.0f;
        rect->right = m_fWrapx + 4.0f;
    }
    rect->top = y - 4.0f;
    rect->bottom = y + 4.0f + GetHeight() * static_cast<float>(GetNumberLines(x, y, text));
}

// 0x71A700
// Draws a full string at (x,y), optionally with a background rectangle / window.
void CFont::PrintString(float x, float y, const GxtChar* text) {
    if (*text == '\0' || *text == '*')
        return;

    if (m_bFontBackground) {
        CRect rt;
        RenderState.m_color = m_Color;
        GetTextRect(&rt, x, y, text);

        if (m_bEnlargeBackgroundBox) {
            rt.left -= 1.0f; rt.right += 1.0f;
            rt.bottom += 1.0f; rt.top -= 1.0f;
            FrontEndMenuManager.DrawWindow(rt, nullptr, 0, m_FontBackgroundColor, false, true);
        } else {
            CSprite2d::DrawRect(rt, m_FontBackgroundColor);
        }
        m_bFontBackground = false;
    }

    ProcessStringToDisplay(x, y, text);
}

// 0x71A820
// Prints a string anchored from its bottom edge (used for multi-line bottom alignment).
void CFont::PrintStringFromBottom(float x, float y, const GxtChar* text) {
    float drawY = y - GetHeight() * static_cast<float>(GetNumberLines(x, y, text));
    if (m_fSlant != 0.0f)
        drawY -= (m_fSlantRefPoint.x - x) * m_fSlant + m_fSlantRefPoint.y;
    PrintString(x, drawY, text);
}

/**
 * FIX_BUGS: Prevent out-of-bounds read in font proportional value lookups.
 *
 * Bug:     std::array<uint8,208>::operator[] crashes with index 208.
 * Trigger:  FindSubFontCharacter(letterId=1, fontStyle=PRICEDOWN) returns 208,
 *           but m_propValues only contains indices 0..207.
 * Context:  The original game used a plain C array where index 208 silently
 *           read one byte past the end. Switching to std::array exposes this
 *           as a hard crash (or checked debug assert).
 * Fix:      Clamp the propId to propValues.size() - 1 (i.e. 207) in both
 *           GetCharacterSize and GetLetterIdPropValue. This maps the invalid
 *           index to the last valid slot, preserving original runtime behaviour
 *           without invoking undefined behaviour.
 */
// 0x719750
// Returns the screen-space width of a single character using the current font style
// and proportional metrics. The std::min clamp prevents the 208-out-of-bounds crash.
float CFont::GetCharacterSize(uint8_t letterId) {
    uint8_t propId = letterId;

    if (letterId == '?') {
        propId = 0;
        letterId = 0;
    }

    if (m_FontStyle)
        propId = FindSubFontCharacter(letterId, m_FontStyle);
    else if (propId == 145)
        propId = '@';
    else if (propId > 155)
        propId = 0;

    const auto& propValues = gFontData[m_FontTextureId].m_propValues;
    if (m_bFontPropOn) {
        const uint8_t safeIdx = std::min<uint8_t>(propId, static_cast<uint8_t>(propValues.size() - 1));
        return (static_cast<float>(propValues[safeIdx]) + m_nFontOutlineSize) * m_Scale.x;
    } else {
        return (static_cast<float>(gFontData[m_FontTextureId].m_unpropValue) + m_nFontOutlineSize) * m_Scale.x;
    }
}

// 0x719670
// Returns the pixel height of the current font scale.
// The a1 parameter is kept for binary compatibility but is not implemented here.
float CFont::GetHeight(bool a1) {
    assert(a1 == false && "NOT IMPLEMENTED");
    const float y = a1 ? 0.0f : m_Scale.y;
    return y * 32.0f / 2.0f + y + y;
}

// 0x719670 
float GetScriptLetterSize(uint8_t letterId) {
    return plugin::CallAndReturn<float, 0x719670, uint8_t>(letterId);
}

// 0x7192C0
// Maps a base letterId to a sub-font glyph index (used by PRICEDOWN and others).
// NOTE: For PRICEDOWN letterId==1 this returns 208, which is clamped downstream
// in GetCharacterSize / GetLetterIdPropValue to avoid out-of-bounds access.
uint8_t CFont::FindSubFontCharacter(uint8_t letterId, uint8_t fontStyle) {
    if (fontStyle == 1) { // eFontStyle::FONT_PRICEDOWN
        switch (letterId) {
        case 1:  return 208;
        case 4:  return 93;
        case 7:  return 206;
        case 8:
        case 9:  return letterId + 86;
        case 14: return 207;
        case 26: return 154;
        default: break;
        }
    }

    if (letterId == 6)                      return 10;
    if (letterId >=  16 && letterId <=  25) return letterId - 128;
    if (letterId == 31)                     return 91;
    if (letterId >=  33 && letterId <=  58) return letterId + 122;
    if (letterId == 62)                     return 32;
    if (letterId >=  65 && letterId <=  90) return letterId + 90;
    if (letterId >=  96 && letterId <= 118) return letterId + 85;
    if (letterId >= 119 && letterId <= 140) return letterId + 62;
    if (letterId >= 141 && letterId <= 142) return 204;
    if (letterId == 143)                    return 205;

    return letterId;
}

// 0x718770
// Returns the raw proportional width value for a letterId (0..207).
// Same std::min clamp as GetCharacterSize to prevent out-of-bounds reads.
float GetLetterIdPropValue(uint8_t letterId) {
    uint8_t id = (letterId == '?') ? 0 : letterId;
    const auto& propValues = gFontData[CFont::RenderState.m_wFontTexture].m_propValues;

    if (CFont::RenderState.m_bPropOn) {
        const uint8_t safeId = std::min<uint8_t>(id, static_cast<uint8_t>(propValues.size() - 1));
        return static_cast<float>(propValues[safeId]);
    } else {
        return static_cast<float>(gFontData[CFont::RenderState.m_wFontTexture].m_unpropValue);
    }
}