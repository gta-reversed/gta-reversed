#include "StdInc.h"

#include "DecisionMakerTypes.h"

void CDecisionMakerTypes::InjectHooks() {
    RH_ScopedClass(CDecisionMakerTypes);
    RH_ScopedCategory("DecisionMakers");

    RH_ScopedGlobalInstall(GetInstance, 0x4684F0, { .reversed = false });

    //RH_ScopedOverloadedInstall(LoadEventIndices, "", 0x5BB9F0, int32(CDecisionMakerTypes::*)(int32 *, const char*), { .reversed = false });
    RH_ScopedOverloadedInstall(LoadEventIndices, "", 0x600840, void(CDecisionMakerTypes::*)(), { .reversed = false });
    //RH_ScopedInstall(HasResponse, 0x6042B0, { .reversed = false });
    RH_ScopedInstall(RemoveDecisionMaker, 0x6043A0, { .reversed = false });
    RH_ScopedInstall(FlushDecisionMakerEventResponse, 0x604490, { .reversed = false });
    RH_ScopedInstall(AddEventResponse, 0x6044C0, { .reversed = false });
    RH_ScopedOverloadedInstall(MakeDecision, "", 0x606E70, void(CDecisionMakerTypes::*)( CPed*, eEventType, int32, bool, eTaskType, eTaskType, eTaskType, eTaskType, bool, int16&, int16&), { .reversed = false });
    RH_ScopedOverloadedInstall(MakeDecision, "", 0x606F80, eTaskType(CDecisionMakerTypes::*)( CPedGroup *, eEventType, int32, bool, eTaskType, eTaskType, eTaskType, eTaskType), { .reversed = false });
    RH_ScopedInstall(AddDecisionMaker, 0x607050, { .reversed = false });
    RH_ScopedInstall(CopyDecisionMaker, 0x6070F0);
}

// 0x607050
int32 CDecisionMakerTypes::AddDecisionMaker(CDecisionMaker* decisionMaker, eDecisionTypes decisionMakerType, bool bDecisionMakerForMission) {
    return plugin::CallMethodAndReturn<int32, 0x607050, CDecisionMakerTypes*, CDecisionMaker*, eDecisionTypes, bool>(this, decisionMaker, decisionMakerType, bDecisionMakerForMission);
}

// 0x4684F0
CDecisionMakerTypes* CDecisionMakerTypes::GetInstance() {
    return plugin::CallAndReturn<CDecisionMakerTypes*, 0x4684F0>();
}

// 0x606E70
void CDecisionMakerTypes::MakeDecision(CPed* ped, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskTypeToAvoid1, eTaskType taskTypeToAvoid2, eTaskType taskTypeToAvoid3, eTaskType taskTypeToSeek, bool bUseInGroupDecisionMaker, int16& taskType, int16& facialTaskType) {
    plugin::CallMethod<0x606E70>(this, ped, eventType, eventSourceType, bIsPedInVehicle, taskTypeToAvoid1, taskTypeToAvoid2, taskTypeToAvoid3, taskTypeToSeek, bUseInGroupDecisionMaker, &taskType, &facialTaskType);
}

// 0x6043A0
void CDecisionMakerTypes::RemoveDecisionMaker(eDecisionTypes dm) {
    return plugin::Call<0x6043A0>(dm);
}

// 0x606F80
eTaskType CDecisionMakerTypes::MakeDecision(CPedGroup* pedGroup, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskId1, eTaskType taskId2, eTaskType taskId3, eTaskType taskId4) {
    return plugin::CallMethodAndReturn<eTaskType, 0x606F80, CDecisionMakerTypes*, CPedGroup*, int32, int32, bool, int32, int32, int32, int32>(
        this, pedGroup, eventType, eventSourceType, bIsPedInVehicle, taskId1, taskId2, taskId3, taskId4);
}

// 0x6044C0
void CDecisionMakerTypes::AddEventResponse(int32 decisionMakerIndex, eEventType eventType, eTaskType taskId, float* responseChances, int32* flags) {
    plugin::CallMethod<0x6044C0, CDecisionMakerTypes*, int32, int32, int32, float*, int32*>(this, decisionMakerIndex, eventType, taskId, responseChances, flags);
}

// 0x604490
void CDecisionMakerTypes::FlushDecisionMakerEventResponse(int32 decisionMakerIndex, eEventType eventId) {
    plugin::CallMethod<0x604490>(this, decisionMakerIndex, eventId);
}

void CDecisionMakerTypes::LoadEventIndices() {
    plugin::CallMethod<0x600840>(this);
}

// 0x6070F0
int32 CDecisionMakerTypes::CopyDecisionMaker(int32 index, eDecisionTypes type, bool isDecisionMakerForMission) {
    if (index != -1) {
        return AddDecisionMaker(
            &this->m_DecisionMakers[index],
            type,
            isDecisionMakerForMission
        );
    }
    if (type != eDecisionTypes::DEFAULT_DECISION_MAKER) {
        return AddDecisionMaker(
            &this->m_DefaultMissionPedGroupDecisionMaker,
            type,
            isDecisionMakerForMission
        );
    }
    return AddDecisionMaker(
        &this->m_DefaultMissionPedDecisionMaker,
        DEFAULT_DECISION_MAKER,
        isDecisionMakerForMission
    );
}
