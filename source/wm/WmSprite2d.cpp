#include "WmSprite2d.h"
#include "WmCompat.h"

void(__cdecl *WmSprite2d::AddToBuffer)(WmRect  const& posn, WmRGBA  const& color, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4);
void *WmSprite2d::fpDelete;
void *WmSprite2d::fpDraw;
void(__cdecl *WmSprite2d::DrawRect)(WmRect  const& posn, WmRGBA  const& color);
void(__cdecl *WmSprite2d::RenderVertexBuffer)();
void *WmSprite2d::fpSetRenderState;

void WmSprite2d::Delete()
{
	((void(__thiscall *)(WmSprite2d *))fpDelete)(this);
}

void WmSprite2d::Draw(WmRect const&pos, WmRGBA const&color)
{
	((void(__thiscall *)(WmSprite2d*, WmRect const&, WmRGBA const&))fpDraw)(this, pos, color);
}

void WmSprite2d::SetRenderState()
{
	((void(__thiscall *)(WmSprite2d *))fpSetRenderState)(this);
}

void WmSprite2d::Init10U()
{
	AddToBuffer = injector::raw_ptr(0x728200).get();
	fpDelete = injector::raw_ptr(0x727240).get();
	fpDraw = injector::raw_ptr(0x728350).get();
	DrawRect = injector::raw_ptr(0x727B60).get();
	RenderVertexBuffer = injector::raw_ptr(0x7273D0).get();
	fpSetRenderState = injector::raw_ptr(0x727B30).get();
}

void WmSprite2d::InitSteam()
{
	AddToBuffer = injector::aslr_ptr(0x757D90).get();
	fpDelete = injector::aslr_ptr(0x756980).get();
	fpDraw = injector::aslr_ptr(0x757EE0).get();
	DrawRect = injector::aslr_ptr(0x757690).get();
	RenderVertexBuffer = injector::aslr_ptr(0x756B20).get();
	fpSetRenderState = injector::aslr_ptr(0x757660).get();
}
