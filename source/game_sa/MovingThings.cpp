#include "StdInc.h"

#include "MovingThings.h"
#include "PlaneTrails.h"
#include "Escalators.h"
#include "PointLights.h"

void CMovingThings::InjectHooks() {
    RH_ScopedClass(CMovingThings);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x717D00);
    RH_ScopedInstall(Shutdown, 0x717C20);
    RH_ScopedInstall(Update, 0x7185B0);
    RH_ScopedInstall(Render, 0x717150);
    RH_ScopedInstall(Render_BeforeClouds, 0x7178F0);
}

// 0x717D00
void CMovingThings::Init() {
    CPlaneTrails::Init();
    CPointLights::Init();
    CEscalators::Init();
}

// 0x717C20
void CMovingThings::Shutdown() {
    CEscalators::Shutdown();
}

// 0x7185B0
void CMovingThings::Update() {
    ZoneScoped;

    CPlaneTrails::Update();
    CEscalators::Update();
}

// 0x717150
void CMovingThings::Render() {
    ZoneScoped;

    // NOP
}

// 0x7178F0
void CMovingThings::Render_BeforeClouds() {
    CPlaneTrails::Render();
}
