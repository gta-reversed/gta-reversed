#include "WmFont.h"
#include "WmSprite.h"
#include "WmSprite2d.h"
#include "WmCharTable.h"
#include "WmScriptTextDrawer.h"
#include "WmRw.h"
#include "WmDWrite.h"

#include "WmCompat.h"


float WmFont::fix_value_2;
float WmFont::fix_value_2_chs;

const float WmFont::scale_x_rec = 1.0f / 16.0f;
const float WmFont::scale_y_rec = 1.0f / 12.8f;
const float WmFont::scale_x_rec_chs = 1.0f / 64.0f;
const float WmFont::scale_y_rec_chs = 1.0f / 51.2f;
const float WmFont::fix_value_1 = 0.0021f;
const float WmFont::fix_value_1_chs = 0.0021f / 4.0f;

WmSprite2d *WmFont::m_Sprites;
WmSprite2d WmFont::m_ChsSprite;
WmSprite2d WmFont::m_GlyphSprite;
WmSprite2d *WmFont::m_ButtonSprites;

WmLetterWidths *WmFont::m_FontWidths;
WmRenderState *WmFont::m_FontBuffer;
WmBufPtr *WmFont::m_FontBufferIter;
WmRenderState *WmFont::RenderState;

unsigned __int8 *WmFont::m_nPS2SymbolId;
bool *WmFont::m_bNewLine;
WmRGBA *WmFont::m_Color;
WmVec2 *WmFont::m_fScale;
bool *WmFont::m_bFontJustify;
bool *WmFont::m_bFontCentreAlign;
bool *WmFont::m_bFontRightAlign;
bool *WmFont::m_bFontPropOn;
float *WmFont::m_fWrapx;
float *WmFont::m_fFontCentreSize;
float *WmFont::m_fRightJustifyWrap;
unsigned __int8 *WmFont::m_nFontTextureID;
unsigned __int8 *WmFont::m_nFontStyle;
__int8 *WmFont::m_nFontOutlineSize;

unsigned char(__cdecl *WmFont::FindSubFontCharacter)(unsigned char, unsigned char);
const char *(__cdecl *WmFont::ParseToken)(const char *, WmRGBA &, bool, char *);
void(__cdecl *WmFont::PrintKeyTokenFormat)(char *);
void(__cdecl *WmFont::RenderString)(float, float, const char *, const char *, float);
void(__cdecl *WmFont::SetColor)(WmRGBA);

float __cdecl WmFont::GetScaledLetterWidthNormal(unsigned __int32 arg_char)
{
    unsigned char charWidth;

    if (arg_char >= 0x60)
    {
        charWidth = SBCLetterWidth;
    }
    else
    {
        if (arg_char == '?')
        {
            arg_char = 0;
        }

        if (*m_nFontStyle != 0)
        {
            arg_char = FindSubFontCharacter(arg_char, *m_nFontStyle);
        }

        if (*m_bFontPropOn)
        {
            charWidth = m_FontWidths[*m_nFontTextureID].PropValues[arg_char];
        }
        else
        {
            charWidth = m_FontWidths[*m_nFontTextureID].UnpropValue;
        }
    }

    return ((charWidth + *m_nFontOutlineSize) * m_fScale->x);
}

float WmFont::GetScaledLetterWidthScript(unsigned __int32 arg_char)
{
    WmScriptTextDrawer &drawer = WmScriptTextDrawer::m_TextDrawers[*WmScriptTextDrawer::m_CurrentTextDrawerIndex];
    
    unsigned char charWidth, style;

    if (arg_char >= 0x60)
    {
        charWidth = SBCLetterWidth;
    }
    else
    {
        if (arg_char == '?')
        {
            arg_char = 0;
        }

        switch (drawer.m_nFontStyle)
        {
        case 2:
            style = 0;
            arg_char = FindSubFontCharacter(arg_char, 2);
            break;

        case 3:
            style = 1;
            arg_char = FindSubFontCharacter(arg_char, 1);
            break;

        default:
            style = drawer.m_nFontStyle;
            break;
        }

        if (drawer.m_bIsProportional)
        {
            charWidth = m_FontWidths[style].PropValues[arg_char];
        }
        else
        {
            charWidth = m_FontWidths[style].UnpropValue;
        }
    }

    return ((charWidth + drawer.m_OutlineSize) * drawer.m_LetterScale.x);
}

