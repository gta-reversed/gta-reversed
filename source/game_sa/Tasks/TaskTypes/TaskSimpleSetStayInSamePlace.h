#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleSetStayInSamePlace : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_SET_STAY_IN_SAME_PLACE;

    CTaskSimpleSetStayInSamePlace(bool bStayInSamePlace);
    ~CTaskSimpleSetStayInSamePlace() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskSimpleSetStayInSamePlace(m_StayInSamePlace); }
    bool      MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }
    bool      ProcessPed(CPed* ped) override;

public:
    static void InjectHooks();

private:
    bool m_StayInSamePlace{};
};
VALIDATE_SIZE(CTaskSimpleSetStayInSamePlace, 0xC);
