/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Font.h"

#include "eLanguage.h"

auto& FontRenderStateBuf = StaticRef<std::array<CFontChar, 9>>(0xC716B0);
auto& pEmptyChar = StaticRef<CFontChar*>(0xC716A8);

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
char* CFont::ParseToken(char* text, CRGBA& color, bool isBlip, char* tag) {
    // info about tokens: https://gtamods.com/wiki/GXT#Tokens
    char* next            = ++text;

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
    pEmptyChar                 = &FontRenderStateBuf[0]; // FontRenderStatePointer.pRenderState

    CSprite::InitSpriteBuffer();
}

// Draw text we have in buffer
// 0x719840
void CFont::RenderFontBuffer() {
    if (pEmptyChar == &FontRenderStateBuf[0]) {
        // nothing to render
        return;
    }

    Sprite[RenderState.m_wFontTexture].SetRenderState();
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));

    RenderState.Set(&FontRenderStateBuf[0]);

    CRGBA col = RenderState.m_color;
    float x   = RenderState.m_vPosn.x;
    float y   = RenderState.m_vPosn.y;

    // That font render state buffer is laid in memory like this:
    // [ CFontChar struct (config) ] 'H' 'e' 'l' 'l' 'o' '\0' [ PAD ] [ next CFontChar struct ] ...
    // ^~~ &FontRenderStateBuf[0]    ^~ `textPtr`        ^    ^~ Align for the next struct
    //                                                   |~ Terminator
    uint8_t* textPtr = (uint8_t*)(&FontRenderStateBuf[1]);
    while (textPtr < (uint8_t*)pEmptyChar) {
        if (*textPtr == '\0') {
            // 4-byte align in pad so we get the next struct. TODO refactor
            CFontChar* nextState = (CFontChar*)(((uintptr_t)(textPtr + 1) + 3) & ~3);

            if ((uint8_t*)nextState >= (uint8_t*)pEmptyChar) {
                break;
            }

            RenderState.Set(nextState);
            y   = RenderState.m_vPosn.y;
            x   = RenderState.m_vPosn.x;
            col = RenderState.m_color;

            // advance to the next text
            textPtr = (uint8_t*)(nextState + 1);
            continue;
        }

        PS2Symbol = EXSYMBOL_NONE;

        while (*textPtr == '~' && PS2Symbol == EXSYMBOL_NONE) {
            textPtr = (uint8_t*)ParseToken((char*)textPtr, col, RenderState.m_bContainImages, nullptr);
            if (!RenderState.m_bContainImages) {
                RenderState.m_color = col;
            }
        }

        // ASCII offset usually starts at space (32)
        uint8_t letter = *textPtr - 32;
        uint8_t charID = letter;

        if (RenderState.m_nFontStyle != eFontTextureStyle::SUBTITLES_AND_GOTHIC) {
            charID = FindSubFontCharacter(letter, RenderState.m_nFontStyle);
            letter = charID;
        } else {
            if (letter == 145) {
                letter = 64;
            } else if (letter > 155) {
                letter = 0;
            }

            charID = letter;
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
            uint8_t propID = charID;
            if (charID == '?') {
                propID = 0;
            }

            float charWidth;
            if (RenderState.m_bPropOn) {
                charWidth = gFontData[RenderState.m_wFontTexture].m_propValues[propID];
            } else {
                charWidth = gFontData[RenderState.m_wFontTexture].m_unpropValue;
            }

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
    pEmptyChar = &FontRenderStateBuf[0];
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
int16 CFont::ProcessCurrentString(bool print, float x, float y, const GxtChar* text) {
    return plugin::CallAndReturn<int16, 0x71A220, bool, float, float, const GxtChar*>(print, x, y, text);
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
    return plugin::CallAndReturn<float, 0x719670, uint8>(letterId);
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
    RH_ScopedInstall(RenderFontBuffer, 0x719840, { .reversed = true });
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