float __cdecl WmFont::GetScaledLetterWidthDrawing(unsigned __int32 arg_char)
{
    unsigned char charWidth;

    if (arg_char >= 0x60)
    {
        charWidth = SBCLetterWidth;
    }
    else
    {
        if (arg_char == '?')
        {
            arg_char = 0;
        }

        if (RenderState->FontStyle != 0)
        {
            arg_char = FindSubFontCharacter(arg_char, RenderState->FontStyle);
        }

        if (RenderState->Prop)
        {
            charWidth = m_FontWidths[RenderState->TextureID].PropValues[arg_char];
        }
        else
        {
            charWidth = m_FontWidths[RenderState->TextureID].UnpropValue;
        }
    }

    return ((charWidth + RenderState->OutlineSize) * RenderState->Scale.x);
}

float WmFont::GetStringWidth(const char *arg_text, bool bGetAll, bool bScript)
{
    char strbuf[400];
    char *bufIter = strbuf;

    float result = 0.0f;

    bool succeeded = false;
    bool StopAtDelim = false;
    
    strncpy(strbuf, arg_text, 400);
    strbuf[399] = 0;

    PrintKeyTokenFormat(strbuf);

    while (true)
    {
        unsigned __int32 code = utf8::unchecked::peek_next(bufIter);

        if (code == '\0')
        {
            break;
        }
        else if (code == ' ' && !bGetAll)
        {
            break;
        }
        else if (code == '~')
        {
            if (!bGetAll && (StopAtDelim || succeeded))
            {
                break;
            }

            do 
            {
                ++bufIter;
            } while (*bufIter != '~');

            ++bufIter;

            if (succeeded || *bufIter == '~')
            {
                StopAtDelim = true;
            }
        }
        else if (code >= 0x80)
        {
            if (bGetAll || !succeeded)
            {
                if (bScript)
                {
                    result += GetScaledLetterWidthScript(code - 0x20);
                }
                else
                {
                    result += GetScaledLetterWidthNormal(code - 0x20);
                }

                succeeded = true;
            }

            if (!bGetAll)
            {
                break;
            }
        }
        else
        {
            if (!bGetAll && code == ' ' && StopAtDelim)
            {
                break;
            }

            if (bScript)
            {
                result += GetScaledLetterWidthScript(code - 0x20);
            }
            else
            {
                result += GetScaledLetterWidthNormal(code - 0x20);
            }

            succeeded = true;
        }

        utf8::unchecked::next(bufIter);
    }

    return result;
}

const char *WmFont::GetNextSpace(const char *arg_pointer)
{
    const char *var_pointer = arg_pointer;

    while (true)
    {
        unsigned __int32 code = utf8::unchecked::peek_next(var_pointer);

        if (code == 0 || code == ' ' || code == '~')
        {
            break;
        }
        else if (code >= 0x80)
        {
            if (var_pointer == arg_pointer)
            {
                utf8::unchecked::next(var_pointer);
            }

            break;
        }

        utf8::unchecked::next(var_pointer);
    }

    return var_pointer;
}

