/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CPed;

constexpr auto NUM_PEDGROUPS = 8;

class CPedGroups {
public:
    static inline std::array<uint16, NUM_PEDGROUPS>& ScriptReferenceIndex = *reinterpret_cast<std::array<uint16, NUM_PEDGROUPS>*>(0xC098D0);
    static inline std::array<char, NUM_PEDGROUPS>& ms_activeGroups = *reinterpret_cast<std::array<char, NUM_PEDGROUPS>*>(0xC098E0);
    static inline bool& ms_bIsPlayerOnAMission = *reinterpret_cast<bool*>(0xC098E8);
    static inline uint32& ms_iNoOfPlayerKills = *reinterpret_cast<uint32*>(0xC098EC);
    static inline std::array<CPedGroup, NUM_PEDGROUPS>& ms_groups = *reinterpret_cast<std::array<CPedGroup, NUM_PEDGROUPS>*>(0xC09920);

public:
    static void InjectHooks();

    // return the index of the added group, return -1 if failed.
    static int32 AddGroup();

    static void RemoveGroup(int32 groupID);
    static void RemoveAllFollowersFromGroup(int32 groupId);

    static void Init();

    static void RegisterKillByPlayer();

    static void CleanUpForShutDown();

    static bool IsGroupLeader(CPed* ped);

    static CPedGroup* GetPedsGroup(const CPed* ped);
    static int32 GetGroupId(const CPedGroup* pedGroup);

    static void Process();

    static bool AreInSameGroup(const CPed* ped1, const CPed* ped2);
    static bool IsInPlayersGroup(CPed* ped);

    // inlined
    static CPedGroup& GetGroup(int32 groupId);

    static int32 FindFreeGroupSlot() {
        for (auto i = 0; i < NUM_PEDGROUPS; i++) {
            if (!ms_activeGroups[i]) {
                return i;
            }
        }
        return -1;
    }
};
