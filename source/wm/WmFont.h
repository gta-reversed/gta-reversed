#pragma once
#include "game.h"


class WmSprite2d;

struct WmLetterWidths
{
    unsigned __int8 PropValues[209];
    unsigned __int8 UnpropValue;
};

class WmRenderState
{
public:
    float		Useless;
    WmVec2	Pos;
    WmVec2	Scale;
    WmRGBA		Color;
    float		JustifyWrap;
    float		Slant;
    WmVec2	SlantRefPoint;
    bool		IsBlip;
    __int8		FontStyle;
    bool		Prop;
    __int8		_pad1;
    __int16		TextureID;
    __int8		OutlineSize;
    __int8		_pad2;
};
static_assert(sizeof(WmRenderState) == 0x30, "Class WmRenderState is wrong.");

class WmDetails
{
    WmRGBA Color;
    WmVec2 Scale;
    float Slant;
    WmVec2 SlantRefPoint;
    bool Justify;
    bool Centre;
    bool RightJustify;
    bool Background;
    bool EnlargeBackground;
    bool Prop;
    bool IsBlip;
    __int8 pad1;
    float Alpha;
    WmRGBA BackgroundColor;
    float WrapX;
    float CentreSize;
    float RightJustifyWrap;
    __int8 TextureID;
    __int8 FontStyle;
    __int8 Shadow;
    WmRGBA DropColor;
    __int8 OutlineSize;
    __int8 pad2[3];
};
static_assert(sizeof(WmDetails) == 0x40, "Struct WmDetails is wrong.");

union WmBufPtr
{
    WmRenderState *pdata;
    const char *ptext;
    unsigned __int32 addr;
};
static_assert(sizeof(WmBufPtr) == 0x4, "Union WmBufPtr is wrong.");

class WmFont
{
public:
    static float fix_value_2;
    static float fix_value_2_chs;

    static const float scale_x_rec;
    static const float scale_y_rec;
    static const float scale_x_rec_chs;
    static const float scale_y_rec_chs;
    static const float fix_value_1;
    static const float fix_value_1_chs;






    static const unsigned char SBCLetterWidth = 32;

    static WmSprite2d *m_Sprites;
    static WmSprite2d m_ChsSprite;
    static WmSprite2d m_GlyphSprite;
    static WmSprite2d *m_ButtonSprites;

    static WmLetterWidths *m_FontWidths;

    static WmRenderState *m_FontBuffer;
    static WmBufPtr *m_FontBufferIter;
    static WmRenderState *RenderState;

    static unsigned __int8 *m_nPS2SymbolId;
    static bool *m_bNewLine;
    static WmRGBA *m_Color;
    static WmVec2 *m_fScale;
    static bool *m_bFontJustify;
    static bool *m_bFontCentreAlign;
    static bool *m_bFontRightAlign;
    static bool *m_bFontPropOn;
    static float *m_fWrapx;
    static float *m_fFontCentreSize;
    static float *m_fRightJustifyWrap;
    static unsigned __int8 *m_nFontTextureID;
    static unsigned __int8 *m_nFontStyle;
    static __int8 *m_nFontOutlineSize;

    static unsigned char(__cdecl *FindSubFontCharacter)(unsigned char, unsigned char);
    static const char *(__cdecl *ParseToken)(const char *, WmRGBA &, bool, char *);
    static void(__cdecl *PrintKeyTokenFormat)(char *);
    static void(__cdecl *RenderString)(float, float, const char *, const char *, float);
    static void(__cdecl *SetColor)(WmRGBA);

    static float GetScaledLetterWidthNormal(unsigned __int32);
    static float GetScaledLetterWidthScript(unsigned __int32);
    static float GetScaledLetterWidthDrawing(unsigned __int32);

    static float GetStringWidth(const char *, bool, bool);
    static const char *GetNextSpace(const char *);

    static short ProcessCurrentString(bool, float, float, const char *);

    static void RenderFontBuffer();

    static void PrintCHSChar(float, float, unsigned __int32);

    static void Init10U();
    static void InitSteam();
};