short WmFont::ProcessCurrentString(bool print, float arg_x, float arg_y, const char *arg_text)
{
    const char *esi = arg_text;
    const char *ebp = esi;
    const char *edi;

    __int16 result = 0;
    __int16 numWords = 0;

    bool emptyLine = true;
    char tag = '\0';

    WmRGBA fontColor = *m_Color;
    WmRGBA tagColor;

    // A narrow box (HUD zone/radio names, labels) was sized for latin text and fits only
    // ~3 full-width CJK glyphs, so names used to break mid-word. Keep a short,
    // space-less CJK/latin run on one line instead of hard-breaking it.
    bool keepOneLine = false;
    if (arg_text && (*m_bFontCentreAlign || *m_bFontRightAlign)) {
        const float limit = *m_bFontCentreAlign ? *m_fFontCentreSize : *m_fRightJustifyWrap;
        if (limit > 0.0f && limit < 400.0f) {
            keepOneLine = true;
            for (const unsigned char* q = (const unsigned char*)arg_text; *q; ) {
                if (*q == '~') {                       // colour/format tag: skip ~...~
                    const unsigned char* end = (const unsigned char*)strchr((const char*)q + 1, '~');
                    if (!end) {
                        keepOneLine = false;
                        break;
                    }
                    q = end + 1;
                    continue;
                }
                ++q;
            }
            // still wrap when the run is far wider than the box (real sentences)
            if (keepOneLine && GetStringWidth(arg_text, true, false) > limit * 4.0f) {
                keepOneLine = false;
            }
        }
    }

    float xBound;
    float yBound = arg_y;
    float strWidth, widthLimit;
    float var_110 = 0.0f;
    float var_10C;
    float var_124;

    char var_100[256];

    if (*m_bFontCentreAlign || *m_bFontRightAlign)
    {
        xBound = 0.0f;
    }
    else
    {
        xBound = arg_x;
    }

    while (*esi != '\0')
    {
        *m_nPS2SymbolId = 0;
        strWidth = GetStringWidth(esi, false, false);

        if (*esi == '~')
        {
            esi = ParseToken(esi, tagColor, true, &tag);
        }

        if (*m_bFontCentreAlign)
        {
            widthLimit = *m_fFontCentreSize;
        }
        else if (*m_bFontRightAlign)
        {
            widthLimit = arg_x - *m_fRightJustifyWrap;
        }
        else
        {
            widthLimit = *m_fWrapx;
        }

        strWidth += xBound;

        if ((strWidth <= widthLimit || emptyLine || keepOneLine) && !*m_bNewLine)
        {
            xBound = strWidth;
            esi = GetNextSpace(esi);

            if (*esi != '\0')
            {
                if (!emptyLine)
                {
                    ++numWords;
                }

                if (*esi == ' ')
                {
                    xBound += GetScaledLetterWidthNormal(0);
                    ++esi;
                }

                var_110 = xBound;
                emptyLine = false;
            }
            else
            {
                if (*m_bFontCentreAlign)
                {
                    var_124 = arg_x - xBound * 0.5f;
                }
                else if (*m_bFontRightAlign)
                {
                    var_124 = arg_x - xBound;
                }
                else
                {
                    var_124 = arg_x;
                }

                ++result;

                if (print)
                {
                    RenderString(var_124, yBound, ebp, esi, 0.0f);
                }
            }
        }
        else
        {
            var_10C = 0.0f;
            if (*m_nPS2SymbolId != 0)
            {
                esi -= 3;
            }

            edi = esi - 3;

            if (!*m_bNewLine)
            {
                edi = esi;
            }

            if (*m_bFontCentreAlign)
            {
                var_124 = arg_x - xBound * 0.5f;
            }
            else
            {
                if (*m_bFontJustify)
                {
                    var_10C = (*m_fWrapx - var_110) / numWords;
                }

                if (*m_bFontRightAlign)
                {
                    var_124 = arg_x - (xBound - GetScaledLetterWidthNormal(0));
                }
                else
                {
                    var_124 = arg_x;
                }
            }

            ++result;

            if (print)
            {
                RenderString(var_124, yBound, ebp, edi, var_10C);
            }

            if (tag != '\0')
            {
                var_100[0] = '~';
                var_100[1] = tag;
                var_100[2] = '~';

                if (*m_bNewLine)
                {
                    edi += 3;
                }

                strcpy(&var_100[3], edi);

                esi = var_100;
                tag = '\0';
            }

            *m_bNewLine = false;
            yBound += m_fScale->y * 18.0f;

            if (*m_bFontCentreAlign || *m_bFontRightAlign)
            {
                xBound = 0.0f;
            }
            else
            {
                xBound = arg_x;
            }

            ebp = esi;
            emptyLine = true;
            var_110 = 0.0f;
            numWords = 0;
        }

        *m_nPS2SymbolId = 0;
    };

    if (print)
    {
        SetColor(fontColor);
    }

    return result;
}

