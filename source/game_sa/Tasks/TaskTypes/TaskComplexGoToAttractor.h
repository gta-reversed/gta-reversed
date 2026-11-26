#pragma once

#include "TaskComplex.h"


class NOTSA_EXPORT_VTABLE CTaskComplexGoToAttractor : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_GO_TO_ATTRACTOR;

    CTaskComplexGoToAttractor(CPedAttractor* attractor, const CVector& pos, float heading, float attrTime, int32 queueNumber, eMoveState ms);
    CTaskComplexGoToAttractor(const CTaskComplexGoToAttractor&);
    ~CTaskComplexGoToAttractor() override = default; // 0x66B6A0

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexGoToAttractor{ *this }; } // 0x66D130
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return m_pSubTask->MakeAbortable(ped, priority, event); } // 0x66B6B0
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; } // 0x66B710

    auto GetAttractPos() const { return m_AttractPos; }

public:
    static void InjectHooks();

private:
    CPedAttractor* m_Attractor{};
    CVector        m_AttractPos{};
    float          m_AttractHeading{};
    float          m_AttractTime{};
    int32          m_Slot{};
    eMoveState     m_MoveState{};
};
VALIDATE_SIZE(CTaskComplexGoToAttractor, 0x2C);
