/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Font.h"

#include "eLanguage.h"

// Strings to be rendered. Each string is stored as its render state (`CFontChar`) followed by
// the null terminated text, padded to 4 bytes. (See `CFont::PrintString` and `CFont::RenderFontBuffer`)
auto& FontBuffer     = StaticRef<std::array<uint8, 0x200>>(0xC716B0);
auto& FontBufferIter = StaticRef<uint8*>(0xC716A8); // End of the used part of `FontBuffer`

auto& gFontData = StaticRef<std::array<tFontData, 2>>(0xC718B0);

void CFont::InjectHooks() {
    RH_ScopedClass(CFont);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BA690);
    RH_ScopedInstall(Shutdown, 0x7189B0);
    RH_ScopedInstall(PrintChar, 0x718A10);
    RH_ScopedInstall(GetNextSpace, 0x718ED0);
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
    RH_ScopedOverloadedInstall(PrintString, "Buffer", 0x719B40, void(*)(float, float, const GxtChar*, const GxtChar*, float));
    RH_ScopedInstall(GetStringWidth, 0x71A0E0);
    RH_ScopedInstall(DrawFonts, 0x71A210);
    RH_ScopedInstall(ProcessCurrentString, 0x71A220);
    RH_ScopedInstall(GetNumberLines, 0x71A5E0);
    RH_ScopedInstall(ProcessStringToDisplay, 0x71A600);
    RH_ScopedInstall(GetTextRect, 0x71A620);
    RH_ScopedOverloadedInstall(PrintString, "", 0x71A700, void(*)(float, float, const GxtChar*));
    RH_ScopedInstall(PrintStringFromBottom, 0x71A820);
    RH_ScopedInstall(GetCharacterSize, 0x719750);
    RH_ScopedInstall(LoadFontValues, 0x7187C0);
    RH_ScopedGlobalInstall(GetScriptLetterSize, 0x719670);

    // `GetScriptLetterSize` (0x719670) and `RenderFontBuffer` (0x719840) expect `ecx` and `edx` to be preserved across the call
    RH_ScopedInstall(FindSubFontCharacter, 0x7192C0, { .StackArgumentsToPreserve = 2, .PreserveRegisters = true });

    RH_ScopedGlobalInstall(GetLetterIdPropValue, 0x718770);
}

// 0x7187C0
void CFont::LoadFontValues() {
    CFileMgr::SetDir("");
    auto* file = CFileMgr::OpenFile("DATA\\FONTS.DAT", "rb");

    char attrib[32];

    uint32 totalFonts = 0;
    uint32 fontId = 0;

    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '\0' || *line == '#')
            continue;

        if (sscanf_s(line, "%s", SCANF_S_STR(attrib)) == EOF)
            continue;

        if (!memcmp(attrib, "[TOTAL_FONTS]", 14)) {
            auto nextLine = CFileLoader::LoadLine(file);

            VERIFY(sscanf_s(nextLine, "%d", &totalFonts) == 1);
        }
        else if (!memcmp(attrib, "[FONT_ID]", 10)) {
            auto nextLine = CFileLoader::LoadLine(file);

            VERIFY(sscanf_s(nextLine, "%d", &fontId) == 1);
        }
        else if (!memcmp(attrib, "[REPLACEMENT_SPACE_CHAR]", 25)) {
            auto nextLine = CFileLoader::LoadLine(file);
            uint8 spaceValue;

            VERIFY(sscanf_s(nextLine, "%hhu", &spaceValue) == 1);
            gFontData[fontId].m_spaceValue = spaceValue;
        }
        else if (!memcmp(attrib, "[PROP]", 7)) {
            for (int32 i = 0; i < 26; i++) {
                auto nextLine = CFileLoader::LoadLine(file);
                int32 propValues[8]{};

                VERIFY(sscanf_s(nextLine, "%d  %d  %d  %d  %d  %d  %d  %d",
                    &propValues[0], &propValues[1], &propValues[2], &propValues[3],
                    &propValues[4], &propValues[5], &propValues[6], &propValues[7]
                ) == 8);

                for (auto j = 0u; j < std::size(propValues); j++) {
                    gFontData[fontId].m_propValues[i * 8 + j] = propValues[j];
                }
            }
        }
        else if (!memcmp(attrib, "[UNPROP]", 9)) {
            auto nextLine = CFileLoader::LoadLine(file);
            uint32 unpropValue;

            VERIFY(sscanf_s(nextLine, "%d", &unpropValue) == 1);
            gFontData[fontId].m_unpropValue = unpropValue;
        }
    }

    CFileMgr::CloseFile(file);
}

