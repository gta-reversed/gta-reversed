/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Font.h"

#include "TheScripts.h"
#include "eLanguage.h"

class RenderFontBuffer {
public:
    // Render font buffer is laid in memory like this:
    // [ CFontChar struct (config) ] 'H' 'e' 'l' 'l' 'o' '\0' [ PAD ] [ next CFontChar struct ] ...
    // ^~~ &s_RenderFontBuffer[0]    ^~ `->GetText()`    ^    ^~ Align for the next struct
    //                                                   |~ Terminator
    CFontChar* GetTop() { return reinterpret_cast<CFontChar*>(&m_UnderlyingBuffer[0]); }

    CFontChar* FindNext(void* p) {
        if (*static_cast<GxtChar*>(p) != '\0') {
            NOTSA_UNREACHABLE("expected null terminator to calculate the next fontchar");
        }
        const auto next = ((uintptr)p + 4) & ~3; // align to 4 bytes
        return reinterpret_cast<CFontChar*>(&m_UnderlyingBuffer[next - (uintptr)m_UnderlyingBuffer.data()]);
    }
private:
    std::array<std::byte, 512> m_UnderlyingBuffer{}; // Not an array of CFontChars, refer to RenderFontBuffer
};
auto& s_RenderFontBuffer = StaticRef<RenderFontBuffer>(0xC716B0);
auto& s_RenderEndPtr = StaticRef<void*>(0xC716A8); // Points to the end of queued font renders at s_RenderFontBuffer

auto& gFontData = StaticRef<std::array<tFontData, 2>>(0xC718B0);

// 0x7187C0
void CFont::LoadFontValues() {
    CFileMgr::SetDir("");
    auto* file = CFileMgr::OpenFile("DATA\\FONTS.DAT", "rb");

    char   attrib[32]{};
    uint32 totalFonts{}, fontId{};
    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '\0' || *line == '#') {
            continue;
        }

        if (sscanf_s(line, "%s", SCANF_S_STR(attrib)) == EOF) {
            continue;
        }

        if (!memcmp(attrib, "[TOTAL_FONTS]", 14)) {
            auto nextLine = CFileLoader::LoadLine(file);

            VERIFY(sscanf_s(nextLine, "%d", &totalFonts) == 1);
        } else if (!memcmp(attrib, "[FONT_ID]", 10)) {
            auto nextLine = CFileLoader::LoadLine(file);

            VERIFY(sscanf_s(nextLine, "%d", &fontId) == 1);
        } else if (!memcmp(attrib, "[REPLACEMENT_SPACE_CHAR]", 25)) {
            auto  nextLine = CFileLoader::LoadLine(file);
            uint8 spaceValue;

            VERIFY(sscanf_s(nextLine, "%hhu", &spaceValue) == 1);
            gFontData[fontId].m_spaceValue = spaceValue;
        } else if (!memcmp(attrib, "[PROP]", 7)) {
            for (int32 i = 0; i < 26; i++) {
                auto  nextLine = CFileLoader::LoadLine(file);
                int32 propValues[8]{};

                VERIFY(sscanf_s(nextLine, "%d  %d  %d  %d  %d  %d  %d  %d",
                    &propValues[0], &propValues[1], &propValues[2], &propValues[3],
                    &propValues[4], &propValues[5], &propValues[6], &propValues[7]
                ) == 8);

                for (auto j = 0u; j < std::size(propValues); j++) {
                    gFontData[fontId].m_propValues[i * 8 + j] = propValues[j];
                }
            }
        } else if (!memcmp(attrib, "[UNPROP]", 9)) {
            auto   nextLine = CFileLoader::LoadLine(file);
            uint32 unpropValue;

            VERIFY(sscanf_s(nextLine, "%d", &unpropValue) == 1);
            gFontData[fontId].m_unpropValue = unpropValue;
        }
    }

    CFileMgr::CloseFile(file);
}

