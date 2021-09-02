/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

/*
    https://gtamods.com/wiki/Decision_Maker
*/

enum eDecisionTypes {
    DECISION_ON_FOOT = 0,
    DECISION_IN_VEHICLE = 1
};

enum eDecisionRelationship {
    DECISION_RELATIONSHIP_NEUTRAL = 0,
    DECISION_RELATIONSHIP_PLAYER = 1,
    DECISION_RELATIONSHIP_FRIEND = 2,
    DECISION_RELATIONSHIP_THREAT = 3
};

class CDecision {
public:
    int32 m_anTaskTypes[6];                    // see eTaskType
    uint8 m_anResponseChances[6][4]; // 4 different relationships : see eDecisionRelationship
    uint8 m_anTypeFlags[2][6];       // 2 different types : see eDecisionTypes

    // 0x6040C0
    inline CDecision() {
        // SetDefault();
    }
};

VALIDATE_SIZE(CDecision, 0x3C);
