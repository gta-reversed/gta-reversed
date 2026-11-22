#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleArrestPed : public CTaskSimple {
public:
    CPed* m_Ped;
    bool  m_bFinished;
    CAnimBlendAssociation* m_Assoc;

public:
    static constexpr auto Type = TASK_SIMPLE_ARREST_PED;

    CTaskSimpleArrestPed(CPed* ped);
    ~CTaskSimpleArrestPed() override;

    eTaskType GetTaskType() const override { return Type; } // 0x68B680
    CTask* Clone() const override { return new CTaskSimpleArrestPed(m_Ped); } // 0x68CD10
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed*) override;
    void StartAnim(CPed* ped);

    static void FinishAnimArrestPedCB(CAnimBlendAssociation* assoc, void* data);

};
VALIDATE_SIZE(CTaskSimpleArrestPed, 0x14);
extern void CTaskSimpleArrestPed__InjectHooks();
