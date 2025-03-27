/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PedGroupMembership.h"
#include "PedGroupIntelligence.h"

class CVector;
class CPed;
class CVehicle;

class CPedGroup {
public:
    static void InjectHooks();

    CPedGroup() = default;
    ~CPedGroup() = default;

    //! Find follower closest to the leader
    float FindDistanceToNearestMember(CPed** ppOutNearestMember);

    //! Clear state
    void Flush();

    //! Find member closest to `ped`
    CPed* GetClosestGroupPed(CPed* ped, float* pOutDistance);

    //! Find distance of the furthest member to `ped`
    float FindDistanceToFurthestMember();

    //! Is anyone from this group using the given car
    bool IsAnyoneUsingCar(const CVehicle* vehicle);

    //! todo
    void PlayerGaveCommand_Attack(CPed* playerPed, CPed* ped);

    //! todo
    void PlayerGaveCommand_Gather(CPed* ped);

    //! Update routine
    void Process();

    //! Remove all followers of the group [That is, all members, excluding the leader]
    void RemoveAllFollowers();

    //! Teleport the whole group [incl. leader] to a position
    void Teleport(const CVector& pos);

    //! Get id of this group
    int32 GetId() const;
    bool IsActive() const;

    auto& GetIntelligence()     { return m_groupIntelligence; }
    auto& GetMembership() const { return m_groupMembership; }
    auto& GetMembership()       { return m_groupMembership; }

private: // Wrappers for hooks
    // 0x5FC150
    CPedGroup* Constructor() {
        this->CPedGroup::CPedGroup();
        return this;
    }

    // 0x5FC190
    CPedGroup* Destructor() {
        this->CPedGroup::~CPedGroup();
        return this;
    }

public:
    CPed*                 m_pPed{};
    bool                  m_bMembersEnterLeadersVehicle{true};
    CPedGroupMembership   m_groupMembership{*this};
    CPedGroupIntelligence m_groupIntelligence{this};
    bool                  m_bIsMissionGroup{};
};
VALIDATE_SIZE(CPedGroup, 0x2D4);
