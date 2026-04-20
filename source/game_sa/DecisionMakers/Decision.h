/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <extensions/utility.hpp>

/*
    https://gtamods.com/wiki/Decision_Maker
*/

enum eTaskType : int32;

enum eDecisionTypes {
    PLAYER_DECISION_MAKER = -2,
    DEFAULT_DECISION_MAKER = -1,

    PED_DECISION_MAKER = 0,
    GROUP_DECISION_MAKER = 1
};
NOTSA_WENUM_DEFS_FOR(eDecisionTypes);

enum eDecisionRelationship {
    DECISION_RELATIONSHIP_NEUTRAL = 0,
    DECISION_RELATIONSHIP_PLAYER = 1,
    DECISION_RELATIONSHIP_FRIEND = 2,
    DECISION_RELATIONSHIP_THREAT = 3
};

class CDecision {
public:
    constexpr static auto MAX_NUM_DECISIONS = 0x29; // 41
    constexpr static auto MAX_NUM_CHOICES   = 0x6;  // 6

public:
    notsa::mdarray<eTaskType, MAX_NUM_CHOICES>    m_Tasks;
    notsa::mdarray<uint8, MAX_NUM_CHOICES, 4>     m_Probs; // 4 different relationships : see eDecisionRelationship
    notsa::mdarray<bool, 2, MAX_NUM_CHOICES>      m_Bools; // 2 different types : see eDecisionTypes

public:
    static void InjectHooks();

    CDecision();
    CDecision(const CDecision&) = default;

    void SetDefault();
    void From(const CDecision& rhs);
    void Set(
        notsa::mdarray<int32, MAX_NUM_CHOICES>&    tasks,
        notsa::mdarray<float, MAX_NUM_CHOICES, 4>& probs,
        notsa::mdarray<int32, MAX_NUM_CHOICES, 2>& bools,
        notsa::mdarray<float, MAX_NUM_CHOICES, 6>& facialProbs
    );
    void MakeDecision(
        int32 eventSourceType,
        bool isInVehicle,
        eTaskType taskTypeToReject1,
        eTaskType taskTypeToReject2,
        eTaskType taskTypeToReject3,
        eTaskType taskTypeToSeek,
        int16&    outTaskType,
        int16&    outFacialTaskType
    );
    void Add(eTaskType taskType, float* pProbs, int32* pBools);
    bool HasResponse() const noexcept;
};

VALIDATE_SIZE(CDecision, 0x3C);
