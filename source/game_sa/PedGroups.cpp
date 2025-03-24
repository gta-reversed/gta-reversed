#include "StdInc.h"

#include "PedGroups.h"

void CPedGroups::InjectHooks() {
    RH_ScopedClass(CPedGroups);
    RH_ScopedCategory("Ped Groups");

    RH_ScopedInstall(AddGroup, 0x5FB800);
    RH_ScopedInstall(RemoveGroup, 0x5FB870);
    RH_ScopedInstall(RemoveAllFollowersFromGroup, 0x5FB8A0);
    RH_ScopedInstall(Init, 0x5FB8C0);
    RH_ScopedInstall(RegisterKillByPlayer, 0x5F7E30);
    RH_ScopedInstall(CleanUpForShutDown, 0x5FB930);
    RH_ScopedInstall(IsGroupLeader, 0x5F7E40);
    RH_ScopedInstall(GetPedsGroup, 0x5F7E80);
    RH_ScopedInstall(GetGroupId, 0x5F7EE0);
    RH_ScopedInstall(Process, 0x5FC800, {.reversed=false});
    RH_ScopedInstall(AreInSameGroup, 0x5F7F40);
    RH_ScopedInstall(IsInPlayersGroup, 0x5F7F10);
    // RH_ScopedInstall(GetGroup, 0x0, {.reversed=false});
}

#ifdef ANDROID
void CPedGroups::Save() {
    
}

void CPedGroups::Load() {
    
}
#endif

auto GetActiveGroups() {
    return CPedGroups::ms_groups | rng::views::filter([](auto& g) {
        return CPedGroups::ms_activeGroups[&g - CPedGroups::ms_groups.data()];
    });
}

// 0x5FB800
int32 CPedGroups::AddGroup() {
    const auto slot = FindFreeGroupSlot();
    if (slot == -1) {
        return -1;
    }

    ms_activeGroups[slot] = true;
    ms_groups[slot].Flush();
    return slot;
}

// 0x5FB870
void CPedGroups::RemoveGroup(int32 groupId) {
    if (ms_activeGroups[groupId]) {
        ms_activeGroups[groupId] = false;
        ms_groups[groupId].Flush();
    }
}

// 0x5FB8A0
void CPedGroups::RemoveAllFollowersFromGroup(int32 groupId) {
    if (ms_activeGroups[groupId]) {
        ms_groups[groupId].RemoveAllFollowers();
    }
}

// 0x5FB8C0
void CPedGroups::Init() {
    for (auto i = 0; i < NUM_PEDGROUPS; i++) {
        ScriptReferenceIndex[i] = 1;

        if (ms_activeGroups[i]) {
            ms_activeGroups[i] = false;
            for (auto& member : ms_groups[i].GetMembership().GetMembers()) {
                ms_groups[i].GetMembership().RemoveMember(&member);
            }
        }
    }
}

// 0x5F7E30
void CPedGroups::RegisterKillByPlayer() {
    if (ms_bIsPlayerOnAMission)
        ms_iNoOfPlayerKills++;
}

// 0x5FB930
void CPedGroups::CleanUpForShutDown() {
    rng::for_each(ms_groups, &CPedGroup::Flush);
}

// 0x5F7E40
bool CPedGroups::IsGroupLeader(CPed* ped) {
    for (auto& group : GetActiveGroups()) {
        if (group.GetMembership().IsLeader(ped)) {
            return true;
        }
    }
    return false;
}

// 0x5F7E80
CPedGroup* CPedGroups::GetPedsGroup(const CPed* ped) {
    for (auto& group : GetActiveGroups()) {
        if (group.GetMembership().IsMember(ped)) {
            return &group;
        }
    }
    return nullptr;
}

// 0x5F7EE0
int32 CPedGroups::GetGroupId(const CPedGroup* pedGroup) {
    for (const auto&& [i, group] : notsa::enumerate(ms_groups)) {
        if (&group == pedGroup) {
            return i;
        }
    }
    return -1;
}

// 0x5FC800
void CPedGroups::Process() {
    plugin::Call<0x5FC800>();
    /*
    for (auto&& [i, group] : notsa::enumerate(GetActiveGroups())) {
        group.GetMembership().Process();
        group.GetIntelligence().Process();

        // ranges::size doesn't work for some reason
        if (!group.GetMembership().GetMembers().size()) {
            RemoveGroup(i);
        }
    }

    auto decisionMarks{ -1 };

    if (CTheScripts::IsPlayerOnAMission() && !ms_bIsPlayerOnAMission) {
        ms_iNoOfPlayerKills = 0;
        decisionMarks       = 9;
    } else if (CTheScripts::IsPlayerOnAMission() || ms_bIsPlayerOnAMission) {
        if (!CTheScripts::IsPlayerOnAMission() || ms_iNoOfPlayerKills != 0) {
            if (!CTheScripts::IsPlayerOnAMission())

        }
    }

    for (auto& group : GetActiveGroups()) {
        if (!group.m_bIsMissionGroup) {
            continue;
        }
        group.GetIntelligence().m_DecisionMakerType = decisionMarks;
    }

    ms_bIsPlayerOnAMission = CTheScripts::IsPlayerOnAMission();
    if (!ms_bIsPlayerOnAMission) {
        ms_iNoOfPlayerKills = 0;
    }
    */
}

// 0x5F7F10
bool CPedGroups::IsInPlayersGroup(CPed* ped) {
    // Group 0 is player's group.
    for (auto& mem : GetGroup(0).GetMembership().GetMembers()) {
        if (&mem == ped) {
            return true;
        }
    }
    return false;
}

CPedGroup& CPedGroups::GetGroup(int32 groupId) {
    return ms_groups[groupId];
}

// 0x5F7F40
bool CPedGroups::AreInSameGroup(const CPed* ped1, const CPed* ped2) {
    for (const auto& group : GetActiveGroups()) {
        const auto& mem = group.GetMembership();

        if (mem.IsMember(ped1)) {
            return mem.IsMember(ped2);
        }
    }
    return false;
}
