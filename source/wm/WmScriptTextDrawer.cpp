#include "WmScriptTextDrawer.h"
#include "WmCompat.h"

WmScriptTextDrawer *WmScriptTextDrawer::m_TextDrawers;
unsigned __int16 *WmScriptTextDrawer::m_CurrentTextDrawerIndex;

void WmScriptTextDrawer::Init10U()
{
	m_TextDrawers= injector::raw_ptr(0xA913E8).get();
	m_CurrentTextDrawerIndex = injector::raw_ptr(0xA44B68).get();
}

void WmScriptTextDrawer::InitSteam()
{
	m_TextDrawers = injector::aslr_ptr(0xB08D40).get();
	m_CurrentTextDrawerIndex = injector::aslr_ptr(0xABC4D8).get();
}