// 0x5BA690
void CFont::Initialise() {
    const auto fontsTxd = CTxdStore::AddTxdSlot("fonts");
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

    const auto ps2btnsTxd = CTxdStore::AddTxdSlot("ps2btns");
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
void CFont::Shutdown() {
    rng::for_each(Sprite, &CSprite2d::Delete);
    CTxdStore::SafeRemoveTxdSlot("fonts"); // FIX_BUGS: Added check for if slot exists
    rng::for_each(ButtonSprite, &CSprite2d::Delete);
    CTxdStore::SafeRemoveTxdSlot("ps2btns"); // FIX_BUGS: Added check for if slot exists
}

// this adds a single character into rendering buffer
// 0x718A10
void CFont::PrintChar(float x, float y, char character) {
    // out of screen
    if (x < 0.0f || x > SCREEN_WIDTH || y < 0.0f || y > SCREEN_HEIGHT) {
        return;
    }

    if (PS2Symbol) {
        // extra symbol to be drawn (e.g. PS2 buttons)
        const CRect rt{
            x,
            2.0f * RenderState.m_fHeight + y,
            17.0f * RenderState.m_fHeight + x,
            19.0f * RenderState.m_fHeight + y
        };

        ButtonSprite[PS2Symbol].Draw(rt, { 255, 255, 255, RenderState.m_color.a });
        return;
    }

    bool  zeroed{};
    uint8 letter = character;

    if (!letter || letter == '?') {
        letter    = 0;
        zeroed    = true;
        character = 0;
    }

    if (RenderState.m_nFontStyle == eFontTextureStyle::PRICEDOWN && letter == 0xD0) {
        letter = 0;
        zeroed = true;
    }

    const auto u1     = (letter % 16) / 16.0f;
    const auto v_base = (letter / 16);

    if (RenderState.m_wFontTexture >= 2) { // IDA: if (m_wFontTexture < 2) [else branch]
        float v18 = v_base / 16.0f;

        if (!zeroed) {
            const auto  propValue = GetLetterIdPropValue(character) / 32.0f;
            const CRect rt{
                x,
                y,
                32.0f * propValue * RenderState.m_fWidth + x,
                16.0f * RenderState.m_fHeight + y
            };
            const auto u2 = (propValue / 16.0f) + u1;
            const auto v2 = v18;
            const auto u3 = u1;
            const auto v3 = v18 + 0.0625f;
            const auto u4 = u2 - 0.0001f;
            const auto v4 = v3 - 0.0001f;
            CSprite2d::AddToBuffer(rt, RenderState.m_color, u1, v18, u2, v2, u3, v3, u4, v4);
        }
        return;
    }

    const auto v19 = v_base / 12.8f;
    if (!zeroed) {
        CRect rt{};
        rt.left = x;

        if (RenderState.m_fSlant == 0.0f) {
            rt.bottom = y;
            rt.right  = 32.0f * RenderState.m_fWidth + x;

            if (letter < 0xC0) {
                rt.top        = 20.0f * RenderState.m_fHeight + y;

                const auto v1 = v19 + 0.0021f;
                const auto u2 = u1 + 0.0615f; // u1 + 0.0625 - 0.001
                const auto v2 = v1;
                const auto u3 = u1;
                const auto v3 = v19 + 0.076025f; // v19 + 0.078125 - 0.0021
                const auto u4 = u2;
                const auto v4 = v3;
                CSprite2d::AddToBuffer(rt, RenderState.m_color, u1, v1, u2, v2, u3, v3, u4, v4);
            } else {
                rt.top        = 16.0f * RenderState.m_fHeight + y;

                const auto v7 = v19 + 0.078125f;
                const auto v1 = v19 + 0.0021f;
                const auto u2 = u1 + 0.0615f;
                const auto v2 = v1;
                const auto u3 = u1;
                const auto v3 = v7 - 0.016f;
                const auto u4 = u2;
                const auto v4 = v7 - 0.015f;
                CSprite2d::AddToBuffer(rt, RenderState.m_color, u1, v1, u2, v2, u3, v3, u4, v4);
            }
        } else { // Slant != 0.0f
            rt.bottom = y + 0.015f;
            rt.right  = 32.0f * RenderState.m_fWidth + x;
            rt.top    = 20.0f * RenderState.m_fHeight + y + 0.015f;

            float v8  = v19 + 0.078125f;

            float v1  = v19 + 0.00055f;
            float u2  = u1 + 0.0615f;
            float v2  = v19 + 0.0121f;
            float u3  = u1;
            float v3  = v8 - 0.009f;
            float u4  = u2;
            float v4  = v8 + 0.0079f; // v8 - 0.0021 + 0.01
            CSprite2d::AddToBuffer(rt, RenderState.m_color, u1, v1, u2, v2, u3, v3, u4, v4);
        }
    }
}

// Tags processing
// 0x718F00
const GxtChar* CFont::ParseToken(const GxtChar* text, CRGBA& color, bool isBlip, GxtChar* tag) {
    // info about tokens: https://gtamods.com/wiki/GXT#Tokens
    const GxtChar* next   = ++text;

    const auto ApplyStyle = [&](eHudColours hudColor) {
        if (!isBlip) {
            color = HudColour.GetRGBA(hudColor, color.a);
        }

        if (tag) {
            *tag = *next;
        }
    };

    switch (*next) {
    case '<':
        PS2Symbol = EXSYMBOL_DPAD_LEFT;
        break;
    case '>':
        PS2Symbol = EXSYMBOL_DPAD_RIGHT;
        break;
    case 'A':
    case 'a':
        PS2Symbol = EXSYMBOL_L3;
        break;
    case 'B':
    case 'b':
        ApplyStyle(HUD_COLOUR_DARK_BLUE);
        break;
    case 'C':
    case 'c':
        PS2Symbol = EXSYMBOL_R3;
        break;
    case 'D':
    case 'd':
        PS2Symbol = EXSYMBOL_DPAD_DOWN;
        break;
    case 'G':
    case 'g':
        ApplyStyle(HUD_COLOUR_GREEN);
        break;
    case 'H':
    case 'h':
        if (!isBlip) {
            color = {
                (uint8)std::min((float)color.r * 1.5f, 255.0f),
                (uint8)std::min((float)color.g * 1.5f, 255.0f),
                (uint8)std::min((float)color.b * 1.5f, 255.0f),
                color.a
            };
        }
        if (tag) {
            *tag = *next;
        }
        break;
    case 'J':
    case 'j':
        PS2Symbol = EXSYMBOL_R1;
        break;
    case 'K':
    case 'k':
        PS2Symbol = EXSYMBOL_KEY;
        break;
    case 'M':
    case 'm':
        PS2Symbol = EXSYMBOL_L2;
        break;
    case 'N':
    case 'n':
        m_bNewLine = true;
        break;
    case 'O':
    case 'o':
        PS2Symbol = EXSYMBOL_CIRCLE;
        break;
    case 'P':
    case 'p':
        ApplyStyle(HUD_COLOUR_PURPLE);
        break;
    case 'Q':
    case 'q':
        PS2Symbol = EXSYMBOL_SQUARE;
        break;
    case 'R':
    case 'r':
        ApplyStyle(HUD_COLOUR_RED);
        break;
    case 'S':
    case 's':
        ApplyStyle(HUD_COLOUR_LIGHT_GRAY);
        break;
    case 'T':
    case 't':
        PS2Symbol = EXSYMBOL_TRIANGLE;
        break;
    case 'U':
    case 'u':
        PS2Symbol = EXSYMBOL_DPAD_UP;
        break;
    case 'V':
    case 'v':
        PS2Symbol = EXSYMBOL_R2;
        break;
    case 'W':
    case 'w':
        ApplyStyle(HUD_COLOUR_LIGHT_GRAY);
        break;
    case 'X':
    case 'x':
        PS2Symbol = EXSYMBOL_CROSS;
        break;
    case 'Y':
    case 'y':
        ApplyStyle(HUD_COLOUR_CREAM);
        break;
    case 'l':
        ApplyStyle(HUD_COLOUR_BLACK);
        break;
    default:
        break;
    }

    if (*next != '~') {
        // skip text to the next '~' character.
        for (; *next && *next != '~'; next++)
            ;
    }
    return next + (*next ? 1 : 2);
}


// Text scaling depends on current language
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
    }
}

