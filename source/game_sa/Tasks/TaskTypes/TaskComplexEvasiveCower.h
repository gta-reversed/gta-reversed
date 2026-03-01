#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexEvasiveCower : public CTaskComplex {
public:
    CVector  m_Pos;
    CEntity* m_Entity;

public:
    static constexpr auto Type = TASK_COMPLEX_EVASIVE_COWER;

    CTaskComplexEvasiveCower(CEntity* entity, const CVector& pos);
    ~CTaskComplexEvasiveCower() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexEvasiveCower(m_Entity, m_Pos); }
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; }

    CTask* CreateSubTask(eTaskType taskType);
};
VALIDATE_SIZE(CTaskComplexEvasiveCower, 0x1C);
