#include "StdInc.h"

#include <functional>
#include "PedGroupMembership.h"

CPedGroupMembership::CPedGroupMembership(CPedGroup& group) :
    m_group{&group}
{
}

CPedGroupMembership::~CPedGroupMembership() {
    Flush();
}

// 0x5F8020
void CPedGroupMembership::AddFollower(CPed* ped) {
    ped->bHasGroupDriveTask = false;

    // Peds in the player's group can't drown
    if (const auto leader = GetLeader()) {
        if (leader->IsPlayer()) { // same effect as m_pPlayerData, no?
            assert(leader->m_pPlayerData); // Test above theory
            ped->bDrownsInWater = false;
        }
    }

    if (IsFollower(ped) || GetLeader() == ped) { // TODO/BUG/NOTE: Stuff that was set above is now not reverted... I'm not sure if that's intended
        return;
    }

    const auto memId = FindIdForNewMember();
    assert(memId != -1); 
    if (memId == -1) {
        return;
    }

    AddMember(ped, memId);
    GivePedRandomObjectToHold(ped);
}

// 0x5F6AE0
void CPedGroupMembership::AddMember(CPed* member, int32 memIdx) {
    assert(member);
    assert(!m_members[memIdx]);

    m_members[memIdx] = member;
    CEntity::RegisterReference(m_members[memIdx]);
    /* dead code before checking if the member is in the player's group */
    if (!member->IsPlayer()) {
        member->GetIntelligence()->SetPedDecisionMakerType(DM_EVENT_DRAGGED_OUT_CAR);
    }
}

// 0x5FB240
void CPedGroupMembership::AppointNewLeader() {
    if (HasLeader()) {
        return;
    }

    const auto memId = FindNewLeaderToAppoint();
    if (memId == -1) {
        return;
    }

    const auto leader = m_members[memId];
    RemoveMember(memId); // Must call as it does some cleanup
    AddMember(leader, LEADER_MEM_ID);
}

// 0x5F6A50
size_t CPedGroupMembership::CountMembers() {
    return rng::count_if(m_members, notsa::NotIsNull{});
}

// 0x5F6AA0
int32 CPedGroupMembership::CountMembersExcludingLeader() {
    const auto cnt = CountMembers();
    return HasLeader()
        ? cnt - 1
        : cnt;
}

// 0x5FB160
void CPedGroupMembership::Flush() {
    for (auto&& [i, mem] : rngv::enumerate(m_members)) {
        if (mem) {
            RemoveMember(i);
        }
    }
}

CPed* CPedGroupMembership::GetLeader() const {
    return m_members[LEADER_MEM_ID];
}

// 0x5F69B0
CPed* CPedGroupMembership::GetMember(int32 memberId) {
    return m_members[memberId];
}

int32 CPedGroupMembership::GetMemberId(const CPed* ped) const {
    for (auto&& [id, mem] : rngv::enumerate(m_members)) {
        if (mem == ped) {
            return id;
        }
    }
    return -1;
}

// 0x5F69E0
bool CPedGroupMembership::IsFollower(const CPed* ped) const {
    return !IsLeader(ped) && IsMember(ped);
}

// 0x5F69C0
bool CPedGroupMembership::IsLeader(const CPed* ped) const {
    return ped && GetLeader() == ped;
}

// 0x5F6A10
bool CPedGroupMembership::IsMember(const CPed* ped) const {
    return ped && notsa::contains(m_members, ped);
}

// 0x5FBA60
void CPedGroupMembership::Process() {
    // Remove dead members (except player ped)
    for (auto&& [i, mem] : rngv::enumerate(m_members)) {
        if (!mem || mem->IsAlive()) {
            continue;
        }
        if (IsLeader(mem) && mem->IsPlayer()) { // Player always stays in it's group
            continue;
        }
        RemoveMember(i);
    }

    // If no leader, try appointing a new one
    if (!HasLeader()) {
        AppointNewLeader();
        if (!HasLeader()) { // Couldn't appoint a new leader
            return;
        }
    }

    // Now that we have a leader, check for separation distance and remove members further than it
    const auto& leaderPos = GetLeader()->GetPosition();
    for (auto&& [i, mem] : rngv::enumerate(m_members)) {
        if (!mem || mem->bNeverLeavesGroup) {
            continue;
        }
        if (IsLeader(mem)) { // Leader's distance to itself always 0, thus ignore
            continue;
        }
        if (sq(m_separationRange) >= (leaderPos - mem->GetPosition()).SquaredMagnitude()) {
            continue;
        }
        RemoveMember(i); // Ped is too far
    }
}

// 0x5FB190
void CPedGroupMembership::RemoveAllFollowers(bool bCreatedByMissionOnly) {
    for (auto&& [i, mem] : rngv::enumerate(m_members)) {
        if (!mem || IsLeader(mem)) { // Leader isn't a follower
            continue;
        }
        if (bCreatedByMissionOnly && mem->IsCreatedBy(PED_MISSION)) {
            continue;
        }
        RemoveMember(i);
    }
}

// 0x5FB1D0
void CPedGroupMembership::RemoveNFollowers(size_t count) {
    if (count == 0) { // Nothing to do
        return;
    }
    for (auto&& [i, mem] : rngv::enumerate(m_members)) {
        if (IsLeader(mem)) { // Leader isn't a follower
            continue;
        }
        if (mem->IsCreatedBy(PED_MISSION)) {
            continue;
        }
        RemoveMember(i);
        if (!--count) {
            break;
        }
    }
}

