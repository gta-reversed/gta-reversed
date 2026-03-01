#include "StdInc.h"

#include "BreakManager_c.h"

BreakManager_c& g_breakMan = *(BreakManager_c*)0xBB4240;

void BreakManager_c::InjectHooks() {
    RH_ScopedClass(BreakManager_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x59E650);
    RH_ScopedInstall(Exit, 0x59E660);
    RH_ScopedInstall(ResetAll, 0x59E720);
    RH_ScopedInstall(Add, 0x59E9B0);
    RH_ScopedInstall(Update, 0x59E670);
    RH_ScopedInstall(Render, 0x59E6A0);
}

// 0x59E650
bool BreakManager_c::Init() {
    return true;
}

// 0x59E660
void BreakManager_c::Exit() {
    // NOP
}

// 0x59E720
void BreakManager_c::ResetAll() {
    for (BreakObject_c& object : m_aObjects) {
        if (object.m_bActive) {
            object.Exit();
        }
    }
}

// 0x59E9B0
bool BreakManager_c::Add(CObject* object, CVector* vecPos, float fVelocityRand, bool bSmash) {
    BreakObject_c* slot = GetFirstFreeSlot();
    return slot && slot->Init(object, vecPos, fVelocityRand, bSmash);
}

// 0x59E700
BreakObject_c* BreakManager_c::GetFirstFreeSlot() {
    for (BreakObject_c& object : m_aObjects) {
        if (!object.m_bActive) {
            return &object;
        }
    }
    return nullptr;
}

// 0x59E670
void BreakManager_c::Update(float timeStep) {
    ZoneScoped;

    for (BreakObject_c& object : m_aObjects) {
        if (object.m_bActive) {
            object.Update(timeStep);
        }
    }
}

// 0x59E6A0
void BreakManager_c::Render(uint8 state) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));

    for (BreakObject_c& object : m_aObjects) {
        if (object.m_bActive) {
            object.Render(state);
        }
    }
}
