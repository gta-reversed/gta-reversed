#include "StdInc.h"

#include "Escalator.h"

void CEscalator::InjectHooks() {
    RH_ScopedClass(CEscalator);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(SwitchOff, 0x717860, { .reversed = false });
    RH_ScopedInstall(AddThisOne, 0x717970, { .reversed = false });
    RH_ScopedInstall(Update, 0x717D30, { .reversed = false });
}

// 0x717110
CEscalator::CEscalator() {
    m_nObjectsCreated = 0;
    std::ranges::fill(m_pObjects, nullptr);
}

// 0x717860
void CEscalator::SwitchOff() {
    plugin::CallMethod<0x717860, CEscalator*>(this);
}

// 0x717970
void CEscalator::AddThisOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecEnd, const CVector& vecTop, bool bMoveDown, CEntity* entity) {
    plugin::CallMethod<0x717970, CEscalator*, const CVector&, const CVector&, const CVector&, const CVector&, bool, CEntity*>(this, vecStart, vecBottom, vecEnd, vecTop, bMoveDown, entity);
}

// 0x717D30
void CEscalator::Update() {
    plugin::CallMethod<0x717D30, CEscalator*>(this);
}
