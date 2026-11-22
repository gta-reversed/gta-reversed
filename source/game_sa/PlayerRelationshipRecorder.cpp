#include "StdInc.h"

#include "PlayerRelationshipRecorder.h"
#include "TaskCategories.h"

void CPlayerRelationshipRecorder::InjectHooks() {
    RH_ScopedClass(CPlayerRelationshipRecorder);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Flush, 0x61A2A0);
    RH_ScopedInstall(ClearRelationshipWithPlayer, 0x61A150);
    RH_ScopedInstall(AddRelationship, 0x61A180);
    RH_ScopedInstall(GetRelationshipWithPlayer, 0x61A1A0);
    RH_ScopedInstall(RecordRelationshipWithPlayer, 0x61A1D0);
}

// 0x61A130
CPlayerRelationshipRecorder::CPlayerRelationshipRecorder() {
    Flush();
}

// 0x61A2C0
CPlayerRelationshipRecorder::~CPlayerRelationshipRecorder() {
    Flush();
}

// 0x61A2A0
void CPlayerRelationshipRecorder::Flush() {
    for (auto& relationship : m_Relationships) {
        relationship.Flush();
    }
}

// 0x61A180
void CPlayerRelationshipRecorder::AddRelationship(const CPed* ped, int32 value) {
    auto& rel = m_Relationships[0];
    if (rel.Ped) {
        rel.Ped          = ped;
        rel.Relationship = (uint8)value;
    }
}

// 0x61A1D0
void CPlayerRelationshipRecorder::RecordRelationshipWithPlayer(const CPed* ped) {
    ClearRelationshipWithPlayer(ped);

    auto* const task = ped->GetTaskManager().GetActiveTask();
    if (!task) {
        return;
    }

    // NOTE: Only the 2nd out param of each check is used
    bool unused{}, isTask{};
    CTaskCategories::IsKillPedTask(task, unused, isTask);
    if (!isTask) {
        CTaskCategories::IsFollowPedTask(task, unused, isTask);
    }
    if (isTask) {
        AddRelationship(ped, 3);
        return;
    }

    CTaskCategories::IsKillPedTask(task, unused, isTask);
    if (isTask) {
        AddRelationship(ped, 7);
    }
}

// 0x61A1A0
int32 CPlayerRelationshipRecorder::GetRelationshipWithPlayer(const CPed* ped) {
    for (auto& relationship : m_Relationships) {
        if (relationship.Ped != ped) {
            continue;
        }

        return relationship.Relationship;
    }
    return 0;
}

// 0x61A150
void CPlayerRelationshipRecorder::ClearRelationshipWithPlayer(const CPed* ped) {
    for (auto& relationship : m_Relationships) {
        if (relationship.Ped != ped) {
            continue;
        }

        return relationship.Flush();
    }
}

// 0x61A2E0
CPlayerRelationshipRecorder& GetPlayerRelationshipRecorder() {
    static auto& g_sPlayerRelationshipRecorder = StaticRef<CPlayerRelationshipRecorder*>(0xC17084);

    if (!g_sPlayerRelationshipRecorder) {
        g_sPlayerRelationshipRecorder = new CPlayerRelationshipRecorder();
    }
    return *g_sPlayerRelationshipRecorder;
}
