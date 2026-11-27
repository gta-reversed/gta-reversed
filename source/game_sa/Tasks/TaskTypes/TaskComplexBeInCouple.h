#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexBeInCouple : public CTaskComplex {
    enum ePartnerType : int32 {
        NONE,
        RIGHT,
        LEFT,
    };

public:
    static constexpr auto Type = TASK_COMPLEX_BE_IN_COUPLE;

    CTaskComplexBeInCouple(
        CPed* partner,
        bool  isLeader,
        bool  holdHands       = true,
        bool  lookAtEachOther = true,
        float giveUpDist      = 10.f
    );
    CTaskComplexBeInCouple(const CTaskComplexBeInCouple&);
    ~CTaskComplexBeInCouple() override = default; // 0x683780

    eTaskType GetTaskType() const override { return Type; }                          // 0x683770
    CTask*    Clone() const override { return new CTaskComplexBeInCouple{ *this }; } // 0x6839C0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    void         AbortArmIK(CPed* ped);
    ePartnerType GetPartnerSide(CPed* ped) const;

    static void InjectHooks();

private:
    ePartnerType m_PartnerType{};
    CPed::Ref    m_Partner{}; //!< The other ped this (the owner of the task) ped is together with
    bool         m_IsLeader{};
    bool         m_HoldHands{};
    bool         m_LookAtEachOther{};
    float        m_GiveUpDist{};
    int8         m_PrevSide{}; //!< `ePartnerType` of the last arm pointing
};
VALIDATE_SIZE(CTaskComplexBeInCouple, 0x20);
