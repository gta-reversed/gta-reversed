#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleDrownInCar : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_DROWN_IN_CAR;

    CTaskSimpleDrownInCar() = default;  // 0x62FF20
    ~CTaskSimpleDrownInCar() override = default; // 0x62FF60

    eTaskType GetTaskType() const override { return Type; }; // 0x62FF40
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; } // 0x62FF50
    CTask* Clone() const override { return new CTaskSimpleDrownInCar(); } // 0x636010
    bool ProcessPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
