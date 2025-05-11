#pragma once

#include "Decision.h"
#include "DecisionMaker.h"
#include "Enums/eTaskType.h"

class CPed;
class CPedGroup;

enum class eDecisionMakerType : int32 {
    UNKNOWN                 = -1,

    PED_GROUPMEMBER         = 0, // 0x0
    PED_COP                 = 1, // 0x1
    PED_RANDOM1             = 2, // 0x2
    PED_RANDOM2             = 3, // 0x3
    PED_RANDOM3             = 4, // 0x4
    PED_FIREMAN             = 5, // 0x5
    PED_EMPTY               = 6, // 0x6
    PED_INDOORS             = 7, // 0x7

    GROUP_RANDOM_AGGRESSIVE = 8, // 0x8
    GROUP_RANDOM_PASSIVE    = 9, // 0x9

    COUNT_GAME_DM = 10, //!< Number of built-in decision makers

    MISSION0                = 10, // 0xA
    MISSION1                = 11, // 0xB
    MISSION2                = 12, // 0xC
    MISSION3                = 13, // 0xD
    MISSION4                = 14, // 0xE
    MISSION5                = 15, // 0xF
    MISSION6                = 16, // 0x10
    MISSION7                = 17, // 0x11
    MISSION8                = 18, // 0x12
    MISSION9                = 19, // 0x13

    COUNT_TOTAL             = 20, // 0x14
};

class CDecisionMakerTypesFileLoader {
public:
    static void InjectHooks();

    static void ReStart();
    static void GetPedDMName(int32 index, char* name);
    static void GetGrpDMName(int32 index, char* name);
    static void LoadDefaultDecisionMaker();
    static int32 LoadDecisionMaker(const char* filepath, eDecisionTypes decisionMakerType, bool bDecisionMakerForMission);
    static void LoadDecisionMaker(const char* filepath, CDecisionMaker* decisionMaker);
    static void UnloadDecisionMaker(eDecisionTypes dm);
};

class CDecisionMakerTypes {
public:
    static constexpr auto NUM_TYPES = 20u;

    static inline auto& ScriptReferenceIndex = *(std::array<uint16, NUM_TYPES>*)0xC0AFF4;
    static inline auto& m_IsActive          = *(std::array<bool, NUM_TYPES>*)0xC0B01C;

    static void InjectHooks();

    static CDecisionMakerTypes* GetInstance();

    int32 AddDecisionMaker(CDecisionMaker* decisionMaker, eDecisionTypes decisionMakerType, bool bDecisionMakerForMission);
    void MakeDecision(CPed* ped, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskTypeToAvoid1, eTaskType taskTypeToAvoid2, eTaskType taskTypeToAvoid3, eTaskType taskTypeToSeek, bool bUseInGroupDecisionMaker, int16& taskType, int16& facialTaskType);
    void RemoveDecisionMaker(eDecisionTypes dm);
    eTaskType MakeDecision(CPedGroup* pedGroup, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskId1, eTaskType taskId2, eTaskType taskId3, eTaskType taskId4);
    void AddEventResponse(int32 decisionMakerIndex, eEventType eventType, eTaskType taskId, float* responseChances, int32* flags);
    void FlushDecisionMakerEventResponse(int32 decisionMakerIndex, eEventType eventId);

public:
    int32          m_NoOfDecisionMakers{};
    CDecisionMaker m_DecisionMakers[+eDecisionMakerType::COUNT_TOTAL]{};
    int32          m_EventIndices[+eEventType::EVENT_TOTAL_NUM_EVENTS]{};
    CDecisionMaker m_DefaultRandomPedDecisionMaker{};
    CDecisionMaker m_DefaultMissionPedDecisionMaker{};
    CDecisionMaker m_DefaultPlayerPedDecisionMaker{};
    CDecisionMaker m_DefaultRandomPedGroupDecisionMaker{};
    CDecisionMaker m_DefaultMissionPedGroupDecisionMaker{};
};
