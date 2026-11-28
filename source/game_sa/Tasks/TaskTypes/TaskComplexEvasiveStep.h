#pragma once

#include "TaskComplex.h"
#include <extensions/EntityRef.hpp>

class NOTSA_EXPORT_VTABLE CTaskComplexEvasiveStep : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_EVASIVE_STEP;

    CTaskComplexEvasiveStep(CEntity* entity, const CVector& moveDir);
    CTaskComplexEvasiveStep(const CTaskComplexEvasiveStep& o); // NOTSA
    ~CTaskComplexEvasiveStep() override = default;             // 0x653350

    eTaskType GetTaskType() const override { return Type; }                           // 0x653340
    CTask*    Clone() const override { return new CTaskComplexEvasiveStep{ *this }; } // 0x655870
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; } // 0x653550

    CTask* CreateSubTask(eTaskType taskType);

    static void InjectHooks();

private:
    CVector            m_MoveDir{}; //!< Direction the ped steps away in (from `CTaskComplexHitPedWithCar::ComputeEvasiveStepMoveDir` etc.)
    notsa::EntityRef<> m_Entity{};  //!< The entity (vehicle) being avoided
};
VALIDATE_SIZE(CTaskComplexEvasiveStep, 0x1C);
