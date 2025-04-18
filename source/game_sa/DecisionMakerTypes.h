#pragma once

#include "Decision.h"
#include "Enums/eTaskType.h"

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
    static inline auto& m_IsActive          = *(std::array<bool, NUM_TYPES>*)0xC0B01C;

    static void InjectHooks();

    static CDecisionMakerTypes* GetInstance();

    int32 AddDecisionMaker(CDecisionMaker* decisionMaker, eDecisionTypes decisionMakerType, bool bUseMissionCleanup);
    void MakeDecision(CPed* ped, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskTypeToAvoid1, eTaskType taskTypeToAvoid2, eTaskType taskTypeToAvoid3, eTaskType taskTypeToSeek, bool bUseInGroupDecisionMaker, int16& taskType, int16& facialTaskType);
    eTaskType MakeDecision(CPedGroup* pedGroup, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskId1, eTaskType taskId2, eTaskType taskId3, eTaskType taskId4);
    void AddEventResponse(int32 decisionMakerIndex, eEventType eventType, eTaskType taskId, float* responseChances, int32* flags);
    void FlushDecisionMakerEventResponse(int32 decisionMakerIndex, eEventType eventId);
};
