#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleSetStayInSamePlace : public CTaskSimple {
public:
    bool m_bStayInSamePlace;

public:
    static constexpr auto Type = TASK_SIMPLE_SET_STAY_IN_SAME_PLACE;

    explicit CTaskSimpleSetStayInSamePlace(bool bStayInSamePlace);
    ~CTaskSimpleSetStayInSamePlace() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleSetStayInSamePlace(m_bStayInSamePlace); }
    bool MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }
    bool ProcessPed(CPed* ped) override;
};
VALIDATE_SIZE(CTaskSimpleSetStayInSamePlace, 0xC);
