#include "StdInc.h"

#include "StencilShadowObject.h"

void CStencilShadowObject::InjectHooks() {
    RH_ScopedClass(CStencilShadowObject);
    RH_ScopedCategoryGlobal();

    // RH_ScopedInstall(Shutdown, 0x711390);
    // RH_ScopedInstall(Render, 0x710D50);
    // RH_ScopedInstall(RenderForVehicle, 0x70FAE0);
    // RH_ScopedInstall(RenderForObject, 0x710310);
}

// 0x711280
CStencilShadowObject::CStencilShadowObject() {
    plugin::CallMethod<0x711280, CStencilShadowObject*>(this);
}

// 0x711310
CStencilShadowObject::~CStencilShadowObject() {
    plugin::CallMethod<0x711310, CStencilShadowObject*>(this);
}

// 0x711390
void CStencilShadowObject::Shutdown() {
    plugin::Call<0x711390>();
}

// 0x710D50
void CStencilShadowObject::Render(CRGBA* color) {
    plugin::Call<0x710D50, CRGBA*>(color);
}

// 0x70FAE0
void CStencilShadowObject::RenderForVehicle(CDummy* dummy) {
    plugin::Call<0x70FAE0, CDummy*>(dummy);
}

// 0x710310
void CStencilShadowObject::RenderForObject(CStencilShadowObject* object) {
    plugin::Call<0x710310, CStencilShadowObject*>(object);
}

// 0x7113B0
bool gRenderStencil() {
    return plugin::CallAndReturn<bool, 0x7113B0>();

    // todo:
    const auto depth = RwRasterGetDepth(RwCameraGetRaster(Scene.m_pRwCamera));
    if ( g_fx.GetFxQuality() < FXQUALITY_MEDIUM || depth < 32 )
        return false;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,                RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,            RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,                 RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,                RWRSTATE(2u));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONMASK,      RWRSTATE(-1u));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONWRITEMASK, RWRSTATE(-1u));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTION,          RWRSTATE(8u));
    RwRenderStateSet(rwRENDERSTATESTENCILFAIL,              RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESTENCILPASS,              RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONREF,       RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(7u));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(3u));

    const auto black = CRGBA(0, 0, 0, 255);
    // RenderStencil(black); // 0x710D50

    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(8u));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(2u));

    // RenderStencil(black);

    // WTF is up with these states?
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(0)); // same state
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(2u));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(2u));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,                RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(1u)); // same state
    RwRenderStateSet(rwRENDERSTATESRCBLEND,                 RWRSTATE(5u));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,                RWRSTATE(6u));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,            RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONREF,       RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTION,          RWRSTATE(4u));
    RwRenderStateSet(rwRENDERSTATESTENCILFAIL,              RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESTENCILPASS,              RWRSTATE(1u));

    CSprite2d::InitPerFrame();

    auto color = CRGBA(0, 0, 0, 50u);
    color.a = (color.a * CTimeCycle::m_CurrentColours.m_nShadowStrength) >> 8;
    CRect rect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
    CSprite2d::DrawRect(rect, color);

    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(2u));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(2u));

    return true;
}