// Set text color
// 0x719430
void CFont::SetColor(CRGBA color) {
    m_Color = color;

    if (m_fFontAlpha < 255.0f) {
        m_Color.a = (uint8)(float(color.a) * m_fFontAlpha / 255.0f);
    }
}

// Set text style
// 0x719490
void CFont::SetFontStyle(eFontStyle style) {
    switch (style) {
    case eFontStyle::FONT_GOTHIC:
    case eFontStyle::FONT_MENU:
        m_FontTextureName  = eFontTextureName::FONT2;
        m_FontTextureStyle = (style == eFontStyle::FONT_MENU) ? eFontTextureStyle::MENU : eFontTextureStyle::SUBTITLES_AND_GOTHIC;
        break;
    case eFontStyle::FONT_SUBTITLES:
    case eFontStyle::FONT_PRICEDOWN:
        m_FontTextureName = eFontTextureName::FONT1;
        m_FontTextureStyle = (style == eFontStyle::FONT_PRICEDOWN) ? eFontTextureStyle::PRICEDOWN : eFontTextureStyle::SUBTITLES_AND_GOTHIC;
        break;
    default:
        NOTSA_UNREACHABLE("invalid font style {}", style);
    }
}

// Drop color is used for text shadow and text outline
// 0x719510
void CFont::SetDropColor(CRGBA color) {
    m_FontDropColor = color;

    if (m_fFontAlpha < 255.0f) {
        m_FontDropColor.a = (uint8)(float(m_Color.a) * m_fFontAlpha);
    }
}

