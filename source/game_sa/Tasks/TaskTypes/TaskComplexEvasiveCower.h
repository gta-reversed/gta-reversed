#pragma once

#include "TaskComplex.h"
#include <extensions/EntityRef.hpp>

class NOTSA_EXPORT_VTABLE CTaskComplexEvasiveCower : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_EVASIVE_COWER;

    CTaskComplexEvasiveCower(CEntity* entity, const CVector& pos);
    ~CTaskComplexEvasiveCower() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexEvasiveCower(m_Entity, m_Pos); }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

    CTask* CreateSubTask(eTaskType taskType);

public:
    static void InjectHooks();

private:
    CVector            m_Pos{};
    notsa::EntityRef<> m_Entity{};
};
VALIDATE_SIZE(CTaskComplexEvasiveCower, 0x1C);
