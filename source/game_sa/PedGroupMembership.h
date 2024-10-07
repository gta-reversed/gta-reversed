/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <ranges>
#include <concepts>

class CPed;
class CPedGroup;

const int32 TOTAL_PED_GROUP_MEMBERS = 8;
// -1 because every ped group has a leader, and the leader cannot be a follower
const int32 TOTAL_PED_GROUP_FOLLOWERS = TOTAL_PED_GROUP_MEMBERS - 1;

class CPedGroupMembership {
public:

    static constexpr int32 LEADER_MEM_ID = 7; ///< Leader's member ID
    //static const float& ms_fMaxSeparation;
    //static const float& ms_fPlayerGroupMaxSeparation;

public:
    static void InjectHooks();

    CPedGroupMembership(CPedGroup& group);
    CPedGroupMembership(const CPedGroupMembership&) = default; // 0x5FB140
    ~CPedGroupMembership();

    //! R*'s implementation of `operator=` xD [0x5F7FE0]
    void From(const CPedGroupMembership& obj) { *this = obj; }

    //! Add a new follower to the group. If there's no more memory, they aren't added
    void AddFollower(CPed* ped);

    //! Set a new leader [Should be called only if there's no leader]
    void AppointNewLeader();

    //! Count of members
    size_t CountMembers();

    //! Count of followers
    int32 CountMembersExcludingLeader();

    //! Flush the toilet
    void Flush();

    //! Update the states
    void Process();

    //! If there isn't one null is returned.
    CPed* GetLeader() const;

    //! Set a new leader
    void SetLeader(CPed* ped);

    //! Whenever there's a leader
    bool HasLeader() const { return GetLeader() != nullptr; }

    //! Is ped the leader
    bool IsLeader(const CPed* ped) const;

    //! Get the member using it's id
    CPed* GetMember(int32 memberId);

    //! Get a member's id
    int32 GetMemberId(const CPed* ped) const;

    //! Is ped a follower (A member, but not the leader)
    bool IsFollower(const CPed* ped) const;

    //! Is ped a member (follower or leader)
    bool IsMember(const CPed* ped) const;

    //! Remove all followers of this group [That is, all members except the leader]
    void RemoveAllFollowers(bool bCreatedByGameOnly);

    //! Remove a specific member by it's ID
    void RemoveMember(int32 memberID);

    //! Remove a specific member
    void RemoveMember(CPed* ped);

    //! I'll let you guess
    void RemoveNFollowers(size_t count);

    //! Get a random ped from the group. Might return null.
    CPed* GetRandom();

    //! Whenever `AddFollower` can be called to add a new follower
    bool CanAddFollower();

    //! Get all the present members [Returns a view of references]
    auto GetMembers(bool bIncludeLeader = true) {
        assert(LEADER_MEM_ID == m_members.size() - 1); // the drop below requires this
        return m_members
             | rng::views::drop(bIncludeLeader ? 0 : 1) // Last member is the leader
             | rng::views::filter(notsa::NotIsNull{})
             | rng::views::transform([](CPed* mem) -> CPed& { return *mem; }); // Dereference
    }

    //! Get followers [that is, members excl. the leader]
    auto GetFollowers() { return GetMembers(false); }

    // Closest member, closest member distance sq
    using FindClosestMemberResult = std::tuple<CPed*, float>;

    /*!
    * @notsa
    * @brief Find the member of this group closest to the ped.
    * 
    * @param ped  The ped to which the cloest member should be searched for
    * @param pred Custom predicate to filter members
    * 
    * @return The closest member (may be null, in which case the distance should be considered invalid), and it's sq. dist from `ped`
    */
    template<std::predicate<CPed&> Pred>
    auto GetMemberClosestToIf(CPed* ped, Pred&& pred, bool includeLeader = true) -> FindClosestMemberResult {
        const auto& pedPos = ped->GetPosition();
        float closestDistSq{ std::numeric_limits<float>::max() };
        CPed* closest{};
        for (auto& mem : GetMembers(includeLeader)) {
            if (&mem == ped) {
                continue;
            }
            if (!std::invoke(pred, mem)) {
                continue;
            }
            if (const auto distSq = (pedPos - mem.GetPosition()).SquaredMagnitude(); closestDistSq > distSq) {
                closestDistSq = distSq;
                closest       = &mem;
            }
        }
        return { closest, closestDistSq };
    }

    /// Wrapper around `GetMemberClosestToIf`, using an always-true predicate
    auto GetMemberClosestTo(CPed* ped, bool includeLeader = true) { return GetMemberClosestToIf(ped, [](CPed&) { return true; }, includeLeader); }

    //! Find follower closest to the leader
    auto FindClosestFollowerToLeader() -> FindClosestMemberResult;

    static eModelID GetObjectForPedToHold();
private:
    //! NOTSA
    int32 FindNewLeaderToAppoint() const;

    //! NOTSA
    int32 FindIdForNewMember() const;

    //! Based on code from 0x5F80BE
    void  GivePedRandomObjectToHold(CPed* mem, bool onlyIfUnarmed = true) const;

    //! Add a member by placing it at the given index in the `m_members` array
    void AddMember(CPed* member, int32 memIdx);

private:
    CPedGroup*                                 m_group{};
    std::array<CPed*, TOTAL_PED_GROUP_MEMBERS> m_members{}; // m_apMembers[LEADER_MEM_ID] is the leader. The array is not contigous and it might have null values in it!
    float                                      m_separationRange{60.f};
};
VALIDATE_SIZE(CPedGroupMembership, 0x28);