// Need to call this each frame
// 0x719800
void CFont::InitPerFrame() {
    ZoneScoped;

    m_nFontOutline             = 0;
    m_nFontOutlineOrShadow     = 0;
    m_nFontShadow              = 0;
    m_bNewLine                 = false;
    PS2Symbol                  = EXSYMBOL_NONE;
    RenderState.m_wFontTexture = 0;                  // todo: -1
    s_RenderEndPtr             = s_RenderFontBuffer.GetTop();

    CSprite::InitSpriteBuffer();
}

// Draw text we have in buffer
// 0x719840
void CFont::RenderFontBuffer() {
    if (s_RenderFontBuffer.GetTop() == s_RenderEndPtr) {
        // nothing to render
        return;
    }

    Sprite[RenderState.m_wFontTexture].SetRenderState();
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));

    RenderState.Set(s_RenderFontBuffer.GetTop());

    CRGBA col = RenderState.m_color;
    float x   = RenderState.m_vPosn.x;
    float y   = RenderState.m_vPosn.y;

    auto* textPtr = s_RenderFontBuffer.GetTop()->GetText();
    while (textPtr < s_RenderEndPtr) {
        if (*textPtr == '\0') {
            CFontChar* nextState = s_RenderFontBuffer.FindNext(textPtr);

            if (nextState >= s_RenderEndPtr) {
                break;
            }

            RenderState.Set(nextState);
            y   = RenderState.m_vPosn.y;
            x   = RenderState.m_vPosn.x;
            col = RenderState.m_color;

            textPtr = nextState->GetText();
            continue;
        }

        PS2Symbol = EXSYMBOL_NONE;

        while (*textPtr == '~' && PS2Symbol == EXSYMBOL_NONE) {
            textPtr = const_cast<GxtChar*>(ParseToken(textPtr, col, RenderState.m_bContainImages, nullptr));
            if (!RenderState.m_bContainImages) {
                RenderState.m_color = col;
            }
        }

        // ASCII offset usually starts at space (32)
        auto letter = *textPtr - 32;
        auto charID = letter;
        if (RenderState.m_nFontStyle != eFontTextureStyle::SUBTITLES_AND_GOTHIC) {
            charID = FindSubFontCharacter(letter, RenderState.m_nFontStyle);
            letter = charID;
        } else {
            if (letter == 145) {
                letter = 64;
            } else if (letter > 155) {
                letter = 0;
            }
        }

        if (RenderState.m_fSlant != 0.0f) {
            y = (RenderState.m_vSlanRefPoint.x - x) * RenderState.m_fSlant + RenderState.m_vSlanRefPoint.y;
        }

        if (PS2Symbol == EXSYMBOL_NONE || !RenderState.m_bContainImages) {
            PrintChar(x, y, letter);
            charID = letter;
        }

        if (PS2Symbol != EXSYMBOL_NONE) {
            x += (RenderState.m_fHeight * 17.0f) + RenderState.m_nOutline;
        } else {
            const auto charWidth = GetLetterIdPropValue(charID);
            x += (charWidth + RenderState.m_nOutline) * RenderState.m_fWidth;
        }

        // apply wrap spacing if invalid
        if (!charID) {
            x += RenderState.m_fWrap;
        }

        if (*textPtr) {
            if (PS2Symbol != EXSYMBOL_NONE) {
                PS2Symbol = EXSYMBOL_NONE;
                Sprite[RenderState.m_wFontTexture].SetRenderState();
            } else {
                textPtr++;
            }
        } else if (PS2Symbol != EXSYMBOL_NONE) {
            PS2Symbol = EXSYMBOL_NONE;
            Sprite[RenderState.m_wFontTexture].SetRenderState();
        }
    }

    CSprite::FlushSpriteBuffer();
    CSprite2d::RenderVertexBuffer();

    // reset for the next frame
    s_RenderEndPtr = s_RenderFontBuffer.GetTop();
}