void WmFont::RenderFontBuffer()
{
    WmRGBA var_color;
    WmVec2 pos;
    unsigned __int32 var_char;

    WmBufPtr ebx;

    if (m_FontBufferIter->pdata == m_FontBuffer)
    {
        return;
    }

    RenderState = m_FontBuffer;

    var_color = RenderState->Color;

    pos = RenderState->Pos;

    ebx.pdata = m_FontBuffer + 1;

    while (ebx.addr < m_FontBufferIter->addr)
    {
        if (*ebx.ptext == '\0')
        {
            ++ebx.ptext;

            while ((ebx.addr & 3) != 0)
            {
                ++ebx.ptext;
            }

            if (ebx.addr >= m_FontBufferIter->addr)
            {
                break;
            }

            *RenderState = *ebx.pdata;

            var_color = RenderState->Color;

            pos = RenderState->Pos;

            ++ebx.pdata;
        }

        *m_nPS2SymbolId = 0;

        while (*ebx.ptext == '~')
        {
            if (*m_nPS2SymbolId != 0)
            {
                break;
            }

            ebx.ptext = ParseToken(ebx.ptext, var_color, RenderState->IsBlip, nullptr);

            if (!RenderState->IsBlip)
            {
                RenderState->Color = var_color;
            }
        }

        var_char = utf8::unchecked::peek_next(ebx.ptext) - 0x20;

        if (RenderState->Slant != 0.0f)
        {
            pos.y = (RenderState->SlantRefPoint.x - pos.x) * RenderState->Slant + RenderState->SlantRefPoint.y;
        }

        if (*m_nPS2SymbolId == 0 || !RenderState->IsBlip)
        {
            if (var_char < 0x60)
            {
                m_Sprites[RenderState->TextureID].SetRenderState();
            }
            else
            {
                // NotSA: glyph from a cached DirectWrite texture instead of the PNG atlas
                const int glyphPx = (int)(RenderState->Scale.y * 20.0f * 4.0f + 0.5f);   // 4 px/unit = wm_sachs atlas density
                m_GlyphSprite.m_pRwTexture = WmDWrite::GetGlyph(var_char + 0x20, glyphPx);
                m_GlyphSprite.SetRenderState();
            }

            (*WmRw::m_RwEngineInstance)->dOpenDevice.fpRenderStateSet(RwRenderState::rwRENDERSTATEVERTEXALPHAENABLE, (void *)1);

            PrintCHSChar(pos.x, pos.y, var_char);
            WmSprite::FlushSpriteBuffer();
            WmSprite2d::RenderVertexBuffer();
        }

        if (*m_nPS2SymbolId == 0)
        {
            pos.x += GetScaledLetterWidthDrawing(var_char);
        }
        else
        {
            pos.x += (RenderState->Scale.y * 17.0f + RenderState->OutlineSize);
        }

        if (var_char == 0)
        {
            pos.x += RenderState->JustifyWrap;
        }

        if (*m_nPS2SymbolId != 0)
        {
            *m_nPS2SymbolId = 0;
        }
        else if (*ebx.ptext != '\0')
        {
            utf8::unchecked::next(ebx.ptext);
        }
    }

    m_FontBufferIter->pdata = m_FontBuffer;
}

void WmFont::PrintCHSChar(float arg_x, float arg_y, unsigned __int32 arg_char)
{
    WmRect rect;
    float row, column;
    WmCharTable::CharPos cpos;

    if (arg_y < 0.0f || WmRw::m_RsGlobal->maximumHeight < arg_y || arg_x < 0.0f || WmRw::m_RsGlobal->maximumWidth < arg_x)
    {
        return;
    }

    if (*m_nPS2SymbolId != 0)
    {
        rect.y2 = RenderState->Scale.y * 2.0f + arg_y;
        rect.x2 = RenderState->Scale.y * 17.0f + arg_x;
        rect.y1 = RenderState->Scale.y * 19.0f + arg_y;
        rect.x1 = arg_x;
        m_ButtonSprites[*m_nPS2SymbolId].Draw(rect, WmRGBA(255, 255, 255, RenderState->Color.alpha));
        return;
    }

    if (arg_char == 0 || arg_char == '?')
    {
        return;
    }

    cpos = WmCharTable::GetCharPos(arg_char);

    row = cpos.rowIndex;
    column = cpos.columnIndex;

    rect.x1 = arg_x;
    rect.y2 = arg_y;
    rect.x2 = RenderState->Scale.x * 32.0f + arg_x;
    rect.y1 = RenderState->Scale.y * 20.0f + arg_y;

    if (arg_char < 0x60)
    {
        row *= scale_y_rec;
        column *= scale_x_rec;

        WmSprite2d::AddToBuffer(rect, RenderState->Color,
            column,
            row + fix_value_2,
            column + scale_x_rec - fix_value_1,
            row + fix_value_2,
            column,
            row + scale_y_rec - fix_value_2,
            column + scale_x_rec - fix_value_1,
            row + scale_y_rec - fix_value_2);
    }
    else
    {
        // NotSA: per-glyph texture -> full UV range (V flipped: texture rows are bottom-up)
        WmSprite2d::AddToBuffer(rect, RenderState->Color,
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f);
    }
}

