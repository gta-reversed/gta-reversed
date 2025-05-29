/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Decision.h"
#include "eDecisionMakerEvents.h"

class CDecisionMaker {
public:
    CDecision m_aDecisions[DM_TOTAL_NUM_EVENTS];

public:
    static void InjectHooks();

    CDecisionMaker();
};

VALIDATE_SIZE(CDecisionMaker, 0x99C);
