#pragma once


class WmSprite
{
public:
	static void(__cdecl * FlushSpriteBuffer)();

	static void Init10U();
	static void InitSteam();
};