// 0x5BA690
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
void CFont::Shutdown() {
    std::ranges::for_each(Sprite, [](CSprite2d& sprite) { sprite.Delete(); });
    CTxdStore::SafeRemoveTxdSlot("fonts"); // FIX_BUGS: Added check for is slot exists
    std::ranges::for_each(ButtonSprite, [](CSprite2d& sprite) { sprite.Delete(); });
    CTxdStore::SafeRemoveTxdSlot("ps2btns"); // FIX_BUGS: Added check for is slot exists
}

// Adds a single character to the sprite buffer (Or draws the button if `PS2Symbol` is set)
// 0x718A10
void CFont::PrintChar(float x, float y, char character) {
    if (y < 0.0f || SCREEN_HEIGHT < y || x < 0.0f || SCREEN_WIDTH < x) {
        return; // Off-screen
    }

    if (PS2Symbol) {
        CRect rect;
        rect.left   = x;
        rect.top    = RenderState.m_fHeight * 19.0f + y;
        rect.right  = RenderState.m_fHeight * 17.0f + x;
        rect.bottom = RenderState.m_fHeight * 2.0f + y;
        ButtonSprite[PS2Symbol].Draw(rect, CRGBA(255, 255, 255, RenderState.m_color.a));
        return;
    }

    auto letter  = (uint8)(character);
    bool isSpace = false;
    if (!letter || letter == '?') {
        letter  = 0;
        isSpace = true;
    }

    const auto propValue = GetLetterIdPropValue(letter) * (1.0f / 32.0f);

    if (RenderState.m_nFontStyle == 1 && letter == 208) {
        letter = 0;
    }

    const auto row = (float)(letter >> 4);
    const auto u   = (float)(letter & 0xF) * (1.0f / 16.0f);

    if (RenderState.m_wFontTexture != 0 && RenderState.m_wFontTexture != 1) {
        const auto v = row * (1.0f / 16.0f);
        if (isSpace) {
            return;
        }

        CRect rect;
        rect.left   = x;
        rect.top    = RenderState.m_fHeight * 32.0f * 0.5f + y;
        rect.right  = RenderState.m_fWidth * 32.0f * propValue + x;
        rect.bottom = y;

        const auto u2 = propValue * (1.0f / 16.0f) + u;
        const auto v3 = v + (1.0f / 16.0f);
        CSprite2d::AddToBuffer(rect, RenderState.m_color, u, v, u2, v, u, v3, u2 - 0.0001f, v3 - 0.0001f);
        return;
    }

    // The font textures have 16 x 12.8 characters
    const auto v = (float)(row * 0.078125);
    if (isSpace) {
        return;
    }

    CRect rect;
    rect.left  = x;
    rect.right = RenderState.m_fWidth * 32.0f + x;

    const auto u2 = u + (1.0f / 16.0f) - 0.001f;
    if (RenderState.m_fSlant == 0.0f) {
        rect.bottom = y;

        const auto v1 = v + 0.0021f;
        if (letter >= 192) {
            rect.top = RenderState.m_fHeight * 32.0f * 0.5f + y;

            const auto v3 = v + 0.078125f;
            CSprite2d::AddToBuffer(rect, RenderState.m_color, u, v1, u2, v1, u, v3 - 0.016f, u2, v3 - 0.015f);
        } else {
            rect.top = RenderState.m_fHeight * 40.0f * 0.5f + y;

            const auto v3 = v + 0.078125f - 0.0021f;
            CSprite2d::AddToBuffer(rect, RenderState.m_color, u, v1, u2, v1, u, v3, u2, v3);
        }
    } else {
        rect.bottom = y + 0.015f;
        rect.top    = RenderState.m_fHeight * 40.0f * 0.5f + y + 0.015f;

        const auto v3 = v + 0.078125f;
        CSprite2d::AddToBuffer(rect, RenderState.m_color, u, v + 0.00055f, u2, v + 0.0121f, u, v3 - 0.009f, u2, v3 - 0.0021f + 0.01f);
    }
}

