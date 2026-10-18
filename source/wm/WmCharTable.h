#pragma once
#include "WmFont.h"

class WmCharTable
{
public:
	struct CharPos
	{
		unsigned __int8 rowIndex;
		unsigned __int8 columnIndex;
	};

	static CharPos GetCharPos(unsigned __int32 chr);

	static void InitTable();

private:
	static CharPos m_Table[];
};
