#include "StdInc.h"
#include "MentalState.h"

void CMentalState::InjectHooks() {
    RH_ScopedClass(CMentalState);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(IncrementAnger, 0x421050, { .reversed = false });
    RH_ScopedInstall(Process, 0x6008A0, { .reversed = false });
}

// 0x421050
void CMentalState::IncrementAnger(int32 anger) {
    plugin::CallMethod<0x421050, CMentalState*, int32>(this, anger);
}

// 0x6008A0
void CMentalState::Process() {
    plugin::CallMethod<0x6008A0, CMentalState*>(this);
}
