#include "StdInc.h"

#include "DecisionMakerTypesFileLoader.h"

void CDecisionMakerTypesFileLoader::InjectHooks() {
    RH_ScopedClass(CDecisionMakerTypesFileLoader);
    RH_ScopedCategory("DecisionMakers");

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
        UnloadDecisionMaker((eDecisionMakerType)(i));
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
        UnloadDecisionMaker((eDecisionMakerType)(i));
    }

    CDecisionMakerTypes::GetInstance()->LoadEventIndices();

    LoadDecisionMaker("RANDOM.ped", &CDecisionMakerTypes::GetInstance()->m_DefaultRandomPedDecisionMaker);
    LoadDecisionMaker("m_norm.ped", &CDecisionMakerTypes::GetInstance()->m_DefaultMissionPedDecisionMaker);
    LoadDecisionMaker("m_plyr.ped", &CDecisionMakerTypes::GetInstance()->m_DefaultPlayerPedDecisionMaker);

    LoadDecisionMaker("RANDOM.grp", &CDecisionMakerTypes::GetInstance()->m_DefaultRandomPedGroupDecisionMaker);
    LoadDecisionMaker("MISSION.grp", &CDecisionMakerTypes::GetInstance()->m_DefaultMissionPedGroupDecisionMaker);

    LoadDecisionMaker("GangMbr.ped", PED_DECISION_MAKER, false);
    LoadDecisionMaker("Cop.ped", PED_DECISION_MAKER, false);
    LoadDecisionMaker("R_Norm.ped", PED_DECISION_MAKER, false);
    LoadDecisionMaker("R_Tough.ped", PED_DECISION_MAKER, false);
    LoadDecisionMaker("R_Weak.ped", PED_DECISION_MAKER, false);
    LoadDecisionMaker("Fireman.ped", PED_DECISION_MAKER, false);
    LoadDecisionMaker("m_empty.ped", PED_DECISION_MAKER, false);
    LoadDecisionMaker("Indoors.ped", PED_DECISION_MAKER, false);
    LoadDecisionMaker("RANDOM.grp", GROUP_DECISION_MAKER, false);
    LoadDecisionMaker("RANDOM2.grp", GROUP_DECISION_MAKER, false);
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

        auto* const d = &decisionMaker->Decisions[CDecisionMakerTypes::GetInstance()->m_EventIndices[eventType]];
        d->SetDefault();
        d->Set(tasks, probs, bools, facialProbs);
    }

    CFileMgr::CloseFile(file);

    return true;
}

// 0x607A70
void CDecisionMakerTypesFileLoader::UnloadDecisionMaker(eDecisionMakerType dm) {
    CDecisionMakerTypes::GetInstance()->RemoveDecisionMaker(dm);
}
