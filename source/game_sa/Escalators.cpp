#include "StdInc.h"

#include "Escalators.h"
#include "Escalator.h"

CEscalator(&CEscalators::aEscalators)[NUM_ESCALATORS] = *(CEscalator(*)[NUM_ESCALATORS])0xC6E9A8;

void CEscalators::InjectHooks() {
    RH_ScopedClass(CEscalators);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x717C50);
    RH_ScopedInstall(Shutdown, 0x717940);
    RH_ScopedInstall(AddOne, 0x717C90);
    RH_ScopedInstall(Update, 0x718580);
}

// 0x717940
void CEscalators::Shutdown() {
    for (CEscalator& escalator : aEscalators) {
        escalator.SwitchOff();
        escalator.m_bExist = false;
    }
}

// 0x717C50
void CEscalators::Init() {
    Shutdown();

    for (CEscalator& escalator : aEscalators) {
        escalator.SwitchOff();
    }
}

// 0x717C90
void CEscalators::AddOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecEnd, const CVector& vecTop, bool bMoveDown, CEntity* entity) {
    for (CEscalator& escalator : aEscalators) {
        if (!escalator.m_bExist) {
            escalator.AddThisOne(vecStart, vecBottom, vecEnd, vecTop, bMoveDown, entity);
            break;
        }
    }
}

// 0x718580
void CEscalators::Update() {
    if (CReplay::Mode != MODE_PLAYBACK) {
        for (CEscalator& escalator : aEscalators) {
            if (escalator.m_bExist)
                escalator.Update();
        }
    }
}
