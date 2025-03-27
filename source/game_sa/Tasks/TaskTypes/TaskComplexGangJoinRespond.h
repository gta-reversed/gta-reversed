#pragma once

#include "TaskComplex.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexGangJoinRespond : public CTaskComplex {
public:
    bool  m_response{};
    bool  m_animsReferenced{};
    int32 m_attempts{};

public:
    static constexpr auto Type = TASK_COMPLEX_GANG_JOIN_RESPOND;

    explicit CTaskComplexGangJoinRespond(bool response);
    ~CTaskComplexGangJoinRespond() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexGangJoinRespond(m_response); } // 0x662290;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; } // 0x661790;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexGangJoinRespond* Constructor(uint8 a2) { this->CTaskComplexGangJoinRespond::CTaskComplexGangJoinRespond(a2); return this; }
    CTaskComplexGangJoinRespond* Destructor() { this->CTaskComplexGangJoinRespond::~CTaskComplexGangJoinRespond(); return this; }
};
VALIDATE_SIZE(CTaskComplexGangJoinRespond, 0x14);