void WmFont::Init10U()
{
    fix_value_2 = 0.0021f;
    fix_value_2_chs = 0.0021f / 4.0f;

    m_Sprites = injector::raw_ptr(0xC71AD0).get();
    m_ButtonSprites = injector::raw_ptr(0xC71AD8).get();

    m_FontWidths = injector::raw_ptr(0xC718B0).get();

    m_FontBuffer = injector::raw_ptr(0xC716B0).get();
    m_FontBufferIter = injector::raw_ptr(0xC716A8).get();
    RenderState = injector::raw_ptr(0xC71AA0).get();

    m_nPS2SymbolId = injector::raw_ptr(0xC71A54).get();
    m_bNewLine = injector::raw_ptr(0xC71A55).get();
    m_Color = injector::raw_ptr(0xC71A60).get();
    m_fScale = injector::raw_ptr(0xC71A64).get();
    m_bFontJustify = injector::raw_ptr(0xC71A78).get();
    m_bFontCentreAlign = injector::raw_ptr(0xC71A79).get();
    m_bFontRightAlign = injector::raw_ptr(0xC71A7A).get();
    m_bFontPropOn = injector::raw_ptr(0xC71A7D).get();
    m_fWrapx = injector::raw_ptr(0xC71A88).get();
    m_fFontCentreSize = injector::raw_ptr(0xC71A8C).get();
    m_fRightJustifyWrap = injector::raw_ptr(0xC71A90).get();
    m_nFontTextureID = injector::raw_ptr(0xC71A94).get();
    m_nFontStyle = injector::raw_ptr(0xC71A95).get();
    m_nFontOutlineSize = injector::raw_ptr(0xC71A9B).get();

    FindSubFontCharacter = injector::raw_ptr(0x7192C0).get();
    ParseToken = injector::raw_ptr(0x718F00).get();
    PrintKeyTokenFormat = injector::raw_ptr(0x69E160).get();
    RenderString = injector::raw_ptr(0x719B40).get();
    SetColor = injector::raw_ptr(0x719430).get();
}

void WmFont::InitSteam()
{
    fix_value_2 = 0.005f;
    fix_value_2_chs = 0.005f / 4.0f;

    m_Sprites = injector::aslr_ptr(0xCD20A8).get();
    m_ButtonSprites = injector::aslr_ptr(0xCD1FFC).get();

    m_FontWidths = injector::aslr_ptr(0xCD1E58).get();

    m_FontBuffer = injector::aslr_ptr(0xCD1C58).get();
    m_FontBufferIter = injector::aslr_ptr(0xCD1C50).get();
    RenderState = injector::aslr_ptr(0xCD2078).get();

    m_nPS2SymbolId = injector::aslr_ptr(0xCD1C54).get();
    m_bNewLine = injector::aslr_ptr(0xCD1C55).get();
    m_Color = injector::aslr_ptr(0xCD2038).get();
    m_fScale = injector::aslr_ptr(0xCD203C).get();
    m_bFontJustify = injector::aslr_ptr(0xCD2050).get();
    m_bFontCentreAlign = injector::aslr_ptr(0xCD2051).get();
    m_bFontRightAlign = injector::aslr_ptr(0xCD2052).get();
    m_bFontPropOn = injector::aslr_ptr(0xCD2055).get();
    m_fWrapx = injector::aslr_ptr(0xCD2060).get();
    m_fFontCentreSize = injector::aslr_ptr(0xCD2064).get();
    m_fRightJustifyWrap = injector::aslr_ptr(0xCD2068).get();
    m_nFontTextureID = injector::aslr_ptr(0xCD206C).get();
    m_nFontStyle = injector::aslr_ptr(0xCD206D).get();
    m_nFontOutlineSize = injector::aslr_ptr(0xCD2073).get();

    FindSubFontCharacter = injector::aslr_ptr(0x736B30).get();
    ParseToken = injector::aslr_ptr(0x736690).get();
    PrintKeyTokenFormat = injector::aslr_ptr(0x6CBC50).get();
    RenderString = injector::aslr_ptr(0x737440).get();
    SetColor = injector::aslr_ptr(0x736CA0).get();
}
