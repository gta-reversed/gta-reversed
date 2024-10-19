#pragma once

#include "Decision.h"

class CPed;
class CPedGroup;
class CDecisionMaker;

class CDecisionMakerTypesFileLoader {
public:
    static void InjectHooks();

    static void ReStart();
    static void GetPedDMName(int32 index, char* name);
    static void GetGrpDMName(int32 index, char* name);
    static void LoadDefaultDecisionMaker();
    static int32 LoadDecisionMaker(const char* filepath, eDecisionTypes decisionMakerType, bool bUseMissionCleanup);
    static void LoadDecisionMaker(const char* filepath, CDecisionMaker* decisionMaker);
};

class CDecisionMakerTypes {
public:
    static constexpr auto NUM_TYPES = 20u;

    static inline auto& ScriptReferenceIndex = *(std::array<uint16, NUM_TYPES>*)0xC0AFF4;
    static inline auto& m_bIsActive          = *(std::array<bool, NUM_TYPES>*)0xC0B01C;

    static void InjectHooks();

    static CDecisionMakerTypes* GetInstance();

    int32 AddDecisionMaker(CDecisionMaker* decisionMaker, eDecisionTypes decisionMakerType, bool bUseMissionCleanup);
    void MakeDecision(CPed* ped, int32 eventType, int32 eventSourceType, bool bIsPedInVehicle, int32 taskTypeToAvoid1, int32 taskTypeToAvoid2, int32 taskTypeToAvoid3, int32 taskTypeToSeek, bool bUseInGroupDecisionMaker, int16& taskType, int16& facialTaskType);
    int32 MakeDecision(CPedGroup* pedGroup, int32 eventType, int32 eventSourceType, bool bIsPedInVehicle, int32 taskId1, int32 taskId2, int32 taskId3, int32 taskId4);
    void AddEventResponse(int32 decisionMakerIndex, int32 eventType, int32 taskId, float* responseChances, int32* flags);
    void FlushDecisionMakerEventResponse(int32 decisionMakerIndex, int32 eventId);
};
