#include "StdInc.h"

#include "PlayerRelationshipRecorder.h"
#include "TaskCategories.h"

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
        rel.Ped = ped;
        rel.Relationship = value;
    }
}

// 0x61A1D0
void CPlayerRelationshipRecorder::RecordRelationshipWithPlayer(const CPed* ped) {
    plugin::CallMethod<0x61A1D0, CPlayerRelationshipRecorder*, const CPed*>(this, ped);
    return;

    // sheeeet
    ClearRelationshipWithPlayer(ped);

    bool outIsKillPedTask[4];

    auto task1 = ped->GetTaskManager().GetActiveTask();
    if (task1) {
        outIsKillPedTask[2] = false;

        bool unk = false;
        CTaskCategories::IsKillPedTask(task1, outIsKillPedTask[2], unk);
        if (unk || (outIsKillPedTask[3] = 0, outIsKillPedTask[0] = 0, CTaskCategories::IsFollowPedTask(task1, outIsKillPedTask[3], outIsKillPedTask[0]), outIsKillPedTask[0])) {
            AddRelationship(ped, 3);
        } else {
            bool unk1 = false;
            outIsKillPedTask[1] = false;
            CTaskCategories::IsKillPedTask(task1, unk1, outIsKillPedTask[1]);
            if (outIsKillPedTask[1]) {
                AddRelationship(ped, 7);
            }
        }
    }
}

// 0x61A1A0
int32 CPlayerRelationshipRecorder::GetRelationshipWithPlayer(const CPed* ped) {
    for (auto& relationship : m_Relationships) {
        if (relationship.Ped != ped)
            continue;

        return relationship.Relationship;
    }
    return 0;
}

// 0x61A150
void CPlayerRelationshipRecorder::ClearRelationshipWithPlayer(const CPed* ped) {
    for (auto& relationship : m_Relationships) {
        if (relationship.Ped != ped)
            continue;

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