// 0x718ED0
const GxtChar* CFont::GetNextSpace(const GxtChar* string) {
    for (; *string != ' '; string++) {
        if (!*string || *string == '~') {
            break;
        }
    }
    return string;
}

// Tags processing
// 0x718F00
char* CFont::ParseToken(char* text, CRGBA& color, bool isBlip, char* tag) {
    // info about tokens: https://gtamods.com/wiki/GXT#Tokens

    char* next = ++text;

    auto ApplyStyle = [&](eHudColours hudColor) {
        if (!isBlip)
            color = HudColour.GetRGBA(hudColor, color.a);

        if (tag)
            *tag = *next;
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

        if (tag)
            *tag = *next;
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
        for(; *next && *next != '~'; next++);
    }

    if (*next)
        return next + 1;

    return next + 2;
}

// Text scaling
// 0x719380
void CFont::SetScale(float w, float h) {
    m_Scale.Set(w, h);
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

// Set text rotation point
// 0x719400
void CFont::SetSlantRefPoint(float x, float y) {
    m_fSlantRefPoint.Set(x, y);
}

// Set text rotation angle
// 0x719420
void CFont::SetSlant(float value) {
    m_fSlant = value;
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
    case eFontStyle::FONT_PRICEDOWN:
        m_FontTextureId = 1;
        m_FontStyle = 1;
        break;
    case eFontStyle::FONT_MENU:
        m_FontTextureId = 0;
        m_FontStyle = 2;
        break;
    default:
        m_FontTextureId = static_cast<uint8>(style);
        m_FontStyle = 0;
    }
}

// Set line width at right
// 0x7194D0
void CFont::SetWrapx(float value) {
    m_fWrapx = value;
}

// Set line width at center
// 0x7194E0
void CFont::SetCentreSize(float value) {
    m_fFontCentreSize = value;
}

// 0x7194F0
void CFont::SetRightJustifyWrap(float value) {
    m_fRightJustifyWrap = value;
}

// Like a 'global' font alpha, multiplied with each text alpha (from SetColor)
// 0x719500
void CFont::SetAlphaFade(float alpha) {
    m_fFontAlpha = alpha;
}

// Drop color is used for text shadow and text outline
// 0x719510
void CFont::SetDropColor(CRGBA color) {
    m_FontDropColor = color;

    if (m_fFontAlpha < 255.0f) {
        m_FontDropColor.a = (uint8)(float(m_Color.a) * m_fFontAlpha);
    }
}

// Set shadow size
// 0x719570
void CFont::SetDropShadowPosition(int16 value) {
    m_nFontOutlineSize = 0;
    m_nFontOutlineOrShadow = 0;
    m_nFontShadow = (uint8)value;
}

// Set outline size
// 0x719590
void CFont::SetEdge(int8 value) {
    m_nFontShadow = 0;
    m_nFontOutlineSize = value;
    m_nFontOutlineOrShadow = value;
}

// Toggles character proportions in text
// 0x7195B0
void CFont::SetProportional(bool on) {
    m_bFontPropOn = on;
}

// Setups text background
// 0x7195C0
void CFont::SetBackground(bool enable, bool includeWrap) {
    m_bFontBackground = enable;
    m_bEnlargeBackgroundBox = includeWrap;
}

// Sets background color
// 0x7195E0
void CFont::SetBackgroundColor(CRGBA color) {
    m_FontBackgroundColor = color;
}

// 0x719600
void CFont::SetJustify(bool on) {
    m_bFontJustify = on;
}

// 0x719610
void CFont::SetOrientation(eFontAlignment alignment) {
    m_bFontCentreAlign = alignment == eFontAlignment::ALIGN_CENTER;
    m_bFontRightAlign = alignment == eFontAlignment::ALIGN_RIGHT;
}

// Need to call this each frame
// 0x719800
void CFont::InitPerFrame() {
    ZoneScoped;

    m_nFontOutline = 0;
    m_nFontOutlineOrShadow = 0;
    m_nFontShadow = 0;
    m_bNewLine = false;
    PS2Symbol = EXSYMBOL_NONE;
    RenderState.m_wFontTexture = (uint16)(-1); // Forces `PrintString` to set it
    FontBufferIter = FontBuffer.data();

    CSprite::InitSpriteBuffer();
}

