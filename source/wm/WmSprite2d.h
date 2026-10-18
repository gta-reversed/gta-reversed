#pragma once


struct RwTexture;

class WmRGBA;
class WmRect;

class WmSprite2d
{

public:
	RwTexture *m_pRwTexture;

public:
	WmSprite2d()
	{
		m_pRwTexture = nullptr;
	}

	static void(__cdecl *AddToBuffer)(WmRect  const&, WmRGBA  const&, float, float, float, float, float, float, float, float);

	void Delete();
	static void *fpDelete;

	void Draw(WmRect const&, WmRGBA const&);
	static void *fpDraw;

	static void(__cdecl *DrawRect)(WmRect  const&, WmRGBA  const&);
	static void(__cdecl *RenderVertexBuffer)();

	void SetRenderState();
	static void *fpSetRenderState;

	static void Init10U();
	static void InitSteam();
};
