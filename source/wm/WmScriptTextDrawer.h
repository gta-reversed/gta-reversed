#pragma once

#include "game.h"

class WmScriptTextDrawer
{
public:
	static WmScriptTextDrawer *m_TextDrawers;
	static unsigned __int16 *m_CurrentTextDrawerIndex;

	WmVec2		m_LetterScale;
	WmRGBA			m_Color;
	__int8			field_C;
	bool			m_bCentered;
	bool			m_bWithBackGround;
	__int8			pad_F;
	WmVec2		m_LineScale;
	WmRGBA			m_BackgroundBoxColor;
	bool			m_bIsProportional;
	WmRGBA			m_BackgroundColor;
	unsigned __int8 m_ShadowSize;
	__int8			m_OutlineSize;
	__int8			field_23;
	bool			m_bIsAlignRight;
	__int8			pad_25;
	__int8			pad_26;
	__int8			pad_27;
	unsigned __int32 m_nFontStyle;
	WmVec2		m_Position;
	char			m_aGxtEntry[8];
	__int32			param1;
	__int32			param2;

	static void Init10U();
	static void InitSteam();
};
static_assert(sizeof(WmScriptTextDrawer) == 0x44, "Class WmScriptTextDrawer is wrong.");
