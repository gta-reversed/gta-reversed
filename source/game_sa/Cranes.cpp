#include "StdInc.h"

#include "Cranes.h"

void CCranes::InjectHooks() {
    RH_ScopedClass(CCranes);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(InitCranes, 0x6F3FC0);
    RH_ScopedInstall(UpdateCranes, 0x6F3FE0);
    RH_ScopedInstall(IsThisCarBeingCarriedByAnyCrane, 0x6F3FF0);
    RH_ScopedInstall(IsThisCarBeingTargettedByAnyCrane, 0x6F4000);
    RH_ScopedInstall(IsThisCarPickedUp, 0x6F3FD0);
}

// 0x6F3FC0
void CCranes::InitCranes() {
    // NOP
}

// 0x6F3FE0
void CCranes::UpdateCranes() {
    ZoneScoped;

    // NOP
}

// 0x6F3FF0
bool CCranes::IsThisCarBeingCarriedByAnyCrane(CVehicle* vehicle) {
    return false;
}

// 0x6F4000
bool CCranes::IsThisCarBeingTargettedByAnyCrane(CVehicle* vehicle) {
    return false;
}

// 0x6F3FD0
bool CCranes::IsThisCarPickedUp(float a1, float a2, CVehicle* vehicle) {
    return false;
}
