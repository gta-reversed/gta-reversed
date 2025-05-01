#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseWaterCannon : public CTaskComplex {
public:
    CFire* m_pFire;

public:
    static constexpr auto Type = TASK_COMPLEX_USE_WATER_CANNON;

    explicit CTaskComplexUseWaterCannon(CFire* pFire);
    ~CTaskComplexUseWaterCannon() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    static void InjectHooks();
    CTaskComplexUseWaterCannon* Constructor(CFire* pFire);
};
VALIDATE_SIZE(CTaskComplexUseWaterCannon, 0x10);