// Draw text we have in buffer
// 0x719840
void CFont::RenderFontBuffer() {
    if (FontBufferIter == FontBuffer.data()) {
        return;
    }

    Sprite[(int16)RenderState.m_wFontTexture].SetRenderState();
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));

    // Render state of the string currently being processed
    CRGBA     color;
    CVector2D pos;
    const auto SetRenderState = [&](const uint8* state) {
        RenderState.Set(*reinterpret_cast<const CFontChar*>(state));
        color = RenderState.m_color;
        pos   = RenderState.m_vPosn;
    };

    SetRenderState(FontBuffer.data());
    for (auto* it = FontBuffer.data() + sizeof(CFontChar); it < FontBufferIter;) {
        if (*it == '\0') { // End of the string, next one begins (padded to 4 bytes)
            do {
                it++;
            } while ((uintptr_t)(it) % 4 != 0);
            if (it >= FontBufferIter) {
                break;
            }
            SetRenderState(it);
            it += sizeof(CFontChar);
        }

        PS2Symbol = EXSYMBOL_NONE;
        while (*it == '~' && !PS2Symbol) {
            it = reinterpret_cast<uint8*>(ParseToken(reinterpret_cast<char*>(it), color, RenderState.m_bContainImages, nullptr));
            if (!RenderState.m_bContainImages) {
                RenderState.m_color = color;
            }
        }

        // NOTE: If the string ends right after a token, the null terminator is printed (as a space)
        auto letter = (uint8)(*it - ' ');
        if (RenderState.m_nFontStyle) {
            letter = FindSubFontCharacter(letter, RenderState.m_nFontStyle);
        } else if (letter == 145) {
            letter = '@';
        } else if (letter > 155) {
            letter = 0;
        }

        if (RenderState.m_fSlant != 0.0f) {
            pos.y = (RenderState.m_vSlanRefPoint.x - pos.x) * RenderState.m_fSlant + RenderState.m_vSlanRefPoint.y;
        }

        if (!PS2Symbol || !RenderState.m_bContainImages) {
            PrintChar(pos.x, pos.y, (char)(letter));
        }

        if (PS2Symbol) {
            pos.x += RenderState.m_fHeight * 17.0f + (float)(int8)(RenderState.m_nOutline);
        } else {
            const auto& fontData = gFontData[(int16)RenderState.m_wFontTexture];
            const auto  width    = RenderState.m_bPropOn
                ? fontData.m_propValues[letter == '?' ? 0 : letter]
                : fontData.m_unpropValue;
            pos.x += ((float)(int8)(RenderState.m_nOutline) + (float)(width)) * RenderState.m_fWidth;
        }

        if (letter == 0) { // Space
            pos.x += RenderState.m_fWrap;
        }

        if (!PS2Symbol) {
            if (*it) {
                it++;
            }
            continue;
        }

        // The character after the button is not consumed, it's processed in the next iteration
        Sprite[(int16)RenderState.m_wFontTexture].SetRenderState(); // Drawing the button has changed the render state
        PS2Symbol = EXSYMBOL_NONE;
    }

    CSprite::FlushSpriteBuffer();
    CSprite2d::RenderVertexBuffer();

    FontBufferIter = FontBuffer.data();
}

