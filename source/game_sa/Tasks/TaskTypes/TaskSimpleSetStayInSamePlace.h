#pragma once
#include "TaskSimple.h"

class CTaskSimpleSetStayInSamePlace : public CTaskSimple
{
public:
    uint8 m_bStayInSamePlace;
    uint8 _pad_9[3];
private:
    CTaskSimpleSetStayInSamePlace* Constructor(bool bStayInSamePlace);
public:
    CTaskSimpleSetStayInSamePlace(bool bStayInSamePlace);
    ~CTaskSimpleSetStayInSamePlace() override {}

    static void InjectHooks();

    CTask* Clone() override { return new CTaskSimpleSetStayInSamePlace(m_bStayInSamePlace); }
    eTaskType GetTaskType() override { return TASK_SIMPLE_SET_STAY_IN_SAME_PLACE; }
    bool ProcessPed(CPed* ped) override;
    bool MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) override { return true; }

    bool ProcessPed_Reversed(CPed* ped);
};

VALIDATE_SIZE(CTaskSimpleSetStayInSamePlace, 0xC);