// 0x71A0E0
float CFont::GetStringWidth(const GxtChar* string, bool full, bool scriptText) {
    GxtChar data[400]{};
    strncpy_s((char*)data, sizeof(data), AsciiFromGxtChar(string), CMessages::GetStringLength(string));
    CMessages::InsertPlayerControlKeysInString(data);

    float width{};
    bool  lastWasTag{}, lastWasLetter{};
    auto* pStr = data;
    while (true) {
        if (*pStr == ' ' && !full || *pStr == '\0') {
            break;
        }

        if (*pStr == '~') {
            if (!full && (lastWasTag || lastWasLetter)) {
                return width;
            }

            auto* next = pStr + 1;

            if (*next != '~') {
                for (; *next && *next != '~'; next++)
                    ;
            }

            pStr = next + 1;

            if (lastWasLetter || *pStr == '~') {
                lastWasTag = true;
            }
        } else {
            if (!full && *pStr == ' ' && lastWasTag) {
                return width;
            }

            const auto upper = *pStr++ - 0x20;
            if (scriptText) {
                width += GetScriptLetterSize(upper);
            } else {
                width += GetCharacterSize(upper);
            }

            lastWasLetter = true;
        }
    }
    return width;
}

// same as RenderFontBuffer() (0x71A210)
void CFont::DrawFonts() {
    ZoneScoped;
    RenderFontBuffer();
}

// 0x71A220
// 0x71A220
int16 CFont::ProcessCurrentString(bool print, float x, float y, const GxtChar* text) {
    // state variables per each line
    bool           isFirstWordInLine{}; // we check this so if even the first word doesn't fit to the line we don't break infinitely.
    GxtChar        tag{};               // this makes using old tags (colors etc) work even if we line breaked in middle
    float          width{};             // running total width of line, including the char currently being processed.
    float          processedWidth{};    // same as `width` except doesn't include the current one
    const GxtChar* lineStart{};         // points to the first char to be printed at the new line
    int16          spaceCount{};        // for font justifing

    GxtChar        savedTagBuffer[256]{};
    const auto ResetForNextLine = [&]() {
        width             = (m_bFontCentreAlign || m_bFontRightAlign) ? 0.0f : x;
        processedWidth    = 0.0f;
        isFirstWordInLine = true;
        tag               = 0;
        lineStart         = text;
        spaceCount        = 0;
    };
    ResetForNextLine();

    auto numLines{ 0 };
    while (*text) {
        PS2Symbol              = EXSYMBOL_NONE;
        const auto stringWidth = GetStringWidth(text, false, false);

        if (*text == '~') {
            CRGBA outColor{};
            text = ParseToken(text, outColor, true, &tag);
        }

        const auto wrapLimit = [x] {
            if (m_bFontCentreAlign) {
                return m_fFontCentreSize;
            } else if (m_bFontRightAlign) {
                return x - m_fRightJustifyWrap;
            } else {
                return m_fWrapx;
            }
        }();

        const auto totalWidth = stringWidth + width;
        if ((totalWidth <= wrapLimit || isFirstWordInLine) && !m_bNewLine) {
            width = totalWidth;

            for (GxtChar ch = *text; ch != ' '; ch = *++text) {
                if (ch == '\0' || ch == '~') {
                    break;
                }
            }

            if (*text) {
                if (!isFirstWordInLine) {
                    spaceCount++;
                }
                if (*text != '~') {
                    text++;
                    width += GetCharacterSize('\0');
                }
                processedWidth    = width;
                isFirstWordInLine = false;
                continue;
            }

            const auto drawX = [x, width] {
                if (m_bFontCentreAlign) {
                    return x - width / 2.0f;
                } else if (m_bFontRightAlign) {
                    return x - width;
                } else {
                    return x;
                }
            }();

            numLines++;
            if (print) {
                PrintString(drawX, y, lineStart, text, 0.0f);
            }
            continue;
        }

        if (PS2Symbol) {
            text -= 3;
        }

        const GxtChar* printEnd = m_bNewLine ? text - 3 : text;

        float drawX{ x }, wrap{};
        if (m_bFontJustify) {
            if (!m_bFontCentreAlign && spaceCount > 0) {
                wrap = (m_fWrapx - processedWidth) / (float)spaceCount;
            }
        } else if (m_bFontCentreAlign) {
            drawX = x - width / 2.0f;
        } else if (m_bFontRightAlign) {
            drawX = x - (width - GetCharacterSize(0));
        }

        numLines++;
        if (print) {
            PrintString(drawX, y, lineStart, printEnd, wrap);
        }

        if (tag) {
            // read the saved tag from previous line to not forget its formatting

            // we can't directly put to savedTagBuffer because in text may overlap with it, thus making the memcpy UB!
            notsa::format_to_sz(gString, "~{}~", tag);
            CMessages::StringCopy(reinterpret_cast<GxtChar*>(gString + 3), text, std::size(gString) - 4);
            CMessages::StringCopy(savedTagBuffer, GxtCharFromAscii(gString), std::size(savedTagBuffer));
            text = savedTagBuffer;
        }

        m_bNewLine = false;
        y += m_Scale.y * 18.0f;
        ResetForNextLine();
    }

    if (print) {
        SetColor(m_Color);
    }
    return numLines;
}