// Adds the string [start, end) to the font buffer (Along with the drop shadow or edge)
// 0x719B40
void CFont::PrintString(float x, float y, const GxtChar* start, const GxtChar* end, float spaceWidth) {
    auto savedColor = m_Color;

    if (RenderState.m_wFontTexture != m_FontTextureId) {
        RenderFontBuffer(); // Different font texture, must render what we have so far
        RenderState.m_wFontTexture = m_FontTextureId;
    }

    // Prints the string with the drop color at the given offsets (multiplied by `size` in PS2 pixels) to make the shadow/edge
    const auto PrintWithDropColor = [&](uint8& size, std::initializer_list<CVector2D> offsets) {
        const auto sizeSigned = (float)(int8)(size);
        const auto prevSize   = size;
        const auto color      = m_Color;

        m_Color       = m_FontDropColor;
        m_bFontIsBlip = true;
        size          = 0;

        const auto stretchX = SCREEN_WIDTH * (1.0f / (float)DEFAULT_SCREEN_WIDTH);
        const auto stretchY = SCREEN_HEIGHT * (1.0f / (float)DEFAULT_SCREEN_HEIGHT);
        if (m_fSlant != 0.0f) {
            // NOTE: Not restored afterwards, so the string itself is drawn with the moved slant reference point too
            m_fSlantRefPoint.x += stretchX * sizeSigned;
            m_fSlantRefPoint.y += stretchY * sizeSigned;
        }
        for (const auto& offset : offsets) {
            PrintString(
                x + offset.x * (stretchX * sizeSigned),
                y + offset.y * (stretchY * sizeSigned),
                start,
                end,
                spaceWidth
            );
        }

        m_Color       = color;
        size          = prevSize;
        m_bFontIsBlip = false;
    };
    if (m_nFontShadow != 0) {
        PrintWithDropColor(m_nFontShadow, { { 1.0f, 1.0f } });
    } else if (m_nFontOutlineSize != 0) {
        PrintWithDropColor(m_nFontOutlineSize, {
            {  1.0f, -1.0f },
            { -1.0f, -1.0f },
            {  1.0f,  1.0f },
            { -1.0f,  1.0f },
            {  1.0f,  0.0f },
            { -1.0f,  0.0f },
            {  0.0f,  1.0f },
            {  0.0f, -1.0f },
        });
    }

    // Make sure the render state and the string fits
    if (FontBufferIter >= FontBuffer.data() + FontBuffer.size() - sizeof(CFontChar) - 4 - (end - start)) {
        RenderFontBuffer();
    }

    auto& state = *reinterpret_cast<CFontChar*>(FontBufferIter);
    state.m_vPosn          = { x, y };
    state.m_fWidth         = m_Scale.x;
    state.m_fHeight        = m_Scale.y;
    state.m_color          = m_Color;
    state.m_fWrap          = spaceWidth;
    state.m_fSlant         = m_fSlant;
    state.m_vSlanRefPoint  = m_fSlantRefPoint;
    state.m_nFontStyle     = m_FontStyle;
    state.m_bPropOn        = m_bFontPropOn;
    state.m_wFontTexture   = m_FontTextureId;
    state.m_bContainImages = m_bFontIsBlip;
    state.m_nOutline       = m_nFontOutline;
    FontBufferIter += sizeof(CFontChar);

    for (auto* it = start; it < end;) {
        if (*it != '~') {
            *FontBufferIter++ = *it++;
            continue;
        }

        // Colors set by the tokens are applied to the rest of the text (unless it's a shadow/edge)
        // NOTE: `RenderState` is the state of the last rendered string here, not this one's
        auto       color = m_Color;
        const auto next  = reinterpret_cast<const GxtChar*>(ParseToken(const_cast<char*>(reinterpret_cast<const char*>(it)), color, RenderState.m_bContainImages, nullptr));
        if (!m_bFontIsBlip && !PS2Symbol) {
            m_Color.r = savedColor.r = color.r;
            m_Color.g = savedColor.g = color.g;
            m_Color.b = savedColor.b = color.b;
        }
        PS2Symbol = EXSYMBOL_NONE;

        while (it != next) {
            *FontBufferIter++ = *it++; // Tokens are processed again when rendering
        }
    }

    *FontBufferIter++ = '\0';
    while ((uintptr_t)(FontBufferIter) % 4 != 0) {
        FontBufferIter++;
    }

    if (!m_bFontIsBlip) {
        m_Color = savedColor;
    }
}

