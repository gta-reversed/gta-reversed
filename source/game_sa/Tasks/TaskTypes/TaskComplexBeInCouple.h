#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexBeInCouple : public CTaskComplex {
    enum WalkSide : int32 {
        NONE,
        RIGHT,
        LEFT,
    };
public:
    WalkSide m_walkOnSide{};
    CPed*    m_partner{}; // The other ped this (the owner of the task) ped is together with
    bool     m_isLeader{};
    bool     m_holdHands{};
    bool     m_lookAtEachOther{};
    float    m_giveUpDist{};
    int8     m_prevSide{};
public:
    static constexpr auto Type = TASK_COMPLEX_BE_IN_COUPLE;

    CTaskComplexBeInCouple(
        CPed* pPed,
        bool  isLeader,
        bool  holdHands = true,
        bool  lookAtEachOther = true,
        float giveUpDist = 10.f
    );
    ~CTaskComplexBeInCouple() override;

    eTaskType GetTaskType() const override { return Type; } // 0x683770
    CTask* Clone() const override { return new CTaskComplexBeInCouple(m_partner, m_isLeader, m_holdHands, m_lookAtEachOther, m_giveUpDist); } // 0x6839C0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    void AbortArmIK(CPed* ped);
};
VALIDATE_SIZE(CTaskComplexBeInCouple, 0x20);
