#include "StdInc.h"

#include "PedGroups.h"

void CPedGroups::InjectHooks() {
    RH_ScopedClass(CPedGroups);
    RH_ScopedCategoryGlobal();

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
    return plugin::CallAndReturn<int32, 0x5FB800>();
}

// 0x5FB870
void CPedGroups::RemoveGroup(int32 groupId) {
    plugin::Call<0x5FB870, int32>(groupId);
}

// 0x5FB8A0
void CPedGroups::RemoveAllFollowersFromGroup(int32 groupId) {
    plugin::Call<0x5FB8A0, int32>(groupId);
}

// 0x5FB8C0
void CPedGroups::Init() {
    plugin::Call<0x5FB8C0>();
}

// 0x5F7E30
void CPedGroups::RegisterKillByPlayer() {
    if (ms_bIsPlayerOnAMission)
        ms_iNoOfPlayerKills++;
}

// 0x5FB930
void CPedGroups::CleanUpForShutDown() {
    plugin::Call<0x5FB930>();
}

// 0x5F7E40
bool CPedGroups::IsGroupLeader(CPed* ped) {
    return plugin::CallAndReturn<bool, 0x5F7E40, CPed*>(ped);
}

// 0x5F7E80
CPedGroup* CPedGroups::GetPedsGroup(const CPed* ped) {
    return plugin::CallAndReturn<CPedGroup*, 0x5F7E80>(ped);
}

// 0x5F7EE0
int32 CPedGroups::GetGroupId(const CPedGroup* pedGroup) {
    return plugin::CallAndReturn<int32, 0x5F7EE0, const CPedGroup*>(pedGroup);
}

// 0x5FC800
void CPedGroups::Process() {
    plugin::Call<0x5FC800>();
}

// 0x5F7F10
bool CPedGroups::IsInPlayersGroup(CPed* ped) {
    return plugin::CallAndReturn<bool, 0x5F7F10, CPed*>(ped);
}

CPedGroup& CPedGroups::GetGroup(int32 groupId) {
    assert(ms_activeGroups[groupId]);
    return ms_groups[groupId];
}

// 0x5F7F40
bool CPedGroups::AreInSameGroup(const CPed* ped1, const CPed* ped2) {
    return plugin::CallAndReturn<bool, 0x5F7F40, const CPed*, const CPed*>(ped1, ped2);
}
