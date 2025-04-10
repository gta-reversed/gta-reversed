#include "StdInc.h"

#include "CreepingFire.h"
#include "FireManager.h"

uint8 (&CCreepingFire::m_aFireStatus)[32][32] = *(uint8(*)[32][32])0xB71B68;

void CCreepingFire::InjectHooks() {
    RH_ScopedClass(CCreepingFire);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(SetReadyToBurn, 0x539CC0);
    RH_ScopedInstall(Update, 0x539CE0);
    RH_ScopedInstall(TryToStartFireAtCoors, 0x53A450);
}

// 0x539CC0
void CCreepingFire::SetReadyToBurn() {
    for (int32 i = 0; i < 32; i++) {
        for (int32 j = 0; j < 32; j++) {
            m_aFireStatus[i][j] = 0;
        }
    }
}

// 0x539CE0
void CCreepingFire::Update() {
    ZoneScoped;

    uint8& status = m_aFireStatus[CTimer::GetFrameCounter() % 32][(CTimer::GetFrameCounter() / 32) % 32];
    if (status == 4) {
        status = 0;
    } else if (status > 4 && status <= 6) {
        status -= 1;
    }
}

// 0x53A450
bool CCreepingFire::TryToStartFireAtCoors(CVector pos, uint8 nGenerations, bool a5, bool bScriptFire, float fDistance) {
    uint8& status = m_aFireStatus[(size_t)pos.x % 32][(size_t)pos.y % 32];
    if (status == 0)
        return false;

    if (!gFireManager.PlentyFiresAvailable())
        return false;

    CEntity* hitEntity{};
    CColPoint cp;
    if (!CWorld::ProcessVerticalLine(pos, pos.z - fDistance, cp, hitEntity, true, false, false, false, false, false, nullptr))
        return false;

    pos.z = cp.m_vecPoint.z;
    status = 6;

    CFire* fire = gFireManager.StartFire(pos, 0.8f, true, nullptr, 20'000u, nGenerations, !bScriptFire);
    if (!fire)
        return false;

    fire->firstGeneration = false;
    fire->createdByScript = bScriptFire;
    return true;
}
