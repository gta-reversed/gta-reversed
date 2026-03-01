#include "StdInc.h"

#include "WaterCannons.h"
#include "WaterCannon.h"

CWaterCannon (&CWaterCannons::aCannons)[3] = *(CWaterCannon (*)[3])0xC80740;

void CWaterCannons::InjectHooks() {
    RH_ScopedClass(CWaterCannons);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x728C80);
    RH_ScopedInstall(UpdateOne, 0x728CB0);
    RH_ScopedInstall(Update, 0x72A3C0);
    RH_ScopedInstall(Render, 0x729B30);
}

// 0x728C80
void CWaterCannons::Init() {
    for (auto& cannon : aCannons) {
        cannon.Init();
    }
}

// 0x728CB0
void CWaterCannons::UpdateOne(uint32 vehicle, CVector* start, CVector* end) {
    for (auto& cannon : aCannons) {
        if (cannon.m_nId == vehicle) {
            cannon.Update_NewInput(start, end);
            return;
        }
    }

    // find free slot and update
    auto i = 0;
    while (aCannons[i].m_nId) {
        if (++i >= MAX_WATER_CANNONS)
            return;
    }

    auto& cannon = aCannons[i];
    cannon.Init();
    cannon.m_nId = vehicle;
    cannon.Update_NewInput(start, end);
}

// 0x72A3C0
void CWaterCannons::Update() {
    ZoneScoped;

    int16 index = 0;
    for (auto& cannon : aCannons) {
        cannon.m_Audio.Service();
        if (cannon.m_nId > 0) {
            cannon.Update_OncePerFrame(index);
        }
        ++index;
    }
}

// 0x729B30
void CWaterCannons::Render() {
    ZoneScoped;

    for (auto& cannon : aCannons) {
        cannon.Render();
    }
}
