#include "StdInc.h"

#include <PedStats.h>
#include "DecisionMakerTypes.h"

void CDecisionMakerTypes::InjectHooks() {
    RH_ScopedClass(CDecisionMakerTypes);
    RH_ScopedCategory("DecisionMakers");

    RH_ScopedGlobalInstall(GetInstance, 0x4684F0);

    RH_ScopedOverloadedInstall(LoadEventIndices, "", 0x5BB9F0, void(CDecisionMakerTypes::*)(EventIndicesArray&, const char*));
    RH_ScopedOverloadedInstall(LoadEventIndices, "", 0x600840, void(CDecisionMakerTypes::*)());
    //RH_ScopedInstall(HasResponse, 0x6042B0, { .reversed = false });
    RH_ScopedInstall(RemoveDecisionMaker, 0x6043A0);
    RH_ScopedInstall(FlushDecisionMakerEventResponse, 0x604490);
    RH_ScopedInstall(AddEventResponse, 0x6044C0);
    RH_ScopedOverloadedInstall(MakeDecision, "", 0x606E70, void(CDecisionMakerTypes::*)(CPed*, eEventType, int32, bool, eTaskType, eTaskType, eTaskType, eTaskType, bool, int16&, int16&));
    RH_ScopedOverloadedInstall(MakeDecision, "", 0x606F80, eTaskType(CDecisionMakerTypes::*)(CPedGroup*, eEventType, int32, bool, eTaskType, eTaskType, eTaskType, eTaskType), { .reversed = false });
    RH_ScopedInstall(AddDecisionMaker, 0x607050);
    RH_ScopedInstall(CopyDecisionMaker, 0x6070F0);
}

// 0x607050
int32 CDecisionMakerTypes::AddDecisionMaker(CDecisionMaker* decisionMaker, eDecisionTypes decisionMakerType, bool bDecisionMakerForMission) {
    const auto dm = GetInactiveDecisionMaker(bDecisionMakerForMission);
    if (!dm) {
        return -1;
    }
    const auto idx = GetDecisionMakerIndex(dm);

    m_IsActive[idx] = true;
    m_Types[idx] = decisionMakerType;
    *dm = *decisionMaker;

    return idx;
}

// 0x4684F0
CDecisionMakerTypes* CDecisionMakerTypes::GetInstance() {
    static CDecisionMakerTypes* const instance = new CDecisionMakerTypes();
    return instance;
}

// 0x606E70
void CDecisionMakerTypes::MakeDecision(CPed* ped, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskTypeToAvoid1, eTaskType taskTypeToAvoid2, eTaskType taskTypeToAvoid3, eTaskType taskTypeToSeek, bool bUseInGroupDecisionMaker, int16& taskType, int16& facialTaskType) {
    const auto MakeDecisionUsingMaker = [&] (CDecisionMaker* dm) {
        taskType       = TASK_NONE;
        facialTaskType = TASK_INVALID;
        return dm->Decisions[m_EventIndices[eventType]].MakeDecision(
            eventSourceType,
            bIsPedInVehicle,
            taskTypeToAvoid1,
            taskTypeToAvoid2,
            taskTypeToAvoid3,
            taskTypeToSeek,
            taskType
        );
    };
    const auto dmType = bUseInGroupDecisionMaker
        ? ped->GetIntelligence()->m_nDecisionMakerTypeInGroup
        : ped->GetIntelligence()->m_nDecisionMakerType;
    switch ((int32)(dmType.get())) {
    case -2: // PLAYER_DECISION_MAKER
        return MakeDecisionUsingMaker(&m_DefaultPlayerPedDecisionMaker);
    case -1: // DEFAULT_DECISION_MAKER
        return ped->IsCreatedByMission()
            ? MakeDecisionUsingMaker(&m_DefaultMissionPedDecisionMaker)
            : MakeDecisionUsingMaker(&m_DefaultRandomPedDecisionMaker);
    default:
        return MakeDecisionUsingMaker(&m_DecisionMakers[+dmType]);
    }
}

