#include "StdInc.h"

#include "Escalators.h"
#include "Escalator.h"

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
    rng::for_each(aEscalators, &CEscalator::Remove);
}

// 0x717C50
void CEscalators::Init() {
    Shutdown();
    rng::for_each(aEscalators, &CEscalator::SwitchOff);
}

// 0x717C90
void CEscalators::AddOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecTop, const CVector& vecEnd, bool moveDown, CEntity* entity) {
    for (CEscalator& escalator : aEscalators) {
        if (!escalator.m_Exist) {
            escalator.AddThisOne(vecStart, vecBottom, vecTop, vecEnd, moveDown, entity);
            break;
        }
    }
}

// 0x718580
void CEscalators::Update() {
    if (CReplay::Mode != MODE_PLAYBACK) {
        rng::for_each(GetAllExists(), &CEscalator::Update);
    }
}
