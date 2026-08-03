#include "StdInc.h"
#include "PedList.h"

void CPedList::InjectHooks() {
    RH_ScopedClass(CPedList);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Empty, 0x699DB0);
    RH_ScopedInstall(BuildListFromGroup_NoLeader, 0x699DD0);
    RH_ScopedInstall(ExtractPedsWithGuns, 0x69A4C0);
    RH_ScopedInstall(BuildListFromGroup_NotInCar_NoLeader, 0x69A340, { .reversed = false });
    RH_ScopedInstall(BuildListOfPedsOfPedType, 0x69A3B0, { .reversed = false });
    RH_ScopedInstall(RemovePedsAttackingPedType, 0x69A450, { .reversed = false });
    RH_ScopedInstall(RemovePedsThatDontListenToPlayer, 0x69A420, { .reversed = false });
}

// 0x699DB0
void CPedList::Empty() {
    *this = {};
}

// 0x699DD0
void CPedList::BuildListFromGroup_NoLeader(CPedGroupMembership& groupMembership) {
    m_count = 0;
    for (auto* const mem : groupMembership.GetMembers(false)) {
        AddMember(mem);
    }
    ClearUnused();
}

// 0x69A4C0
void CPedList::ExtractPedsWithGuns(CPedList& from) {
    for (auto i = 0u; i < from.m_count; i++) {
        if (!from.Get(i)->GetActiveWeapon().IsTypeMelee()) {
            AddMember(from.Get(i));
            from.RemoveMemberNoFill(i);
        }
    }
    from.FillUpHoles();
}


// After nulling out a field in the
// array there might be a hole, so it has to be filled
void CPedList::FillUpHoles() {
    rng::fill(rng::remove(m_peds, nullptr), nullptr);
}

// 0x69A340
void CPedList::BuildListFromGroup_NotInCar_NoLeader(CPedGroupMembership* pedGroupMembership) {
    plugin::CallMethod<0x69A340>(this, pedGroupMembership);
}

// 0x69A3B0
void CPedList::BuildListOfPedsOfPedType(int32 pedType) {
    plugin::CallMethod<0x69A3B0>(this, pedType);
}

// 0x69A450
void CPedList::RemovePedsAttackingPedType(int32 pedType) {
    plugin::CallMethod<0x69A450>(this, pedType);
}

// 0x69A420
void CPedList::RemovePedsThatDontListenToPlayer() {
    plugin::CallMethod<0x69A420>(this);
}

//
// NOTSA section
//

// nulls out everything after the first `m_count` elements
void CPedList::ClearUnused() {
    rng::fill(m_peds | std::views::drop(m_count), nullptr);
}

void CPedList::AddMember(CPed* ped) {
    m_peds[m_count++] = ped;
}

// Must call FillUpHoles afterwards!
void CPedList::RemoveMemberNoFill(int32 i) {
    m_peds[i] = nullptr;
    m_count--;
}

CPed* CPedList::Get(int32 i) {
    return m_peds[i];
}
