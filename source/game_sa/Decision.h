/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

/*
    https://gtamods.com/wiki/Decision_Maker
*/

enum eTaskType : int32;

enum eDecisionTypes {
    DEFAULT_DECISION_MAKER = 0,
    PLAYER_DECISION_MAKER = 1
};

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
    eTaskType m_anTaskTypes[MAX_NUM_CHOICES];
    uint8     m_anResponseChances[MAX_NUM_CHOICES][4]; // 4 different relationships : see eDecisionRelationship
    uint8     m_anTypeFlags[2][MAX_NUM_CHOICES];       // 2 different types : see eDecisionTypes

public:
    static void InjectHooks();

    CDecision();
    CDecision(const CDecision&) = default;

    void SetDefault();
    void From(const CDecision& rhs);
};

VALIDATE_SIZE(CDecision, 0x3C);
