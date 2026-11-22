#pragma once

#include "TaskComplex.h"
#include <extensions/EntityRef.hpp>

class NOTSA_EXPORT_VTABLE CTaskComplexEvasiveStep : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_EVASIVE_STEP;

    CTaskComplexEvasiveStep(CEntity* entity, const CVector& pos);
    ~CTaskComplexEvasiveStep() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexEvasiveStep(m_Entity, m_Pos); }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; } // 0x653550

    CTask* CreateSubTask(eTaskType taskType);

public:
    static void InjectHooks();

public:
    CVector            m_Pos;
    notsa::EntityRef<> m_Entity;
};
VALIDATE_SIZE(CTaskComplexEvasiveStep, 0x1C);
