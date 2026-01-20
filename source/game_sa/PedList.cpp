#include "StdInc.h"
#include "PedList.h"

void CPedList::InjectHooks() {
    RH_ScopedClass(CPedList);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Empty, 0x699DB0);
    RH_ScopedInstall(BuildListFromGroup_NoLeader, 0x699DD0);
    RH_ScopedInstall(ExtractPedsWithGuns, 0x69A4C0);
    RH_ScopedInstall(BuildListFromGroup_NotInCar_NoLeader, 0x69A340);
    RH_ScopedInstall(BuildListOfPedsOfPedType, 0x69A3B0);
    RH_ScopedInstall(RemovePedsAttackingPedType, 0x69A450);
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
void CPedList::BuildListFromGroup_NotInCar_NoLeader(CPedGroupMembership* group) {
    m_count = 0;

    for (auto* const member : group->GetMembers(false)) {
        if (!member->GetIntelligence()->IsInACarOrEnteringOne()) {
            AddMember(member);
        }
    }

    ClearUnused();
}

// 0x69A3B0
void CPedList::BuildListOfPedsOfPedType(int32 pedType) {
    CPedPool* pool = GetPedPool();
    m_count = 0;

    if (!pool || !pool->GetSize()) { ClearUnused(); return; }

    for (int32 i = pool->GetSize() - 1; i >= 0; --i) {
        CPed* ped = pool->GetAt(i);
        if (!ped) continue;

        if (ped->m_nPedType == pedType) {
            AddMember(ped);
        }
    }

    ClearUnused();
}

// 0x69A450
void CPedList::RemovePedsAttackingPedType(int32 pedType) {
    const int32 count = m_count;

    for (int32 i = 0; i < count; ++i) {
        CPed* ped = m_peds[i];
        if (!ped) continue;

        CTask* task = ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_KILL_PED_ON_FOOT);
        const CPed* target = nullptr;

        if (const auto* kill = notsa::cast_if_present<CTaskComplexKillPedOnFoot>(task)) {
            target = kill->m_target;
        }

        if (!task || !target || target->m_nPedType != pedType) {
            RemoveMemberNoFill(i);
        }
    }

    FillUpHoles();
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
    if (m_count < MAX_NUM_PEDS_IN_LIST) { m_peds[m_count++] = ped; }
}

// Must call FillUpHoles afterwards!
void CPedList::RemoveMemberNoFill(int32 i) {
    m_peds[i] = nullptr;
    m_count--;
}

CPed* CPedList::Get(int32 i) {
    return m_peds[i];
}