// 0x71A0E0
float CFont::GetStringWidth(const GxtChar* string, bool full, bool scriptText) {
    size_t len = CMessages::GetStringLength(string);
    GxtChar data[400] = { 0 };

    strncpy_s((char*)data, sizeof(data), AsciiFromGxtChar(string), len);
    CMessages::InsertPlayerControlKeysInString(data);

    float width = 0.0f;
    bool lastWasTag = false, lastWasLetter = false;
    auto* pStr = data;

    while (true) {
        if (*pStr == ' ' && !full)
            break;
        if (*pStr == '\0')
            break;

        if (*pStr == '~') {
            if (!full && (lastWasTag || lastWasLetter))
                return width;

            auto* next = pStr + 1;

            if (*next != '~') {
                for (; *next && *next != '~'; next++);
            }

            pStr = next + 1;

            if (lastWasLetter || *pStr == '~')
                lastWasTag = true;
        }
        else {
            if (!full && *pStr == ' ' && lastWasTag)
                return width;

            char upper = *pStr - 0x20;

            pStr++;
            if (scriptText) {
                width += GetScriptLetterSize(upper);
            }
            else {
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

// Splits the text into lines, and if `print` is set adds them to the font buffer
// 0x71A220
int16 CFont::ProcessCurrentString(bool print, float x, float y, const GxtChar* text) {
    const auto IsLeftAligned = [] { return !m_bFontCentreAlign && !m_bFontRightAlign; };

    const auto savedColor = m_Color;

    int16 numLines{};
    int16 numSpaces{};             // Number of spaces in the current line
    bool  isLineEmpty{ true };
    char  tag{};                   // Color token (`~r~`, etc) found in the text, it's continued in the next line
    float lineWidth{};             // Width of the current line without the last word
    float lineEnd{ IsLeftAligned() ? x : 0.0f }; // End of the current line (Width if not left aligned)
    auto  lineY = y;
    CRGBA tokenColor;

    std::array<GxtChar, 256> lineContinuation; // Text of the next line (prefixed with the color token)

    auto* lineStart = text;
    for (auto* it = text; *it;) {
        PS2Symbol = EXSYMBOL_NONE;

        auto wordEnd = GetStringWidth(it, false, false);
        if (*it == '~') {
            it = reinterpret_cast<const GxtChar*>(ParseToken(const_cast<char*>(reinterpret_cast<const char*>(it)), tokenColor, true, &tag));
        }

        const auto wrapX = m_bFontCentreAlign
            ? m_fFontCentreSize
            : m_bFontRightAlign
                ? x - m_fRightJustifyWrap
                : m_fWrapx;
        wordEnd += lineEnd;
        if ((wordEnd > wrapX && !isLineEmpty) || m_bNewLine) { // Word doesn't fit (or new line forced) - Finish the current line
            float spaceWidth = 0.0f;

            if (PS2Symbol) {
                it -= 3; // Begin the next line with the button
            }
            const auto* end = m_bNewLine ? it - 3 : it; // Don't print the `~n~`

            float lineX;
            if (m_bFontCentreAlign) {
                lineX = x - lineEnd * 0.5f;
            } else {
                if (m_bFontJustify) {
                    spaceWidth = (m_fWrapx - lineWidth) / (float)(numSpaces);
                }
                lineX = m_bFontRightAlign
                    ? x - (lineEnd - GetCharacterSize(0)) // Trailing space doesn't count
                    : x;
            }

            numLines++;
            if (print) {
                PrintString(lineX, lineY, lineStart, end, spaceWidth);
            }

            if (tag) { // Next line continues with the same color
                sprintf_s(gString, "~%c~", tag);

                // Same as `strcpy(lineContinuation, gString); strcat(lineContinuation, end)`, but `end` might point into `lineContinuation` already
                // NOTE: In that case the null terminator copied from `gString` may overwrite the rest of the text (eg.: if a button didn't fit into the line)
                //       Which also prevents an endless loop in that case (as the button would never fit)
                // NOTE: Overflows if the rest of the text doesn't fit
                auto* dst = lineContinuation.data();
                for (const auto* src = gString; (*dst = (GxtChar)(*src)) != '\0'; src++, dst++);
                if (m_bNewLine) {
                    end += 3;
                }
                for (const auto* src = end; (*dst = *src) != '\0'; src++, dst++);
                assert(dst < lineContinuation.data() + lineContinuation.size());

                it  = lineContinuation.data();
                tag = 0;
            }

            m_bNewLine = false;
            lineY += m_Scale.y * 32.0f * 0.5f + (m_Scale.y + m_Scale.y);
            lineEnd = IsLeftAligned() ? x : 0.0f;

            lineStart   = it;
            numSpaces   = 0;
            lineWidth   = 0.0f;
            isLineEmpty = true;
        } else { // Word fits, add it to the line
            lineEnd = wordEnd;

            it = GetNextSpace(it);
            if (!*it) { // End of the text, finish the last line
                const auto lineX = m_bFontCentreAlign
                    ? x - lineEnd * 0.5f
                    : m_bFontRightAlign
                        ? x - lineEnd
                        : x;

                numLines++;
                if (print) {
                    PrintString(lineX, lineY, lineStart, it, 0.0f);
                }
            } else {
                if (!isLineEmpty) {
                    numSpaces++;
                }
                if (*it != '~') { // Must be a space
                    lineEnd += GetCharacterSize(0);
                    it++;
                }
                lineWidth   = lineEnd;
                isLineEmpty = false;
            }
        }

        if (PS2Symbol) {
            PS2Symbol = EXSYMBOL_NONE;
        }
    }

    if (print) {
        SetColor(savedColor);
    }

    return numLines;
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
void CFont::GetTextRect(CRect* rect, float x, float y, const GxtChar* text) {
    if (m_bFontCentreAlign) {
        rect->left = x - (m_fFontCentreSize / 2.0f + 4.0f);
        rect->right = m_fFontCentreSize / 2.0f + x + 4.0f;
    }
    else if (m_bFontRightAlign) {
        rect->left = m_fRightJustifyWrap - 4.0f;
        rect->right = x;
    }
    else {
        rect->left = x - 4.0f;
        rect->right = m_fWrapx + 4.0f;
    }

    rect->top = y - 4.0f;
    rect->bottom = y + 4.0f + GetHeight() * (float)GetNumberLines(x, y, text);
}

// 0x71A700
void CFont::PrintString(float x, float y, const GxtChar* text) {
    if (*text == '\0' || *text == '*')
        return;

    if (m_bFontBackground) {
        CRect rt;

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

    if (m_fSlant != 0.0f)
        drawY -= (m_fSlantRefPoint.x - x) * m_fSlant + m_fSlantRefPoint.y;

    PrintString(x, drawY, text);
}

// 0x719750
float CFont::GetCharacterSize(uint8 letterId) {
    uint8 propValueIdx = letterId;

    if (letterId == '?') {
        letterId = 0;
        propValueIdx = 0;
    }

    if (m_FontStyle)
        propValueIdx = FindSubFontCharacter(letterId, m_FontStyle);
    else if (propValueIdx == 145)
        propValueIdx = '@';
    else if (propValueIdx > 155)
        propValueIdx = 0;

    if (m_bFontPropOn) {
        return ((float)gFontData[m_FontTextureId].m_propValues[propValueIdx] + (float)m_nFontOutlineSize) * m_Scale.x;
    } else {
        return ((float)gFontData[m_FontTextureId].m_unpropValue + (float)m_nFontOutlineSize) * m_Scale.x;
    }
}

// Android
float CFont::GetHeight(bool a1) {
    assert(a1 == false && "NOT IMPLEMENTED");
    const float y = a1 ? 0.0f : m_Scale.y;
    return y * 32.0f / 2.0f + y + y;
}

// Same as `CFont::GetCharacterSize`, but uses the style of the script text being set up
// 0x719670, original name unknown
float GetScriptLetterSize(uint8 letterId) {
    if (letterId == '?') {
        letterId = 0;
    }

    const auto& text = CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];

    uint8 fontTextureId, propValueIdx;
    switch (text.FontStyle) {
    case FONT_MENU:
        fontTextureId = 0;
        propValueIdx  = CFont::FindSubFontCharacter(letterId, 2);
        break;
    case FONT_PRICEDOWN:
        fontTextureId = 1;
        propValueIdx  = CFont::FindSubFontCharacter(letterId, 1);
        break;
    default:
        fontTextureId = (uint8)((eFontStyle)(text.FontStyle));
        propValueIdx  = letterId == 145
            ? '@'
            : letterId > 155
                ? 0
                : letterId;
        break;
    }

    const auto& fontData = gFontData[fontTextureId];
    const auto  width    = text.IsProportional
        ? fontData.m_propValues[propValueIdx]
        : fontData.m_unpropValue;
    return ((float)(width) + (float)(text.TextEdge)) * text.Scale.x;
}

// 0x7192C0
uint8 CFont::FindSubFontCharacter(uint8 letterId, uint8 fontStyle) {
    if (fontStyle == 1) { // eFontStyle::FONT_PRICEDOWN
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
float GetLetterIdPropValue(uint8 letterId) {
    uint8 id = letterId;

    if (letterId == '?')
        id = 0;

    if (CFont::RenderState.m_bPropOn)
        return gFontData[CFont::RenderState.m_wFontTexture].m_propValues[id];
    else
        return gFontData[CFont::RenderState.m_wFontTexture].m_unpropValue;
}
