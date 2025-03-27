#pragma once

#include "TaskSimpleDie.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleDrown : public CTaskSimpleDie {
public:
    bool m_bShouldExecute;

public:
    static constexpr auto Type = TASK_SIMPLE_DROWN;

    CTaskSimpleDrown();
    ~CTaskSimpleDrown() override = default; // 0x62FF10, 0x6378D0

    eTaskType GetTaskType() const override { return Type; } // 0x62FF00
    CTask* Clone() const override { return new CTaskSimpleDrown(); } // 0x635E60
    bool ProcessPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
