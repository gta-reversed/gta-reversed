#include "StdInc.h"

#include "Escalator.h"

void CEscalator::InjectHooks() {
    RH_ScopedClass(CEscalator);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x717110);
    RH_ScopedInstall(Destructor, 0x717130);

    RH_ScopedInstall(SwitchOff, 0x717860);
    RH_ScopedInstall(AddThisOne, 0x717970, { .reversed = false });
    RH_ScopedInstall(Update, 0x717D30, { .reversed = false });
}

// 0x717860
void CEscalator::SwitchOff() {
    // debug leftover
    // static bool& deletingEscalator = StaticRef<bool>(0xC6E98C);

    if (!m_nObjectsCreated) {
        return;
    }

    for (auto* object : m_pObjects | rngv::take(m_nNumTopPlanes + m_nNumBottomPlanes + m_nNumIntermediatePlanes)) {
        if (!object) {
            continue;
        }

        CWorld::Remove(object);
        // deletingEscalator = true;
        delete std::exchange(object, nullptr);
        // deletingEscalator = false;
    }
    m_nObjectsCreated = 0;
}

// 0x717970
void CEscalator::AddThisOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecEnd, const CVector& vecTop, bool bMoveDown, CEntity* entity) {
    plugin::CallMethod<0x717970, CEscalator*, const CVector&, const CVector&, const CVector&, const CVector&, bool, CEntity*>(this, vecStart, vecBottom, vecEnd, vecTop, bMoveDown, entity);
}

// 0x717D30
void CEscalator::Update() {
    plugin::CallMethod<0x717D30, CEscalator*>(this);
}
