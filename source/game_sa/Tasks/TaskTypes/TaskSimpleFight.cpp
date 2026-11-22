#include "StdInc.h"

#include "TaskSimpleFight.h"

// 0x61C470
CTaskSimpleFight::CTaskSimpleFight(CEntity* entity, int32 nCommand, uint32 nIdlePeriod) : CTaskSimple() {
    plugin::CallMethod<0x61C470, CTaskSimpleFight*, CEntity*, int32, uint32>(this, entity, nCommand, nIdlePeriod);
}

// 0x61C530
CTaskSimpleFight::~CTaskSimpleFight() {
    plugin::CallMethod<0x61C530, CTaskSimpleFight*>(this);
}

// 0x61C5E0
bool CTaskSimpleFight::ControlFight(CEntity* entity, uint8 command) {
    return plugin::CallMethodAndReturn<bool, 0x61C5E0, CTaskSimpleFight*, CEntity*, uint8>(this, entity, command);
}

// 0x4ABDC0
bool CTaskSimpleFight::IsComboSet() {
    return GetComboData().m_wFlags & (0x10u << ((uint8)m_nCurrentMove & 31)); // x86 SHL masks the count
}

// 0x4ABDF0
bool CTaskSimpleFight::IsHitComboSet() {
    if (m_nCurrentMove > FIGHT_ATTACK_HIT_3) {
        return false;
    }
    return GetComboData().m_wFlags & (0x1000u << ((uint8)m_nCurrentMove & 31)); // x86 SHL masks the count
}

// 0x4ABDA0
AssocGroupId CTaskSimpleFight::GetComboAnimGroupID() {
    return GetComboData().m_nAnimGroup;
}

// NOTSA (inlined) - Combo sets below `MELEE_COMBO_UNARMED_1` use the first entry
CMeleeInfo& CTaskSimpleFight::GetComboData() const {
    return m_aComboData[std::max(m_nComboSet - MELEE_COMBO_UNARMED_1, 0)];
}

// 0x5BEDC0
void CTaskSimpleFight::LoadMeleeData() {
    ZoneScoped;

    plugin::Call<0x5BEDC0>();
}

// 0x6239F0
bool CTaskSimpleFight::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x6239F0, CTaskSimpleFight*, CPed*, eAbortPriority, const CEvent*>(this, ped, priority, event);
}

// 0x629920
bool CTaskSimpleFight::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x629920, CTaskSimpleFight*, CPed*>(this, ped);
}

void CTaskSimpleFight::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleFight, 0x86D684, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(ControlFight, 0x61C5E0, { .Reversed = false });
    RH_ScopedInstall(IsComboSet, 0x4ABDC0);
    RH_ScopedInstall(IsHitComboSet, 0x4ABDF0);
    RH_ScopedInstall(GetComboAnimGroupID, 0x4ABDA0);
    RH_ScopedInstall(LoadMeleeData, 0x5BEDC0, { .Reversed = false });
}

CTaskSimpleFight* CTaskSimpleFight::Constructor(CEntity* entity, int32 nCommand, uint32 nIdlePeriod) {
    this->CTaskSimpleFight::CTaskSimpleFight(entity, nCommand, nIdlePeriod);
    return this;
}

CTaskSimpleFight* CTaskSimpleFight::Destructor() {
    this->CTaskSimpleFight::~CTaskSimpleFight();
    return this;
}
