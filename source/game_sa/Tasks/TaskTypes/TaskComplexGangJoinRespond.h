#pragma once

#include "TaskComplex.h"


class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexGangJoinRespond : public CTaskComplex {
    static constexpr int32 MAX_ATTEMPTS = 10; //!< Max. number of times we wait for the anims to stream in before giving up
public:
    static constexpr auto Type          = TASK_COMPLEX_GANG_JOIN_RESPOND;

    CTaskComplexGangJoinRespond(bool response);
    ~CTaskComplexGangJoinRespond() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexGangJoinRespond(m_JoinResponse); } // 0x662290;
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; } // 0x661790;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

public:
    static void InjectHooks();

private:
    bool  m_JoinResponse{};       // 0xC  - Whenever to accept (`true`) or decline (`false`) the invitation
    bool  m_AreAnimsReferenced{}; // 0xD
    int32 m_NumOfAttempts{};      // 0x10 - Number of times we've waited for the anims to stream in
};
VALIDATE_SIZE(CTaskComplexGangJoinRespond, 0x14);
