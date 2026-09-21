#include "StdInc.h"
#include "PedList.h"
#include "TaskComplexKillPedOnFoot.h"

void CPedList::InjectHooks() {
    RH_ScopedClass(CPedList);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Empty, 0x699DB0);
    RH_ScopedInstall(BuildListFromGroup_NoLeader, 0x699DD0);
    RH_ScopedInstall(ExtractPedsWithGuns, 0x69A4C0);
    RH_ScopedInstall(BuildListFromGroup_NotInCar_NoLeader, 0x69A340);
    RH_ScopedInstall(BuildListOfPedsOfPedType, 0x69A3B0);
    RH_ScopedInstall(RemovePedsAttackingPedType, 0x69A450);
    RH_ScopedInstall(RemovePedsThatDontListenToPlayer, 0x69A420);
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
    m_count = 0;
    for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        if (CPed* const member = pedGroupMembership->GetMember(i)) {
            if (!member->GetIntelligence()->IsInACarOrEnteringOne()) {
                if (m_count < std::size(m_peds)) {
                    AddMember(member);
                }
            }
        }
    }
    ClearUnused();
}

// 0x69A3B0
void CPedList::BuildListOfPedsOfPedType(int32 pedType) {
    m_count = 0;
    for (int32 i = GetPedPool()->GetSize(); i; i--) {
        CPed* const ped = GetPedPool()->GetAt(i - 1);
        if (ped && ped->m_nPedType == static_cast<ePedType>(pedType) && m_count < std::size(m_peds)) {
            AddMember(ped);
        }
    }
    ClearUnused();
}

// 0x69A450
void CPedList::RemovePedsAttackingPedType(int32 pedType) {
    const auto count = (int32)m_count; // The count is saved beforehand, as it's decremented below
    for (int32 i = 0; i < count; i++) {
        const auto task = notsa::cast_if_present<CTaskComplexKillPedOnFoot>(Get(i)->GetIntelligence()->FindTaskByType(TASK_COMPLEX_KILL_PED_ON_FOOT));
        if (!task || !task->m_target || task->m_target->m_nPedType != static_cast<ePedType>(pedType)) {
            RemoveMemberNoFill(i);
        }
    }
    FillUpHoles();
}

// 0x69A420
void CPedList::RemovePedsThatDontListenToPlayer() {
    const auto count = (int32)m_count; // The count is saved beforehand, as it's decremented below
    for (int32 i = 0; i < count; i++) {
        if (Get(i)->bDoesntListenToPlayerGroupCommands) {
            RemoveMemberNoFill(i);
        }
    }
    FillUpHoles();
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
