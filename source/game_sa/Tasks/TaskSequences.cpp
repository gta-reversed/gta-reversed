#include "StdInc.h"

void CTaskSequences::InjectHooks() {
    RH_ScopedClass(CTaskSequences);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(Init, 0x632D90);
    RH_ScopedInstall(CleanUpForShutdown, 0x632DD0);
}

// 0x632D90
void CTaskSequences::Init() {
    ms_iActiveSequence = -1;
    CleanUpForShutdown();
}

// 0x632DD0
void CTaskSequences::CleanUpForShutdown() {
    for (auto i = 0; i < NUM_SEQUENCES; i++) {
        ms_bIsOpened[i] = false;
        auto taskSequence = &ms_taskSequence[i];
        taskSequence->Flush();
    }
}

// 0x632E00
int32 CTaskSequences::GetAvailableSlot(uint8 slot) {
    assert(false);
    return -1;
}
