#pragma once

#include "TaskSimple.h"
#include <extensions/EntityRef.hpp>

class NOTSA_EXPORT_VTABLE CTaskSimpleEvasiveStep : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_EVASIVE_STEP;

    explicit CTaskSimpleEvasiveStep(CEntity* entity);
    ~CTaskSimpleEvasiveStep() override;

    static void FinishAnimEvasiveStepCB(CAnimBlendAssociation* assoc, void* data);

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskSimpleEvasiveStep(m_Entity); }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    void StartAnim(CPed* ped);

public:
    static void InjectHooks();

public:
    notsa::EntityRef<>     m_Entity{};
    bool                   m_bFinished{};
    CAnimBlendAssociation* m_Assoc{};
};
VALIDATE_SIZE(CTaskSimpleEvasiveStep, 0x14);