// 0x71A620
void CFont::GetTextRect(CRect* rect, float x, float y, const GxtChar* text) {
    if (m_bFontCentreAlign) {
        rect->left  = x - (m_fFontCentreSize / 2.0f + 4.0f);
        rect->right = m_fFontCentreSize / 2.0f + x + 4.0f;
    } else if (m_bFontRightAlign) {
        rect->left  = m_fRightJustifyWrap - 4.0f;
        rect->right = x;
    } else {
        rect->left  = x - 4.0f;
        rect->right = m_fWrapx + 4.0f;
    }

    rect->top    = y - 4.0f;
    rect->bottom = y + 4.0f + GetHeight() * (float)GetNumberLines(x, y, text);
}

// 0x71A700
void CFont::PrintString(float x, float y, const GxtChar* text) {
    if (*text == '\0' || *text == '*') {
        return;
    }

    if (m_bFontBackground) {
        CRect rt{};
        RenderState.m_color = m_Color;
        GetTextRect(&rt, x, y, text);

        if (m_bEnlargeBackgroundBox) {
            rt.left -= 1.0f;
            rt.right += 1.0f;
            rt.bottom += 1.0f;
            rt.top -= 1.0f;

            FrontEndMenuManager.DrawWindow(rt, nullptr, 0, m_FontBackgroundColor, false, true);
        } else {
            CSprite2d::DrawRect(rt, m_FontBackgroundColor);
        }
        m_bFontBackground = false;
    }

    ProcessStringToDisplay(x, y, text);
}

// 0x71A700
void CFont::PrintString(float x, float y, const GxtChar* start, const GxtChar* end, float wrap) {
    NOTSA_UNREACHABLE();
}

// 0x71A820
void CFont::PrintStringFromBottom(float x, float y, const GxtChar* text) {
    float drawY = y - GetHeight() * (float)GetNumberLines(x, y, text);

    if (m_fSlant != 0.0f) {
        drawY -= (m_fSlantRefPoint.x - x) * m_fSlant + m_fSlantRefPoint.y;
    }

    PrintString(x, drawY, text);
}

// 0x719750
float CFont::GetCharacterSize(uint8 letterId) {
    uint8 propValueIdx = letterId;

    if (letterId == '?') {
        letterId     = 0;
        propValueIdx = 0;
    }

    if (m_FontTextureStyle != eFontTextureStyle::SUBTITLES_AND_GOTHIC) {
        propValueIdx = FindSubFontCharacter(letterId, m_FontTextureStyle);
    } else if (propValueIdx == 145) {
        propValueIdx = '@';
    } else if (propValueIdx > 155) {
        propValueIdx = 0;
    }

    if (m_bFontPropOn) {
        return ((float)gFontData[+m_FontTextureName].m_propValues[propValueIdx] + (float)m_nFontOutlineSize) * m_Scale.x;
    } else {
        return ((float)gFontData[+m_FontTextureName].m_unpropValue + (float)m_nFontOutlineSize) * m_Scale.x;
    }
}

// Android
float CFont::GetHeight(bool a1) {
    assert(a1 == false && "NOT IMPLEMENTED");
    const float y = a1 ? 0.0f : m_Scale.y;
    return y * 32.0f / 2.0f + y + y;
}

