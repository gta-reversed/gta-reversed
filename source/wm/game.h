#pragma once

class WmRGBA
{
public:
	unsigned __int8 red, green, blue, alpha;

	WmRGBA() = default;
	WmRGBA(unsigned __int8 r, unsigned __int8 g, unsigned __int8 b, unsigned __int8 a)
	{
		red = r;
		green = g;
		blue = b;
		alpha = a;
	}
};

class WmVec2
{
public:
	float x, y;
};

class WmRect
{
public:
	float x1;
	float y1;
	float x2;
	float y2;

	WmRect()
	{
		x1 = 1000000.0f;
		y1 = -1000000.0f;
		x2 = -1000000.0f;
		y2 = 1000000.0f;
	}
};