// 0x5F80D0
void CPedGroupMembership::RemoveMember(int32 memIdx) {
    const auto mem = m_members[memIdx];
    assert(mem);

    CEntity::ClearReference(m_members[memIdx]); // Does `m_apMembers[memIdx] = nullptr`

    if (mem->IsPlayer()) {
        return;
    }

    if (mem->bClearRadarBlipOnDeath) {
        CRadar::ClearBlipForEntity(mem);
        mem->bClearRadarBlipOnDeath = false;
    }

    mem->GetIntelligence()->RestorePedDecisionMakerType();

    if (const auto leader = GetLeader()) {
        if (const auto plyrdat = leader->m_pPlayerData) {
            mem->bDrownsInWater = true;
        }
    }
}

// 0x5FB210
void CPedGroupMembership::RemoveMember(CPed* ped) {
    if (const auto id = GetMemberId(ped); id != -1) {
        RemoveMember(id);
    }
}

// 0x5FB9C0
void CPedGroupMembership::SetLeader(CPed* ped) {
    assert(ped);

    // Remove member (Just in case he's in the group already)
    RemoveMember(ped);

    // Remov the current leader (If any)
    if (GetLeader()) {
        RemoveMember(GetLeader());
    }

    // Now add member as the leader
    AddMember(ped, LEADER_MEM_ID);
    GivePedRandomObjectToHold(ped);
}

// NOTSA
auto CPedGroupMembership::FindClosestFollowerToLeader() -> FindClosestMemberResult {
    if (const auto leader = GetLeader()) {
        return GetMemberClosestTo(leader);
    }
    return { nullptr, 0.f }; // We return 0.f here, `GetMemberClosestTo` returns FLT_MAX, but it should be ignored anyways, because the CPed* is nullptr
}

// NOTSA
CPed* CPedGroupMembership::GetFirstAvailableMember() {
    for (const auto m : m_members) {
        if (m) {
            return m;
        }
    }
    return nullptr;
}

// 0x5F6950
eModelID CPedGroupMembership::GetObjectForPedToHold() {
    using namespace ModelIndices;
    return CGeneral::RandomChoiceFromList({ (eModelID)MI_GANG_SMOKE, MODEL_INVALID, (eModelID)MI_GANG_DRINK }); // Each has 33% chance
}

// NOTSA
int32 CPedGroupMembership::FindNewLeaderToAppoint() const {
    for (auto&& [i, mem] : rngv::enumerate(m_members)) {
        if (mem) {
            return i;
        }
    }
    return -1;
}

// NOTSA
int32 CPedGroupMembership::FindIdForNewMember() const {
    for (auto&& [i, mem] : rngv::enumerate(m_members)) {
        if (!mem) {
            return i;
        }
    }
    return -1;
}

// Based on code from 0x5F80BE
void CPedGroupMembership::GivePedRandomObjectToHold(CPed* mem, bool onlyIfUnarmed) const {
    if (m_group->m_bIsMissionGroup) {
        return;
    }
    if (onlyIfUnarmed && !mem->IsCurrentlyUnarmed()) {
        return;
    }
    if (const auto modelId = GetObjectForPedToHold(); modelId != MODEL_INVALID) {
        mem->GiveObjectToPedToHold(modelId, true);
    }
}

// NOTSA
bool CPedGroupMembership::CanAddFollower() {
    return std::size(m_members) - 1 >= CountMembers(); // - 1 to compensate for leader
}

// NOTSA
CPed* CPedGroupMembership::GetRandom() {
    return CGeneral::RandomChoice(m_members | rng::views::take(CountMembers()));
}

void CPedGroupMembership::InjectHooks() {
    RH_ScopedClass(CPedGroupMembership);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetObjectForPedToHold, 0x5F6950);
    RH_ScopedInstall(From, 0x5F7FE0);
    RH_ScopedInstall(CountMembersExcludingLeader, 0x5F6AA0);
    RH_ScopedInstall(CountMembers, 0x5F6A50);
    RH_ScopedInstall(IsMember, 0x5F6A10);
    RH_ScopedInstall(IsFollower, 0x5F69E0);
    RH_ScopedInstall(IsLeader, 0x5F69C0);
    RH_ScopedInstall(GetMember, 0x5F69B0);
    RH_ScopedInstall(AddMember, 0x5F6AE0);
    RH_ScopedInstall(AddFollower, 0x5F8020);
    RH_ScopedInstall(GetLeader, 0x5F69A0);
    RH_ScopedInstall(SetLeader, 0x5FB9C0);
    RH_ScopedInstall(AppointNewLeader, 0x5FB240);
    RH_ScopedInstall(RemoveNFollowers, 0x5FB1D0);
    RH_ScopedInstall(RemoveAllFollowers, 0x5FB190);
    RH_ScopedOverloadedInstall(RemoveMember, "ByPed", 0x5FB210, void(CPedGroupMembership::*)(CPed*));
    RH_ScopedOverloadedInstall(RemoveMember, "ByMemIdx", 0x5F80D0, void(CPedGroupMembership::*)(int32));
    RH_ScopedInstall(Process, 0x5FBA60);
}
