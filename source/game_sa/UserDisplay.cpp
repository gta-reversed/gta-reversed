#include "StdInc.h"

#include "UserDisplay.h"
#include "CurrentVehicle.h"
#include "PlaceName.h"

CPlaceName& CUserDisplay::PlaceName = *(CPlaceName*)0xBA18F4;
COnscreenTimer& CUserDisplay::OnscnTimer = *(COnscreenTimer*)0xBA1788;
CCurrentVehicle& CUserDisplay::CurrentVehicle = *(CCurrentVehicle*)0xBA18FC;

void CUserDisplay::InjectHooks() {
    RH_ScopedClass(CUserDisplay);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x571EE0);
    RH_ScopedInstall(Process, 0x5720A0);
}

// 0x571EE0
void CUserDisplay::Init() {
    ZoneScoped;

    PlaceName.Init();
    OnscnTimer.Init();
    CurrentVehicle.Init();
}

// 0x5720A0
void CUserDisplay::Process() {
    ZoneScoped;

    PlaceName.Process();
    OnscnTimer.Process();
    CurrentVehicle.Process();
}