// 0x7192C0
uint8 CFont::FindSubFontCharacter(uint8 letterId, eFontTextureStyle style) {
    if (style == eFontTextureStyle::PRICEDOWN) {
        switch (letterId) {
        case 1:  return 208;
        case 4:  return 93;
        case 7:  return 206;
        case 8:
        case 9:  return letterId + 86;
        case 14: return 207;
        case 26: return 154;
        }
    }

    if (letterId == 6) {
        return 10;
    }
    if (letterId >= 16 && letterId <= 25) {
        return letterId - 128;
    }
    if (letterId == 31) {
        return 91;
    }
    if (letterId >= 33 && letterId <= 58) {
        return letterId + 122;
    }
    if (letterId == 62) {
        return 32;
    }
    if (letterId >= 65 && letterId <= 90) {
        return letterId + 90;
    }
    if (letterId >= 96 && letterId <= 118) {
        return letterId + 85;
    }
    if (letterId >= 119 && letterId <= 140) {
        return letterId + 62;
    }
    if (letterId >= 141 && letterId <= 142) {
        return 204;
    }
    if (letterId == 143) {
        return 205;
    }

    return letterId;
}

// 0x719670, original name unknown
float GetScriptLetterSize(uint8 letterId) {
    letterId = (letterId != '?') ? letterId : 0;
    const auto i = CTheScripts::NumberOfIntroTextLinesThisFrame;

    const auto style = CTheScripts::IntroTextLines[i].FontStyle;
    const auto subFontChar = [letterId, style] {
        switch (style) {
        case FONT_MENU:
            return CFont::FindSubFontCharacter(letterId, eFontTextureStyle::MENU);
        case FONT_PRICEDOWN:
            return CFont::FindSubFontCharacter(letterId, eFontTextureStyle::PRICEDOWN);
        default:
            break;
        }

        if (letterId == 145) {
            return (uint8)'@';
        } else if (letterId > 155) {
            return (uint8)0;
        } else {
            return letterId;
        }
    }();

    const auto fontWidth = [style, subFontChar, i] {
        if (CTheScripts::IntroTextLines[i].IsProportional) {
            return gFontData[style].m_propValues[subFontChar];
        } else {
            return gFontData[style].m_unpropValue;
        }
    }();

    return CTheScripts::IntroTextLines[i].Scale.x * (fontWidth + CTheScripts::IntroTextLines[i].TextEdge);
}

// 0x718770
float GetLetterIdPropValue(uint8 letterId) {
    const uint8 id = (letterId != '?') ? letterId : 0;

    if (CFont::RenderState.m_bPropOn) {
        return gFontData[CFont::RenderState.m_wFontTexture].m_propValues[id];
    } else {
        return gFontData[CFont::RenderState.m_wFontTexture].m_unpropValue;
    }
}

void CFont::InjectHooks() {
    RH_ScopedClass(CFont);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BA690);
    RH_ScopedInstall(Shutdown, 0x7189B0);
    RH_ScopedInstall(PrintChar, 0x718A10);
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
    RH_ScopedInstall(RenderFontBuffer, 0x719840);
    RH_ScopedInstall(GetStringWidth, 0x71A0E0);
    RH_ScopedInstall(DrawFonts, 0x71A210);
    RH_ScopedInstall(ProcessCurrentString, 0x71A220, { .reversed = true });
    RH_ScopedInstall(GetNumberLines, 0x71A5E0);
    RH_ScopedInstall(ProcessStringToDisplay, 0x71A600);
    RH_ScopedInstall(GetTextRect, 0x71A620);
    RH_ScopedOverloadedInstall(PrintString, "null-terminated", 0x71A700, void(*)(float, float, const GxtChar*));
    RH_ScopedOverloadedInstall(PrintString, "ranged", 0x719B40, void(*)(float, float, const GxtChar*, const GxtChar*, float), {.reversed = false});
    RH_ScopedInstall(PrintStringFromBottom, 0x71A820);
    RH_ScopedInstall(GetCharacterSize, 0x719750);
    RH_ScopedInstall(LoadFontValues, 0x7187C0);
    RH_ScopedInstall(FindSubFontCharacter, 0x7192C0);
    RH_ScopedGlobalInstall(GetScriptLetterSize, 0x719670);
    RH_ScopedGlobalInstall(GetLetterIdPropValue, 0x718770);
}
