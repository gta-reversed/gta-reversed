#include "StdInc.h"

#include "DecisionMakerTypes.h"

void CDecisionMakerTypesFileLoader::InjectHooks() {
    RH_ScopedClass(CDecisionMakerTypesFileLoader);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ReStart, 0x607D00);
    RH_ScopedInstall(GetPedDMName, 0x600860);
    RH_ScopedInstall(GetGrpDMName, 0x600880);
    RH_ScopedInstall(UnloadDecisionMaker, 0x607A70, { .enabled = false, .locked = true }); // Causes esp check failure
    RH_ScopedInstall(LoadDefaultDecisionMaker, 0x5BF400);
    RH_ScopedOverloadedInstall(LoadDecisionMaker, "enum", 0x607D30, int32(*)(const char*, eDecisionTypes, bool));
    RH_ScopedOverloadedInstall(LoadDecisionMaker, "ptr", 0x6076B0, bool (*)(const char*, CDecisionMaker*));
}

// 0x607D00
void CDecisionMakerTypesFileLoader::ReStart() {
    for (int32 i = 0; i < +eDecisionMakerType::COUNT_GAME_DM; i++) {
        UnloadDecisionMaker((eDecisionTypes)(i));
    }
}

// 0x600860
void CDecisionMakerTypesFileLoader::GetPedDMName(int32 index, char* name) {
    constexpr auto names = std::to_array({ // 0x600864
        "m_empty.ped",
        "m_norm.ped",
        "m_tough.ped",
        "m_weak.ped",
        "m_steal.ped",
        "MISSION.grp"
    });
    strcpy(name, names[index]);
}

// 0x600880
void CDecisionMakerTypesFileLoader::GetGrpDMName(int32 index, char* name) {
    constexpr auto names = std::to_array({ // 0x8D2378
        "MISSION.grp"
    });
    strcpy(name, names[index]);
}

// 0x5BF400
void CDecisionMakerTypesFileLoader::LoadDefaultDecisionMaker() {
    for (int32 i = 0; i < +eDecisionMakerType::COUNT_TOTAL; i++) {
        UnloadDecisionMaker((eDecisionTypes)(i));
    }

    CDecisionMakerTypes::GetInstance()->LoadEventIndices();

    LoadDecisionMaker("RANDOM.ped", &CDecisionMakerTypes::GetInstance()->m_DefaultRandomPedDecisionMaker);
    LoadDecisionMaker("m_norm.ped", &CDecisionMakerTypes::GetInstance()->m_DefaultMissionPedDecisionMaker);
    LoadDecisionMaker("m_plyr.ped", &CDecisionMakerTypes::GetInstance()->m_DefaultPlayerPedDecisionMaker);

    LoadDecisionMaker("RANDOM.grp", &CDecisionMakerTypes::GetInstance()->m_DefaultRandomPedGroupDecisionMaker);
    LoadDecisionMaker("MISSION.grp", &CDecisionMakerTypes::GetInstance()->m_DefaultMissionPedGroupDecisionMaker);

    LoadDecisionMaker("GangMbr.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("Cop.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("R_Norm.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("R_Tough.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("R_Weak.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("Fireman.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("m_empty.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("Indoors.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("RANDOM.grp", PLAYER_DECISION_MAKER, false);
    LoadDecisionMaker("RANDOM2.grp", PLAYER_DECISION_MAKER, false);
}

// 0x607D30 - Returns script handle for the DM
int32 CDecisionMakerTypesFileLoader::LoadDecisionMaker(const char* filepath, eDecisionTypes decisionMakerType, bool bDecisionMakerForMission) {
    CDecisionMaker decisionMaker;
    LoadDecisionMaker(filepath, &decisionMaker);
    return CDecisionMakerTypes::GetInstance()->AddDecisionMaker(&decisionMaker, decisionMakerType, bDecisionMakerForMission);
}

// 0x6076B0
// Mostly refactored
bool CDecisionMakerTypesFileLoader::LoadDecisionMaker(const char* filepath, CDecisionMaker* decisionMaker) {
    CFileMgr::SetDir("data\\decision\\allowed\\");
    const auto file = CFileMgr::OpenFile(filepath, "r");
    CFileMgr::SetDir("");

    if (!file) {
        NOTSA_LOG_WARN("Failed to open decision maker file: {}", filepath);
        return false;
    }
    (void)(CFileLoader::LoadLine(file)); // Skip first line
    for (;;) {
        const auto line = CFileLoader::LoadLine(file);
        if (!line) {
            break;
        }
        size_t off = 0;

        uint32 eventType{};
        uint32 unused{};
        VERIFY(sscanf(
            line,
            "%d %d %n",
            &eventType, &unused, &off
        ) == 2);

        notsa::mdarray<int32, CDecision::MAX_NUM_CHOICES> tasks{};
        notsa::mdarray<float, CDecision::MAX_NUM_CHOICES, 4> probs{};
        notsa::mdarray<int32, CDecision::MAX_NUM_CHOICES, 2> bools{};
        notsa::mdarray<float, CDecision::MAX_NUM_CHOICES, 6> facialProbs{};

        for (int32 i = 0; i < CDecision::MAX_NUM_CHOICES; i++) {
            uint32 n{};
            VERIFY(sscanf(
                line + off,
                "%d %f %f %f %f %d %d %f %f %f %f %f %f %n",
                &tasks[i],
                &probs[i][DECISION_RELATIONSHIP_FRIEND],
                &probs[i][DECISION_RELATIONSHIP_THREAT],
                &probs[i][DECISION_RELATIONSHIP_PLAYER],
                &probs[i][DECISION_RELATIONSHIP_NEUTRAL],
                &bools[i][1],
                &bools[i][0],
                &facialProbs[i][0],
                &facialProbs[i][1],
                &facialProbs[i][2],
                &facialProbs[i][3],
                &facialProbs[i][4],
                &facialProbs[i][5],
                &n
            ) == 13);
            off += n;
        };

        auto* const d = &decisionMaker->m_aDecisions[CDecisionMakerTypes::GetInstance()->m_EventIndices[eventType]];
        d->SetDefault();
        d->Set(tasks, probs, bools, facialProbs);
    }

    CFileMgr::CloseFile(file);

    return true;
}

// 0x607A70
void CDecisionMakerTypesFileLoader::UnloadDecisionMaker(eDecisionTypes dm) {
    CDecisionMakerTypes::GetInstance()->RemoveDecisionMaker(dm);
}

void CDecisionMakerTypes::InjectHooks() {
    RH_ScopedClass(CDecisionMakerTypes);
    RH_ScopedCategoryGlobal();
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
