#include "StdInc.h"

#include "PedGroups.h"

void CPedGroups::InjectHooks() {
    RH_ScopedClass(CPedGroups);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(AddGroup, 0x5FB800);
    RH_ScopedInstall(RemoveAllFollowersFromGroup, 0x5FB8A0);
    RH_ScopedInstall(Init, 0x5FB8C0);
    RH_ScopedInstall(CleanUpForShutDown, 0x5FB930);
    RH_ScopedInstall(IsGroupLeader, 0x5F7E40);
    RH_ScopedInstall(GetGroupId, 0x5F7EE0);
    RH_ScopedInstall(IsInPlayersGroup, 0x5F7F10);
    RH_ScopedInstall(AreInSameGroup, 0x5F7F40);
    
    RH_ScopedInstall(Process, 0x5FC800);
    RH_ScopedInstall(RemoveGroup, 0x5FB870);
}

#ifdef ANDROID
void CPedGroups::Save() {
    
}

void CPedGroups::Load() {
    
}
#endif

// return the index of the added group , return -1 if failed.
// 0x5FB800
int32 CPedGroups::AddGroup() {
    for (int32 index = 0; index < static_cast<int32>(ms_activeGroups.size()); ++index) {
        if (!ms_activeGroups[index]) {
            ms_activeGroups[index] = 1;
            auto* const group = &GetGroup(index);
            group->Flush();
            return index;
        }
    }

    return -1;
}

// 0x5FB870
void CPedGroups::RemoveGroup(int32 groupId) {
    auto* const group = &GetGroup(groupId);
    if (!std::exchange(ms_activeGroups[groupId], false)) {
        return;
    }
    group->Flush();
}

// 0x5FB8A0
void CPedGroups::RemoveAllFollowersFromGroup(int32 groupId) {
    assert(groupId < 8);
    
    if (!ms_activeGroups[groupId]) {
        return;
    }

    CPedGroup& group = GetGroup(groupId);
    group.GetMembership().RemoveAllFollowers(false);
}

// 0x5FB8C0
void CPedGroups::Init() {
    for (int32 i = 0; i < static_cast<int32>(ms_groups.size()); ++i) {
        if (ms_activeGroups[i]) {
            ms_activeGroups[i] = 0;
            auto* const group = &GetGroup(i);
            group->Flush();
        }

        ScriptReferenceIndex[i] = 1;
    }
}

// 0x5F7E30
void CPedGroups::RegisterKillByPlayer() {
    if (ms_bIsPlayerOnAMission)
        ms_iNoOfPlayerKills++;
}

// 0x5FB930
void CPedGroups::CleanUpForShutDown() {
    for (int32 index = 0; index < static_cast<int32>(ms_groups.size()); ++index) {
        auto* const group = &GetGroup(index);
        group->Flush();
    }
}

// 0x5F7E40
bool CPedGroups::IsGroupLeader(CPed* ped) {
    assert(ped != nullptr);

    for (int32 i = 0; i < static_cast<int32>(ms_groups.size()); ++i) {
        if (ms_activeGroups[i] && GetGroup(i).GetMembership().IsLeader(ped)) {
            return true;
        }
    }

    return false;
}

// 0x5F7E80
CPedGroup* CPedGroups::GetPedsGroup(const CPed* ped) {
    for (int32 i = 0; i < static_cast<int32>(ms_groups.size()); ++i) {
        if (ms_activeGroups[i] && ped) {
            CPedGroup& group = GetGroup(i);
            auto& memberShip = group.GetMembership();
            if (memberShip.IsFollower(ped)) {
                return &group;
            }

            if (memberShip.IsLeader(ped)) {
                return &group;
            }
        }
    }

    return nullptr;
}

// 0x5F7EE0
inline int32 CPedGroups::GetGroupId(const CPedGroup* pedGroup) {
    assert(pedGroup != nullptr);

    for (int32 index = 0; index < static_cast<int32>(ms_groups.size()); ++index) {
        if (pedGroup == &GetGroup(index)) {
            return index;
        }
    }

    return -1;
}

// 0x5FC800
void CPedGroups::Process() {
    for (auto&& [i, g] : GetActiveGroupsWithIDs()) {
        g.Process();
        if (!g.GetMembership().CountMembers()) {
            RemoveGroup(i);
        }
    }

    const auto SetGroupsDecisionMakerType = [](eDecisionMakerType dm) {
        for (auto&& [_, g] : GetActiveGroupsWithIDs()) {
            g.GetIntelligence().SetGroupDecisionMakerType(dm);
        }
    };
    if (CTheScripts::IsPlayerOnAMission() && !ms_bIsPlayerOnAMission) { // 0x5FC897 - Player started a mission
        ms_iNoOfPlayerKills = 0;
        SetGroupsDecisionMakerType(eDecisionMakerType::GROUP_RANDOM_PASSIVE);
    } else if (!CTheScripts::IsPlayerOnAMission() && ms_bIsPlayerOnAMission) { // 0x5FC8B2 - Player ended a mission (inverted)
        SetGroupsDecisionMakerType(eDecisionMakerType::UNKNOWN);
    } else if (CTheScripts::IsPlayerOnAMission() && CPedGroups::ms_iNoOfPlayerKills == 8) { // 0x5FC8BF (inverted)
        SetGroupsDecisionMakerType(eDecisionMakerType::UNKNOWN);
    } else if (CTheScripts::IsPlayerOnAMission()) {
        SetGroupsDecisionMakerType(eDecisionMakerType::GROUP_RANDOM_PASSIVE);
    }

    if (!std::exchange(ms_bIsPlayerOnAMission, CTheScripts::IsPlayerOnAMission())) { // 0x5FC98D
        ms_iNoOfPlayerKills = 0;
    }
}

// 0x5F7F10
bool CPedGroups::IsInPlayersGroup(CPed* ped) {
    if (ped->GetPlayerData()) {
        return false;
    }

    return GetGroup(0).GetMembership().IsMember(ped);
}

CPedGroup& CPedGroups::GetGroup(int32 groupId) {
    //assert(ms_activeGroups[groupId]); // TODO: Look: https://github.com/gta-reversed/gta-reversed/issues/1041
    return ms_groups[groupId];
}

// 0x5F7F40
bool CPedGroups::AreInSameGroup(const CPed* ped1, const CPed* ped2) {
    if (!ped1 || !ped2) {
        return false;
    }

    for (int32 i = 0; i < static_cast<int32>(ms_groups.size()); ++i) {
        if (!ms_activeGroups[i]) {
            continue;
        }

        const auto& membership = GetGroup(i).GetMembership();
        if (membership.IsMember(ped1) && membership.IsMember(ped2)) {
            return true;
        }
    }

    return false;
}
