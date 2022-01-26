#include "StdInc.h"

void CBridge::InjectHooks() {
    RH_ScopedClass(CBridge);
    RH_ScopedCategoryRoot();

    using namespace ReversibleHooks;
    RH_ScopedInstall(Init, 0x41BC70);
    RH_ScopedInstall(Update, 0x41BC80);
    RH_ScopedInstall(FindBridgeEntities, 0x41BCA0);
    RH_ScopedInstall(ShouldLightsBeFlashing, 0x41BC90);
}

// 0x41BC70
void CBridge::Init() {
    // NOP
}

// 0x41BC80
void CBridge::Update() {
    // NOP
}

// used in CEntity::ProcessLightsForEntity
// always false
// 0x41BC90
bool CBridge::ShouldLightsBeFlashing() {
    return false;
}

// 0x41BCA0
void CBridge::FindBridgeEntities() {
    // NOP
}

// used in CPlayerInfo::Process
// 0x41BCB0
bool CBridge::ThisIsABridgeObjectMovingUp(uint32) {
    return false;
}
