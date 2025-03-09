/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"

class CPedGroupMembership;
class CPed;

class CPedList {
public:
    uint32                m_count{};
    std::array<CPed*, 30> m_peds{};

public:
    static void InjectHooks();

    void Empty();
    void BuildListFromGroup_NoLeader(CPedGroupMembership& groupMembership);
    void ExtractPedsWithGuns(CPedList& from);

    // Inlined functions below (Present in Android version)
    void FillUpHoles();
    void BuildListFromGroup_NotInCar_NoLeader(CPedGroupMembership& pedGroupMembership);
    void BuildListOfPedsOfPedType(int32 pedType);
    void RemovePedsAttackingPedType(int32 pedType);
    void RemovePedsThatDontListenToPlayer();

    // NOTSA
    void ClearUnused();
    void AddMember(CPed* ped);
    void RemoveMemberNoFill(int32 i);
    CPed* Get(int32 i);
    auto GetPeds() const { return std::span{ m_peds.data(), m_count }; }
};
VALIDATE_SIZE(CPedList, 0x7C);
