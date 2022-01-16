#include "StdInc.h"

// CStencilShadowObject* CStencilShadows::m_StencilShadowObjects = reinterpret_cast<CStencilShadowObject(*)[64]>(0xC6A198);
CStencilShadowObject* CStencilShadows::pFirstAvailableStencilShadowObject = reinterpret_cast<CStencilShadowObject *>(0xC6A168);
CStencilShadowObject* CStencilShadows::pFirstActiveStencilShadowObject = reinterpret_cast<CStencilShadowObject *>(0xC6A16C);

void CStencilShadows::InjectHooks() {
//    ReversibleHooks::Install("CStencilShadows", "Init", 0x70F9E0, &CStencilShadows::Init);
//    ReversibleHooks::Install("CStencilShadows", "Process", 0x711D90, &CStencilShadows::Process);
}

// 0x70F9E0
void CStencilShadows::Init() {
    plugin::Call<0x70F9E0>();
}

// 0x711D90
void CStencilShadows::Process(RwV3d* cameraPos) {
    plugin::Call<0x711D90, RwV3d*>(cameraPos);
}
