#include "StdInc.h"

void CDecisionMakerTypesFileLoader::InjectHooks() {

}

// 0x600860
void CDecisionMakerTypesFileLoader::GetPedDMName(int32 index, char* name)
{
    plugin::Call<0x600860, int32, char*>(index, name);
}

// 0x600880
void CDecisionMakerTypesFileLoader::GetGrpDMName(int32 index, char* name)
{
    plugin::Call<0x600880, int32, char*>(index, name);
}

// 0x5BF400
void CDecisionMakerTypesFileLoader::LoadDefaultDecisionMaker() {
    plugin::Call<0x5BF400>();
}

// 0x607D30
void CDecisionMakerTypesFileLoader::LoadDecisionMaker(const char* filepath, eDecisionTypes decisionMakerType, bool bUseMissionCleanup)
{
    CDecisionMaker decisionMaker;
    LoadDecisionMaker(filepath, &decisionMaker);
    CDecisionMakerTypes* pDecisionMakerTypes = CDecisionMakerTypes::GetInstance();
    CDecisionMakerTypes::AddDecisionMaker(pDecisionMakerTypes, &decisionMaker, decisionMakerType, bUseMissionCleanup);
}

// 0x6076B0
void CDecisionMakerTypesFileLoader::LoadDecisionMaker(const char *filepath, CDecisionMaker* decisionMaker) {
    plugin::Call<0x6076B0, const char*, CDecisionMaker*>(filepath, decisionMaker);
}

void CDecisionMakerTypes::InjectHooks() {

}

// 0x607050
void CDecisionMakerTypes::AddDecisionMaker(CDecisionMakerTypes* decisionMakerTypes, CDecisionMaker* decisionMaker, eDecisionTypes decisionMakerType, bool bUseMissionCleanup) {
    plugin::Call<0x607050, CDecisionMakerTypes*, CDecisionMaker*, eDecisionTypes, bool>(decisionMakerTypes, decisionMaker, decisionMakerType, bUseMissionCleanup);
}

// 0x4684F0
CDecisionMakerTypes* CDecisionMakerTypes::GetInstance() {
    return plugin::CallAndReturn<CDecisionMakerTypes*, 0x4684F0>();
}

// 0x606E70
void CDecisionMakerTypes::MakeDecision(CPed* ped, int32 eventType, int32 eventSourceType, bool bIsPedInVehicle, int32 taskId1,
    int32 taskId2, int32 taskId3, int32 taskId4, bool bDecisionMakerTypeInGroup, int16* taskId, int16* field_10)
{
    plugin::CallMethod<0x606E70, CDecisionMakerTypes*, CPed*, int32, int32, bool, int32, int32, int32, int32, bool, int16*, int16*>
        (this, ped, eventType, eventSourceType, bIsPedInVehicle, taskId1, taskId2, taskId3, taskId4, bDecisionMakerTypeInGroup, taskId, field_10);
}

// 0x606F80
int32 CDecisionMakerTypes::MakeDecision(CPedGroup* pPedGroup, int32 eventType, int32 eventSourceType, bool bIsPedInVehicle, int32 taskId1, int32 taskId2, int32 taskId3, int32 taskId4) {
    return plugin::CallMethodAndReturn <int32, 0x606F80, CDecisionMakerTypes*, CPedGroup*, int32, int32, bool, int32, int32, int32, int32>
        (this, pPedGroup, eventType, eventSourceType, bIsPedInVehicle, taskId1, taskId2, taskId3, taskId4);
}

// 0x6044C0
void CDecisionMakerTypes::AddEventResponse(int32 decisionMakerIndex, int32 eventType, int32 taskId, float* responseChances, int32* flags)
{
    plugin::CallMethod<0x6044C0, CDecisionMakerTypes*, int32, int32, int32, float*, int32*>
        (this, decisionMakerIndex, eventType, taskId, responseChances, flags);
}

// 0x604490
void CDecisionMakerTypes::FlushDecisionMakerEventResponse(int32 decisionMakerIndex, int32 eventId)
{
    plugin::CallMethod<0x604490, CDecisionMakerTypes*, int32, int32>(this, decisionMakerIndex, eventId);
}