// 0x6043A0
void CDecisionMakerTypes::RemoveDecisionMaker(eDecisionMakerType dm) {
    if (!m_IsActive[+dm]) {
        return;
    }
    switch (const auto type = m_Types[+dm]) {
    case GROUP_DECISION_MAKER: { // 0x60443C
        for (auto&& [id, group] : CPedGroups::GetActiveGroupsWithIDs()) {
            auto* const intel = &group.GetIntelligence();
            if (intel->GetGroupDecisionMakerType() == dm) {
                intel->SetGroupDecisionMakerType(eDecisionMakerType::UNKNOWN);
            }
        }
        break;
    }
    case PED_DECISION_MAKER: { // 0x6043C5
        for (auto& ped : GetPedPool()->GetAllValid()) {
            auto* const intel = ped.GetIntelligence();
            if (intel->GetPedDecisionMakerType() == dm) {
                intel->SetPedDecisionMakerType(ped.m_pStats->m_nDefaultDecisionMaker);
            }
        }
        break;
    }
    default:
        NOTSA_UNREACHABLE("Invalid decision maker type: {}", type.get()); // Originally this was just an else branch, not sure if there are more types than the enum has...
    }
}

// 0x606F80
eTaskType CDecisionMakerTypes::MakeDecision(CPedGroup* pedGroup, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskId1, eTaskType taskId2, eTaskType taskId3, eTaskType taskId4) {
    return plugin::CallMethodAndReturn<eTaskType, 0x606F80, CDecisionMakerTypes*, CPedGroup*, int32, int32, bool, int32, int32, int32, int32>(
        this, pedGroup, eventType, eventSourceType, bIsPedInVehicle, taskId1, taskId2, taskId3, taskId4);
}

// 0x6044C0
void CDecisionMakerTypes::AddEventResponse(int32 dm, eEventType eventType, eTaskType taskId, float* responseChances, int32* flags) {
    m_DecisionMakers[dm].Decisions[this->m_EventIndices[eventType]].Add(
        taskId,
        responseChances,
        flags
    );
}

// 0x604490
void CDecisionMakerTypes::FlushDecisionMakerEventResponse(int32 dm, eEventType eventType) {
    m_DecisionMakers[dm].Decisions[this->m_EventIndices[eventType]].SetDefault();
}

// 0x5BB9F0
void CDecisionMakerTypes::LoadEventIndices(EventIndicesArray& out, const char* filepath) {
    out.fill(0);

    CFileMgr::SetDir("");
    CFileMgr::SetDir("data\\decision\\");
    const auto file = CFileMgr::OpenFile(filepath, "r");
    CFileMgr::SetDir("");
    if (!file) {
        NOTSA_LOG_WARN("Failed to open decision maker event indices file: {}", filepath);
        return;
    }
    for (int32 count = 0;;) {
        char line[256];
        if (!CFileMgr::ReadLine(file, line, sizeof(line))) {
            break;
        }
        if (line[0] && line[0] != '\n') {
            int32 index{};
            char name[256]{};
            VERIFY(sscanf_s(line, "%s %d", SCANF_S_STR(name), &index) == 2);
            out[index] = count++;
        }
    }
    CFileMgr::CloseFile(file);
}

// 0x600840
void CDecisionMakerTypes::LoadEventIndices() {
    LoadEventIndices(m_EventIndices, "PedEvent.txt");
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
        PED_DECISION_MAKER,
        isDecisionMakerForMission
    );
}

// notsa
CDecisionMaker* CDecisionMakerTypes::GetInactiveDecisionMaker(bool bDecisionMakerForMission) {
    const auto from = bDecisionMakerForMission ? 15 : 0;
    const auto to   = bDecisionMakerForMission ? 20 : 15;
    for (auto i = from; i < to; i++) {
        if (!m_IsActive[i]) {
            return &m_DecisionMakers[i];
        }
    }
    return nullptr;
}

int32 CDecisionMakerTypes::GetDecisionMakerIndex(CDecisionMaker* dm) {
    return std::distance(m_DecisionMakers.data(), dm);
}
