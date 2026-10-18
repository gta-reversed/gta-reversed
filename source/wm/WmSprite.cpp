#include "WmSprite.h"
#include "WmCompat.h"

void(__cdecl *WmSprite::FlushSpriteBuffer)();

void WmSprite::Init10U()
{
	FlushSpriteBuffer = injector::raw_ptr(0x70CF20).get();
}

void WmSprite::InitSteam()
{
	FlushSpriteBuffer = injector::aslr_ptr(0x753AE0).get();
}
