#include "WmRw.h"
#include "WmCompat.h"

RwGlobals **WmRw::m_RwEngineInstance;
RsGlobalType *WmRw::m_RsGlobal;

RwImage *(__cdecl *WmRw::RtPNGImageRead)(const char *filename);
int(__cdecl *WmRw::RwImageDestroy)(RwImage *);
RwImage *(__cdecl *WmRw::RwImageFindRasterFormat)(RwImage *, int, int *, int *, int *, int *);
RwRaster *(__cdecl *WmRw::RwRasterCreate)(int, int, int, int);
RwRaster *(__cdecl *WmRw::RwRasterSetFromImage)(RwRaster *, RwImage *);
RwTexture *(__cdecl *WmRw::RwTextureCreate)(RwRaster *);

void WmRw::Init10U()
{
	m_RwEngineInstance = injector::raw_ptr(0xC97B24).get();
	m_RsGlobal = injector::raw_ptr(0xC17040).get();

	RtPNGImageRead = injector::raw_ptr(0x7CF9B0).get();
	RwImageDestroy = injector::raw_ptr(0x802740).get();
	RwImageFindRasterFormat = injector::raw_ptr(0x8042C0).get();
	RwRasterCreate = injector::raw_ptr(0x7FB230).get();
	RwRasterSetFromImage = injector::raw_ptr(0x804290).get();
	RwTextureCreate = injector::raw_ptr(0x7F37C0).get();
}

void WmRw::InitSteam()
{
	m_RwEngineInstance = injector::aslr_ptr(0xD23664).get();
	m_RsGlobal = injector::aslr_ptr(0xCA3DAC).get();

	RtPNGImageRead = injector::aslr_ptr(0x8041C0).get();
	RwImageDestroy = injector::aslr_ptr(0x836F90).get();
	RwImageFindRasterFormat = injector::aslr_ptr(0x838B10).get();
	RwRasterCreate = injector::aslr_ptr(0x82FA80).get();
	RwRasterSetFromImage = injector::aslr_ptr(0x838AE0).get();
	RwTextureCreate = injector::aslr_ptr(0x828000).get();
}
