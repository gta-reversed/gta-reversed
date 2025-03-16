#pragma once

#include "TaskSimpleDie.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleDieInCar : public CTaskSimpleDie {
public:
    static constexpr auto Type = TASK_SIMPLE_DIE_IN_CAR;

    CTaskSimpleDieInCar(AssocGroupId groupId, AnimationId animId);
    ~CTaskSimpleDieInCar() override = default; // 0x62FC70, 0x6375B0

    CTask* Clone() const override { return new CTaskSimpleDieInCar(m_animGroupId, m_animId); } // 0x635EF0
    eTaskType GetTaskType() const override { return Type; } // 0x62FC60
    bool ProcessPed(CPed* ped) override; // 0x6398F0
};
VALIDATE_SIZE(CTaskSimpleDieInCar, 0x28);
