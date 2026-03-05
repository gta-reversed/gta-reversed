#pragma once

#include "DecisionMakerTypes.h"

class CDecisionMakerTypesFileLoader {
public:
    static void InjectHooks();

    static void  ReStart();
    static void  GetPedDMName(int32 index, char* name);
    static void  GetGrpDMName(int32 index, char* name);
    static void  LoadDefaultDecisionMaker();
    static int32 LoadDecisionMaker(const char* filepath, eDecisionTypes decisionMakerType, bool bDecisionMakerForMission);
    static bool  LoadDecisionMaker(const char* filepath, CDecisionMaker* decisionMaker);
    static void  UnloadDecisionMaker(eDecisionMakerType dm);
};
